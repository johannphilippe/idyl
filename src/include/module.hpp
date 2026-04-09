#pragma once

// =============================================================================
// module.hpp — idyl module framework
//
// Provides the types and registry needed to create native modules that expose
// functions callable from idyl programs.
//
// To create a module:
//   1. Subclass `idyl::module::base_module`
//   2. Override `name()` and `functions()`
//   3. Register via `registry::add()`
//
// Module functions receive arguments as `span<const core::value>` and return
// a `core::value`.  Use `std::function` captures for stateful modules.
// =============================================================================

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <dlfcn.h>

#include "core/core.hpp"
#include "utilities/span.hpp"
#include "include/idyl_module_api.h"

// Forward-declare scheduler to avoid circular includes
namespace idyl::time { struct sys_clock_scheduler; }

namespace idyl::module {

    // ── Native function type ───────────────────────────────────────────────
    // std::function so module methods can capture `this` (stateful modules).
    using native_fn = std::function<core::value(span<const core::value>)>;

    // ── Timed function type ────────────────────────────────────────────────
    // Like native_fn but receives the current time and dt from the scheduler.
    // Used for module functions that need temporal awareness (e.g. periodic polls).
    using timed_fn = std::function<core::value(span<const core::value>, double t, double dt)>;

    // ── Parameter descriptor (used by native temporal function entries) ───────
    // Declares one parameter by name with an optional default value.
    struct param_def {
        std::string     name_;
        core::value     default_value_ = core::value::nil();
        bool            has_default_   = false;
    };

    // Convenience builder: required param (no default)
    inline param_def required(std::string name) {
        return {std::move(name), core::value::nil(), false};
    }
    // Convenience builder: param with a numeric default
    inline param_def with_default(std::string name, double v) {
        return {std::move(name), core::value::number(v), true};
    }
    // Convenience builder: param with a time default (ms)
    inline param_def with_default_ms(std::string name, double ms) {
        return {std::move(name), core::value::time_ms(ms), true};
    }

    // ── Function entry ─────────────────────────────────────────────────────
    // Describes one function exposed by a module.
    // For stateless functions: fill name_, fn_, min_arity_, max_arity_.
    // For native temporal functions: set is_native_temporal_ = true and fill
    // params_, native_init_, native_update_.  min/max arity are derived
    // automatically from params_ (required params count as min; total as max).
    struct function_entry {
        std::string name_;
        native_fn   fn_;
        int min_arity_ = 0;
        int max_arity_ = 0;   // -1 = variadic (no upper bound)
        bool is_timed_  = false;   // true → fn_ wraps a timed_fn (scheduler provides t,dt)
        timed_fn timed_fn_;        // set when is_timed_ == true

        // ── Native temporal fields ─────────────────────────────────────────
        // When is_native_temporal_ is true, the evaluator treats this like a
        // temporal lambda: creates a function_instance, runs native_init_ once,
        // and calls native_update_ every tick via tick_instance.
        bool is_native_temporal_ = false;

        // Declared parameters (positional order).  A parameter named "dt"
        // with a time default is treated as the tick interval.
        std::vector<param_def> params_;

        core::native_init_fn   native_init_;    // optional: init state from params
        core::native_update_fn native_update_;  // required when is_native_temporal_
    };

    // ── Base module ────────────────────────────────────────────────────────
    // Subclass this to implement a module.  The registry owns module
    // instances, so captured `this` pointers in function entries remain
    // valid for the program's lifetime.
    struct base_module {
        virtual ~base_module() = default;

        // Unique module name, e.g. "osc", "midi", "audio"
        virtual std::string name() const = 0;

        // Return the list of functions this module provides.
        // Called once at registration time.
        virtual std::vector<function_entry> functions() = 0;
        // Check whether this module's dependencies are satisfied.
        // Called at registration time — if false, the module is skipped
        // with a debug message rather than causing a hard error.
        // Override in modules that require external resources.
        virtual bool available() const { return true; }

        // Human-readable reason when available() returns false.
        virtual std::string unavailable_reason() const { return "unknown"; }
        // Optional: called after the scheduler is available.
        // Modules that need timing (dt-driven sends, etc.) override this.
        virtual void set_scheduler(time::sys_clock_scheduler* /*sched*/) {}
    };

