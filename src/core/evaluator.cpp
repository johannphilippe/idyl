#include "core/evaluator.hpp"
#include "utilities/span.hpp"
#include "utilities/safety.hpp"
#include "utilities/filesystem.hpp"
#include "parser/parse.hpp"

#include <cmath>
#include <algorithm>
#include <set>

namespace idyl::core {

// ════════════════════════════════════════════════════════════════════════════════
// Top-level entry: walk program statements
// ════════════════════════════════════════════════════════════════════════════════

void evaluator::run(const parser::program& program) {
    env_.init();

    for (const auto& stmt : program.statements_) {
        if (!stmt) continue;
        exec_stmt(stmt);
    }
}

// ════════════════════════════════════════════════════════════════════════════════
// Statement execution
// ════════════════════════════════════════════════════════════════════════════════

void evaluator::exec_stmt(const parser::stmt_ptr& stmt) {
    if (!stmt) return;

    switch (stmt->type_) {

    // ── Function definition → register in environment ──────────────────────
    case parser::node_t::function_definition: {
        auto& def = static_cast<const parser::function_definition&>(*stmt);

        // 0-param, no lambda block → constant binding (eagerly evaluate)
        // e.g. "z = func(2.2, 5.5)" or "silence = 0"
        if (def.parameters_.empty() && !def.lambda_block_ && def.body_) {
            value result = eval_expr(def.body_);
            env_.define(def.name_, std::move(result));
            break;
        }

        // Otherwise: true function definition — register for call-time evaluation
        value fn_val;
        fn_val.type_ = value_t::function;
        fn_val.number_ = 0.0;
        fn_val.trigger_ = false;
        env_.define(def.name_, std::move(fn_val));
        // Store the AST node pointer for call-time evaluation
        function_defs_[def.name_] =
            std::static_pointer_cast<parser::function_definition>(
                std::const_pointer_cast<parser::node>(
                    std::shared_ptr<const parser::node>(stmt, &def)));
        break;
    }

    // ── Flow definition → evaluate and register ────────────────────────────
    case parser::node_t::flow_definition: {
        auto& def = static_cast<const parser::flow_definition&>(*stmt);
        // Build flow_data from members
        auto fd = std::make_shared<flow_data>();
        for (const auto& member : def.members_) {
            if (!member) continue;
            flow_member fm;
            fm.name_ = member->name_;
            // Evaluate the member's value expression
            if (member->value_) {
                // The member value is wrapped in an expression node
                if (member->value_->type_ == parser::node_t::flow_literal_expr) {
                    auto& fle = static_cast<const parser::flow_literal_expr&>(*member->value_);
                    if (fle.flow_) {
                        for (const auto& elem : fle.flow_->elements_) {
                            fm.elements_.push_back(eval_expr(elem));
                        }
                    }
                } else if (member->value_->type_ == parser::node_t::generator_expr_node) {
                    auto& gn = static_cast<const parser::generator_expr_node&>(*member->value_);
                    if (gn.generator_) {
                        auto& gen = *gn.generator_;
                        value start = eval_expr(gen.range_start_);
                        value end = eval_expr(gen.range_end_);
                        int n = static_cast<int>(end.as_number());
                        int s = static_cast<int>(start.as_number());
                        env_.push_scope();
                        for (int i = s; i < n; ++i) {
                            env_.define(gen.variable_, value::number(static_cast<double>(i)));
                            fm.elements_.push_back(eval_expr(gen.body_));
                        }
                        env_.pop_scope();
                    }
                } else {
                    // Single expression member
                    fm.elements_.push_back(eval_expr(member->value_));
                }
            }
            fd->members_.push_back(std::move(fm));
        }
        value flow_val;
        flow_val.type_ = value_t::flow;
        flow_val.flow_ = std::move(fd);
        env_.define(def.name_, std::move(flow_val));
        break;
    }

    // ── Process block → reactive execution ─────────────────────────────────
    // Each temporal function binding starts a "segment". Statements that
    // follow (print calls, derived assignments, etc.) are that segment's
    // "reactions" and re-run on every tick of the temporal source.
    case parser::node_t::process_block: {
        auto& proc = static_cast<const parser::process_block&>(*stmt);
        if (!proc.body_) break;

        env_.push_scope();

        struct temporal_segment {
            uint64_t    instance_id;
            std::string bound_var;
            parser::stmt_ptr binding_stmt;  // the original binding statement
            std::vector<parser::stmt_ptr> reactions;
        };

        std::vector<temporal_segment> segments;
        temporal_segment* current_seg = nullptr;
        bool has_temporal = false;

        for (const auto& s : proc.body_->statements_) {
            // Extract variable name if this is a binding
            std::string binding_name;
            if (s->type_ == parser::node_t::function_definition)
                binding_name = static_cast<const parser::function_definition&>(*s).name_;
            else if (s->type_ == parser::node_t::assignment)
                binding_name = static_cast<const parser::assignment&>(*s).name_;

            uint64_t id_before = next_instance_id_;
            exec_stmt(s);
            bool created_instance = (next_instance_id_ > id_before);

            if (created_instance) {
                has_temporal = true;
                segments.push_back({next_instance_id_ - 1, binding_name, s, {}});
                current_seg = &segments.back();
            } else if (current_seg) {
                // Statement follows a temporal binding → reaction
                current_seg->reactions.push_back(s);
            }
        }

        // Subscribe each segment's temporal source to the scheduler.
        // On each tick: update instance → update bound variable → re-run reactions.
        if (scheduler_ && has_temporal) {
            for (auto& seg : segments) {
                auto inst_it = instances_.find(seg.instance_id);
                if (inst_it == instances_.end()) continue;
                auto inst = inst_it->second;
                if (inst->dt_ms_ <= 0.0) continue;

                auto def_it = function_defs_.find(inst->def_name_);
                if (def_it == function_defs_.end()) continue;

                auto def_ptr   = def_it->second;
                std::string var = seg.bound_var;
                auto binding    = seg.binding_stmt;  // original binding statement
                auto reactions  = seg.reactions; // copies shared_ptrs
                auto weak_inst  = std::weak_ptr<function_instance>(inst);

                inst->subscription_id_ = scheduler_->subscribe(inst->dt_ms_,
                    [this, def_ptr, weak_inst, var, binding, reactions]
                    (double /*t*/, double /*dt*/) -> bool {
                        auto si = weak_inst.lock();
                        if (!si || !si->active_) return false;

                        tick_instance(*si, *def_ptr);

                        // Re-evaluate the full binding expression so that
                        // arithmetic around the temporal call is preserved.
                        // e.g. mod = lfo(1hz) + 1 * 2000 + 20
                        // Set retick_instance_ so that the lfo(1hz) call
                        // returns the freshly-ticked output instead of
                        // creating a new instance.
                        retick_instance_ = si.get();
                        if (binding)
                            exec_stmt(binding);
                        retick_instance_ = nullptr;

                        for (const auto& r : reactions)
                            exec_stmt(r);

                        return true;
                    });
            }
        }

        // Keep scope alive for tick callbacks if temporal instances exist
        if (!has_temporal)
            env_.pop_scope();
        break;
    }

    // ── Assignment → evaluate RHS, bind in current scope ───────────────────
    case parser::node_t::assignment: {
        auto& assign = static_cast<const parser::assignment&>(*stmt);
        value val = eval_expr(assign.value_);
        env_.define(assign.name_, std::move(val));
        break;
    }

    // ── Expression statement → evaluate (for side effects / output) ────────
    case parser::node_t::expression_stmt: {
        auto& es = static_cast<const parser::expression_stmt&>(*stmt);
        eval_expr(es.expression_);
        break;
    }

    // ── Library imports → parse and execute the library's definitions ───────
    case parser::node_t::library_import: {
        auto& li = static_cast<const parser::library_import&>(*stmt);
        std::string path = li.path_;

        // Resolve the library path (checks cwd, source dir, ~/.idyl/modules/)
        if (!utilities::get_library_path(path)) {
            // Semantic pass already warned about this — skip silently
            break;
        }

        // Canonicalize to avoid duplicate loading
        std::error_code ec;
        auto canonical = std::filesystem::canonical(path, ec);
        if (ec) break;
        std::string key = canonical.string();

        // Skip if already loaded
        if (loaded_libraries_.count(key)) break;
        loaded_libraries_.insert(key);

        // Parse the library file
        auto lib_program = parser::parse_file(key);
        if (!lib_program) break;

        // Execute every statement in the library (registers functions, flows, etc.)
        const std::string& ns = li.namespace_;
        for (const auto& lib_stmt : lib_program->statements_) {
            if (!lib_stmt) continue;

            // If namespaced, we need to prefix function names
            if (!ns.empty() && lib_stmt->type_ == parser::node_t::function_definition) {
                auto def_ptr = std::static_pointer_cast<parser::function_definition>(lib_stmt);
                std::string original_name = def_ptr->name_;
                def_ptr->name_ = ns + "::" + original_name;
                exec_stmt(lib_stmt);
                def_ptr->name_ = original_name; // restore
            } else if (!ns.empty() && lib_stmt->type_ == parser::node_t::flow_definition) {
                auto def_ptr = std::static_pointer_cast<parser::flow_definition>(lib_stmt);
                std::string original_name = def_ptr->name_;
                def_ptr->name_ = ns + "::" + original_name;
                exec_stmt(lib_stmt);
                def_ptr->name_ = original_name; // restore
            } else {
                exec_stmt(lib_stmt);
            }
        }
        break;
    }

    // ── External module imports → dlopen + register at runtime ─────────────
    case parser::node_t::module_import: {
        auto& mi = static_cast<const parser::module_import&>(*stmt);
        std::string path = mi.path_;

        // Resolve module path (expands to ~/.idyl/modules/<name>.so etc.)
        if (!idyl::utilities::get_module_path(path)) {
            warnings_.push_back({mi.line_, mi.column_,
                "Module '" + mi.path_ + "' not found — skipping."});
            break;
        }

        // Already loaded? Skip (idempotent)
        if (env_.module_registry_ && env_.module_registry_->is_loaded(path)) {
            break;
        }

        if (env_.module_registry_) {
            std::string err = env_.module_registry_->load_external(path, mi.namespace_);
            if (!err.empty()) {
                warnings_.push_back({mi.line_, mi.column_, err});
            } else {
                env_.module_registry_->mark_loaded(path);
            }
        }
        break;
    }

    default:
        break;
    }
}

// ════════════════════════════════════════════════════════════════════════════════
// Expression evaluation
// ════════════════════════════════════════════════════════════════════════════════

value evaluator::eval_expr(const parser::expr_ptr& expr) {
    if (!expr) return value::nil();

    switch (expr->type_) {

    // ── Literal wrapper ────────────────────────────────────────────────────
    case parser::node_t::literal_expr: {
        auto& lit = static_cast<const parser::literal_expr&>(*expr);
        return eval_literal(lit);
    }

    // ── Identifier lookup ──────────────────────────────────────────────────
    case parser::node_t::identifier_expr: {
        auto& id_expr = static_cast<const parser::identifier_expr&>(*expr);
        if (!id_expr.identifier_) return value::nil();
        const std::string& name = id_expr.identifier_->name_;

        // Check scope chain
        if (auto* val = env_.lookup(name)) {
            return *val;
        }
        warn(expr->line_, expr->column_, "undefined identifier '" + name + "'");
        return value::number(0.0); // safe default
    }

    // ── Binary operator ────────────────────────────────────────────────────
    case parser::node_t::binary_op_expr: {
        auto& boe = static_cast<const parser::binary_op_expr&>(*expr);
        if (!boe.op_) return value::nil();
        value lhs = eval_expr(boe.op_->left_);
        value rhs = eval_expr(boe.op_->right_);
        return apply_binop(boe.op_->op_, lhs, rhs, expr->line_, expr->column_);
    }

    // ── Unary operator ─────────────────────────────────────────────────────
    case parser::node_t::unary_op_expr: {
        auto& uoe = static_cast<const parser::unary_op_expr&>(*expr);
        if (!uoe.op_) return value::nil();
        value operand = eval_expr(uoe.op_->operand_);
        return apply_unop(uoe.op_->op_, operand);
    }

    // ── Ternary operator ───────────────────────────────────────────────────
    case parser::node_t::ternary_op_expr: {
        auto& toe = static_cast<const parser::ternary_op_expr&>(*expr);
        if (!toe.op_ || toe.op_->options_.empty()) return value::nil();

        value cond = eval_expr(toe.op_->condition_);
        int n = static_cast<int>(toe.op_->options_.size());
        int idx = static_cast<int>(cond.as_number());
        // Wrap to valid range (like Faust selectN)
        idx = ((idx % n) + n) % n;
        return eval_expr(toe.op_->options_[idx]);
    }

    // ── Memory operator (@ delay) → Phase 2 stub ──────────────────────────
    case parser::node_t::memory_op_expr: {
        auto& moe = static_cast<const parser::memory_op_expr&>(*expr);
        if (!moe.op_ || !moe.op_->expr_) return value::number(0.0);
        // In Phase 1 with no state, @ just returns the current value
        return eval_expr(moe.op_->expr_);
    }

    // ── Function call ──────────────────────────────────────────────────────
    case parser::node_t::function_call_expr: {
        auto& fce = static_cast<const parser::function_call_expr&>(*expr);
        if (!fce.call_) return value::nil();
        return eval_call(*fce.call_);
    }

    // ── Parenthesized expression ───────────────────────────────────────────
    case parser::node_t::parenthesized_expr: {
        auto& pe = static_cast<const parser::parenthesized_expr&>(*expr);
        return eval_expr(pe.expr_);
    }

    // ── Flow literal expression ────────────────────────────────────────────
    case parser::node_t::flow_literal_expr: {
        auto& fle = static_cast<const parser::flow_literal_expr&>(*expr);
        if (!fle.flow_) return value::nil();
        return eval_flow_literal(*fle.flow_);
    }

    // ── Generator expression ───────────────────────────────────────────────
    case parser::node_t::generator_expr_node: {
        auto& gen = static_cast<const parser::generator_expr_node&>(*expr);
        if (!gen.generator_) return value::nil();
        return eval_generator(*gen.generator_);
    }

    // ── Flow access (flow.member or flow[index]) ───────────────────────────
    case parser::node_t::flow_access_expr: {
        auto& fae = static_cast<const parser::flow_access_expr&>(*expr);
        if (!fae.access_) return value::nil();
        value flow = eval_expr(fae.access_->flow_);
        if (flow.type_ != value_t::flow || !flow.flow_) {
            warn(expr->line_, expr->column_, "flow access on non-flow value");
            return value::number(0.0);
        }
        // Member access by name
        if (!fae.access_->member_.empty()) {
            for (const auto& m : flow.flow_->members_) {
                if (m.name_ == fae.access_->member_) {
                    // Single element → unwrap to scalar value directly
                    if (m.elements_.size() == 1) {
                        return m.elements_[0];
                    }
                    // Multiple elements → return as a flow sharing the parent cursors
                    auto fd = std::make_shared<flow_data>();
                    fd->members_.push_back(m);
                    fd->cursors_ = flow.flow_->cursors_; // share cursors
                    value result;
                    result.type_ = value_t::flow;
                    result.flow_ = std::move(fd);
                    return result;
                }
            }
            warn(expr->line_, expr->column_,
                 "flow has no member '" + fae.access_->member_ + "'");
            return value::number(0.0);
        }
        // Index access
        if (fae.access_->index_) {
            value idx = eval_expr(fae.access_->index_);

            if (flow.flow_->members_.empty() ||
                flow.flow_->members_[0].elements_.empty()) {
                return value::number(0.0);
            }

            bool is_trigger = (idx.type_ == value_t::trigger);
            bool is_float   = false;
            double v        = 0.0;
            int int_idx     = 0;

            if (!is_trigger) {
                v = idx.as_number();
                // Float mode: only when value has a fractional part.
                // Integer values (0, 1, 2…) always use integer wrapping.
                is_float = (v != std::floor(v));
                if (!is_float) int_idx = static_cast<int>(v);
            }

            auto& cursors = *flow.flow_->cursors_;

            // ── Single-member flow: return element directly ────────────────
            if (flow.flow_->members_.size() == 1) {
                auto& elems = flow.flow_->members_[0].elements_;
                int n = static_cast<int>(elems.size());
                const std::string& key = flow.flow_->members_[0].name_;

                if (is_trigger) {
                    int& cur = cursors[key];
                    if (idx.trigger_) {
                        int pos = cur % n;
                        cur = (cur + 1) % n;
                        return elems[pos];
                    }
                    return elems[cur % n];
                }
                if (is_float) {
                    double t = std::fmod(v, 1.0);
                    if (t < 0.0) t += 1.0;
                    int i = static_cast<int>(std::floor(t * n)) % n;
                    return elems[i];
                }
                // Integer with wrapping
                int i = ((int_idx % n) + n) % n;
                return elems[i];
            }

            // ── Multi-member flow: return a row slice ──────────────────────
            // Each member is indexed independently based on its own size.
            auto fd = std::make_shared<flow_data>();
            for (const auto& m : flow.flow_->members_) {
                flow_member row_m;
                row_m.name_ = m.name_;
                if (m.elements_.empty()) {
                    fd->members_.push_back(std::move(row_m));
                    continue;
                }
                int mn = static_cast<int>(m.elements_.size());
                int mi;

                if (is_trigger) {
                    // Per-member cursor: each member wraps independently
                    int& cur = cursors[m.name_];
                    if (idx.trigger_) {
                        mi = cur % mn;
                        cur = (cur + 1) % mn;
                    } else {
                        mi = cur % mn;
                    }
                } else if (is_float) {
                    // Per-member float mapping: each member uses its own size
                    double t = std::fmod(v, 1.0);
                    if (t < 0.0) t += 1.0;
                    mi = static_cast<int>(std::floor(t * mn)) % mn;
                } else {
                    // Integer with per-member wrapping
                    mi = ((int_idx % mn) + mn) % mn;
                }

                row_m.elements_.push_back(m.elements_[mi]);
                fd->members_.push_back(std::move(row_m));
            }
            value result;
            result.type_ = value_t::flow;
            result.flow_ = std::move(fd);
            return result;
        }
        return flow;
    }

    // ── Module access → Phase 2+ stub ──────────────────────────────────────
    case parser::node_t::module_access_expr: {
        warn(expr->line_, expr->column_, "module calls not yet supported");
        return value::number(0.0);
    }

    default:
        return value::nil();
    }
}

// ════════════════════════════════════════════════════════════════════════════════
// Literal evaluation
// ════════════════════════════════════════════════════════════════════════════════

value evaluator::eval_literal(const parser::literal_expr& lit) {
    if (!lit.literal_) return value::nil();

    switch (lit.literal_->type_) {
    case parser::node_t::number_literal: {
        auto& nl = static_cast<const parser::number_literal&>(*lit.literal_);
        return value::number(std::stod(nl.value_));
    }
    case parser::node_t::time_literal: {
        auto& tl = static_cast<const parser::time_literal&>(*lit.literal_);
        return value::time_ms(parse_time_to_ms(tl.value_, tl.unit_));
    }
    case parser::node_t::trigger_literal:
        return value::trigger(true);

    case parser::node_t::rest_literal:
        return value::rest();

    case parser::node_t::string_literal: {
        auto& sl = static_cast<const parser::string_literal&>(*lit.literal_);
        return value::string(sl.value_);
    }

    default:
        return value::nil();
    }
}

double evaluator::parse_time_to_ms(const std::string& val, const std::string& unit) {
    double v = std::stod(val);
    if (unit == "ms") return v;
    if (unit == "s")  return v * 1000.0;
    if (unit == "hz") return (v > 0.0) ? (1000.0 / v) : 0.0;
    if (unit == "b")  return v * 500.0; // default: 120 BPM → 1 beat = 500ms
    return v;
}

// ════════════════════════════════════════════════════════════════════════════════
// Binary operator dispatch
// ════════════════════════════════════════════════════════════════════════════════

value evaluator::apply_binop(const std::string& op, const value& lhs, const value& rhs,
                             int line, int col) {
    // ── Type-aware arithmetic ──────────────────────────────────────────────
    // time + time → time,  time * number → time,  time / time → number
    // string + string → string (concatenation), string + other → string
    // Everything else operates on as_number().

    // ── String concatenation ───────────────────────────────────────────────
    if (op == "+" && (lhs.type_ == value_t::string || rhs.type_ == value_t::string)) {
        auto to_str = [](const value& v) -> std::string {
            switch (v.type_) {
                case value_t::string:  return v.string_ ? *v.string_ : "";
                case value_t::number:  { auto s = std::to_string(v.number_); 
                                         // strip trailing zeros: 3.000000 → 3
                                         auto dot = s.find('.');
                                         if (dot != std::string::npos) {
                                             auto last = s.find_last_not_of('0');
                                             if (last == dot) s.erase(dot);
                                             else s.erase(last + 1);
                                         }
                                         return s; }
                case value_t::time:    return std::to_string(v.number_) + "ms";
                case value_t::trigger: return v.trigger_ ? "trigger" : "rest";
                default:               return "nil";
            }
        };
        return value::string(to_str(lhs) + to_str(rhs));
    }

    // ── String comparison ──────────────────────────────────────────────────
    if (lhs.type_ == value_t::string && rhs.type_ == value_t::string) {
        std::string a_s = lhs.string_ ? *lhs.string_ : "";
        std::string b_s = rhs.string_ ? *rhs.string_ : "";
        if (op == "==") return value::number(a_s == b_s ? 1.0 : 0.0);
        if (op == "!=") return value::number(a_s != b_s ? 1.0 : 0.0);
        if (op == "<")  return value::number(a_s <  b_s ? 1.0 : 0.0);
        if (op == ">")  return value::number(a_s >  b_s ? 1.0 : 0.0);
        if (op == "<=") return value::number(a_s <= b_s ? 1.0 : 0.0);
        if (op == ">=") return value::number(a_s >= b_s ? 1.0 : 0.0);
    }

    double a = lhs.as_number();
    double b = rhs.as_number();

    // Determine if result should be time
    bool lhs_time = (lhs.type_ == value_t::time);
    bool rhs_time = (rhs.type_ == value_t::time);

    auto make_result = [&](double v, bool is_time) -> value {
        return is_time ? value::time_ms(v) : value::number(v);
    };

    // ── Arithmetic ─────────────────────────────────────────────────────────
    if (op == "+") return make_result(a + b, lhs_time || rhs_time);
    if (op == "-") return make_result(a - b, lhs_time || rhs_time);

    if (op == "*") {
        // time * number → time,  number * time → time
        bool result_time = lhs_time || rhs_time;
        return make_result(a * b, result_time);
    }

    if (op == "/") {
        if (b == 0.0) {
            warn(line, col, "division by zero, returning 0");
            return value::number(0.0);
        }
        double result = a / b;
        if (std::isnan(result) || std::isinf(result)) {
            warn(line, col, "invalid arithmetic result, returning 0");
            return value::number(0.0);
        }
        // time / time → number (ratio),  time / number → time
        if (lhs_time && rhs_time) return value::number(result);
        if (lhs_time)             return value::time_ms(result);
        return value::number(result);
    }

    if (op == "%") {
        if (b == 0.0) {
            warn(line, col, "modulo by zero, returning 0");
            return value::number(0.0);
        }
        return make_result(std::fmod(a, b), lhs_time);
    }

    // ── Comparison (always number: 0 or 1) ─────────────────────────────────
    if (op == "<")  return value::number(a <  b ? 1.0 : 0.0);
    if (op == ">")  return value::number(a >  b ? 1.0 : 0.0);
    if (op == "<=") return value::number(a <= b ? 1.0 : 0.0);
    if (op == ">=") return value::number(a >= b ? 1.0 : 0.0);
    if (op == "==") return value::number(a == b ? 1.0 : 0.0);
    if (op == "!=") return value::number(a != b ? 1.0 : 0.0);

    // ── Bitwise (integer parts) ────────────────────────────────────────────
    int64_t ia = static_cast<int64_t>(a);
    int64_t ib = static_cast<int64_t>(b);

    if (op == "&")  return value::number(static_cast<double>(ia & ib));
    if (op == "|")  return value::number(static_cast<double>(ia | ib));
    if (op == "^")  return value::number(static_cast<double>(ia ^ ib));
    if (op == "<<") return value::number(static_cast<double>(ia << ib));
    if (op == ">>") return value::number(static_cast<double>(ia >> ib));

    // ── Ternary separator (should not reach here) ──────────────────────────
    if (op == "?") {
        // Already handled in ternary_op_expr
        return value::nil();
    }

    warn(0, 0, "unknown binary operator '" + op + "'");
    return value::number(0.0);
}

// ════════════════════════════════════════════════════════════════════════════════
// Unary operator dispatch
// ════════════════════════════════════════════════════════════════════════════════

value evaluator::apply_unop(const std::string& op, const value& operand) {
    double v = operand.as_number();

    if (op == "-") return value::number(-v);
    if (op == "~") return value::number(static_cast<double>(~static_cast<int64_t>(v)));

    return value::number(0.0);
}

// ════════════════════════════════════════════════════════════════════════════════
// Function calls
// ════════════════════════════════════════════════════════════════════════════════

value evaluator::eval_call(const parser::function_call& call) {
    if (!call.function_) return value::nil();

    // Resolve function name
    std::string fn_name;
    if (call.function_->type_ == parser::node_t::identifier_expr) {
        auto& id = static_cast<const parser::identifier_expr&>(*call.function_);
        if (id.identifier_) fn_name = id.identifier_->name_;
    }

    if (fn_name.empty()) {
        warn(call.line_, call.column_, "cannot resolve function name");
        return value::number(0.0);
    }

    // Evaluate arguments — split positional from named
    std::vector<value> args;
    named_args_t named;
    args.reserve(call.arguments_.size());
    for (const auto& arg : call.arguments_) {
        if (arg && arg->value_) {
            value v = eval_expr(arg->value_);
            if (!arg->name_.empty()) {
                named[arg->name_] = std::move(v);
            } else {
                args.push_back(std::move(v));
            }
        }
    }

    // ── Check builtins first ───────────────────────────────────────────────
    if (auto* bi = env_.lookup_builtin(fn_name)) {
        return eval_builtin(*bi, args);
    }

    // ── Check module functions ─────────────────────────────────────────────
    if (auto* mf = env_.lookup_module_fn(fn_name)) {
        return eval_module_fn(*mf, args, call.line_, call.column_);
    }

    // ── Check user-defined functions ───────────────────────────────────────
    auto it = function_defs_.find(fn_name);
    if (it != function_defs_.end() && it->second) {
        return eval_user_function(*it->second, args, named);
    }

    warn(call.line_, call.column_, "undefined function '" + fn_name + "'");
    return value::number(0.0);
}

value evaluator::eval_builtin(const builtin& fn, const std::vector<value>& args) {
    // Arity already checked by semantic analyzer, but be safe
    int n = static_cast<int>(args.size());
    if (n < fn.min_arity_ || (n > fn.max_arity_ && fn.max_arity_ >= 0) ) {
        warn(0, 0, "builtin '" + fn.name_ + "' arity mismatch");
        return value::number(0.0);
    }
    span<const value> arg_span{args.data(), args.size()};
    return fn.function_(arg_span);
}

value evaluator::eval_module_fn(const module::function_entry& fn,
                                const std::vector<value>& args,
                                int line, int col) {
    int n = static_cast<int>(args.size());
    if (n < fn.min_arity_ || (fn.max_arity_ >= 0 && n > fn.max_arity_)) {
        warn(line, col, "module function '" + fn.name_ + "' arity mismatch: expected "
             + std::to_string(fn.min_arity_) + "-" + std::to_string(fn.max_arity_)
             + ", got " + std::to_string(n));
        return value::nil();
    }
    span<const value> arg_span{args.data(), args.size()};
    return fn.fn_(arg_span);
}

value evaluator::eval_user_function(const parser::function_definition& def,
                                    const std::vector<value>& args,
                                    const named_args_t& named) {
    // Temporal function: has |> lambda block → instantiate with scheduler
    if (def.lambda_block_) {
        return instantiate_temporal(def, args, named);
    }

    // Constant temporal: no lambda block but has a dt parameter
    // (e.g. trig(dt=100ms) = !) — must still be scheduled for periodic ticking.
    bool has_dt = false;
    for (const auto& p : def.parameters_) {
        if (p && (p->name_ == "dt" || p->has_default_time_)) { has_dt = true; break; }
    }
    if (has_dt) {
        return instantiate_temporal(def, args, named);
    }

    if (!def.body_) return value::nil();

    // Push a new scope for parameters
    env_.push_scope();

    // Bind parameters: positional first, then named overrides, then defaults
    size_t pos_idx = 0;
    for (size_t i = 0; i < def.parameters_.size(); ++i) {
        if (!def.parameters_[i]) continue;
        const auto& name = def.parameters_[i]->name_;

        // Check named args first
        auto nit = named.find(name);
        if (nit != named.end()) {
            env_.define(name, nit->second);
        } else if (pos_idx < args.size()) {
            env_.define(name, args[pos_idx++]);
        } else if (def.parameters_[i]->default_value_) {
            env_.define(name, eval_expr(def.parameters_[i]->default_value_));
        } else {
            env_.define(name, value::number(0.0));
        }
    }

    // Evaluate body expression
    value result = eval_expr(def.body_);

    env_.pop_scope();
    return result;
}

// ════════════════════════════════════════════════════════════════════════════════
// Temporal function instantiation
// ════════════════════════════════════════════════════════════════════════════════

value evaluator::instantiate_temporal(const parser::function_definition& def,
                                      const std::vector<value>& args,
                                      const named_args_t& named) {
    // ── Retick mode: return existing instance output ────────────────────
    // During a scheduler callback, the binding expression is re-evaluated.
    // The temporal call (e.g. lfo(1hz)) must return the already-ticked
    // instance output rather than creating a duplicate instance.
    if (retick_instance_ && retick_instance_->def_name_ == def.name_) {
        return retick_instance_->read_output();
    }

    auto inst = std::make_shared<function_instance>();
    inst->id_ = next_instance_id_++;
    inst->active_ = true;
    inst->def_name_ = def.name_;

    // ── Bind parameters: positional first, then named, then defaults ───
    double dt_ms = 0.0;
    size_t pos_idx = 0;
    for (size_t i = 0; i < def.parameters_.size(); ++i) {
        if (!def.parameters_[i]) continue;
        const auto& param = *def.parameters_[i];

        value val;
        auto nit = named.find(param.name_);
        if (nit != named.end()) {
            val = nit->second;
        } else if (pos_idx < args.size()) {
            val = args[pos_idx++];
        } else if (param.default_value_) {
            val = eval_expr(param.default_value_);
        } else {
            val = value::number(0.0);
        }

        inst->params_[param.name_] = val;

        // Detect dt parameter (has_default_time_ flag or name == "dt")
        if (param.name_ == "dt" || param.has_default_time_) {
            dt_ms = val.as_number();
            // If the value is a time type, it's already in ms
            // If it's a raw number, treat as ms
        }
    }
    inst->dt_ms_ = dt_ms;

    // ── Run init block ─────────────────────────────────────────────────────
    if (def.lambda_block_ && def.lambda_block_->init_) {
        // Push a temporary scope to evaluate init expressions with
        // access to parameters
        env_.push_scope();
        for (auto& [name, val] : inst->params_) {
            env_.define(name, val);
        }

        for (const auto& stmt : def.lambda_block_->init_->statements_) {
            if (!stmt) continue;
            if (stmt->type_ == parser::node_t::function_definition) {
                // init: { counter_value = 0 } — parsed as 0-param function def
                auto& init_def = static_cast<const parser::function_definition&>(*stmt);
                if (init_def.parameters_.empty() && init_def.body_) {
                    value v = eval_expr(init_def.body_);
                    inst->current_[init_def.name_] = v;
                }
            } else if (stmt->type_ == parser::node_t::assignment) {
                auto& assign = static_cast<const parser::assignment&>(*stmt);
                value v = eval_expr(assign.value_);
                inst->current_[assign.name_] = v;
            }
        }

        env_.pop_scope();
    }

    // ── First tick: evaluate output with initial state ─────────────────────
    tick_instance(*inst, def);

    // Constant temporal (no lambda block): evaluate body for initial output
    if (!def.lambda_block_ && def.body_) {
        env_.push_scope();
        for (auto& [name, val] : inst->params_) {
            env_.define(name, val);
        }
        value output = eval_expr(def.body_);
        inst->write_output(std::move(output));
        env_.pop_scope();
    }

    // ── Store the instance ─────────────────────────────────────────────────
    instances_[inst->id_] = inst;

    // NOTE: subscription is NOT done here — the process block handler
    // subscribes after collecting the segment's reaction statements.

    // Return the initial output value
    return inst->read_output();
}

// ════════════════════════════════════════════════════════════════════════════════
// Temporal function tick — evaluate one update cycle
// ════════════════════════════════════════════════════════════════════════════════

void evaluator::tick_instance(function_instance& inst,
                              const parser::function_definition& def) {
    if (!def.lambda_block_) return;

    // Build a scope with: parameters + current state
    env_.push_scope();

    for (auto& [name, val] : inst.params_) {
        env_.define(name, val);
    }
    for (auto& [name, val] : inst.current_) {
        env_.define(name, val);
    }

    // ── Evaluate update statements (writes go to next_) ────────────────────
    // Double-buffered: all reads see current_, all writes go to next_.
    // We evaluate each statement, but assignments write to next_ buffer.
    inst.next_.clear();

    for (const auto& stmt : def.lambda_block_->update_statements_) {
        if (!stmt) continue;
        if (stmt->type_ == parser::node_t::function_definition) {
            // "phase = fmod(phase + ...)" parsed as 0-param func def
            auto& upd = static_cast<const parser::function_definition&>(*stmt);
            if (upd.parameters_.empty() && upd.body_) {
                value v = eval_expr(upd.body_);
                inst.next_[upd.name_] = v;
                // Also update the scope so later statements in the same
                // tick can see the new value (sequential semantics for now;
                // true double-buffering can be added later if needed)
                env_.define(upd.name_, v);
            }
        } else if (stmt->type_ == parser::node_t::assignment) {
            auto& assign = static_cast<const parser::assignment&>(*stmt);
            value v = eval_expr(assign.value_);
            inst.next_[assign.name_] = v;
            env_.define(assign.name_, v);
        }
    }

    // ── Evaluate output expression ─────────────────────────────────────────
    if (def.body_) {
        value output = eval_expr(def.body_);
        inst.write_output(std::move(output));
    }

    env_.pop_scope();

    // ── Commit: next → current ─────────────────────────────────────────────
    inst.commit();
}

// ════════════════════════════════════════════════════════════════════════════════
// Flow construction
// ════════════════════════════════════════════════════════════════════════════════

value evaluator::eval_flow_literal(const parser::flow_literal& fl) {
    auto fd = std::make_shared<flow_data>();
    flow_member fm;
    for (const auto& elem : fl.elements_) {
        fm.elements_.push_back(eval_expr(elem));
    }
    fd->members_.push_back(std::move(fm));
    value result;
    result.type_ = value_t::flow;
    result.flow_ = std::move(fd);
    return result;
}

value evaluator::eval_generator(const parser::generator_expr& gen) {
    value start = eval_expr(gen.range_start_);
    value end = eval_expr(gen.range_end_);
    int s = static_cast<int>(start.as_number());
    int n = static_cast<int>(end.as_number());

    auto fd = std::make_shared<flow_data>();
    flow_member fm;

    env_.push_scope();
    for (int i = s; i < n; ++i) {
        env_.define(gen.variable_, value::number(static_cast<double>(i)));
        fm.elements_.push_back(eval_expr(gen.body_));
    }
    env_.pop_scope();

    fd->members_.push_back(std::move(fm));
    value result;
    result.type_ = value_t::flow;
    result.flow_ = std::move(fd);
    return result;
}

// ════════════════════════════════════════════════════════════════════════════════
// Warning system
// ════════════════════════════════════════════════════════════════════════════════

void evaluator::warn(int line, int col, const std::string& msg) {
    warnings_.push_back({line, col, msg});
}

void evaluator::print_warnings() const {
    for (const auto& w : warnings_) {
        std::cerr << w.line_ << "." << w.column_ << ": runtime warning: "
                  << w.message_ << "\n";
    }
}

} // --- idyl::core ---
