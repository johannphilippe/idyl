#pragma once

#include <variant>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <cmath>
#include <cstdint>
#include <unordered_map>
#include <mutex>

// Forward-declare parser types used in core runtime structs.
namespace idyl::parser { struct expression; struct function_definition; }

namespace idyl::core {

    // ── Value tag ──────────────────────────────────────────────────────────────
    // Mirrors semantic::inferred_t but lives in core to avoid circular deps.
    // The evaluator uses this exclusively at runtime.
    enum class value_t {
        number,
        time,
        trigger,
        string,
        flow,
        function,
        module,
        handle,
        instance_ref,   // live reference to a temporal function_instance (used in flow elements)
        nil
    };

    // Forward declarations for complex types
    struct flow_data;
    struct function_instance;

    // ── Runtime value ──────────────────────────────────────────────────────────
    struct value 
    {
        value_t type_ = value_t::nil;
        double number_ = 0.0;  // used for number and time (ms)
        bool trigger_   = false; // used for trigger
        intptr_t handle_ = 0;    // used for handle (opaque C pointer)

        // Complex types (heap-allocated, ref-counted)
        std::shared_ptr<std::string> string_;
        std::shared_ptr<flow_data> flow_;
        // Live reference to a temporal function_instance.
        // Non-null only when type_ == instance_ref.
        // Accessing this value always reads the instance's current output.
        std::shared_ptr<function_instance> instance_ = nullptr;

        // Non-null for local function definitions (defined inside process/lambda
        // scope). Carries the AST directly so the function bypasses function_defs_.
        std::shared_ptr<parser::function_definition> fn_def_ = nullptr;

        // Non-null for closures: the temporal instance whose params/state the
        // function captures.  At call time, the instance's current params_ and
        // current_ are pushed into a scope frame so the body can read them by
        // reference — i.e. it always sees the instance's latest values, not a
        // snapshot taken at definition time.
        std::shared_ptr<function_instance> closure_inst_ = nullptr;

        // ── Constructors ───────────────────────────────────────────────────────
        // Aggregate constructors for the plain types — instance_ defaults to nullptr.
        static value number(double v)       { return {value_t::number, v, false, 0, nullptr, nullptr}; }
        static value time_ms(double ms)     { return {value_t::time, ms, false, 0, nullptr, nullptr}; }
        static value trigger(bool fired)    { return {value_t::trigger, fired ? 1.0 : 0.0, fired, 0, nullptr, nullptr}; }
        static value rest()                 { return {value_t::trigger, 0.0, false, 0, nullptr, nullptr}; } // rest is trigger(false)
        static value nil()                  { return {value_t::nil, 0.0, false, 0, nullptr, nullptr}; }
        static value string(std::string s)  { return {value_t::string, 0.0, false, 0, std::make_shared<std::string>(std::move(s)), nullptr}; }
        static value handle(intptr_t h)     { return {value_t::handle, 0.0, false, h, nullptr, nullptr}; }
        static value function_ref(std::string name) { return {value_t::function, 0.0, false, 0, std::make_shared<std::string>(std::move(name)), nullptr}; }
        // Local function — carries AST directly; does not live in function_defs_.
        static value local_function(std::string name, std::shared_ptr<parser::function_definition> def) {
            value v;
            v.type_ = value_t::function;
            v.string_ = std::make_shared<std::string>(std::move(name));
            v.fn_def_ = std::move(def);
            return v;
        }
        // Closure — local function that also captures a temporal instance for
        // by-reference access to its params and state at call time.
        static value closure(std::string name,
                             std::shared_ptr<parser::function_definition> def,
                             std::shared_ptr<function_instance> inst) {
            value v = local_function(std::move(name), std::move(def));
            v.closure_inst_ = std::move(inst);
            return v;
        }

        // Wraps a live temporal instance.  Reading this value always returns
        // the instance's current output via read_output().
        static value instance_ref(std::shared_ptr<function_instance> inst) {
            value v;
            v.type_ = value_t::instance_ref;
            v.instance_ = std::move(inst);
            return v;
        }

        // Accessor for function-ref values
        const std::string& fn_name() const { static const std::string empty; return string_ ? *string_ : empty; }

        // ── Numeric coercion (for operators) ───────────────────────────────────
        // trigger → 1.0 if fired, 0.0 otherwise
        // time → its ms value
        // instance_ref → delegates to instance's current output
        // nil → 0.0
        // Defined out-of-line (after function_instance) because instance_ref
        // cases call read_output() which requires the complete type.
        double as_number() const;
        bool is_truthy() const;
        std::string as_string() const;

        intptr_t as_handle() const {
            return (type_ == value_t::handle) ? handle_ : static_cast<intptr_t>(number_);
        }

    };

    // ── Flow data ──────────────────────────────────────────────────────────────
    struct flow_member {
        std::string name_;      // empty for single-member flows
        std::string gate_name_; // non-empty → only advance when this member's trigger is live
        std::vector<value> elements_;

