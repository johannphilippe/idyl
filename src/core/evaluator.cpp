#include "core/evaluator.hpp"
#include "utilities/span.hpp"
#include "utilities/safety.hpp"
#include "utilities/filesystem.hpp"
#include "parser/parse.hpp"
#include "debug.hpp"

#include <cmath>
#include <algorithm>
#include <limits>
#include <set>
#include <unordered_set>

namespace idyl::core {

// ════════════════════════════════════════════════════════════════════════════════
// AST identifier collection — used by reaction redistribution
// ════════════════════════════════════════════════════════════════════════════════

// Recursively collect all identifier names referenced by an expression.
static void collect_expr_ids(const parser::expr_ptr& expr,
                              std::unordered_set<std::string>& out) {
    if (!expr) return;
    switch (expr->type_) {
        case parser::node_t::identifier_expr: {
            auto& e = static_cast<const parser::identifier_expr&>(*expr);
            if (e.identifier_) out.insert(e.identifier_->name_);
            break;
        }
        case parser::node_t::binary_op_expr: {
            auto& e = static_cast<const parser::binary_op_expr&>(*expr);
            if (e.op_) {
                collect_expr_ids(e.op_->left_,  out);
                collect_expr_ids(e.op_->right_, out);
            }
            break;
        }
        case parser::node_t::unary_op_expr: {
            auto& e = static_cast<const parser::unary_op_expr&>(*expr);
            if (e.op_) collect_expr_ids(e.op_->operand_, out);
            break;
        }
        case parser::node_t::ternary_op_expr: {
            auto& e = static_cast<const parser::ternary_op_expr&>(*expr);
            if (e.op_) {
                for (const auto& opt : e.op_->options_)
                    collect_expr_ids(opt, out);
                collect_expr_ids(e.op_->condition_, out);
            }
            break;
        }
        case parser::node_t::memory_op_expr: {
            auto& e = static_cast<const parser::memory_op_expr&>(*expr);
            if (e.op_) {
                collect_expr_ids(e.op_->expr_,        out);
                collect_expr_ids(e.op_->delay_count_, out);
            }
            break;
        }
        case parser::node_t::function_call_expr: {
            auto& e = static_cast<const parser::function_call_expr&>(*expr);
            if (e.call_) {
                collect_expr_ids(e.call_->function_, out);
                for (const auto& arg : e.call_->arguments_)
                    if (arg) collect_expr_ids(arg->value_, out);
            }
            break;
        }
        case parser::node_t::flow_access_expr: {
            auto& e = static_cast<const parser::flow_access_expr&>(*expr);
            if (e.access_) {
                collect_expr_ids(e.access_->flow_,  out);
                collect_expr_ids(e.access_->index_, out);
            }
            break;
        }
        case parser::node_t::module_access_expr: {
            auto& e = static_cast<const parser::module_access_expr&>(*expr);
            if (e.access_) {
                collect_expr_ids(e.access_->module_, out);
                for (const auto& arg : e.access_->arguments_)
                    if (arg) collect_expr_ids(arg->value_, out);
            }
            break;
        }
        case parser::node_t::parenthesized_expr: {
            auto& e = static_cast<const parser::parenthesized_expr&>(*expr);
            collect_expr_ids(e.expr_, out);
            break;
        }
        case parser::node_t::generator_expr_node: {
            auto& e = static_cast<const parser::generator_expr_node&>(*expr);
            if (e.generator_) {
                collect_expr_ids(e.generator_->range_start_, out);
                collect_expr_ids(e.generator_->range_end_,   out);
                collect_expr_ids(e.generator_->body_,        out);
            }
            break;
        }
        case parser::node_t::flow_literal_expr: {
            auto& e = static_cast<const parser::flow_literal_expr&>(*expr);
            if (e.flow_)
                for (const auto& elem : e.flow_->elements_)
                    collect_expr_ids(elem, out);
            break;
        }
        default: break;
    }
}

// Collect identifiers from the expression(s) inside a statement.
static void collect_stmt_ids(const parser::stmt_ptr& stmt,
                              std::unordered_set<std::string>& out) {
    if (!stmt) return;
    switch (stmt->type_) {
        case parser::node_t::assignment: {
            auto& a = static_cast<const parser::assignment&>(*stmt);
            collect_expr_ids(a.value_, out);
            break;
        }
        case parser::node_t::expression_stmt: {
            auto& es = static_cast<const parser::expression_stmt&>(*stmt);
            collect_expr_ids(es.expression_, out);
            break;
        }
        case parser::node_t::function_definition: {
            auto& fd = static_cast<const parser::function_definition&>(*stmt);
            collect_expr_ids(fd.body_, out);
            break;
        }
        default: break;
    }
}

// ════════════════════════════════════════════════════════════════════════════════
// Top-level entry: walk program statements
// ════════════════════════════════════════════════════════════════════════════════

void evaluator::run(const parser::program& program) {
    env_.init();
    clocks_.init(120.0);  // main clock at 120 BPM

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
        env_.define(def.name_, value::function_ref(def.name_));
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

        if (!def.parameters_.empty()) {
            // Parametric flow: store the definition for call-time evaluation.
            // A function_ref is placed in the environment so the name resolves
            // through the normal indirect-call path in eval_call.
            flow_defs_[def.name_] = std::static_pointer_cast<parser::flow_definition>(
                std::const_pointer_cast<parser::statement>(stmt));
            env_.define(def.name_, value::function_ref(def.name_));
            break;
        }

        // Zero-parameter flow: evaluate eagerly (original behaviour).
        {
            value flow_val;
            flow_val.type_ = value_t::flow;
            flow_val.flow_ = eval_flow_members(def.members_);
            env_.define(def.name_, std::move(flow_val));
        }
        break;
    }

