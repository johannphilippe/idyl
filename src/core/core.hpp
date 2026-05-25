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

    // ── Function payload (heap box for function values) ────────────────────────
    // Holds the data that a function-type value needs beyond the scalar fields.
    // All three fields are null for a plain global function_ref (only name_ is set);
    // fn_def_ is set for local functions; closure_inst_ additionally for closures.
    struct function_payload {
        std::string name_;
        std::shared_ptr<parser::function_definition> fn_def_;
        std::shared_ptr<function_instance>           closure_inst_;
    };

    // ── Runtime value ──────────────────────────────────────────────────────────
    // Scalar types (number/time/trigger/handle/nil) carry no heap allocation.
    // Complex types box their payload into a single shared_ptr<void>:
    //   string       → std::string
    //   flow         → flow_data
    //   function     → function_payload
    //   instance_ref → function_instance
    //
    // sizeof(value) == 48 bytes (down from 112 with five separate shared_ptrs).
    struct value
    {
        value_t  type_    = value_t::nil;
        double   number_  = 0.0;   // number, time (ms)
        bool     trigger_ = false; // trigger
        intptr_t handle_  = 0;     // handle (opaque)

        std::shared_ptr<void> payload_; // null for scalar types

        // ── Typed payload accessors ────────────────────────────────────────────
        // Caller must have already verified type_ matches before calling these.
        std::string&      str() const { return *static_cast<std::string*>(payload_.get()); }
        function_payload& fn()  const { return *static_cast<function_payload*>(payload_.get()); }

        std::shared_ptr<std::string>      string_ptr() const { return std::static_pointer_cast<std::string>(payload_); }
        std::shared_ptr<function_payload> fn_ptr()     const { return std::static_pointer_cast<function_payload>(payload_); }

        // fn_def and closure_inst live inside the function_payload box:
        std::shared_ptr<parser::function_definition> fn_def() const {
            return (type_ == value_t::function && payload_) ? fn().fn_def_ : nullptr;
        }
        std::shared_ptr<function_instance> closure_inst() const {
            return (type_ == value_t::function && payload_) ? fn().closure_inst_ : nullptr;
        }

        // flow() and inst() are defined after flow_data / function_instance are complete
        flow_data&         flow() const;
        function_instance& inst() const;
        std::shared_ptr<flow_data>         flow_ptr() const;
        std::shared_ptr<function_instance> inst_ptr() const;

        // Accessor for function-ref values
        const std::string& fn_name() const {
            static const std::string empty;
            return (type_ == value_t::function && payload_) ? fn().name_ : empty;
        }

        // ── Static constructors ────────────────────────────────────────────────
        static value number(double v)    { return {value_t::number,  v,             false, 0, nullptr}; }
        static value time_ms(double ms)  { return {value_t::time,    ms,            false, 0, nullptr}; }
        static value trigger(bool fired) { return {value_t::trigger, fired?1.0:0.0, fired, 0, nullptr}; }
        static value rest()              { return {value_t::trigger, 0.0,           false, 0, nullptr}; }
        static value nil()               { return {value_t::nil,     0.0,           false, 0, nullptr}; }
        static value handle(intptr_t h)  { return {value_t::handle,  0.0,           false, h, nullptr}; }

        static value string(std::string s) {
            return {value_t::string, 0.0, false, 0,
                    std::make_shared<std::string>(std::move(s))};
        }
        static value from_flow(std::shared_ptr<flow_data> fd) {
            return {value_t::flow, 0.0, false, 0, std::move(fd)};
        }
        static value function_ref(std::string name) {
            auto fp = std::make_shared<function_payload>();
            fp->name_ = std::move(name);
            return {value_t::function, 0.0, false, 0, std::move(fp)};
        }
        static value local_function(std::string name,
                                    std::shared_ptr<parser::function_definition> def) {
            auto fp = std::make_shared<function_payload>();
            fp->name_ = std::move(name);
            fp->fn_def_ = std::move(def);
            return {value_t::function, 0.0, false, 0, std::move(fp)};
        }
        static value closure(std::string name,
                             std::shared_ptr<parser::function_definition> def,
                             std::shared_ptr<function_instance> inst) {
            auto fp = std::make_shared<function_payload>();
            fp->name_ = std::move(name);
            fp->fn_def_ = std::move(def);
            fp->closure_inst_ = std::move(inst);
            return {value_t::function, 0.0, false, 0, std::move(fp)};
        }
        static value instance_ref(std::shared_ptr<function_instance> inst) {
            return {value_t::instance_ref, 0.0, false, 0, std::move(inst)};
        }

        // ── Numeric coercion (for operators) ───────────────────────────────────
        // Defined out-of-line (after function_instance) because instance_ref
        // cases call read_output() which requires the complete type.
        double as_number() const;
        bool is_truthy() const;
        std::string as_string() const;

        intptr_t as_handle() const {
            return (type_ == value_t::handle) ? handle_ : static_cast<intptr_t>(number_);
        }
    };

    static_assert(sizeof(value) == 48, "value struct size changed unexpectedly");

    // ── Flow data ──────────────────────────────────────────────────────────────
    struct flow_member {
        std::string name_;      // empty for single-member flows
        std::string gate_name_; // non-empty → only advance when this member's trigger is live
        std::vector<value> elements_;      // unique elements (logical)
        std::vector<int>   repeat_counts_; // parallel to elements_; 1 = no repeat

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

        // Physical length = sum of repeat_counts_ (equals elements_.size() when no repeats).
        int physical_length() const {
            int total = 0;
            for (int rc : repeat_counts_) total += rc;
            return total > 0 ? total : static_cast<int>(elements_.size());
        }

        // Map a physical cursor position to a logical (unique) element index.
        int physical_to_logical(int phys) const {
            if (repeat_counts_.empty())
                return elements_.empty() ? 0 : phys % static_cast<int>(elements_.size());
            int remaining = phys;
            for (int L = 0; L < static_cast<int>(elements_.size()); ++L) {
                int rc = (L < static_cast<int>(repeat_counts_.size())) ? repeat_counts_[L] : 1;
                if (remaining < rc) return L;
                remaining -= rc;
            }
            return static_cast<int>(elements_.size()) - 1;
        }
    };

    struct flow_data {
        std::vector<flow_member> members_;

        // Per-member cursors for trigger-driven sequential access.
        // Keyed by member name ("" for unnamed single-member flows).
        // Shared between parent and member-extracted views so that
        // advancing in one advances the other.
        std::shared_ptr<std::unordered_map<std::string, int>> cursors_ =
            std::make_shared<std::unordered_map<std::string, int>>();

        // Physical length: total number of time-steps in the first member,
        // counting repeat_count for each unique element.
        size_t length() const {
            if (members_.empty()) return 0;
            return static_cast<size_t>(members_[0].physical_length());
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

        // Phase-locked first fire time (ms since scheduler origin).
        // 0 = fire at now + dt (default).  Set by sync() to snap to the next
        // grid boundary of the requested period.
        double first_fire_ms_ = 0.0;

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

    // ── value payload accessors — defined here so flow_data/function_instance are complete ──

    inline flow_data&         value::flow()     const { return *static_cast<flow_data*>(payload_.get()); }
    inline function_instance& value::inst()     const { return *static_cast<function_instance*>(payload_.get()); }
    inline std::shared_ptr<flow_data>
                              value::flow_ptr() const { return std::static_pointer_cast<flow_data>(payload_); }
    inline std::shared_ptr<function_instance>
                              value::inst_ptr() const { return std::static_pointer_cast<function_instance>(payload_); }

    // ── value coercion methods — defined here so function_instance is complete ──

    inline double value::as_number() const {
        switch (type_) {
            case value_t::number:       return number_;
            case value_t::time:         return number_;
            case value_t::trigger:      return trigger_ ? 1.0 : 0.0;
            case value_t::handle:       return static_cast<double>(handle_);
            case value_t::instance_ref: return payload_ ? inst().read_output().as_number() : 0.0;
            default:                    return 0.0;
        }
    }

    inline bool value::is_truthy() const {
        switch (type_) {
            case value_t::number:       return number_ != 0.0;
            case value_t::time:         return number_ != 0.0;
            case value_t::trigger:      return trigger_;
            case value_t::string:       return payload_ && !str().empty();
            case value_t::handle:       return handle_ != 0;
            case value_t::function:     return payload_ && !fn().name_.empty();
            case value_t::instance_ref: return payload_ && inst().read_output().is_truthy();
            case value_t::nil:          return false;
            default:                    return true;
        }
    }

    inline std::string value::as_string() const {
        switch (type_) {
            case value_t::string:  return payload_ ? str() : "";
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
            case value_t::instance_ref: return payload_ ? inst().read_output().as_string() : "";
            case value_t::handle: {
                char buf[32];
                std::snprintf(buf, sizeof(buf), "handle@0x%lx",
                              static_cast<unsigned long>(handle_));
                return std::string(buf);
            }
            case value_t::function:
                return payload_ ? ("<fn:" + fn().name_ + ">") : "<fn>";
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
