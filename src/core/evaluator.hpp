#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <memory>

#include "parser/ast.hpp"
#include "core/core.hpp"
#include "core/environment.hpp"
#include "time/scheduler.hpp"

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

        // ── Function / builtin calls ───────────────────────────────────────────
        value eval_call(const parser::function_call& call);
        value eval_builtin(const builtin& fn, const std::vector<value>& args);
        value eval_user_function(const parser::function_definition& def,
                                 const std::vector<value>& args);

        // ── Temporal function instantiation ────────────────────────────────────
        value instantiate_temporal(const parser::function_definition& def,
                                   const std::vector<value>& args);
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