    // ── Process block → reactive execution ─────────────────────────────────
    // Each temporal function binding starts a "segment". Statements that
    // follow (print calls, derived assignments, etc.) are that segment's
    // "reactions" and re-run on every tick of the temporal source.
    case parser::node_t::process_block: {
        auto& proc = static_cast<const parser::process_block&>(*stmt);
        if (!proc.body_) break;

        // ── Listen mode: store blocks for OSC start, pre-start filtered ones ──
        if (listen_mode_) {
            bool pre_start = !process_filter_.empty() && proc.name_ == process_filter_;
            if (!pre_start) {
                // Store for later start via OSC
                if (!proc.name_.empty()) {
                    stored_processes_[proc.name_] =
                        std::static_pointer_cast<parser::process_block>(
                            std::const_pointer_cast<parser::node>(
                                std::shared_ptr<const parser::node>(stmt, &proc)));
                }
                break;
            }
            // Fall through to execute pre-started process
        } else {
            // Normal mode: apply process filter
            if (!process_filter_.empty() && proc.name_ != process_filter_) break;
        }

        ++process_count_;
        env_.push_scope();

        // ── Evaluate optional duration ─────────────────────────────────────────
        double dur_ms = 0.0;   // 0 = run forever
        if (proc.duration_) {
            value dv = eval_expr(proc.duration_);
            dur_ms = dv.as_number();   // time → ms, number → raw
        }

        // ── Catch info: monitors an emitted variable from a temporal instance ──
        struct catch_info {
            std::string watched_emit;               // emitted variable name to watch
            std::vector<parser::stmt_ptr> handler;   // handler statements
            bool fired = false;                      // fire once then deactivate
        };

        struct temporal_segment {
            uint64_t    instance_id;
            std::string bound_var;
            parser::stmt_ptr binding_stmt;  // the original binding statement
            std::vector<parser::stmt_ptr> reactions;
            std::vector<catch_info> catches;
        };

        std::vector<temporal_segment> segments;
        temporal_segment* current_seg = nullptr;
        bool has_temporal = false;

        // Collect catch blocks separately for linking after instance creation
        std::vector<std::shared_ptr<parser::catch_block>> catch_blocks;

        for (const auto& s : proc.body_->statements_) {
            // Catch blocks are collected, not executed directly
            if (s->type_ == parser::node_t::catch_block) {
                catch_blocks.push_back(
                    std::static_pointer_cast<parser::catch_block>(s));
                continue;
            }

            // at_blocks create scheduler subscriptions and must keep the scope alive.
            // Execute them directly (exec_stmt handles the subscription); mark
            // has_temporal so the process scope is not popped after the loop.
            if (s->type_ == parser::node_t::at_block) {
                has_temporal = true;
                exec_stmt(s);
                continue;
            }

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
                uint64_t inst_id = next_instance_id_ - 1;
                segments.push_back({inst_id, binding_name, s, {}, {}});
                current_seg = &segments.back();
                // Register binding so the :: accessor can find this instance
                if (!binding_name.empty())
                    instance_bindings_[binding_name] = inst_id;
            } else if (current_seg) {
                // Statement follows a temporal binding → reaction
                current_seg->reactions.push_back(s);
            }
        }

        // ── Link catch blocks to their source segments ─────────────────────
        for (auto& cb : catch_blocks) {
            if (!cb->expression_) continue;
            std::string source_var;
            if (cb->expression_->type_ == parser::node_t::identifier_expr) {
                auto& ie = static_cast<const parser::identifier_expr&>(*cb->expression_);
                if (ie.identifier_) source_var = ie.identifier_->name_;
            }
            if (source_var.empty()) continue;

            for (auto& seg : segments) {
                if (seg.bound_var == source_var) {
                    seg.catches.push_back({cb->event_type_, cb->handler_, false});
                    break;
                }
            }
        }

        // ── Redistribute reactions to the fastest segment they depend on ──────
        // A reaction belongs to the segment whose bound variable it references
        // with the smallest dt (highest frequency).  This ensures that, e.g.,
        // `print(m1)` placed after `m2` in source still fires at m1's rate
        // rather than m2's rate, because it only depends on m1.
        //
        // Only direct references to segment bound-variables matter here.
        // Derived variables (computed in reactions) are not segment-bound, so
        // a reaction like `print(x)` where x is a reaction-computed variable
        // stays in whatever segment it currently belongs to.
        if (segments.size() > 1) {
            // Map: bound variable name → segment index
            std::unordered_map<std::string, size_t> var_to_seg;
            for (size_t si = 0; si < segments.size(); ++si)
                if (!segments[si].bound_var.empty())
                    var_to_seg[segments[si].bound_var] = si;

            std::vector<std::vector<parser::stmt_ptr>> new_reactions(segments.size());

            for (size_t si = 0; si < segments.size(); ++si) {
                for (const auto& reaction : segments[si].reactions) {
                    std::unordered_set<std::string> ids;
                    collect_stmt_ids(reaction, ids);

                    // Find the fastest (smallest dt) segment bound-var referenced.
                    double min_dt   = std::numeric_limits<double>::max();
                    size_t target   = si;   // default: keep in current segment

                    for (const auto& id : ids) {
                        auto vit = var_to_seg.find(id);
                        if (vit == var_to_seg.end()) continue;
                        size_t ref_si  = vit->second;
                        auto   iit     = instances_.find(segments[ref_si].instance_id);
                        if (iit == instances_.end()) continue;
                        double ref_dt  = iit->second->dt_ms_;
                        if (ref_dt < min_dt) { min_dt = ref_dt; target = ref_si; }
                    }

                    new_reactions[target].push_back(reaction);
                }
            }

            for (size_t si = 0; si < segments.size(); ++si)
                segments[si].reactions = std::move(new_reactions[si]);
        }

