#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <list>
#include <set>
#include <unordered_map>

#include "parser/ast.hpp"
#include "core/core.hpp"
#include "core/environment.hpp"
#include "time/scheduler.hpp"
#include "include/module.hpp"

namespace idyl::core {

    // ── Evaluator ──────────────────────────────────────────────────────────────
    // Tree-walking interpreter with temporal function support.
    // Pure functions are evaluated inline; temporal functions (with |> blocks)
    // are instantiated and subscribed to the scheduler for periodic updates.
    struct evaluator {
        environment env_;

        // AST definitions kept for call-time evaluation of user functions
        std::unordered_map<std::string,
            std::shared_ptr<parser::function_definition>> function_defs_;

        // AST definitions for parametric flow definitions (flow with parameters).
        // Zero-parameter flows are evaluated eagerly at definition time and stored
        // directly in the environment as flow values.  Parametric flows are stored
        // here and evaluated on each call — once per unique argument set (static),
        // or on each tick of the driving temporal source (dynamic, phase 2).
        std::unordered_map<std::string,
            std::shared_ptr<parser::flow_definition>> flow_defs_;

        // Cache for parametric flow call results.
        //
        // Key: (flow definition name, vector of argument values).
        // Value: the built flow_data wrapped in a value.
        //
        // Returning the same flow_data object for identical arguments preserves
        // cursor state across reactive-chain re-executions — the flow advances
        // through its elements on each tick rather than resetting each time.
        //
        // When arguments change (dynamic case), the key changes and a fresh
        // flow_data is built automatically.  The old entry stays in the cache
        // so that if the caller cycles back to a previous argument set, the
        // cursor position is also preserved from that prior run.
        //
        // Size is capped at FLOW_CACHE_MAX entries; when full, the oldest
        // insertion is evicted.  This prevents unbounded growth when a
        // continuously-varying value is used as a flow argument.
        static constexpr std::size_t FLOW_CACHE_MAX = 64;

        struct flow_cache_key {
            std::string name;
            std::vector<value> args;
            std::unordered_map<std::string, value> named;

            bool operator==(const flow_cache_key& o) const;
        };

        struct flow_cache_key_hash {
            std::size_t operator()(const flow_cache_key& k) const;
        };

        // Ordered insertion list for LRU eviction (front = oldest).
        std::list<flow_cache_key> flow_cache_order_;
        std::unordered_map<flow_cache_key, value,
                           flow_cache_key_hash> flow_call_cache_;

        // ── Temporal function instances ────────────────────────────────────────
        // Keyed by instance id.  Each temporal call site creates one.
        std::unordered_map<uint64_t, std::shared_ptr<function_instance>> instances_;
        uint64_t next_instance_id_ = 1;

        // Scheduler (owned externally, nullable for Phase 1 compatibility)
        time::sys_clock_scheduler* scheduler_ = nullptr;

        // Loaded library paths (canonical) — prevents duplicate loading
        std::set<std::string> loaded_libraries_;

        // Library-local scope map: qualified function name → lib_scope_ptr.
        // Populated when a namespaced library is loaded.  Looked up in eval_call
        // and pushed around function body / tick evaluation.
        std::unordered_map<std::string, lib_scope_ptr> fn_library_scope_;

        // ── Process tracking ───────────────────────────────────────────────────
        // Number of process blocks encountered during evaluation. Controls
        // keep-alive behavior: 1 process = unconditional keep-alive.
        int process_count_ = 0;

        // ── Process filter ──────────────────────────────────────────────────────
        // When non-empty, only process blocks whose name matches are executed.
        // Empty string = run all process blocks (default).
        std::string process_filter_;

        // ── Listen mode ─────────────────────────────────────────────────────────
        // When true, process blocks are stored rather than executed (unless
        // they match process_filter_).  OSC messages start/stop them later.
        bool listen_mode_ = false;

        // Stored process block AST nodes keyed by name (listen mode).
        std::unordered_map<std::string,
            std::shared_ptr<parser::process_block>> stored_processes_;

        // Active process subscriptions: process name → instance IDs created
        // when that process was started.  Used by stop_process().
        std::unordered_map<std::string, std::vector<uint64_t>>
            active_process_instances_;

        // ── Process start / stop (listen mode) ─────────────────────────────────
        bool start_process(const std::string& name);
        bool stop_process(const std::string& name);
        std::vector<std::string> list_stored_processes() const;

        // ── Instance bindings ───────────────────────────────────────────────────
        // Maps variable names to temporal instance IDs.  Populated when a
        // process block creates temporal segments.  Used by the :: accessor
        // to resolve emitted values: `a::incr` looks up the instance bound
        // to variable "a" and reads its emitted_ map for "incr".
        std::unordered_map<std::string, uint64_t> instance_bindings_;

        // ── Dynamic parameter expressions ────────────────────────────────────
        // For each temporal instance: maps param name → the AST expression that
        // was passed as that argument at call time.  Re-evaluated on every tick
        // so that temporal values used as parameters automatically propagate.
        // E.g.: lfo(1hz, amplitude=amp) where amp is itself a temporal instance.
        std::unordered_map<uint64_t,
            std::unordered_map<std::string, parser::expr_ptr>> instance_param_exprs_;

        // ── Clock hierarchy ────────────────────────────────────────────────────
        // Global clock tree.  A default "main" clock (120 BPM) is created in
        // run().  `clock()` creates child clocks; `tempo()` sets BPM with
        // propagation.  All clocks bind to main unless parent= overrides.
        clock_registry clocks_;
        