        // Compound-temporal slots (same size as elements_; entries may be null).
        // live_exprs_[i] != null  → slot i must be re-evaluated at access time
        //                           (element is an expression that involves a
        //                            temporal, e.g. sine(2hz) * 127).
        // live_deps_[i]  != null  → the temporal instance depended on by live_exprs_[i];
        //                           set as retick_instance_ during re-evaluation so
        //                           instantiate_* returns its current output instead
        //                           of creating a new instance.
        std::vector<std::shared_ptr<parser::expression>> live_exprs_;
        std::vector<std::shared_ptr<function_instance>>  live_deps_;
    };

    struct flow_data {
        std::vector<flow_member> members_;

        // Per-member cursors for trigger-driven sequential access.
        // Keyed by member name ("" for unnamed single-member flows).
        // Shared between parent and member-extracted views so that
        // advancing in one advances the other.
        std::shared_ptr<std::unordered_map<std::string, int>> cursors_ =
            std::make_shared<std::unordered_map<std::string, int>>();

        size_t length() const {
            if (members_.empty()) return 0;
            return members_[0].elements_.size();
        }
    };

    // Library-local scope type: maps original (unqualified) function names to
    // function_ref values pointing at the ns-qualified version.  Pushed into env
    // while a namespaced library function's body or tick executes so that
    // internal cross-calls (e.g. lfo calling sine_shape) resolve correctly
    // without leaking bare names into global scope.
    using lib_scope_t   = std::unordered_map<std::string, value>;
    using lib_scope_ptr = std::shared_ptr<lib_scope_t>;

    // ── Native temporal callback types ─────────────────────────────────────────
    // Used by both module::function_entry and function_instance to support
    // native (C++) temporal functions with state, update, and emit.
    //
    // native_state_t: key/value map for params, state, and emitted variables.
    // native_init_fn: called once at instantiation; populates initial state.
    // native_update_fn: called every tick; reads params + state, writes next
    //                   state (in-place), emitted vars, and the output value.
    using native_state_t = std::unordered_map<std::string, value>;
    using native_init_fn = std::function<void(
        const native_state_t& params,
        native_state_t& state)>;
    using native_update_fn = std::function<void(
        const native_state_t& params,
        native_state_t& state,
        native_state_t& emitted,
        value& output)>;

    // ── Function instance (temporal) ───────────────────────────────────────────
    // Each call site that invokes a temporal function creates one of these.
    // Owns mutable state (init block vars), bound parameters, and current output.
    // The scheduler ticks this instance at the function's `dt` interval.
    struct function_instance {
        uint64_t id_ = 0;
        // The key under which this function lives in function_defs_.
        // For namespaced library functions this is the qualified name (e.g.
        // "std::sine"), which is what the process-block subscription lookup uses.
        std::string def_name_;

        // For local functions (defined inside process/lambda scope): the AST
        // pointer carried directly, bypassing function_defs_.
        std::shared_ptr<parser::function_definition> local_def_;

        // Library-local scope for namespaced library functions.  Pushed into
        // the environment before each tick / body evaluation so internal
        // cross-calls resolve without polluting global scope.
        lib_scope_ptr library_scope_;

        // Instance-local mutable state (init vars + update vars)
        // Reads come from current_, writes go to next_, then swap.
        std::unordered_map<std::string, value> current_;
        std::unordered_map<std::string, value> next_;

        // Bound parameters (snapshot at instantiation time; updated each tick for dynamic params)
        std::unordered_map<std::string, value> params_;

        // Emitted variables (set by `emit x = ...` in lambda blocks)
        // Updated each tick; only variables marked with emit are stored here.
        std::unordered_map<std::string, value> emitted_;

        // Current output value (recomputed each tick)
        value output_;

        // Native temporal update callback.  Non-null for module/builtin-backed
        // temporal instances.  When set, tick_instance calls this instead of
        // evaluating AST update statements.
        native_update_fn native_update_;

        // dt in ms (0 → trigger-only, no clock subscription)
        double dt_ms_ = 0.0;

        // Elapsed time accumulator: 0 on tick 0, incremented by dt_ms_ after each tick
        double age_ms_ = 0.0;

        // Scheduler subscription handle (0 → not subscribed)
        uint64_t subscription_id_ = 0;

        bool active_ = true;

        // Thread-safe access to output (scheduler thread writes, main reads)
        mutable std::mutex output_mutex_;

        value read_output() const {
            std::lock_guard<std::mutex> lock(output_mutex_);
            value out = output_;
            return out;
        }

        void write_output(value v) {
            std::lock_guard<std::mutex> lock(output_mutex_);
            output_ = std::move(v);
        }

        // Swap double-buffer: next_ becomes current_, next_ is cleared
        void commit() {
            for (auto& [k, v] : next_) {
                current_[k] = std::move(v);
            }
            next_.clear();
        }
    };

