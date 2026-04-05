#pragma once 

#include <variant>
#include <string> 
#include <vector>
#include <memory>
#include <cmath>
#include <unordered_map>
#include <mutex>

namespace idyl::core {

    // ── Value tag ──────────────────────────────────────────────────────────────
    // Mirrors semantic::inferred_t but lives in core to avoid circular deps.
    // The evaluator uses this exclusively at runtime.
    enum class value_t {
        number, 
        time, 
        trigger,
        flow, 
        function, 
        module, 
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

        // Complex types (heap-allocated, ref-counted)
        std::shared_ptr<flow_data> flow_;
        // function_ and module_ will be added in later phases

        // ── Constructors ───────────────────────────────────────────────────────
        static value number(double v)       { return {value_t::number, v, false, nullptr}; }
        static value time_ms(double ms)     { return {value_t::time, ms, false, nullptr}; }
        static value trigger(bool fired)    { return {value_t::trigger, fired ? 1.0 : 0.0, fired, nullptr}; }
        static value rest()                 { return {value_t::trigger, 0.0, false, nullptr}; } // rest is trigger(false)
        static value nil()                  { return {value_t::nil, 0.0, false, nullptr}; }

        // ── Numeric coercion (for operators) ───────────────────────────────────
        // trigger → 1.0 if fired, 0.0 otherwise
        // time → its ms value
        // nil → 0.0
        double as_number() const {
            switch (type_) {
                case value_t::number:  return number_;
                case value_t::time:    return number_;
                case value_t::trigger: return trigger_ ? 1.0 : 0.0;
                default:               return 0.0;
            }
        }

        bool is_truthy() const {
            switch (type_) {
                case value_t::number:  return number_ != 0.0;
                case value_t::time:    return number_ != 0.0;
                case value_t::trigger: return trigger_;
                case value_t::nil:     return false;
                default:               return true;
            }
        }
    };

    // ── Flow data ──────────────────────────────────────────────────────────────
    struct flow_member {
        std::string name_;  // empty for single-member flows
        std::vector<value> elements_;
    };

    struct flow_data {
        std::vector<flow_member> members_;

        size_t length() const {
            if (members_.empty()) return 0;
            return members_[0].elements_.size();
        }
    };

    // ── Function instance (temporal) ───────────────────────────────────────────
    // Each call site that invokes a temporal function creates one of these.
    // Owns mutable state (init block vars), bound parameters, and current output.
    // The scheduler ticks this instance at the function's `dt` interval.
    struct function_instance {
        uint64_t id_ = 0;
        std::string def_name_;   // function definition name (for tick lookup)

        // Instance-local mutable state (init vars + update vars)
        // Reads come from current_, writes go to next_, then swap.
        std::unordered_map<std::string, value> current_;
        std::unordered_map<std::string, value> next_;

        // Bound parameters (immutable after creation)
        std::unordered_map<std::string, value> params_;

        // Current output value (recomputed each tick)
        value output_;

        // dt in ms (0 → trigger-only, no clock subscription)
        double dt_ms_ = 0.0;

        // Scheduler subscription handle (0 → not subscribed)
        uint64_t subscription_id_ = 0;

        bool active_ = true;

        // Thread-safe access to output (scheduler thread writes, main reads)
        mutable std::mutex output_mutex_;

        value read_output() const {
            std::lock_guard<std::mutex> lock(output_mutex_);
            return output_;
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

} // --- idyl::core ---