    // ── Module registry ────────────────────────────────────────────────────
    // Owns all registered modules and provides flat function lookup.
    class registry {
    public:
        // Register a module.  Ownership transfers to the registry.
        void add(std::unique_ptr<base_module> mod) {
            auto fns = mod->functions();
            for (auto& f : fns) {
                index_.emplace(f.name_, std::move(f));
            }
            modules_.push_back(std::move(mod));
        }

        // Conditionally register a native module: checks available()
        // before adding.  Returns true if the module was registered.
        bool try_add(std::unique_ptr<base_module> mod) {
            if (!mod->available()) {
                // Module prerequisites not met — skip silently
                return false;
            }
            add(std::move(mod));
            return true;
        }

        // Look up a function by name across all modules.  Returns nullptr
        // if not found.
        const function_entry* lookup(const std::string& name) const {
            auto it = index_.find(name);
            return it != index_.end() ? &it->second : nullptr;
        }

        // All registered function entries (for semantic analysis registration).
        const std::unordered_map<std::string, function_entry>& all() const {
            return index_;
        }

        // Number of registered modules.
        size_t module_count() const { return modules_.size(); }

        // Provide the scheduler to every registered module that needs it.
        // Also caches the pointer so modules loaded later (via load_builtin)
        // receive the scheduler immediately.
        void provide_scheduler(time::sys_clock_scheduler* sched) {
            sched_ = sched;
            for (auto& m : modules_)
                m->set_scheduler(sched);
        }

        // ── Built-in module catalog ────────────────────────────────────────
        // Modules in the catalog are available by name but not loaded until
        // the program calls `module("name")`.  This keeps the global function
        // namespace clean and avoids loading modules the program doesn't use.

        using module_factory = std::function<std::unique_ptr<base_module>()>;

        // Register a built-in module factory.  Does not load or activate it.
        void register_builtin(const std::string& name, module_factory factory) {
            catalog_.emplace(name, std::move(factory));
        }

        bool has_builtin(const std::string& name) const {
            return catalog_.count(name) > 0;
        }

        // Return the function entries exported by a built-in module without
        // activating it.  Used by the semantic analyzer to register symbols
        // with correct arity info.
        std::vector<function_entry> list_builtin_entries(const std::string& name) const {
            auto it = catalog_.find(name);
            if (it == catalog_.end()) return {};
            return it->second()->functions();
        }

        // Convenience: return just the function names.
        std::vector<std::string> list_builtin_functions(const std::string& name) const {
            auto entries = list_builtin_entries(name);
            std::vector<std::string> names;
            names.reserve(entries.size());
            for (const auto& e : entries) names.push_back(e.name_);
            return names;
        }

        // Load a built-in module and register its functions.
        // `ns` prefixes all function names ("ns::func") when non-empty.
        // Returns empty string on success or an error message on failure.
        std::string load_builtin(const std::string& name,
                                 const std::string& ns = "") {
            if (loaded_builtins_.count(name)) return "";  // idempotent

            auto it = catalog_.find(name);
            if (it == catalog_.end()) return "";  // not a builtin — caller handles

            auto mod = it->second();
            if (!mod->available())
                return "Module '" + name + "' is not available: "
                       + mod->unavailable_reason();

            auto fns = mod->functions();
            for (auto& f : fns) {
                std::string fn_name = ns.empty() ? f.name_ : (ns + "::" + f.name_);
                function_entry fe = f;
                fe.name_ = fn_name;
                index_.emplace(fn_name, std::move(fe));
            }
            if (sched_) mod->set_scheduler(sched_);
            modules_.push_back(std::move(mod));
            loaded_builtins_.insert(name);
            return "";
        }