        // Subscribe each segment's temporal source to the scheduler.
        // On each tick: update instance → update bound variable → re-run reactions → check catches.
        if (scheduler_ && has_temporal) {
            for (auto& seg : segments) {
                auto inst_it = instances_.find(seg.instance_id);
                if (inst_it == instances_.end()) continue;
                auto inst = inst_it->second;
                if (inst->dt_ms_ <= 0.0) continue;

                // Native temporal instances have no AST def — def_ptr stays null.
                // AST temporal instances require a function_def lookup.
                std::shared_ptr<parser::function_definition> def_ptr;
                if (!inst->native_update_) {
                    auto def_it = function_defs_.find(inst->def_name_);
                    if (def_it == function_defs_.end()) continue;
                    def_ptr = def_it->second;
                }

                std::string var = seg.bound_var;
                auto binding    = seg.binding_stmt;
                auto reactions  = seg.reactions;
                auto weak_inst  = std::weak_ptr<function_instance>(inst);
                // Shared so the fired flag persists across ticks
                auto catches    = std::make_shared<std::vector<catch_info>>(std::move(seg.catches));

                inst->subscription_id_ = scheduler_->subscribe(inst->dt_ms_,
                    [this, def_ptr, weak_inst, var, binding, reactions, catches,
                     dur_ms, start_t = scheduler_->now_ms()]
                    (double t, double /*dt*/) -> bool {
                        // ── Duration check ─────────────────────────────────
                        if (dur_ms > 0.0 && (t - start_t) >= dur_ms) {
                            auto si = weak_inst.lock();
                            if (si) si->active_ = false;
                            return false;
                        }

                        auto si = weak_inst.lock();
                        if (!si || !si->active_) return false;

                        tick_instance(*si, def_ptr.get());

                        retick_instance_ = si.get();
                        if (binding)
                            exec_stmt(binding);
                        retick_instance_ = nullptr;

                        for (const auto& r : reactions)
                            exec_stmt(r);

                        // ── Check catch registrations ──────────────────────
                        bool should_end = false;
                        for (auto& c : *catches) {
                            if (c.fired) continue;
                            if (c.watched_emit == "end") continue;  // handled on subscription end

                            auto emit_it = si->emitted_.find(c.watched_emit);
                            if (emit_it != si->emitted_.end() && emit_it->second.is_truthy()) {
                                c.fired = true;
                                for (const auto& h : c.handler)
                                    exec_stmt(h);
                            }
                        }

                        return !should_end;
                    });
            }
        }