    // ── value coercion methods — defined here so function_instance is complete ──
    // (as_number / is_truthy / as_string are declared in value but bodies moved
    //  here because the instance_ref cases call read_output() on function_instance.)

    inline double value::as_number() const {
        switch (type_) {
            case value_t::number:       return number_;
            case value_t::time:         return number_;
            case value_t::trigger:      return trigger_ ? 1.0 : 0.0;
            case value_t::handle:       return static_cast<double>(handle_);
            case value_t::instance_ref: return instance_ ? instance_->read_output().as_number() : 0.0;
            default:                    return 0.0;
        }
    }

    inline bool value::is_truthy() const {
        switch (type_) {
            case value_t::number:       return number_ != 0.0;
            case value_t::time:         return number_ != 0.0;
            case value_t::trigger:      return trigger_;
            case value_t::string:       return string_ && !string_->empty();
            case value_t::handle:       return handle_ != 0;
            case value_t::function:     return string_ && !string_->empty();
            case value_t::instance_ref: return instance_ ? instance_->read_output().is_truthy() : false;
            case value_t::nil:          return false;
            default:                    return true;
        }
    }

    inline std::string value::as_string() const {
        switch (type_) {
            case value_t::string:  return string_ ? *string_ : "";
            case value_t::number: {
                auto s = std::to_string(number_);
                auto dot = s.find('.');
                if (dot != std::string::npos) {
                    auto last = s.find_last_not_of('0');
                    if (last == dot) s.erase(dot);
                    else s.erase(last + 1);
                }
                return s;
            }
            case value_t::time:         return std::to_string(number_) + "ms";
            case value_t::trigger:      return trigger_ ? "trigger" : "rest";
            case value_t::instance_ref: return instance_ ? instance_->read_output().as_string() : "";
            case value_t::handle: {
                char buf[32];
                std::snprintf(buf, sizeof(buf), "handle@0x%lx",
                              static_cast<unsigned long>(handle_));
                return std::string(buf);
            }
            case value_t::function:
                return string_ ? ("<fn:" + *string_ + ">") : "<fn>";
            default: return "";
        }
    }

    // ── Clock node ─────────────────────────────────────────────────────────────
    // A clock maintains a BPM rate and an optional parent relationship.
    // When a parent's BPM changes, all children scale proportionally
    // preserving the ratio established at bind time.
    struct clock_node {
        uint64_t    id_        = 0;
        double      bpm_       = 120.0;
        uint64_t    parent_id_ = 0;      // 0 = no parent (free-running)
        double      ratio_     = 1.0;    // this.bpm / parent.bpm at bind time
        std::vector<uint64_t> children_;
    };

    // ── Clock registry ─────────────────────────────────────────────────────────
    // Manages the global clock hierarchy.  A default "main" clock is created
    // at startup.  All new clocks bind to main unless parent= is specified.
    struct clock_registry {
        std::unordered_map<uint64_t, clock_node> clocks_;
        uint64_t next_id_ = 1;
        uint64_t main_id_ = 0;

        // Initialise with a main clock at the given BPM (default 120)
        void init(double bpm = 120.0) {
            clock_node main_clk;
            main_clk.id_  = next_id_++;
            main_clk.bpm_ = bpm;
            main_clk.parent_id_ = 0;  // no parent
            main_clk.ratio_      = 1.0;
            main_id_ = main_clk.id_;
            clocks_[main_clk.id_] = std::move(main_clk);
        }

        // Create a new clock.  parent_id=0 means free-running.
        // When parent_id > 0 the ratio child/parent is computed and stored.
        uint64_t create(double bpm, uint64_t parent_id) {
            clock_node clk;
            clk.id_  = next_id_++;
            clk.bpm_ = bpm;
            clk.parent_id_ = parent_id;
            if (parent_id != 0) {
                auto pit = clocks_.find(parent_id);
                if (pit != clocks_.end() && pit->second.bpm_ > 0.0) {
                    clk.ratio_ = bpm / pit->second.bpm_;
                    pit->second.children_.push_back(clk.id_);
                }
            }
            uint64_t id = clk.id_;
            clocks_[id] = std::move(clk);
            return id;
        }

        // Set BPM on a clock and recursively propagate to children.
        void set_bpm(uint64_t id, double new_bpm) {
            auto it = clocks_.find(id);
            if (it == clocks_.end()) return;
            it->second.bpm_ = new_bpm;
            for (uint64_t child_id : it->second.children_) {
                auto cit = clocks_.find(child_id);
                if (cit != clocks_.end()) {
                    set_bpm(child_id, new_bpm * cit->second.ratio_);
                }
            }
        }

        // Query BPM of a clock.
        double bpm(uint64_t id) const {
            auto it = clocks_.find(id);
            return (it != clocks_.end()) ? it->second.bpm_ : 120.0;
        }

        // Main clock BPM shorthand.
        double main_bpm() const { return bpm(main_id_); }
    };

} // --- idyl::core ---
