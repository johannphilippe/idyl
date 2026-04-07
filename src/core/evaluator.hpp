#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <set>

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

        // ── Temporal function instances ────────────────────────────────────────
        // Keyed by instance id.  Each temporal call site creates one.
        std::unordered_map<uint64_t, std::shared_ptr<function_instance>> instances_;
        uint64_t next_instance_id_ = 1;

        // Scheduler (owned externally, nullable for Phase 1 compatibility)
        time::sys_clock_scheduler* scheduler_ = nullptr;

        // Loaded library paths (canonical) — prevents duplicate loading
        std::set<std::string> loaded_libraries_;

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

        // ── Clock hierarchy ────────────────────────────────────────────────────
        // Global clock tree.  A default "main" clock (120 BPM) is created in
        // run().  `clock()` creates child clocks; `tempo()` sets BPM with
        // propagation.  All clocks bind to main unless parent= overrides.
        clock_registry clocks_;

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

        // ── Operator dispatch ──────────────────────────────────────────────────
        value apply_binop(const std::string& op, const value& lhs, const value& rhs,
                          int line = 0, int col = 0);
        value apply_unop(const std::string& op, const value& operand);

        // Named argument map: parameter name → evaluated value
        using named_args_t = std::unordered_map<std::string, value>;

        // ── Function / builtin calls ───────────────────────────────────────────
        value eval_call(const parser::function_call& call);
        value eval_builtin(const builtin& fn, const std::vector<value>& args);
        value eval_module_fn(const module::function_entry& fn,
                             const std::vector<value>& args,
                             int line = 0, int col = 0);
        value eval_user_function(const parser::function_definition& def,
                                 const std::vector<value>& args,
                                 const named_args_t& named = {});

        // ── Temporal function instantiation ────────────────────────────────────
        value instantiate_temporal(const parser::function_definition& def,
                                   const std::vector<value>& args,
                                   const named_args_t& named = {});
        void tick_instance(function_instance& inst,
                           const parser::function_definition& def);

        // ── Literals ───────────────────────────────────────────────────────────
        value eval_literal(const parser::literal_expr& lit);
        double parse_time_to_ms(const std::string& val, const std::string& unit);

        // ── Flows ──────────────────────────────────────────────────────────────
        value eval_flow_literal(const parser::flow_literal& fl);
        value eval_generator(const parser::generator_expr& gen);

        // ── Warnings ───────────────────────────────────────────────────────────
        void warn(int line, int col, const std::string& msg);
        void print_warnings() const;
    };

} // --- idyl::core ---