        // Track which instances belong to this process (for stop_process)
        if (!proc.name_.empty() && has_temporal) {
            auto& ids = active_process_instances_[proc.name_];
            for (const auto& seg : segments)
                ids.push_back(seg.instance_id);
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

    // ── @(time_expr): { handler } → one-shot deferred execution ───────────
    // Evaluates the time expression to get a delay in ms, then schedules
    // the handler statements to run exactly once after that delay.
    // If no scheduler is available (or delay <= 0), executes immediately.
    case parser::node_t::at_block: {
        auto& atb = static_cast<const parser::at_block&>(*stmt);
        if (!atb.time_expr_) break;

        value delay_val = eval_expr(atb.time_expr_);
        double delay_ms = delay_val.number_;  // both time and number store value in number_

        if (!scheduler_ || delay_ms <= 0.0) {
            // No scheduler or zero/negative delay → execute immediately
            for (const auto& h : atb.handler_)
                exec_stmt(h);
            break;
        }

        // Capture handler statements and current evaluator reference.
        // The process scope is kept alive (has_temporal = true in process loop),
        // so env_ variables remain valid when the callback fires.
        auto handler = atb.handler_;
        scheduler_->subscribe(delay_ms,
            [this, handler](double /*t*/, double /*dt*/) -> bool {
                for (const auto& h : handler)
                    exec_stmt(h);
                return false;  // one-shot: do not reschedule
            });
        break;
    }

    case parser::node_t::stop_statement: {
        // Iterate through all active processes and stop those matching the target name (or all if target is empty)
        auto& stop_stmt = static_cast<const parser::stop_statement&>(*stmt);
        std::string target = stop_stmt.target_;
        for (auto& [proc_name, instance_ids] : active_process_instances_) {
            if (target.empty() || proc_name == target) {
                for (auto& id : instance_ids) {
                    auto inst_it = instances_.find(id);
                    if (inst_it != instances_.end()) {
                        auto& inst = inst_it->second;
                        if (scheduler_ && inst->subscription_id_ != 0) {
                            scheduler_->unsubscribe(inst->subscription_id_);
                            inst->subscription_id_ = 0;
                        }
                    }
                }
            }
        }
        break;
    }

    case parser::node_t::start_statement: {
        auto& start_stmt = static_cast<const parser::start_statement&>(*stmt);
        std::string target = start_stmt.target_;
        if (!start_process(target)) {
            warnings_.push_back({start_stmt.line_, start_stmt.column_,
                "No stored process named '" + target + "' found to start."});
        }
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

        // When a namespace is given, build a library-local scope: a map from
        // each original (unqualified) function name to a function_ref pointing
        // at the qualified name (ns::name).  This scope is pushed around tick
        // and body evaluation so that library-internal cross-calls (e.g. lfo
        // calling sine_shape) resolve correctly without leaking bare names into
        // global scope.
        lib_scope_ptr lib_scope;
        if (!ns.empty()) {
            lib_scope = std::make_shared<lib_scope_t>();
            for (const auto& s : lib_program->statements_) {
                if (!s) continue;
                if (s->type_ == parser::node_t::function_definition) {
                    auto fn = std::static_pointer_cast<parser::function_definition>(s);
                    std::string qname = ns + "::" + fn->name_;
                    (*lib_scope)[fn->name_] = value::function_ref(qname);
                } else if (s->type_ == parser::node_t::flow_definition) {
                    auto fl = std::static_pointer_cast<parser::flow_definition>(s);
                    std::string qname = ns + "::" + fl->name_;
                    (*lib_scope)[fl->name_] = value::function_ref(qname);
                }
            }
        }

        for (const auto& lib_stmt : lib_program->statements_) {
            if (!lib_stmt) continue;

            if (!ns.empty() && lib_stmt->type_ == parser::node_t::function_definition) {
                auto def_ptr = std::static_pointer_cast<parser::function_definition>(lib_stmt);
                std::string original_name = def_ptr->name_;
                std::string qname = ns + "::" + original_name;
                // Register under qualified name; restore AST node name after.
                def_ptr->name_ = qname;
                exec_stmt(lib_stmt);
                def_ptr->name_ = original_name;
                // Record the lib scope so instantiate_temporal can attach it.
                fn_library_scope_[qname] = lib_scope;
            } else if (!ns.empty() && lib_stmt->type_ == parser::node_t::flow_definition) {
                auto def_ptr = std::static_pointer_cast<parser::flow_definition>(lib_stmt);
                std::string original_name = def_ptr->name_;
                def_ptr->name_ = ns + "::" + original_name;
                exec_stmt(lib_stmt);
                def_ptr->name_ = original_name;
            } else {
                exec_stmt(lib_stmt);
            }
        }
        break;
    }

    // ── Module imports → built-in catalog or external dlopen ───────────────
    case parser::node_t::module_import: {
        auto& mi = static_cast<const parser::module_import&>(*stmt);
        if (!env_.module_registry_) break;

        // ── Built-in module (catalog) ──────────────────────────────────────
        if (env_.module_registry_->has_builtin(mi.path_)) {
            std::string err = env_.module_registry_->load_builtin(mi.path_, mi.namespace_);
            if (!err.empty())
                warnings_.push_back({mi.line_, mi.column_, err});
            break;
        }

        // ── External (.so) module ──────────────────────────────────────────
        std::string path = mi.path_;
        if (!idyl::utilities::get_module_path(path)) {
            warnings_.push_back({mi.line_, mi.column_,
                "Module '" + mi.path_ + "' not found — skipping."});
            break;
        }
        if (env_.module_registry_->is_loaded(path)) break;

        std::string err = env_.module_registry_->load_external(path, mi.namespace_);
        if (!err.empty())
            warnings_.push_back({mi.line_, mi.column_, err});
        else
            env_.module_registry_->mark_loaded(path);
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

        // Check scope chain first (variables, parameters, etc.)
        if (auto* val = env_.lookup(name)) {
            return *val;
        }

        // If the identifier names a known function, return a first-class
        // function reference value.  This allows functions to be passed
        // around, stored in flows, and called indirectly.
        if (env_.lookup_builtin(name))      return value::function_ref(name);
        if (env_.lookup_module_fn(name))    return value::function_ref(name);
        if (function_defs_.count(name))     return value::function_ref(name);

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

    // ── Memory operator (@ delay / prime notation) ────────────────────────
    case parser::node_t::memory_op_expr: {
        return eval_memory_op(static_cast<const parser::memory_op_expr*>(expr.get()));
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
        auto& mae = static_cast<const parser::module_access_expr&>(*expr);
        if (!mae.access_) return value::nil();
        auto& acc = *mae.access_;

        // Resolve the left-hand side to a variable name for instance lookup
        std::string source_var;
        if (acc.module_ && acc.module_->type_ == parser::node_t::identifier_expr) {
            auto& ie = static_cast<const parser::identifier_expr&>(*acc.module_);
            if (ie.identifier_) source_var = ie.identifier_->name_;
        }

        // ── Emit accessor: var::emitted_name ───────────────────────────
        // If the left-hand side is a variable bound to a temporal instance,
        // look up the emitted value from that instance.
        if (!source_var.empty()) {
            auto bind_it = instance_bindings_.find(source_var);
            if (bind_it != instance_bindings_.end()) {
                auto inst_it = instances_.find(bind_it->second);
                if (inst_it != instances_.end()) {
                    auto& inst = *inst_it->second;
                    auto emit_it = inst.emitted_.find(acc.function_);
                    if (emit_it != inst.emitted_.end()) {
                        return emit_it->second;
                    }
                    // Not emitted yet — return nil (first tick hasn't happened)
                    return value::nil();
                }
            }

            // ── Namespaced function call: ns::func(...) ────────────────
            // If it's not an instance binding, try as namespace::function
            std::string qualified = source_var + "::" + acc.function_;
            if (!acc.arguments_.empty()) {
                // Evaluate as a namespaced function call
                std::vector<value> args;
                named_args_t named;
                std::vector<parser::expr_ptr> pos_exprs;
                named_exprs_t named_exprs;
                for (const auto& arg : acc.arguments_) {
                    if (arg && arg->value_) {
                        value v = eval_expr(arg->value_);
                        if (!arg->name_.empty()) {
                            named[arg->name_] = std::move(v);
                            named_exprs[arg->name_] = arg->value_;
                        } else {
                            args.push_back(std::move(v));
                            pos_exprs.push_back(arg->value_);
                        }
                    }
                }
                if (auto* bi = env_.lookup_builtin(qualified))
                    return eval_builtin(*bi, args);
                if (auto* mf = env_.lookup_module_fn(qualified)) {
                    if (mf->is_native_temporal_)
                        return instantiate_native_temporal(*mf, args, named, pos_exprs, named_exprs);
                    return eval_module_fn(*mf, args, expr->line_, expr->column_);
                }
                auto it = function_defs_.find(qualified);
                if (it != function_defs_.end() && it->second)
                    return eval_user_function(*it->second, args, named, qualified, pos_exprs, named_exprs);
            } else {
                // No args — could be a namespaced function ref or value
                if (auto* val = env_.lookup(qualified))
                    return *val;
                if (env_.lookup_builtin(qualified))
                    return value::function_ref(qualified);
                if (env_.lookup_module_fn(qualified))
                    return value::function_ref(qualified);
                if (function_defs_.count(qualified))
                    return value::function_ref(qualified);
            }
        }

        warn(expr->line_, expr->column_, "cannot resolve '::' access");
        return value::nil();
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
    if (unit == "ms")  return v;
    if (unit == "s")   return v * 1000.0;
    if (unit == "hz")  return (v > 0.0) ? (1000.0 / v) : 0.0;
    if (unit == "bpm") return (v > 0.0) ? (60000.0 / v) : 0.0;
    if (unit == "b")   return v * (60000.0 / main_clock_bpm()); // beat duration from main clock
    return v;
}

value evaluator::eval_memory_op(const parser::memory_op_expr* moe) {
    // Key by the inner memory_op node — stable even if memory_op_expr is cloned,
    // because clone() copies the shared_ptr<memory_op> (same underlying pointer).
    const parser::node* key = moe->op_.get();

    // Determine buffer size (1-sample by default)
    size_t size = 1;
    if (moe->op_->delay_count_) {
        value count_val = eval_expr(moe->op_->delay_count_);
        size = std::max(size_t(1), static_cast<size_t>(std::round(count_val.as_number())));
    }

    // Initialize buffer if missing or size changed
    auto it = delay_memories_.find(key);

    value v = eval_expr(moe->op_->expr_); // current value for initialization if needed     
    if (it == delay_memories_.end() || it->second.buffer.size() != size) {
        memory_buffer buf(size);
        for (auto& slot : buf.buffer) slot = v; // initialize with current value
        delay_memories_[key] = std::move(buf);
        it = delay_memories_.find(key);
    }

    memory_buffer& mem = it->second;

    // Read the oldest (most delayed) value — this is what we return
    value out = mem.buffer[mem.write_index];

    // Overwrite that slot with the current value
    mem.buffer[mem.write_index] = v;

    // Advance write index (circular)
    mem.write_index = (mem.write_index + 1) % size;

    return out;
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
                case value_t::handle:  return v.as_string();
                case value_t::function: return v.as_string();
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

    // ── Indirect call: if the name refers to a variable holding a function
    //    reference, resolve through the ref to get the actual function name.
    if (auto* var = env_.lookup(fn_name)) {
        if (var->type_ == value_t::function && var->string_) {
            fn_name = *var->string_;
        }
    }

    // Evaluate arguments — split positional from named.
    // Also keep the raw AST expressions for dynamic parameter re-evaluation.
    std::vector<value> args;
    named_args_t named;
    std::vector<parser::expr_ptr> pos_exprs;
    named_exprs_t named_exprs;
    args.reserve(call.arguments_.size());
    for (const auto& arg : call.arguments_) {
        if (arg && arg->value_) {
            value v = eval_expr(arg->value_);
            if (!arg->name_.empty()) {
                named[arg->name_] = std::move(v);
                named_exprs[arg->name_] = arg->value_;
            } else {
                args.push_back(std::move(v));
                pos_exprs.push_back(arg->value_);
            }
        }
    }

    // ── Clock / tempo intrinsics ────────────────────────────────────────────
    // These need access to named args and evaluator state, so they are
    // handled before generic builtin dispatch.
    if (fn_name == "clock") {
        // clock(bpm_value)  or  clock(bpm_value, parent=handle)
        if (args.empty()) {
            warn(call.line_, call.column_, "clock() requires a BPM argument (e.g. clock(120bpm))");
            return value::nil();
        }
        double period_ms = args[0].as_number();  // bpm literal → period in ms
        double bpm = (period_ms > 0.0) ? (60000.0 / period_ms) : 120.0;

        // Determine parent: default = main clock
        uint64_t parent_id = clocks_.main_id_;
        auto pit = named.find("parent");
        if (pit != named.end()) {
            intptr_t h = pit->second.as_handle();
            parent_id = (h > 0) ? static_cast<uint64_t>(h) : 0;  // 0 = free
        }

        uint64_t id = clocks_.create(bpm, parent_id);
        return value::handle(static_cast<intptr_t>(id));
    }

    if (fn_name == "tempo") {
        // tempo()              → return main clock BPM
        // tempo(bpm)           → set main clock BPM, propagate
        // tempo(handle, bpm)   → set specific clock BPM, propagate
        if (args.empty()) {
            return value::number(clocks_.main_bpm());
        } else if (args.size() == 1) {
            double period_ms = args[0].as_number();
            double bpm = (period_ms > 0.0) ? (60000.0 / period_ms) : 120.0;
            clocks_.set_bpm(clocks_.main_id_, bpm);
            return value::number(bpm);
        } else {
            uint64_t clock_id = static_cast<uint64_t>(args[0].as_handle());
            double period_ms = args[1].as_number();
            double bpm = (period_ms > 0.0) ? (60000.0 / period_ms) : 120.0;
            clocks_.set_bpm(clock_id, bpm);
            return value::number(bpm);
        }
    }

    if (fn_name == "bpm") {
        // bpm(handle) → query BPM of a clock
        if (args.empty()) {
            return value::number(clocks_.main_bpm());
        }
        uint64_t clock_id = static_cast<uint64_t>(args[0].as_handle());
        return value::number(clocks_.bpm(clock_id));
    }

    // ── Check builtins first ───────────────────────────────────────────────
    if (auto* bi = env_.lookup_builtin(fn_name)) {
        return eval_builtin(*bi, args);
    }

    // ── Check module functions ─────────────────────────────────────────────
    if (auto* mf = env_.lookup_module_fn(fn_name)) {
        if (mf->is_native_temporal_)
            return instantiate_native_temporal(*mf, args, named, pos_exprs, named_exprs);
        return eval_module_fn(*mf, args, call.line_, call.column_);
    }

    // ── Check user-defined functions ───────────────────────────────────────
    auto it = function_defs_.find(fn_name);
    if (it != function_defs_.end() && it->second) {
        // Pass fn_name as the qualified_key so temporal instances get the right
        // def_name_ (the key that function_defs_ and fn_library_scope_ use).
        return eval_user_function(*it->second, args, named, fn_name, pos_exprs, named_exprs);
    }

    // ── Check parametric flow definitions ──────────────────────────────────
    auto fit = flow_defs_.find(fn_name);
    if (fit != flow_defs_.end() && fit->second) {
        return eval_flow_call(*fit->second, args, named, pos_exprs, named_exprs);
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
    // Timed module functions receive scheduler time and dt
    if (fn.is_timed_ && fn.timed_fn_) {
        span<const value> arg_span{args.data(), args.size()};
        double t = 0.0, dt = 0.0;
        // TODO: pass real scheduler time when invoked from tick context
        return fn.timed_fn_(arg_span, t, dt);
    }
    span<const value> arg_span{args.data(), args.size()};
    return fn.fn_(arg_span);
}

// ════════════════════════════════════════════════════════════════════════════════
// Native temporal instantiation — module/builtin-backed temporal functions
// ════════════════════════════════════════════════════════════════════════════════

value evaluator::instantiate_native_temporal(const module::function_entry& entry,
                                              const std::vector<value>& args,
                                              const named_args_t& named,
                                              const std::vector<parser::expr_ptr>& pos_exprs,
                                              const named_exprs_t& named_exprs) {
    // Retick guard: if we're inside a scheduler callback for this instance,
    // just return its current output.
    if (retick_instance_ && retick_instance_->def_name_ == entry.name_)
        return retick_instance_->read_output();

    auto inst = std::make_shared<function_instance>();
    inst->id_          = next_instance_id_++;
    inst->active_      = true;
    inst->def_name_    = entry.name_;
    inst->native_update_ = entry.native_update_;

    // ── Bind parameters from entry.params_ ────────────────────────────────
    double dt_ms = 0.0;
    auto& param_exprs = instance_param_exprs_[inst->id_];
    param_exprs.clear();

    size_t pos_idx = 0;
    for (size_t i = 0; i < entry.params_.size(); ++i) {
        const auto& pd = entry.params_[i];
        value val;
        parser::expr_ptr src_expr;

        auto nit = named.find(pd.name_);
        if (nit != named.end()) {
            val = nit->second;
            auto eit = named_exprs.find(pd.name_);
            if (eit != named_exprs.end()) src_expr = eit->second;
        } else if (pos_idx < args.size()) {
            val = args[pos_idx];
            if (pos_idx < pos_exprs.size()) src_expr = pos_exprs[pos_idx];
            ++pos_idx;
        } else if (pd.has_default_) {
            val = pd.default_value_;
        } else {
            val = value::number(0.0);
        }

        if (src_expr) param_exprs[pd.name_] = src_expr;
        inst->params_[pd.name_] = val;

        if (pd.name_ == "dt") dt_ms = val.as_number();
    }
    inst->dt_ms_ = dt_ms;

    // ── Run native init (if provided) ──────────────────────────────────────
    if (entry.native_init_)
        entry.native_init_(inst->params_, inst->current_);

    // ── Compute initial output ─────────────────────────────────────────────
    {
        value output;
        inst->emitted_.clear();
        inst->native_update_(inst->params_, inst->current_, inst->emitted_, output);
        inst->write_output(std::move(output));
    }

    instances_[inst->id_] = inst;
    return inst->read_output();
}

value evaluator::eval_user_function(const parser::function_definition& def,
                                    const std::vector<value>& args,
                                    const named_args_t& named,
                                    const std::string& qualified_key,
                                    const std::vector<parser::expr_ptr>& pos_exprs,
                                    const named_exprs_t& named_exprs) {
    // Temporal function: has |> lambda block → instantiate with scheduler
    if (def.lambda_block_) {
        return instantiate_temporal(def, args, named, qualified_key, pos_exprs, named_exprs);
    }

    // Constant temporal: no lambda block but has a dt parameter
    // (e.g. trig(dt=100ms) = !) — must still be scheduled for periodic ticking.
    bool has_dt = false;
    for (const auto& p : def.parameters_) {
        if (p && (p->name_ == "dt" || p->has_default_time_)) { has_dt = true; break; }
    }
    if (has_dt) {
        return instantiate_temporal(def, args, named, qualified_key, pos_exprs, named_exprs);
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
                                      const named_args_t& named,
                                      const std::string& qualified_key,
                                      const std::vector<parser::expr_ptr>& pos_exprs,
                                      const named_exprs_t& named_exprs) {
    // ── Retick mode: return existing instance output ────────────────────
    // During a scheduler callback, the binding expression is re-evaluated.
    // The temporal call (e.g. lfo(1hz)) must return the already-ticked
    // instance output rather than creating a duplicate instance.
    // The canonical key for this function in function_defs_ is the qualified
    // name when loaded from a namespaced library, otherwise def.name_.
    const std::string& canon_key = qualified_key.empty() ? def.name_ : qualified_key;

    if (retick_instance_ && retick_instance_->def_name_ == canon_key) {
        return retick_instance_->read_output();
    }

    auto inst = std::make_shared<function_instance>();
    inst->id_ = next_instance_id_++;
    inst->active_ = true;
    // Store the qualified key so the process-block subscription lookup finds
    // the right entry in function_defs_ even for namespaced library functions.
    inst->def_name_ = canon_key;
    // Attach library-local scope if this function came from a namespaced lib.
    {
        auto it = fn_library_scope_.find(canon_key);
        if (it != fn_library_scope_.end())
            inst->library_scope_ = it->second;
    }

    // ── Bind parameters: positional first, then named, then defaults ───
    // Also record the raw AST expression for each param so tick_instance can
    // re-evaluate it every tick, picking up changes in dynamic (temporal) inputs.
    double dt_ms = 0.0;
    size_t pos_idx = 0;
    inst->params_.clear();
    auto& param_exprs = instance_param_exprs_[inst->id_];
    param_exprs.clear();

    for (size_t i = 0; i < def.parameters_.size(); ++i) {
        if (!def.parameters_[i]) continue;
        const auto& param = *def.parameters_[i];
        value val;
        parser::expr_ptr src_expr;

        auto nit = named.find(param.name_);
        if (nit != named.end()) {
            val = nit->second;
            auto eit = named_exprs.find(param.name_);
            if (eit != named_exprs.end()) src_expr = eit->second;
        } else if (pos_idx < args.size()) {
            val = args[pos_idx];
            if (pos_idx < pos_exprs.size()) src_expr = pos_exprs[pos_idx];
            ++pos_idx;
        } else if (param.default_value_) {
            val = eval_expr(param.default_value_);
            // Defaults are definition-time constants — no dynamic expr stored.
        } else {
            val = value::number(0.0);
        }

        if (src_expr) param_exprs[param.name_] = src_expr;
        inst->params_[param.name_] = val;

        // Detect dt parameter (has_default_time_ flag or name == "dt")
        if (param.name_ == "dt" || param.has_default_time_) {
            dt_ms = val.as_number();
        }
    }
    inst->dt_ms_ = dt_ms;




    // ── Run init block ─────────────────────────────────────────────────────
    if (def.lambda_block_ && def.lambda_block_->init_) {
        if (inst->library_scope_) {
            env_.push_scope();
            for (auto& [name, val] : *inst->library_scope_)
                env_.define(name, val);
        }
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
                env_.define(static_cast<const parser::assignment&>(*stmt).name_, v);  // pre-define for self-reference
            } else if (stmt->type_ == parser::node_t::expression_stmt) {
                auto& es = static_cast<const parser::expression_stmt&>(*stmt);
                eval_expr(es.expression_);
            }
        }

        env_.pop_scope();
        if (inst->library_scope_) env_.pop_scope();
    }

    // ── Seed initial output ──────────────────────────────────────────────
    // With init block: run a "first pass" of the update block, but skip
    // any statement whose LHS variable was set by init.  This lets the
    // init block establish the starting values for those variables, while
    // still computing any output variables that were not initialised
    // (e.g. `out` in oscillators).  Init'd variables start their first
    // scheduler tick from the init value on the second tick.
    // Without init block: run the full first update tick immediately.
    if (def.lambda_block_ && def.body_) {
        if (def.lambda_block_->init_) {
            if (inst->library_scope_) {
                env_.push_scope();
                for (auto& [name, val] : *inst->library_scope_)
                    env_.define(name, val);
            }
            env_.push_scope();
            for (auto& [name, val] : inst->params_)
                env_.define(name, val);
            for (auto& [name, val] : inst->current_)
                env_.define(name, val);

            // Collect variable names set by the init block so we can skip
            // their update statements on this first pass.
            std::set<std::string> init_names;
            for (const auto& s : def.lambda_block_->init_->statements_) {
                if (!s) continue;
                if (s->type_ == parser::node_t::function_definition) {
                    auto& idef = static_cast<const parser::function_definition&>(*s);
                    if (idef.parameters_.empty()) init_names.insert(idef.name_);
                } else if (s->type_ == parser::node_t::assignment) {
                    init_names.insert(
                        static_cast<const parser::assignment&>(*s).name_);
                }
            }

            // First pass: run update statements for variables NOT in init
            inst->next_.clear();
            for (const auto& stmt : def.lambda_block_->update_statements_) {
                if (!stmt) continue;
                if (stmt->type_ == parser::node_t::function_definition) {
                    auto& upd = static_cast<const parser::function_definition&>(*stmt);
                    if (upd.parameters_.empty() && upd.body_ &&
                            !init_names.count(upd.name_)) {
                        value v = eval_expr(upd.body_);
                        inst->next_[upd.name_] = v;
                        env_.define(upd.name_, v);
                    }
                } else if (stmt->type_ == parser::node_t::assignment) {
                    auto& assign = static_cast<const parser::assignment&>(*stmt);
                    if (!init_names.count(assign.name_)) {
                        value v = eval_expr(assign.value_);
                        inst->next_[assign.name_] = v;
                        env_.define(assign.name_, v);
                        if (assign.is_emit_)
                            inst->emitted_[assign.name_] = v;
                    }
                } else if (stmt->type_ == parser::node_t::expression_stmt) {
                    auto& es = static_cast<const parser::expression_stmt&>(*stmt);
                    eval_expr(es.expression_);
                }
            }
            inst->commit();

            value output = eval_expr(def.body_);
            inst->write_output(std::move(output));
            env_.pop_scope();
            if (inst->library_scope_) env_.pop_scope();
        } else {
            // No init: run first update immediately (no dt delay)
            tick_instance(*inst, &def);
        }
    }

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
                              const parser::function_definition* def) {
    // ── Re-evaluate dynamic parameter expressions (all instance types) ────
    // This lets dt and other params evolve when driven by temporal sources.
    {
        auto expr_it = instance_param_exprs_.find(inst.id_);
        if (expr_it != instance_param_exprs_.end() && !expr_it->second.empty()) {
            std::vector<std::pair<std::string, value>> param_updates;
            for (const auto& [name, snapshot] : inst.params_) {
                auto pit = expr_it->second.find(name);
                if (pit != expr_it->second.end() && pit->second) {
                    param_updates.push_back({name, eval_expr(pit->second)});
                }
            }
            for (auto& [name, val] : param_updates) {
                if (name == "dt") {
                    double new_dt = val.as_number();
                    if (new_dt != inst.dt_ms_ && new_dt > 0.0) {
                        inst.dt_ms_ = new_dt;
                        if (scheduler_ && inst.subscription_id_ != 0)
                            scheduler_->update_dt(inst.subscription_id_, new_dt);
                    }
                }
                inst.params_[name] = std::move(val);
            }
        }
    }

    // ── Native temporal tick ───────────────────────────────────────────────
    if (inst.native_update_) {
        inst.emitted_.clear();
        value output;
        inst.native_update_(inst.params_, inst.current_, inst.emitted_, output);
        inst.write_output(std::move(output));
        return;  // no commit() needed — native callbacks write current_ directly
    }

    if (!def || !def->lambda_block_) return;

    // If this function came from a namespaced library, push its library-local
    // scope first so internal cross-calls (e.g. lfo → sine_shape) resolve
    // without leaking bare names into global scope.
    if (inst.library_scope_) {
        env_.push_scope();
        for (auto& [name, val] : *inst.library_scope_)
            env_.define(name, val);
    }

    // Build a scope with: parameters (snapshot after re-eval above) + current state
    env_.push_scope();

    for (const auto& [name, val] : inst.params_) {
        env_.define(name, val);
    }
    for (const auto& [name, val] : inst.current_) {
        env_.define(name, val);
    }

    // ── Evaluate update statements (writes go to next_) ────────────────────
    inst.next_.clear();
    inst.emitted_.clear();

    for (const auto& stmt : def->lambda_block_->update_statements_) {
        if (!stmt) continue;
        if (stmt->type_ == parser::node_t::function_definition) {
            auto& upd = static_cast<const parser::function_definition&>(*stmt);
            if (upd.parameters_.empty() && upd.body_) {
                value v = eval_expr(upd.body_);
                inst.next_[upd.name_] = v;
                env_.define(upd.name_, v);
            }
        } else if (stmt->type_ == parser::node_t::assignment) {
            auto& assign = static_cast<const parser::assignment&>(*stmt);
            value v = eval_expr(assign.value_);
            if (assign.name_ == "dt") {
                double new_dt = v.as_number();
                if (new_dt != inst.dt_ms_ && new_dt > 0.0) {
                    inst.dt_ms_ = new_dt;
                    if (scheduler_ && inst.subscription_id_ != 0)
                        scheduler_->update_dt(inst.subscription_id_, new_dt);
                }
            }
            inst.next_[assign.name_] = v;
            env_.define(assign.name_, v);
            if (assign.is_emit_) {
                inst.emitted_[assign.name_] = v;
            }
        } else if (stmt->type_ == parser::node_t::expression_stmt) {
            auto& es = static_cast<const parser::expression_stmt&>(*stmt);
            eval_expr(es.expression_);
        }
    }

    // ── Evaluate output expression ─────────────────────────────────────────
    if (def->body_) {
        value output = eval_expr(def->body_);
        inst.write_output(std::move(output));
    }

    env_.pop_scope();
    if (inst.library_scope_) env_.pop_scope();

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
// Parametric flow evaluation
// ════════════════════════════════════════════════════════════════════════════════

std::shared_ptr<flow_data> evaluator::eval_flow_members(
        const std::vector<std::shared_ptr<parser::flow_member>>& members) {
    auto fd = std::make_shared<flow_data>();
    for (const auto& member : members) {
        if (!member) continue;
        flow_member fm;
        fm.name_ = member->name_;
        if (member->value_) {
            if (member->value_->type_ == parser::node_t::flow_literal_expr) {
                auto& fle = static_cast<const parser::flow_literal_expr&>(*member->value_);
                if (fle.flow_) {
                    for (const auto& elem : fle.flow_->elements_)
                        fm.elements_.push_back(eval_expr(elem));
                }
            } else if (member->value_->type_ == parser::node_t::generator_expr_node) {
                auto& gn = static_cast<const parser::generator_expr_node&>(*member->value_);
                if (gn.generator_) {
                    auto& gen = *gn.generator_;
                    int s = static_cast<int>(eval_expr(gen.range_start_).as_number());
                    int n = static_cast<int>(eval_expr(gen.range_end_).as_number());
                    env_.push_scope();
                    for (int i = s; i < n; ++i) {
                        env_.define(gen.variable_, value::number(static_cast<double>(i)));
                        fm.elements_.push_back(eval_expr(gen.body_));
                    }
                    env_.pop_scope();
                }
            } else {
                fm.elements_.push_back(eval_expr(member->value_));
            }
        }
        fd->members_.push_back(std::move(fm));
    }
    return fd;
}

// ── flow_cache_key equality and hash ──────────────────────────────────────────

static bool values_equal(const value& a, const value& b) {
    if (a.type_ != b.type_) return false;
    switch (a.type_) {
        case value_t::number:
        case value_t::time:    return a.number_ == b.number_;
        case value_t::trigger: return a.trigger_ == b.trigger_;
        case value_t::handle:  return a.handle_  == b.handle_;
        case value_t::string:
            if (!a.string_ && !b.string_) return true;
            if (!a.string_ || !b.string_) return false;
            return *a.string_ == *b.string_;
        default:               return true;  // nil == nil, function refs not used as args
    }
}

bool evaluator::flow_cache_key::operator==(const flow_cache_key& o) const {
    if (name != o.name) return false;
    if (args.size() != o.args.size()) return false;
    for (size_t i = 0; i < args.size(); ++i)
        if (!values_equal(args[i], o.args[i])) return false;
    if (named.size() != o.named.size()) return false;
    for (const auto& [k, v] : named) {
        auto it = o.named.find(k);
        if (it == o.named.end() || !values_equal(v, it->second)) return false;
    }
    return true;
}

std::size_t evaluator::flow_cache_key_hash::operator()(const flow_cache_key& k) const {
    std::size_t h = std::hash<std::string>{}(k.name);
    for (const auto& v : k.args) {
        std::size_t vh = 0;
        vh ^= std::hash<int>{}(static_cast<int>(v.type_)) + 0x9e3779b9 + (vh << 6);
        vh ^= std::hash<double>{}(v.number_)               + 0x9e3779b9 + (vh << 6);
        vh ^= std::hash<bool>{}(v.trigger_)                + 0x9e3779b9 + (vh << 6);
        h  ^= vh + 0x9e3779b9 + (h << 6) + (h >> 2);
    }
    for (const auto& [name, v] : k.named) {
        std::size_t vh = std::hash<std::string>{}(name);
        vh ^= std::hash<double>{}(v.number_) + 0x9e3779b9 + (vh << 6);
        h  ^= vh + 0x9e3779b9 + (h << 6) + (h >> 2);
    }
    return h;
}

value evaluator::eval_flow_call(const parser::flow_definition& def,
                                const std::vector<value>& args,
                                const named_args_t& named,
                                const std::vector<parser::expr_ptr>& pos_exprs,
                                const named_exprs_t& named_exprs) {
    // Phase 2 hook: pos_exprs / named_exprs reserved for detecting temporal
    // arguments and registering reactive bindings.

    // Look up in cache. Identical arguments → same flow_data → cursors persist
    // across reactive-chain re-evaluations. Changed arguments → new entry.
    flow_cache_key key{def.name_, args, named};

    auto cached = flow_call_cache_.find(key);
    if (cached != flow_call_cache_.end())
        return cached->second;

    // Build the flow.
    env_.push_scope();

    for (size_t i = 0; i < def.parameters_.size() && i < args.size(); ++i)
        env_.define(def.parameters_[i]->name_, args[i]);
    for (const auto& [pname, val] : named)
        env_.define(pname, val);
    for (const auto& param : def.parameters_) {
        if (!param || env_.lookup(param->name_)) continue;
        if (param->default_value_)
            env_.define(param->name_, eval_expr(param->default_value_));
    }

    value result;
    result.type_ = value_t::flow;
    result.flow_ = eval_flow_members(def.members_);

    env_.pop_scope();

    // Evict oldest entry if cache is full.
    if (flow_call_cache_.size() >= FLOW_CACHE_MAX) {
        flow_call_cache_.erase(flow_cache_order_.front());
        flow_cache_order_.pop_front();
    }
    flow_cache_order_.push_back(key);
    flow_call_cache_[std::move(key)] = result;
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

// ════════════════════════════════════════════════════════════════════════════════
// Process start / stop (listen mode)
// ════════════════════════════════════════════════════════════════════════════════

bool evaluator::start_process(const std::string& name) {
    auto it = stored_processes_.find(name);
    if (it == stored_processes_.end()) {
        std::cout << "Process '" << name << "' not found.\n";
        return false;
    }

    // If already running, stop first
    if (active_process_instances_.count(name))
    {
        std::cout << "Process '" << name << "' is already running, restarting it.\n";
        stop_process(name);
    }

    // Temporarily disable listen mode so exec_stmt actually runs the block
    auto saved_filter = process_filter_;
    auto saved_listen = listen_mode_;
    process_filter_ = name;
    listen_mode_ = false;

    // Execute the process block statements to start the process.  
    std::cout << "Starting process '" << name << "'.\n";
    exec_stmt(std::static_pointer_cast<parser::statement>(it->second));

    process_filter_ = saved_filter;
    listen_mode_ = saved_listen;
    return true;
}

bool evaluator::stop_process(const std::string& name) {
    auto it = active_process_instances_.find(name);
    if (it == active_process_instances_.end()) return false;

    for (uint64_t inst_id : it->second) {
        auto inst_it = instances_.find(inst_id);
        if (inst_it == instances_.end()) continue;

        auto& inst = inst_it->second;
        inst->active_ = false;

        if (scheduler_ && inst->subscription_id_ != 0)
            scheduler_->unsubscribe(inst->subscription_id_);
    }

    active_process_instances_.erase(it);
    return true;
}

std::vector<std::string> evaluator::list_stored_processes() const {
    std::vector<std::string> names;
    names.reserve(stored_processes_.size());
    for (const auto& [name, _] : stored_processes_)
        names.push_back(name);
    return names;
}

} // --- idyl::core ---
