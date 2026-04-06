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

    // ── Function entry ─────────────────────────────────────────────────────
    // Describes one function exposed by a module.
    struct function_entry {
        std::string name_;
        native_fn   fn_;
        int min_arity_ = 0;
        int max_arity_ = 0;   // -1 = variadic (no upper bound)
        bool is_timed_  = false;   // true → fn_ wraps a timed_fn (scheduler provides t,dt)
        timed_fn timed_fn_;        // set when is_timed_ == true
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
        void provide_scheduler(time::sys_clock_scheduler* sched) {
            for (auto& m : modules_)
                m->set_scheduler(sched);
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
        std::vector<void*> dl_handles_;           // dlopen handles
        std::vector<std::string> loaded_paths_;   // already-loaded paths

    };

} // namespace idyl::module