        // ── Delay memories  ────────────────────────────────────────────────────
        // For memory ops (e.g. delay, feedback), maps expression pointer to a
        // vector of past values (the "memory").  The expression pointer is the
        // key because the memory is tied to that specific AST node (i.e. the
        // delay operator in the source code), not to a function instance or
        // temporal instance.  This allows multiple delay instances to coexist
        // without interference, and for the memory to persist across function calls
        // without being tied to a specific temporal instance.
        struct memory_buffer 
        {
            memory_buffer(size_t size = 0) : buffer(size), write_index(0) {}
            std::vector<value> buffer; // the actual memory buffer
            size_t write_index = 0;           // current write index for circular buffer behavior
        };
        std::unordered_map<const parser::node*, memory_buffer> delay_memories_;

        // Convenience: current main clock BPM.
        double main_clock_bpm() const { return clocks_.main_bpm(); }

        // ── Retick context ─────────────────────────────────────────────────────
        // During a scheduler retick callback, this points to the instance being
        // reticked.  When set, instantiate_temporal() returns the existing
        // instance's output instead of creating a new instance.  This allows
        // the full enclosing expression (e.g. lfo(1hz) + 1 * 2000 + 20) to be
        // re-evaluated with the updated temporal value.
        function_instance* retick_instance_ = nullptr;

        // Runtime warnings (non-fatal)
        struct runtime_warning {
            int line_ = 0;
            int column_ = 0;
            std::string message_;
        };
        std::vector<runtime_warning> warnings_;

        // ── Public API ─────────────────────────────────────────────────────────
        void run(const parser::program& program);

        // ── Expression evaluation ──────────────────────────────────────────────
        value eval_expr(const parser::expr_ptr& expr);

        // ── Statement execution ────────────────────────────────────────────────
        void exec_stmt(const parser::stmt_ptr& stmt);

        // ── Memory operation execution ─────────────────────────────────────────
        value eval_memory_op(const parser::memory_op_expr* moe);

        // ── Operator dispatch ──────────────────────────────────────────────────
        value apply_binop(const std::string& op, const value& lhs, const value& rhs,
                          int line = 0, int col = 0);
        value apply_unop(const std::string& op, const value& operand);

        // Named argument map: parameter name → evaluated value
        using named_args_t  = std::unordered_map<std::string, value>;
        // Named expression map: parameter name → AST expression (for dynamic params)
        using named_exprs_t = std::unordered_map<std::string, parser::expr_ptr>;

        // ── Function / builtin calls ───────────────────────────────────────────
        value eval_call(const parser::function_call& call);
        value eval_builtin(const builtin& fn, const std::vector<value>& args);
        value eval_module_fn(const module::function_entry& fn,
                             const std::vector<value>& args,
                             int line = 0, int col = 0);
        // qualified_key: the key under which the function is stored in
        // function_defs_ (may differ from def.name_ for namespaced library fns).
        // pos_exprs / named_exprs: raw AST expressions parallel to args/named,
        // stored on the instance for dynamic re-evaluation each tick.
        value eval_user_function(const parser::function_definition& def,
                                 const std::vector<value>& args,
                                 const named_args_t& named = {},
                                 const std::string& qualified_key = {},
                                 const std::vector<parser::expr_ptr>& pos_exprs = {},
                                 const named_exprs_t& named_exprs = {});

        // ── Temporal function instantiation ────────────────────────────────────
        value instantiate_temporal(const parser::function_definition& def,
                                   const std::vector<value>& args,
                                   const named_args_t& named = {},
                                   const std::string& qualified_key = {},
                                   const std::vector<parser::expr_ptr>& pos_exprs = {},
                                   const named_exprs_t& named_exprs = {});

        // Instantiate a native temporal module function.  Creates a
        // function_instance backed by native_init_ / native_update_ callbacks
        // instead of an AST definition.
        value instantiate_native_temporal(const module::function_entry& entry,
                                          const std::vector<value>& args,
                                          const named_args_t& named = {},
                                          const std::vector<parser::expr_ptr>& pos_exprs = {},
                                          const named_exprs_t& named_exprs = {});

        // def is nullable: nullptr when inst is a native temporal instance.
        void tick_instance(function_instance& inst,
                           const parser::function_definition* def);

        // ── Literals ───────────────────────────────────────────────────────────
        value eval_literal(const parser::literal_expr& lit);
        double parse_time_to_ms(const std::string& val, const std::string& unit);

        // ── Flows ──────────────────────────────────────────────────────────────
        value eval_flow_literal(const parser::flow_literal& fl);
        value eval_generator(const parser::generator_expr& gen);

        // Shared helper: build a flow_data from a list of flow_member nodes.
        // Assumes parameters are already bound in the current scope.
        std::shared_ptr<flow_data> eval_flow_members(
            const std::vector<std::shared_ptr<parser::flow_member>>& members);

        // Build a flow_data from a parametric flow definition with the supplied
        // argument values bound to parameters.  Used by eval_call when a call
        // resolves to a flow_defs_ entry.
        // pos_exprs / named_exprs are kept for phase 2 (dynamic recomputation)
        // but are unused in the static implementation.
        value eval_flow_call(const parser::flow_definition& def,
                             const std::vector<value>& args,
                             const named_args_t& named = {},
                             const std::vector<parser::expr_ptr>& pos_exprs = {},
                             const named_exprs_t& named_exprs = {});

        // ── Warnings ───────────────────────────────────────────────────────────
        void warn(int line, int col, const std::string& msg);
        void print_warnings() const;
    };

} // --- idyl::core ---