        // ── External module loading (dlopen) ───────────────────────────────
        // Load a shared library from `path`, call idyl_module_init(), and
        // register the exported functions.  Optionally namespace them under
        // `ns` (e.g. "m::func" when user wrote `m = module("name")`).
        //
        // Returns an error message on failure, or empty string on success.
        // *Never* crashes — all errors are caught and reported as strings.
        std::string load_external(const std::string& path,
                                  const std::string& ns = "") {
            // Open the shared library
            void* handle = dlopen(path.c_str(), RTLD_LAZY);
            if (!handle) {
                return "Could not load module '" + path + "': "
                       + std::string(dlerror());
            }

            // Look up the init function
            auto init_fn = reinterpret_cast<idyl_module_init_fn>(
                dlsym(handle, "idyl_module_init"));
            if (!init_fn) {
                std::string err = "Module '" + path
                    + "' has no 'idyl_module_init' symbol — not a valid idyl module.";
                dlclose(handle);
                return err;
            }

            // Call init to get the function table
            int count = 0;
            idyl_fn_entry* entries = nullptr;
            try {
                entries = init_fn(&count);
            } catch (...) {
                dlclose(handle);
                return "Module '" + path + "': idyl_module_init threw an exception.";
            }

            if (!entries || count <= 0) {
                dlclose(handle);
                return "Module '" + path + "' exported no functions.";
            }

            // Wrap each C function entry into our native_fn / function_entry
            for (int i = 0; i < count; ++i) {
                const auto& e = entries[i];
                if (!e.name || !e.fn) continue;

                std::string fn_name = ns.empty()
                    ? std::string(e.name)
                    : ns + "::" + std::string(e.name);

                // Capture the C function pointer — the dlopen handle stays alive
                idyl_fn raw_fn = e.fn;
                native_fn wrapped = [raw_fn](span<const core::value> args) -> core::value {
                    // Convert core::value args → idyl_value array
                    std::vector<idyl_value> c_args(args.size_);
                    for (size_t j = 0; j < args.size_; ++j) {
                        auto& v = args[j];
                        auto& cv = c_args[j];
                        cv.number  = v.number_;
                        cv.trigger = v.trigger_ ? 1 : 0;
                        cv.string  = nullptr;
                        cv.handle  = v.handle_;

                        switch (v.type_) {
                            case core::value_t::number:
                                cv.type = IDYL_NUMBER; break;
                            case core::value_t::time:
                                cv.type = IDYL_TIME; break;
                            case core::value_t::trigger:
                                cv.type = IDYL_TRIGGER; break;
                            case core::value_t::string:
                                cv.type = IDYL_STRING;
                                cv.string = v.string_ ? v.string_->c_str() : "";
                                break;
                            case core::value_t::handle:
                                cv.type = IDYL_HANDLE; break;
                            default:
                                cv.type = IDYL_NIL; break;
                        }
                    }

                    // Call the module function
                    idyl_value result = raw_fn(c_args.data(),
                                              static_cast<int>(c_args.size()));

                    // Convert result back to core::value
                    switch (result.type) {
                        case IDYL_NUMBER:  return core::value::number(result.number);
                        case IDYL_TIME:    return core::value::time_ms(result.number);
                        case IDYL_TRIGGER: return core::value::trigger(result.trigger != 0);
                        case IDYL_STRING:
                            return core::value::string(
                                result.string ? result.string : "");
                        case IDYL_HANDLE:  return core::value::handle(result.handle);
                        default:           return core::value::nil();
                    }
                };

                function_entry fe;
                fe.name_      = fn_name;
                fe.fn_        = std::move(wrapped);
                fe.min_arity_ = e.min_arity;
                fe.max_arity_ = e.max_arity;
                index_.emplace(fn_name, std::move(fe));
            }

            // Keep the handle alive for the program's lifetime
            dl_handles_.push_back(handle);
            return "";  // success
        }

        // Check if an external module has already been loaded from `path`.
        bool is_loaded(const std::string& path) const {
            // We track by path string — re-loading is idempotent.
            for (const auto& p : loaded_paths_) {
                if (p == path) return true;
            }
            return false;
        }

        void mark_loaded(const std::string& path) {
            loaded_paths_.push_back(path);
        }

        ~registry() {
            // Close all dlopen handles
            for (auto* h : dl_handles_) {
                if (h) dlclose(h);
            }
        }

    private:
        std::vector<std::unique_ptr<base_module>> modules_;
        std::unordered_map<std::string, function_entry> index_;
        std::vector<void*> dl_handles_;                       // dlopen handles
        std::vector<std::string> loaded_paths_;               // already-loaded .so paths
        std::unordered_map<std::string, module_factory> catalog_;  // builtin catalog
        std::unordered_set<std::string> loaded_builtins_;     // loaded builtin names
        time::sys_clock_scheduler* sched_ = nullptr;          // cached for lazy loads

    };

} // namespace idyl::module