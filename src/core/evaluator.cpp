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
#include <sstream>
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
                if (e.access_->index_) {
                    // Index access: the accessor drives the evaluation rate.
                    // Only collect IDs from the index so that temporal variables
                    // inside the flow expression (e.g. `mod` in `f(i+mod)[cnt]`)
                    // do not pull the reaction onto a faster segment — the flow
                    // contents are sampled at the accessor's rate, not recomputed
                    // independently for every dependency change.
                    collect_expr_ids(e.access_->index_, out);
                } else {
                    // Member access (`flow.member`): no accessor expression;
                    // timing is driven by the flow itself.
                    collect_expr_ids(e.access_->flow_, out);
                }
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
        case parser::node_t::on_block: {
            auto& ob = static_cast<const parser::on_block&>(*stmt);
            collect_expr_ids(ob.trigger_expr_, out);
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

        // Otherwise: true function definition — register for call-time evaluation.
        auto ast_ptr = std::static_pointer_cast<parser::function_definition>(
            std::const_pointer_cast<parser::node>(
                std::shared_ptr<const parser::node>(stmt, &def)));

        if (env_.scopes_.size() > 1) {
            // Local scope (process block, init block, update block):
            // store AST in the value itself — do NOT write to function_defs_.
            env_.define(def.name_, value::local_function(def.name_, ast_ptr));
        } else {
            // Global scope: register in function_defs_ as usual.
            env_.define(def.name_, value::function_ref(def.name_));
            function_defs_[def.name_] = ast_ptr;
        }
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

        // Always store named process ASTs so hot_reload() can diff against them.
        if (!proc.name_.empty()) {
            stored_processes_[proc.name_] =
                std::static_pointer_cast<parser::process_block>(
                    std::const_pointer_cast<parser::node>(
                        std::shared_ptr<const parser::node>(stmt, &proc)));
        }

        // ── Listen mode: store blocks for OSC start, pre-start filtered ones ──
        if (listen_mode_) {
            bool pre_start = !process_filter_.empty() && proc.name_ == process_filter_;
            if (!pre_start) {
                break;  // stored above; wait for OSC start command
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

        std::vector<live_segment> segments;
        live_segment* current_seg = nullptr;
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
                auto rxn = std::make_shared<reaction_set>();
                rxn->binding_stmt = s;
                auto inst_it = instances_.find(inst_id);
                double seg_dt  = inst_it != instances_.end() ? inst_it->second->dt_ms_  : 0.0;
                std::string dn = inst_it != instances_.end() ? inst_it->second->def_name_ : "";
                // Record ALL instance IDs created by this statement for retick_pool_
                std::vector<uint64_t> all_ids;
                for (uint64_t id = id_before; id < next_instance_id_; ++id)
                    all_ids.push_back(id);
                live_segment seg_entry;
                seg_entry.instance_id      = inst_id;
                seg_entry.all_instance_ids = std::move(all_ids);
                seg_entry.bound_var        = binding_name;
                seg_entry.def_name         = dn;
                seg_entry.dt_ms            = seg_dt;
                seg_entry.rxn              = rxn;
                segments.push_back(std::move(seg_entry));
                current_seg = &segments.back();
                // Register binding so the :: accessor can find this instance
                if (!binding_name.empty())
                    instance_bindings_[binding_name] = inst_id;
            } else if (current_seg) {
                // Statement follows a temporal binding → reaction
                current_seg->rxn->reactions.push_back(s);
            }
        }

        // ── Link catch blocks to their source segments ─────────────────────
        // Named: catch timer::sig  — links to the segment whose bound_var == "timer".
        // Anonymous (experimental): catch metro(dt=500ms)::sig  — evaluates the
        // instance expression, subscribes it to the scheduler, and attaches the
        // handler to that new instance's reaction_set.  The instance lives for the
        // lifetime of the process (tracked in active_process_instances_).
        for (auto& cb : catch_blocks) {
            if (!cb->instance_expr_) continue;

            // Determine whether this is a named or anonymous instance.
            std::string source_var;
            if (cb->instance_expr_->type_ == parser::node_t::identifier_expr) {
                auto& ie = static_cast<const parser::identifier_expr&>(*cb->instance_expr_);
                if (ie.identifier_) source_var = ie.identifier_->name_;
            }

            bool matched_named = false;
            if (!source_var.empty()) {
                for (auto& seg : segments) {
                    if (seg.bound_var == source_var) {
                        seg.rxn->catches.push_back({cb->signal_name_, cb->handler_, false});
                        matched_named = true;
                        break;
                    }
                }
            }

            // Anonymous instance: evaluate the call expression to create the instance.
            if (!matched_named) {
                uint64_t id_before = next_instance_id_;
                eval_expr(cb->instance_expr_);
                if (next_instance_id_ <= id_before) continue; // didn't create an instance

                uint64_t anon_id  = next_instance_id_ - 1;
                auto inst_it      = instances_.find(anon_id);
                if (inst_it == instances_.end()) continue;
                auto anon_inst    = inst_it->second;

                auto rxn = std::make_shared<reaction_set>();
                rxn->catches.push_back({cb->signal_name_, cb->handler_, false});

                // Track the anonymous instance under the process name so it is
                // cancelled when the process stops or is hot-reloaded.
                if (!proc.name_.empty())
                    active_process_instances_[proc.name_].push_back(anon_id);

                has_temporal = true;

                if (scheduler_ && anon_inst->dt_ms_ > 0.0) {
                    std::shared_ptr<parser::function_definition> def_ptr;
                    if (!anon_inst->native_update_) {
                        def_ptr = anon_inst->local_def_;
                        if (!def_ptr) {
                            std::shared_lock lock(defs_mutex_);
                            auto dit = function_defs_.find(anon_inst->def_name_);
                            if (dit != function_defs_.end()) def_ptr = dit->second;
                        }
                    }
                    auto weak_anon = std::weak_ptr<function_instance>(anon_inst);
                    anon_inst->subscription_id_ = scheduler_->subscribe(anon_inst->dt_ms_,
                        [this, def_ptr, weak_anon, rxn]
                        (double, double) -> bool {
                            auto si = weak_anon.lock();
                            if (!si || !si->active_) return false;

                            tick_instance(si, def_ptr.get());

                            std::lock_guard<std::mutex> lk(rxn->mutex);
                            for (auto& c : rxn->catches) {
                                if (c.fired) continue;
                                // Check emitted_ first; fall back to return value.
                                auto emit_it = si->emitted_.find(c.watched_emit);
                                bool live = (emit_it != si->emitted_.end())
                                            ? emit_it->second.is_truthy()
                                            : si->output_.is_truthy();
                                if (live) {
                                    c.fired = true;
                                    for (const auto& h : c.handler) exec_stmt(h);
                                    return false; // one-shot
                                }
                            }
                            return true;
                        });
                }
            }
        }

        // ── Redistribute reactions to the fastest segment they depend on ──────
        redistribute_reactions(segments);

        // ── Reset stale trigger bindings after setup ──────────────────────────
        // The setup loop above calls exec_stmt() for every statement, including
        // reactions like print(m1, m2).  Temporal instantiation sets the env
        // binding to trigger(true) as its initial output.  The setup loop's
        // print call produces the t=0 "both firing" line correctly.  After that
        // we reset all trigger bindings to rest so the first scheduler tick
        // starts from a clean state: the scheduler will set them trigger only
        // when their instance actually fires.
        if (has_temporal) {
            for (const auto& seg : segments) {
                if (!seg.bound_var.empty()) {
                    value* v = env_.lookup(seg.bound_var);
                    if (v && v->type_ == value_t::trigger)
                        *v = value::rest();
                }
            }
            epoch_flush_pending_ = false;
        }

        // Subscribe each segment's temporal source to the scheduler.
        // The closure captures rxn by shared_ptr — not by value — so hot_reload()
        // can swap the reaction content without re-subscribing or causing a gap.
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
                    def_ptr = inst->local_def_;
                    if (!def_ptr) {
                        auto def_it = function_defs_.find(inst->def_name_);
                        if (def_it == function_defs_.end()) continue;
                        def_ptr = def_it->second;
                    }
                }

                std::string var  = seg.bound_var;
                auto rxn         = seg.rxn;   // shared_ptr — live read each tick
                auto weak_inst   = std::weak_ptr<function_instance>(inst);
                auto all_ids     = seg.all_instance_ids;

                inst->subscription_id_ = scheduler_->subscribe(inst->dt_ms_,
                    [this, def_ptr, weak_inst, var, rxn, all_ids,
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

                        // (epoch-transition flush removed — see epoch_flush below)

                        tick_instance(si, def_ptr.get());

                        // Snapshot reactions under lock so that hot_reload()
                        // can safely replace them on the main thread.
                        parser::stmt_ptr binding_snap;
                        std::vector<parser::stmt_ptr> reactions_snap;
                        std::vector<parser::stmt_ptr> shared_snap;
                        {
                            std::lock_guard<std::mutex> lk(rxn->mutex);
                            binding_snap   = rxn->binding_stmt;
                            reactions_snap = rxn->reactions;
                            shared_snap    = rxn->shared_reactions;
                        }

                        // Tick all other instances created by the same binding
                        // statement (e.g. print(a(), b(), c()) creates 3 instances;
                        // only the last one drives the scheduler, but all must advance).
                        retick_pool_.clear();
                        for (uint64_t id : all_ids) {
                            auto it = instances_.find(id);
                            if (it == instances_.end()) continue;
                            auto& pool_inst = it->second;
                            if (pool_inst.get() == si.get() || !pool_inst->active_) continue;

                            std::shared_ptr<parser::function_definition> pool_def;
                            if (pool_inst->local_def_) {
                                pool_def = pool_inst->local_def_;
                            } else {
                                std::shared_lock<std::shared_mutex> rl(defs_mutex_);
                                auto def_it = function_defs_.find(pool_inst->def_name_);
                                if (def_it != function_defs_.end()) pool_def = def_it->second;
                            }
                            tick_instance(pool_inst, pool_def.get());
                            retick_pool_.push_back(pool_inst.get());
                        }

                        retick_instance_ = si.get();
                        if (binding_snap) exec_stmt(binding_snap);
                        retick_instance_ = nullptr;
                        retick_pool_.clear();

                        // Run segment-local reactions immediately.
                        for (const auto& r : reactions_snap) exec_stmt(r);

                        // Queue shared reactions for the end of this epoch.
                        // Dedup by AST pointer so they fire exactly once even
                        // when multiple segments fire at the same scheduler time.
                        for (const auto& r : shared_snap) {
                            if (r && epoch_deferred_dedup_.insert(r.get()).second)
                                epoch_deferred_.push_back(r);
                        }

                        // Catches: lock to guard against concurrent clear()
                        // during hot swap.  Handlers execute while holding the
                        // lock — they are short statements and cannot call
                        // hot_reload, so no deadlock risk.
                        bool should_end = false;
                        {
                            std::lock_guard<std::mutex> lk(rxn->mutex);
                            for (auto& c : rxn->catches) {
                                if (c.fired) continue;
                                if (c.watched_emit == "end") continue;

                                auto emit_it = si->emitted_.find(c.watched_emit);
                                if (emit_it != si->emitted_.end() && emit_it->second.is_truthy()) {
                                    c.fired = true;
                                    for (const auto& h : c.handler)
                                        exec_stmt(h);
                                }
                            }
                        }

                        // ── Defer trigger reset to end of epoch ─────────────
                        if (!var.empty()) {
                            value* bound = env_.lookup(var);
                            if (bound && bound->type_ == value_t::trigger)
                                epoch_reset_vars_.push_back(var);
                        }

                        // ── Schedule epoch flush (0.1ms one-shot) ────────────
                        // Fires after all same-time callbacks complete, giving
                        // shared reactions a fully-consistent snapshot.
                        if (!epoch_flush_pending_) {
                            epoch_flush_pending_ = true;
                            scheduler_->subscribe(0.1, [this](double, double) -> bool {
                                for (const auto& r : epoch_deferred_)
                                    exec_stmt(r);
                                epoch_deferred_.clear();
                                epoch_deferred_dedup_.clear();
                                for (const auto& vname : epoch_reset_vars_) {
                                    value* v = env_.lookup(vname);
                                    if (v) *v = value::rest();
                                }
                                epoch_reset_vars_.clear();
                                epoch_flush_pending_ = false;
                                return false;
                            });
                        }

                        return !should_end;
                    });
            }
        }

        // Track which instances belong to this process.
        // active_process_instances_ keeps backward compat for stop_process.
        // live_processes_ is the authoritative record for named processes
        // and enables hot reload diffing.
        if (!proc.name_.empty() && has_temporal) {
            auto& ids = active_process_instances_[proc.name_];
            for (const auto& seg : segments)
                ids.push_back(seg.instance_id);

            live_process lp;
            lp.name        = proc.name_;
            lp.ast         = stored_processes_.count(proc.name_)
                                 ? stored_processes_[proc.name_] : nullptr;
            lp.segments    = segments;   // copies shared_ptr<reaction_set> per segment
            lp.scope_depth = env_.scopes_.size() - 1;
            live_processes_[proc.name_] = std::move(lp);
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

    // ── on expr: { handler } → conditional reaction ────────────────────────
    // Evaluates the trigger expression; runs the body only when it is a live
    // trigger.  Executed as a normal reaction on every tick of its driving
    // segment — redistribution places it in the right segment automatically.
    case parser::node_t::on_block: {
        auto& ob = static_cast<const parser::on_block&>(*stmt);
        if (!ob.trigger_expr_) break;
        value guard = eval_expr(ob.trigger_expr_);
        if (guard.type_ == value_t::trigger && guard.trigger_) {
            for (const auto& h : ob.handler_)
                exec_stmt(h);
        }
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
                        return resolve_flow_element(m, 0);
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

            // Per-call-site cursors: each index expression in the source owns
            // an independent cursor map so multiple reads of the same flow
            // (polyphony) never interfere with each other.
            auto& site_cursor_ptr = flow_site_cursors_[fae.access_.get()];
            if (!site_cursor_ptr)
                site_cursor_ptr = std::make_shared<std::unordered_map<std::string, int>>();
            auto& cursors = *site_cursor_ptr;

            // ── Single-member flow: return element directly ────────────────
            if (flow.flow_->members_.size() == 1) {
                auto& elems = flow.flow_->members_[0].elements_;
                int n = static_cast<int>(elems.size());
                const std::string& key = flow.flow_->members_[0].name_;

                const auto& member0 = flow.flow_->members_[0];
                if (is_trigger) {
                    int& cur = cursors[key];
                    if (idx.trigger_) {
                        int pos = cur % n;
                        cur = (cur + 1) % n;
                        return resolve_flow_element(member0, pos);
                    }
                    return resolve_flow_element(member0, cur % n);
                }
                if (is_float) {
                    double t = std::fmod(v, 1.0);
                    if (t < 0.0) t += 1.0;
                    int i = static_cast<int>(std::floor(t * n)) % n;
                    return resolve_flow_element(member0, i);
                }
                // Integer with wrapping
                int i = ((int_idx % n) + n) % n;
                return resolve_flow_element(member0, i);
            }

            // ── Multi-member flow: return a row slice ──────────────────────
            // Each member is indexed independently based on its own size.
            auto fd = std::make_shared<flow_data>();
            // Track resolved elements by name so gated members can read the
            // gate member's output for this tick (members are processed in order).
            std::unordered_map<std::string, value> resolved_this_tick;
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
                    // Per-member cursor: each member wraps independently.
                    // A gated member (on <gate>) only advances when the gate
                    // member's element for this tick is a live trigger.
                    int& cur = cursors[m.name_];
                    bool should_advance = idx.trigger_;
                    if (should_advance && !m.gate_name_.empty()) {
                        auto git = resolved_this_tick.find(m.gate_name_);
                        should_advance = (git != resolved_this_tick.end() &&
                                          git->second.type_ == value_t::trigger &&
                                          git->second.trigger_);
                    }
                    if (should_advance) {
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

                value elem = resolve_flow_element(m, mi);
                if (!m.name_.empty())
                    resolved_this_tick[m.name_] = elem;
                row_m.elements_.push_back(std::move(elem));
                row_m.live_exprs_.push_back(nullptr);
                row_m.live_deps_.push_back(nullptr);
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
                    if (speculative_exec_) return value::number(0.0);
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
    //    For local functions, the value carries the AST directly.
    //    For closures, it also carries a reference to the owning instance.
    std::shared_ptr<parser::function_definition> local_fn_def;
    std::shared_ptr<function_instance> closure_inst;
    if (auto* var = env_.lookup(fn_name)) {
        if (var->type_ == value_t::function && var->string_) {
            fn_name = *var->string_;
            if (var->fn_def_) local_fn_def = var->fn_def_;
            if (var->closure_inst_) closure_inst = var->closure_inst_;
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

    // ── Clock handle callable ───────────────────────────────────────────────
    // If fn_name resolves to a clock handle in the environment, treat the call
    // as a beat/duration query on that specific clock.
    //   first_clock(2b)   → 2 * (60000 / clock_bpm) ms
    //   first_clock(500ms)→ 500 ms  (pass-through of evaluated ms value)
    //   first_clock()     → 1 beat duration at clock_bpm
    if (auto* var_val = env_.lookup(fn_name)) {
        if (var_val->type_ == value_t::handle) {
            uint64_t clock_id = static_cast<uint64_t>(var_val->as_handle());
            double clock_bpm = clocks_.bpm(clock_id);
            double beat_ms = (clock_bpm > 0.0) ? (60000.0 / clock_bpm) : 500.0;

            // Try to intercept raw beat literal before it was converted with main BPM
            if (!pos_exprs.empty()) {
                const auto* raw = pos_exprs[0].get();
                if (raw && raw->type_ == parser::node_t::literal_expr) {
                    const auto& le = static_cast<const parser::literal_expr&>(*raw);
                    if (le.literal_ && le.literal_->type_ == parser::node_t::time_literal) {
                        const auto& tl = static_cast<const parser::time_literal&>(*le.literal_);
                        if (tl.unit_ == "b") {
                            double beats = std::stod(tl.value_);
                            return value::number(beats * beat_ms);
                        }
                    }
                }
            }
            // Non-beat arg: return the evaluated value as-is (already in ms)
            if (!args.empty())
                return value::number(args[0].as_number());
            // No args: return 1 beat duration
            return value::number(beat_ms);
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
        // tempo(handle)        → return BPM of a specific clock
        // tempo(bpm)           → set main clock BPM, propagate
        // tempo(handle, bpm)   → set specific clock BPM, propagate
        if (args.empty()) {
            return value::number(clocks_.main_bpm());
        } else if (args.size() == 1) {
            if (args[0].type_ == value_t::handle) {
                // Query BPM of the given clock handle
                uint64_t clock_id = static_cast<uint64_t>(args[0].as_handle());
                return value::number(clocks_.bpm(clock_id));
            }
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
        // During speculative exec (hot-reload scan pass) suppress side-effecting
        // module calls (e.g. cs_note) — they only need to return a dummy value.
        if (speculative_exec_) return value::number(0.0);
        return eval_module_fn(*mf, args, call.line_, call.column_);
    }

    // ── Check user-defined functions ───────────────────────────────────────
    // Local function (defined in process/init/update scope): AST is carried
    // in the value directly — no lookup in function_defs_.
    if (local_fn_def) {
        return eval_user_function(*local_fn_def, args, named, fn_name, pos_exprs, named_exprs, &call, local_fn_def, closure_inst);
    }
    auto it = function_defs_.find(fn_name);
    if (it != function_defs_.end() && it->second) {
        // Pass fn_name as the qualified_key so temporal instances get the right
        // def_name_ (the key that function_defs_ and fn_library_scope_ use).
        return eval_user_function(*it->second, args, named, fn_name, pos_exprs, named_exprs, &call);
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
    // Retick guard: if we're inside a scheduler callback for this instance
    // (or re-evaluating a compound flow slot whose dep is this function),
    // return the current output without creating a new instance.
    if (retick_instance_ && retick_instance_->def_name_ == entry.name_)
        return retick_instance_->read_output();
    for (auto* p : retick_pool_) {
        if (p && p->def_name_ == entry.name_)
            return p->read_output();
    }

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
    last_instantiated_ = inst;
    return inst->read_output();
}

value evaluator::eval_user_function(const parser::function_definition& def,
                                    const std::vector<value>& args,
                                    const named_args_t& named,
                                    const std::string& qualified_key,
                                    const std::vector<parser::expr_ptr>& pos_exprs,
                                    const named_exprs_t& named_exprs,
                                    const parser::function_call* call_site,
                                    std::shared_ptr<parser::function_definition> local_def,
                                    std::shared_ptr<function_instance> closure_inst) {
    // ── Scan parameters for trigger / dt flags ─────────────────────────────
    bool has_trigger_param = false;
    bool has_dt_param      = false;
    for (const auto& p : def.parameters_) {
        if (!p) continue;
        if (p->is_trigger_parameter_)                    has_trigger_param = true;
        if (p->name_ == "dt" || p->has_default_time_)   has_dt_param = true;
    }

    // ── Trigger-driven function ────────────────────────────────────────────
    // A function with trigger parameter(s) and no dt is gated by the trigger:
    //  • If any trigger param is rest  → short-circuit, return rest.
    //  • If trigger is live            → run lambda update block (if any),
    //                                    evaluate body, return result.
    // This keeps the function as a plain *reaction* in the process block
    // (no temporal instance, no scheduler subscription) while still
    // allowing it to maintain per-call-site state via trigger_driven_instances_.
    if (has_trigger_param && !has_dt_param) {
        // Bind all parameters into a fresh scope
        env_.push_scope();
        size_t pos_idx = 0;
        for (const auto& p : def.parameters_) {
            if (!p) continue;
            value v;
            auto nit = named.find(p->name_);
            if (nit != named.end()) {
                v = nit->second;
            } else if (pos_idx < args.size()) {
                v = args[pos_idx++];
            } else if (p->default_value_) {
                v = eval_expr(p->default_value_);
            } else {
                v = value::number(0.0);
            }
            env_.define(p->name_, v);
        }

        // Check whether every trigger parameter is currently live
        bool any_trigger_live = false;
        for (const auto& p : def.parameters_) {
            if (!p || !p->is_trigger_parameter_) continue;
            value* vp = env_.lookup(p->name_);
            if (vp && vp->type_ == value_t::trigger && vp->trigger_) {
                any_trigger_live = true;
                break;
            }
        }

        // Trigger is rest → short-circuit without running body / update block
        if (!any_trigger_live) {
            env_.pop_scope();
            return value::rest();
        }

        // Trigger is live — handle stateful lambda update block if present
        if (def.lambda_block_) {
            // Find or create the persistent instance for this call site
            std::shared_ptr<function_instance> tdi;
            if (call_site) {
                auto it = trigger_driven_instances_.find(call_site);
                if (it == trigger_driven_instances_.end()) {
                    // First firing: create instance for state, run init block
                    tdi = std::make_shared<function_instance>();
                    tdi->id_       = 0; // not scheduler-tracked
                    tdi->active_   = true;
                    tdi->def_name_ = qualified_key.empty() ? def.name_ : qualified_key;

                    // Bind current param values into instance
                    pos_idx = 0;
                    for (const auto& p : def.parameters_) {
                        if (!p) continue;
                        value* vp = env_.lookup(p->name_);
                        tdi->params_[p->name_] = vp ? *vp : value::number(0.0);
                    }

                    // Run init block
                    if (def.lambda_block_->init_) {
                        for (const auto& stmt : def.lambda_block_->init_->statements_) {
                            if (!stmt) continue;
                            if (stmt->type_ == parser::node_t::assignment) {
                                auto& a = static_cast<const parser::assignment&>(*stmt);
                                value v = eval_expr(a.value_);
                                tdi->current_[a.name_] = v;
                                env_.define(a.name_, v);
                            } else if (stmt->type_ == parser::node_t::function_definition) {
                                auto& fd = static_cast<const parser::function_definition&>(*stmt);
                                if (fd.parameters_.empty() && fd.body_) {
                                    value v = eval_expr(fd.body_);
                                    tdi->current_[fd.name_] = v;
                                    env_.define(fd.name_, v);
                                }
                            }
                        }
                    }
                    trigger_driven_instances_[call_site] = tdi;
                } else {
                    tdi = it->second;
                    // Restore persisted state into current scope
                    for (const auto& [n, v] : tdi->current_)
                        env_.define(n, v);
                }
            }

            // Run the update block (trigger fired)
            if (tdi) tdi->next_.clear();
            for (const auto& stmt : def.lambda_block_->update_statements_) {
                if (!stmt) continue;
                if (stmt->type_ == parser::node_t::expression_stmt) {
                    eval_expr(static_cast<const parser::expression_stmt&>(*stmt).expression_);
                } else if (stmt->type_ == parser::node_t::assignment) {
                    auto& a = static_cast<const parser::assignment&>(*stmt);
                    value v = eval_expr(a.value_);
                    if (tdi) { tdi->next_[a.name_] = v; }
                    env_.define(a.name_, v);
                    if (a.is_emit_ && tdi) tdi->emitted_[a.name_] = v;
                } else if (stmt->type_ == parser::node_t::function_definition) {
                    auto& upd = static_cast<const parser::function_definition&>(*stmt);
                    if (upd.parameters_.empty() && upd.body_) {
                        value v = eval_expr(upd.body_);
                        if (tdi) tdi->next_[upd.name_] = v;
                        env_.define(upd.name_, v);
                    }
                }
            }
            if (tdi) tdi->commit();
        }

        // Evaluate body
        value result = def.body_ ? eval_expr(def.body_) : value::rest();
        env_.pop_scope();
        return result;
    }

    // ── Standard temporal function: has |> lambda block → instantiate ──────
    if (def.lambda_block_) {
        return instantiate_temporal(def, args, named, qualified_key, pos_exprs, named_exprs, local_def);
    }

    // ── Constant temporal: no lambda block but has dt parameter ────────────
    // e.g. trig(dt=100ms) = ! — must be scheduled for periodic ticking.
    if (has_dt_param) {
        return instantiate_temporal(def, args, named, qualified_key, pos_exprs, named_exprs, local_def);
    }

    if (!def.body_) return value::nil();

    // ── Pure function evaluation ────────────────────────────────────────────
    // For closures: push the owning instance's params and state first so the
    // body can read them by reference (sees current values, not a snapshot).
    if (closure_inst) {
        env_.push_scope();
        for (const auto& [n, v] : closure_inst->params_)
            env_.define(n, v);
        for (const auto& [n, v] : closure_inst->current_)
            env_.define(n, v);
    }
    env_.push_scope();

    size_t pos_idx = 0;
    for (size_t i = 0; i < def.parameters_.size(); ++i) {
        if (!def.parameters_[i]) continue;
        const auto& name = def.parameters_[i]->name_;

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

    value result = eval_expr(def.body_);
    env_.pop_scope();
    if (closure_inst) env_.pop_scope();
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
                                      const named_exprs_t& named_exprs,
                                      std::shared_ptr<parser::function_definition> local_def) {
    // ── Retick mode: return existing instance output ────────────────────
    // During a scheduler callback, the binding expression is re-evaluated.
    // The temporal call (e.g. lfo(1hz)) must return the already-ticked
    // instance output rather than creating a duplicate instance.
    // The canonical key for this function in function_defs_ is the qualified
    // name when loaded from a namespaced library, otherwise def.name_.
    const std::string& canon_key = qualified_key.empty() ? def.name_ : qualified_key;

    if (retick_instance_ && retick_instance_->def_name_ == canon_key)
        return retick_instance_->read_output();
    for (auto* p : retick_pool_) {
        if (p && p->def_name_ == canon_key)
            return p->read_output();
    }

    auto inst = std::make_shared<function_instance>();
    inst->id_ = next_instance_id_++;
    inst->active_ = true;
    // Store the qualified key so the process-block subscription lookup finds
    // the right entry in function_defs_ even for namespaced library functions.
    inst->def_name_ = canon_key;
    // For local functions, carry the AST directly so schedule_instance can
    // tick without looking up function_defs_.
    if (local_def) inst->local_def_ = local_def;
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
        env_.define("age", value::number(0.0));
        for (auto& [name, val] : inst->params_) {
            env_.define(name, val);
        }

        for (const auto& stmt : def.lambda_block_->init_->statements_) {
            if (!stmt) continue;
            if (stmt->type_ == parser::node_t::function_definition) {
                auto& init_def = static_cast<const parser::function_definition&>(*stmt);
                if (init_def.parameters_.empty() && !init_def.lambda_block_ && init_def.body_) {
                    // Constant binding: counter_value = 0
                    value v = eval_expr(init_def.body_);
                    inst->current_[init_def.name_] = v;
                    env_.define(init_def.name_, v);
                } else if (!init_def.parameters_.empty() || init_def.lambda_block_) {
                    // Parametric local function — stored as a closure so callers
                    // outside the tick scope can still read params/state by reference.
                    auto fn_ptr = std::static_pointer_cast<parser::function_definition>(
                        std::const_pointer_cast<parser::statement>(stmt));
                    auto fn_val = value::closure(init_def.name_, fn_ptr, inst);
                    inst->current_[init_def.name_] = fn_val;
                    env_.define(init_def.name_, fn_val);
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
            env_.define("age", value::number(inst->age_ms_));
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
                    } else if (!upd.parameters_.empty() || upd.lambda_block_) {
                        // Tick-local alias — stored as a closure so the body can
                        // read current instance state via the scope chain.
                        auto fn_ptr = std::static_pointer_cast<parser::function_definition>(
                            std::const_pointer_cast<parser::statement>(stmt));
                        env_.define(upd.name_, value::closure(upd.name_, fn_ptr, inst));
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
            inst->age_ms_ += inst->dt_ms_;
        } else {
            // No init: run first update immediately (no dt delay)
            tick_instance(inst, &def);
        }
    }

    // Constant temporal (no lambda block): evaluate body for initial output.
    // age_ms_ is incremented so the first scheduler tick sees age = dt, not 0 again.
    if (!def.lambda_block_ && def.body_) {
        env_.push_scope();
        env_.define("age", value::number(0.0));
        for (auto& [name, val] : inst->params_) {
            env_.define(name, val);
        }
        value output = eval_expr(def.body_);
        inst->write_output(std::move(output));
        env_.pop_scope();
        inst->age_ms_ += inst->dt_ms_;
    }

    // ── Store the instance ─────────────────────────────────────────────────
    instances_[inst->id_] = inst;
    last_instantiated_ = inst;

    // NOTE: subscription is NOT done here — either the process block handler
    // subscribes after collecting the segment's reaction statements, or
    // auto_schedule_flow() subscribes flow-owned instances after flow construction.

    // Return the initial output value
    return inst->read_output();
}

// ════════════════════════════════════════════════════════════════════════════════
// Temporal function tick — evaluate one update cycle
// ════════════════════════════════════════════════════════════════════════════════

void evaluator::tick_instance(std::shared_ptr<function_instance> inst_ptr,
                              const parser::function_definition* def) {
    function_instance& inst = *inst_ptr;
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

    // ── Compact temporal (no lambda block): re-evaluate body each tick ────────
    if (!def || !def->lambda_block_) {
        if (!def || !def->body_) return;
        env_.push_scope();
        env_.define("age", value::number(inst.age_ms_));
        for (const auto& [name, val] : inst.params_)
            env_.define(name, val);
        value output = eval_expr(def->body_);
        inst.write_output(std::move(output));
        env_.pop_scope();
        inst.age_ms_ += inst.dt_ms_;
        return;
    }

    // If this function came from a namespaced library, push its library-local
    // scope first so internal cross-calls (e.g. lfo → sine_shape) resolve
    // without leaking bare names into global scope.
    if (inst.library_scope_) {
        env_.push_scope();
        for (auto& [name, val] : *inst.library_scope_)
            env_.define(name, val);
    }

    // Build a scope with: age + parameters (snapshot after re-eval above) + current state
    env_.push_scope();

    env_.define("age", value::number(inst.age_ms_));
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
            if (upd.parameters_.empty() && !upd.lambda_block_ && upd.body_) {
                // Bare assignment (0-param constant binding)
                value v = eval_expr(upd.body_);
                inst.next_[upd.name_] = v;
                env_.define(upd.name_, v);
            } else if (!upd.parameters_.empty() || upd.lambda_block_) {
                // Tick-local alias — stored as a closure so escaped values see
                // current instance state when called.
                auto fn_ptr = std::static_pointer_cast<parser::function_definition>(
                    std::const_pointer_cast<parser::statement>(stmt));
                auto fn_val = value::closure(upd.name_, fn_ptr, inst_ptr);
                env_.define(upd.name_, fn_val);
                // Not stored in inst.next_ — tick-local, does not persist between ticks
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
    inst.age_ms_ += inst.dt_ms_;
}

// ════════════════════════════════════════════════════════════════════════════════
// Flow construction
// ════════════════════════════════════════════════════════════════════════════════

value evaluator::eval_flow_literal(const parser::flow_literal& fl) {
    auto fd = std::make_shared<flow_data>();
    flow_member fm;

    for (const auto& elem : fl.elements_) {
        last_instantiated_ = nullptr;
        value v = eval_expr(elem);

        if (last_instantiated_) {
            // Element involved a temporal function call.
            if (elem->type_ == parser::node_t::function_call_expr) {
                // Direct call (e.g. sine(5hz, dt=200ms)) → live instance_ref slot.
                fm.elements_.push_back(value::instance_ref(last_instantiated_));
                fm.live_exprs_.push_back(nullptr);
                fm.live_deps_.push_back(nullptr);
            } else {
                // Compound expression (e.g. sine(2hz)*127+64) → live-expr slot.
                // The snapshot value is stored as fallback; the expression is
                // re-evaluated at access time using last_instantiated_ as dep.
                fm.elements_.push_back(v);
                fm.live_exprs_.push_back(
                    std::static_pointer_cast<parser::expression>(elem));
                fm.live_deps_.push_back(last_instantiated_);
            }
        } else {
            fm.elements_.push_back(v);
            fm.live_exprs_.push_back(nullptr);
            fm.live_deps_.push_back(nullptr);
        }
    }

    fd->members_.push_back(std::move(fm));
    auto_schedule_flow(*fd);

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

// Helper: push a single evaluated element (with temporal detection) into fm.
static void push_flow_element(flow_member& fm,
                              const parser::expr_ptr& elem,
                              value v,
                              std::shared_ptr<function_instance> last_inst) {
    if (last_inst) {
        if (elem->type_ == parser::node_t::function_call_expr) {
            fm.elements_.push_back(value::instance_ref(last_inst));
            fm.live_exprs_.push_back(nullptr);
            fm.live_deps_.push_back(nullptr);
        } else {
            fm.elements_.push_back(std::move(v));
            fm.live_exprs_.push_back(
                std::static_pointer_cast<parser::expression>(elem));
            fm.live_deps_.push_back(std::move(last_inst));
        }
    } else {
        fm.elements_.push_back(std::move(v));
        fm.live_exprs_.push_back(nullptr);
        fm.live_deps_.push_back(nullptr);
    }
}

std::shared_ptr<flow_data> evaluator::eval_flow_members(
        const std::vector<std::shared_ptr<parser::flow_member>>& members) {
    auto fd = std::make_shared<flow_data>();
    for (const auto& member : members) {
        if (!member) continue;
        flow_member fm;
        fm.name_      = member->name_;
        fm.gate_name_ = member->gate_name_;
        if (member->value_) {
            if (member->value_->type_ == parser::node_t::flow_literal_expr) {
                auto& fle = static_cast<const parser::flow_literal_expr&>(*member->value_);
                if (fle.flow_) {
                    for (const auto& elem : fle.flow_->elements_) {
                        last_instantiated_ = nullptr;
                        value v = eval_expr(elem);
                        push_flow_element(fm, elem, std::move(v), last_instantiated_);
                    }
                }
            } else if (member->value_->type_ == parser::node_t::generator_expr_node) {
                // Generator expressions: temporal calls inside a range body would
                // create one instance per iteration, which is not meaningful.
                // Evaluate statically (no temporal detection).
                auto& gn = static_cast<const parser::generator_expr_node&>(*member->value_);
                if (gn.generator_) {
                    auto& gen = *gn.generator_;
                    int s = static_cast<int>(eval_expr(gen.range_start_).as_number());
                    int n = static_cast<int>(eval_expr(gen.range_end_).as_number());
                    env_.push_scope();
                    for (int i = s; i < n; ++i) {
                        env_.define(gen.variable_, value::number(static_cast<double>(i)));
                        fm.elements_.push_back(eval_expr(gen.body_));
                        fm.live_exprs_.push_back(nullptr);
                        fm.live_deps_.push_back(nullptr);
                    }
                    env_.pop_scope();
                }
            } else {
                last_instantiated_ = nullptr;
                value v = eval_expr(member->value_);
                push_flow_element(fm, member->value_, std::move(v), last_instantiated_);
            }
        }
        fd->members_.push_back(std::move(fm));
    }
    auto_schedule_flow(*fd);
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
// Flow element resolution and auto-scheduling
// ════════════════════════════════════════════════════════════════════════════════

value evaluator::resolve_flow_element(const flow_member& m, int i) {
    const value& v = m.elements_[i];

    // Direct temporal slot: return current instance output (thread-safe via mutex)
    if (v.type_ == value_t::instance_ref && v.instance_)
        return v.instance_->read_output();

    // Compound temporal slot: re-evaluate the stored expression with a retick
    // guard so the temporal call inside it returns the dep's current output
    // rather than creating a new instance.
    if (i < static_cast<int>(m.live_exprs_.size()) && m.live_exprs_[i]) {
        function_instance* dep =
            (i < static_cast<int>(m.live_deps_.size()) && m.live_deps_[i])
            ? m.live_deps_[i].get() : nullptr;

        function_instance* prev = retick_instance_;
        retick_instance_ = dep;
        value result = eval_expr(
            std::static_pointer_cast<parser::expression>(m.live_exprs_[i]));
        retick_instance_ = prev;
        return result;
    }

    return v;
}

void evaluator::schedule_instance(std::shared_ptr<function_instance> inst) {
    if (!scheduler_ || !inst || inst->dt_ms_ <= 0.0 || inst->subscription_id_ != 0)
        return;

    inst->subscription_id_ = scheduler_->subscribe(inst->dt_ms_,
        [this, weak = std::weak_ptr<function_instance>(inst)]
        (double /*t*/, double /*dt*/) -> bool {
            auto si = weak.lock();
            if (!si || !si->active_) return false;
            const parser::function_definition* def_ptr = si->local_def_.get();
            if (!def_ptr) {
                auto def_it = function_defs_.find(si->def_name_);
                if (def_it != function_defs_.end()) def_ptr = def_it->second.get();
            }
            tick_instance(si, def_ptr);
            return true;
        });
}

void evaluator::auto_schedule_flow(flow_data& fd) {
    for (const auto& m : fd.members_) {
        for (size_t i = 0; i < m.elements_.size(); ++i) {
            std::shared_ptr<function_instance> inst;
            if (m.elements_[i].type_ == value_t::instance_ref)
                inst = m.elements_[i].instance_;
            else if (i < m.live_deps_.size() && m.live_deps_[i])
                inst = m.live_deps_[i];

            if (inst)
                schedule_instance(inst);
        }
    }
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

// ════════════════════════════════════════════════════════════════════════════════
// redistribute_reactions — move each reaction to the fastest segment it depends on
// ════════════════════════════════════════════════════════════════════════════════

void evaluator::redistribute_reactions(std::vector<live_segment>& segments) {
    if (segments.size() <= 1) return;

    // Map: variable name → (segment index, dt_ms).
    // Populated from segment bound vars, then augmented with reaction LHS
    // names after the first pass (so downstream reactions like print(c)
    // follow c = counter(m) to the fastest segment in pass 2).
    std::unordered_map<std::string, std::pair<size_t, double>> var_to_seg;
    for (size_t si = 0; si < segments.size(); ++si) {
        if (segments[si].bound_var.empty()) continue;
        // Use seg.dt_ms directly — works for both real and pre-distribution segments.
        double dt = segments[si].dt_ms;
        if (dt <= 0.0) {
            auto iit = instances_.find(segments[si].instance_id);
            if (iit == instances_.end()) continue;
            dt = iit->second->dt_ms_;
        }
        var_to_seg[segments[si].bound_var] = {si, dt};
    }

    // Map: variable name → ALL segment indices where it is assigned.
    // When a reaction LHS (e.g. c = rcounter(m, reset)) is placed in
    // multiple segments, downstream reactions (e.g. print(c)) must fire
    // in every one of those segments — not just the fastest one.
    // Populated during the augment step between pass 1 and pass 2.
    std::unordered_map<std::string, std::unordered_set<size_t>> var_all_segs;

    // Helper: one redistribution pass over all reactions.
    // When a reaction references variables from multiple distinct segments,
    // it is placed in ALL of those segments so that every trigger can fire it.
    // When it references only one segment (or none), it goes to that one only.
    auto one_pass = [&](std::vector<std::vector<parser::stmt_ptr>>& current)
        -> std::vector<std::vector<parser::stmt_ptr>>
    {
        std::vector<std::vector<parser::stmt_ptr>> result(segments.size());

        for (size_t si = 0; si < segments.size(); ++si) {
            for (const auto& reaction : current[si]) {
                std::unordered_set<std::string> ids;
                collect_stmt_ids(reaction, ids);

                // Collect every distinct segment referenced by this reaction.
                std::unordered_set<size_t> ref_segs;
                double min_dt = std::numeric_limits<double>::max();
                size_t fastest = si;

                for (const auto& id : ids) {
                    auto vit = var_to_seg.find(id);
                    if (vit == var_to_seg.end()) continue;

                    // If this variable is assigned in multiple segments (e.g.
                    // c = rcounter(m, reset) lives in both seg0 and seg1),
                    // expand to all of those segments so downstream reactions
                    // like print(c) fire whenever c is recomputed — not just
                    // on the fastest segment's tick.
                    auto mit = var_all_segs.find(id);
                    if (mit != var_all_segs.end() && mit->second.size() > 1) {
                        for (size_t seg_idx : mit->second) {
                            ref_segs.insert(seg_idx);
                            double seg_dt = segments[seg_idx].dt_ms;
                            if (seg_dt <= 0.0) {
                                auto iit = instances_.find(segments[seg_idx].instance_id);
                                if (iit == instances_.end()) continue;
                                seg_dt = iit->second->dt_ms_;
                            }
                            if (seg_dt > 0.0 && seg_dt < min_dt) {
                                min_dt = seg_dt;
                                fastest = seg_idx;
                            }
                        }
                    } else {
                        double ref_dt = vit->second.second;
                        size_t ref_si = vit->second.first;
                        if (ref_dt > 0.0) {
                            ref_segs.insert(ref_si);
                            if (ref_dt < min_dt) {
                                min_dt = ref_dt;
                                fastest = ref_si;
                            }
                        }
                    }
                }

                if (ref_segs.size() > 1) {
                    // Multi-segment reaction (e.g. rcounter(m, reset)): add to
                    // every referenced segment so any trigger can fire it.
                    for (size_t target : ref_segs)
                        result[target].push_back(reaction);
                } else {
                    // Single-segment or unresolved: assign to fastest (or keep current).
                    result[fastest].push_back(reaction);
                }
            }
        }

        // Deduplicate each bucket by AST node pointer.  A multi-segment
        // reaction placed in bucket A and B will appear in both input buckets
        // on the second pass, causing duplicates without this step.
        for (auto& bucket : result) {
            std::unordered_set<const parser::statement*> seen;
            auto it = std::remove_if(bucket.begin(), bucket.end(),
                [&seen](const parser::stmt_ptr& r) {
                    return r && !seen.insert(r.get()).second;
                });
            bucket.erase(it, bucket.end());
        }

        return result;
    };

    // Collect the initial per-segment reaction lists into a working copy
    std::vector<std::vector<parser::stmt_ptr>> work(segments.size());
    for (size_t si = 0; si < segments.size(); ++si)
        work[si] = segments[si].rxn->reactions;

    // Pass 1 — redistribute using segment bound vars only
    work = one_pass(work);

    // Augment var_to_seg with assignment LHS names produced by pass-1 reactions,
    // so that downstream reactions (e.g. print(c) where c = counter(m)) can be
    // moved to the correct segment in pass 2.
    for (size_t si = 0; si < segments.size(); ++si) {
        double seg_dt = segments[si].dt_ms;
        if (seg_dt <= 0.0) {
            auto iit = instances_.find(segments[si].instance_id);
            if (iit == instances_.end()) continue;
            seg_dt = iit->second->dt_ms_;
        }
        if (seg_dt <= 0.0) continue; // skip unscheduled segments

        for (const auto& reaction : work[si]) {
            if (!reaction || reaction->type_ != parser::node_t::assignment) continue;
            const auto& a = static_cast<const parser::assignment&>(*reaction);
            if (!a.name_.empty()) {
                // Track every segment this variable is assigned in.
                var_all_segs[a.name_].insert(si);
                // Also keep the fastest-segment entry for single-seg lookups.
                auto vit = var_to_seg.find(a.name_);
                if (vit == var_to_seg.end() || seg_dt < vit->second.second)
                    var_to_seg[a.name_] = {si, seg_dt};
            }
        }
    }

    // Pass 2 — re-check with augmented var_to_seg (handles chained deps)
    work = one_pass(work);

    for (size_t si = 0; si < segments.size(); ++si) {
        std::lock_guard<std::mutex> lk(segments[si].rxn->mutex);
        segments[si].rxn->reactions = std::move(work[si]);
    }

    // ── Separate shared reactions from local ones ─────────────────────────────
    // A reaction that appears in more than one segment's list must fire exactly
    // once per epoch (not once per firing segment).  Move those reactions from
    // reactions → shared_reactions so the scheduler callback can queue them
    // with epoch-level deduplication rather than running them immediately.
    {
        std::unordered_map<const parser::statement*, int> refcount;
        for (size_t si = 0; si < segments.size(); ++si)
            for (const auto& r : segments[si].rxn->reactions)
                if (r) refcount[r.get()]++;

        std::unordered_set<const parser::statement*> shared_set;
        for (const auto& [ptr, cnt] : refcount)
            if (cnt > 1) shared_set.insert(ptr);

        for (size_t si = 0; si < segments.size(); ++si) {
            std::lock_guard<std::mutex> lk(segments[si].rxn->mutex);
            if (shared_set.empty()) {
                // No shared reactions — clear any stale entries from a prior
                // run (e.g. a hot reload that removed a multi-segment reaction).
                segments[si].rxn->shared_reactions.clear();
            } else {
                std::vector<parser::stmt_ptr> local, shared;
                for (const auto& r : segments[si].rxn->reactions) {
                    if (r && shared_set.count(r.get()))
                        shared.push_back(r);
                    else
                        local.push_back(r);
                }
                segments[si].rxn->reactions        = std::move(local);
                segments[si].rxn->shared_reactions = std::move(shared);
            }
        }
    }
}

// ════════════════════════════════════════════════════════════════════════════════
// kill_instance — deactivate and unsubscribe a single temporal instance
// ════════════════════════════════════════════════════════════════════════════════

void evaluator::kill_instance(uint64_t id) {
    auto it = instances_.find(id);
    if (it == instances_.end()) return;
    it->second->active_ = false;
    if (scheduler_ && it->second->subscription_id_ != 0) {
        scheduler_->unsubscribe(it->second->subscription_id_);
        it->second->subscription_id_ = 0;
    }
}

// ════════════════════════════════════════════════════════════════════════════════
// diff_and_apply — minimal live update of a running named process
// ════════════════════════════════════════════════════════════════════════════════

void evaluator::diff_and_apply(live_process& lp,
                                const parser::process_block& new_proc,
                                const std::shared_ptr<parser::process_block>& new_ast) {
    // Build a name→index map for the new process body (same walk as setup loop).
    // new_segs[varname] = {binding_stmt, reactions, dt_ms, def_name}
    struct new_seg_info {
        std::string              def_name;
        double                   dt_ms   = 0.0;
        parser::stmt_ptr         binding_stmt;
        std::vector<parser::stmt_ptr> reactions;
    };
    std::unordered_map<std::string, new_seg_info> new_segs;
    std::vector<std::string> new_order;   // preserves source order for later

    // Index of existing live segments by bound variable name.
    // Used below to implement the trigger-parameter fallback.
    std::unordered_map<std::string, const live_segment*> old_seg_by_var;
    for (const auto& seg : lp.segments)
        if (!seg.bound_var.empty())
            old_seg_by_var[seg.bound_var] = &seg;

    // Catch blocks collected here, then linked to segments after pass 2.
    std::vector<std::shared_ptr<parser::catch_block>> new_catch_blocks;

    // Helper: walk an expression's AST to extract the outermost function name
    // without evaluating anything.  Used below to detect segment function changes
    // without running speculative exec (which races with the scheduler thread).
    auto extract_fn_name = [](auto& self, const parser::expr_ptr& expr) -> std::string {
        if (!expr) return "";
        switch (expr->type_) {
            case parser::node_t::function_call_expr: {
                auto& fce = static_cast<const parser::function_call_expr&>(*expr);
                if (!fce.call_ || !fce.call_->function_) return "";
                return self(self, fce.call_->function_);
            }
            case parser::node_t::identifier_expr: {
                auto& ie = static_cast<const parser::identifier_expr&>(*expr);
                return ie.identifier_ ? ie.identifier_->name_ : "";
            }
            case parser::node_t::binary_op_expr: {
                auto& boe = static_cast<const parser::binary_op_expr&>(*expr);
                if (!boe.op_) return "";
                std::string n = self(self, boe.op_->left_);
                return n.empty() ? self(self, boe.op_->right_) : n;
            }
            default: return "";
        }
    };

    {
        new_seg_info* cur = nullptr;
        for (const auto& s : new_proc.body_->statements_) {
            if (!s) continue;

            // Collect catch blocks — linked to segments in pass 2.5 below.
            if (s->type_ == parser::node_t::catch_block) {
                new_catch_blocks.push_back(
                    std::static_pointer_cast<parser::catch_block>(s));
                continue;
            }

            // @-blocks: do NOT re-execute on hot reload.
            // Re-executing them would call scheduler_->subscribe() while the
            // scheduler thread is running, potentially firing duplicate or
            // spurious one-shot events and disrupting clock timing.
            // Existing @-block timers from process start continue running.
            if (s->type_ == parser::node_t::at_block) {
                continue;
            }

            // on_block falls through to the bname.empty() path below,
            // which adds it to cur->reactions without executing it now.

            std::string bname;
            if (s->type_ == parser::node_t::function_definition)
                bname = static_cast<const parser::function_definition&>(*s).name_;
            else if (s->type_ == parser::node_t::assignment)
                bname = static_cast<const parser::assignment&>(*s).name_;

            // Only named bindings can create temporal instances.
            // Pure reaction statements (bname empty) never do.
            if (bname.empty()) {
                if (cur) cur->reactions.push_back(s);
                continue;
            }

            // If this binding was already a live segment, reuse its metadata
            // without speculative exec.  Speculative exec writes to env_ in a
            // pushed scope which is briefly visible to the scheduler thread —
            // that creates a data race on shared env_ that can corrupt reads
            // (e.g. the scheduler seeing cnt=trigger(true) from the pushed
            // scope instead of the real counter value) and fire spurious events.
            //
            // We do an AST-only check to detect whether the temporal function
            // itself changed.  If it did, we fall through to speculative exec
            // (function-change is rare; for the common hot-reload of reactions
            // only, the race is completely avoided).
            auto old_it = old_seg_by_var.find(bname);
            if (old_it != old_seg_by_var.end()) {
                const live_segment& oldseg = *old_it->second;

                // Extract function name from the new binding's RHS (AST only, no eval).
                parser::expr_ptr value_expr;
                if (s->type_ == parser::node_t::assignment)
                    value_expr = static_cast<const parser::assignment&>(*s).value_;
                else if (s->type_ == parser::node_t::function_definition) {
                    auto& fd = static_cast<const parser::function_definition&>(*s);
                    if (!fd.lambda_block_) value_expr = fd.body_;
                }
                std::string new_fn = extract_fn_name(extract_fn_name, value_expr);

                // If function name is unchanged (or undeterminable), skip exec.
                if (new_fn.empty() || new_fn == oldseg.def_name) {
                    // Re-extract the dt from the new AST so that a dt change
                    // (e.g. counter(dt=0.3s) → counter(dt=0.5s)) is reflected
                    // in nsi.dt_ms and triggers the scheduler update in pass 1.
                    auto extract_dt_arg = [](const parser::stmt_ptr& stmt) -> parser::expr_ptr {
                        parser::expr_ptr call_expr;
                        if (stmt->type_ == parser::node_t::assignment)
                            call_expr = static_cast<const parser::assignment&>(*stmt).value_;
                        else if (stmt->type_ == parser::node_t::function_definition) {
                            auto& fd = static_cast<const parser::function_definition&>(*stmt);
                            if (!fd.lambda_block_) call_expr = fd.body_;
                        }
                        if (!call_expr) return nullptr;
                        if (call_expr->type_ == parser::node_t::binary_op_expr) {
                            auto& boe = static_cast<const parser::binary_op_expr&>(*call_expr);
                            if (boe.op_) call_expr = boe.op_->left_;
                        }
                        if (!call_expr || call_expr->type_ != parser::node_t::function_call_expr)
                            return nullptr;
                        const auto& fce =
                            static_cast<const parser::function_call_expr&>(*call_expr);
                        if (!fce.call_) return nullptr;
                        for (const auto& arg : fce.call_->arguments_)
                            if (arg && arg->name_ == "dt") return arg->value_;
                        return nullptr;
                    };

                    double new_dt = oldseg.dt_ms;
                    if (auto dt_expr = extract_dt_arg(s)) {
                        value dt_val = eval_expr(dt_expr);
                        if (dt_val.number_ > 0.0) new_dt = dt_val.number_;
                    }

                    new_seg_info nsi;
                    nsi.def_name     = oldseg.def_name;
                    nsi.dt_ms        = new_dt;
                    nsi.binding_stmt = s;
                    new_segs[bname]  = std::move(nsi);
                    new_order.push_back(bname);
                    cur = &new_segs[bname];
                    continue;
                }
                // Function changed — fall through to speculative exec so pass 1
                // detects the def_name mismatch and kills/recreates the segment.
            }

            // Speculative exec: run the statement in a temporary scope to find
            // out whether it creates a temporal instance (and if so, what
            // def_name and dt the new instance has).
            // Only reached for genuinely new segments or function-change cases.
            //
            // speculative_exec_ suppresses module function side effects
            // (is_native_temporal_ == false) so that reactions like
            // `a = note(spike!, freq)` don't call cs_note at hot-reload time.
            uint64_t id_before = next_instance_id_;
            speculative_exec_ = true;
            env_.push_scope();
            exec_stmt(s);
            env_.pop_scope();
            speculative_exec_ = false;
            bool created = (next_instance_id_ > id_before);

            if (created) {
                uint64_t new_id = next_instance_id_ - 1;
                auto inst_it = instances_.find(new_id);
                std::string dn;
                double      dt = 0.0;
                if (inst_it != instances_.end()) {
                    dn = inst_it->second->def_name_;
                    dt = inst_it->second->dt_ms_;
                    kill_instance(new_id);
                    instances_.erase(inst_it);
                }
                --next_instance_id_;

                new_seg_info nsi;
                nsi.def_name     = dn;
                nsi.dt_ms        = dt;
                nsi.binding_stmt = s;
                new_segs[bname]  = std::move(nsi);
                new_order.push_back(bname);
                cur = &new_segs[bname];
            } else {
                // No instance created.  Two sub-cases:
                //   (a) bname was never a segment — it's a reaction.
                //   (b) bname IS an existing segment but the temporal function
                //       wasn't called because its trigger parameter was rest.
                //       Preserve the old segment so pass 1 doesn't kill it.
                auto old_it2 = old_seg_by_var.find(bname);
                if (old_it2 != old_seg_by_var.end()) {
                    const live_segment& oldseg = *old_it2->second;
                    new_seg_info nsi;
                    nsi.def_name     = oldseg.def_name;
                    nsi.dt_ms        = oldseg.dt_ms;
                    nsi.binding_stmt = s;
                    new_segs[bname]  = std::move(nsi);
                    new_order.push_back(bname);
                    cur = &new_segs[bname];
                } else if (cur) {
                    cur->reactions.push_back(s);
                }
            }
        }
    }

    // ── Pre-redistribute reactions ─────────────────────────────────────────
    // Distribute reactions across new_segs before any hot-swap so that the
    // very first tick after hot-reload uses correct segment assignments.
    // Without this step, reactions end up in the "last segment before them"
    // bucket (e.g. `mod` at 100ms) and fire at the wrong rate during the
    // brief window between the pass-1 hot-swap and the final redistribution.
    {
        std::vector<live_segment> pre_segs;
        pre_segs.reserve(new_order.size());
        for (const auto& varname : new_order) {
            auto nit = new_segs.find(varname);
            if (nit == new_segs.end()) continue;
            live_segment ps;
            ps.bound_var   = varname;
            ps.def_name    = nit->second.def_name;
            ps.dt_ms       = nit->second.dt_ms;
            ps.instance_id = 0;   // not used after redistribute_reactions patch
            ps.rxn         = std::make_shared<reaction_set>();
            ps.rxn->reactions = nit->second.reactions;
            pre_segs.push_back(std::move(ps));
        }

        redistribute_reactions(pre_segs);

        size_t idx = 0;
        for (const auto& varname : new_order) {
            auto nit = new_segs.find(varname);
            if (nit == new_segs.end()) continue;
            if (idx < pre_segs.size())
                nit->second.reactions = pre_segs[idx].rxn->reactions;
            ++idx;
        }

    }

    // Track which old segments survive, to avoid double-killing
    std::unordered_set<std::string> handled;

    // ── Pass 1: update or remove existing segments ─────────────────────────
    for (auto& old_seg : lp.segments) {
        auto nit = new_segs.find(old_seg.bound_var);

        if (nit == new_segs.end()) {
            // Variable removed → kill instance
            kill_instance(old_seg.instance_id);
            instance_bindings_.erase(old_seg.bound_var);
            continue;
        }

        auto& ns = nit->second;
        handled.insert(old_seg.bound_var);

        if (ns.def_name != old_seg.def_name) {
            // Different temporal function → kill old; will create new in pass 2
            kill_instance(old_seg.instance_id);
            instance_bindings_.erase(old_seg.bound_var);
            handled.erase(old_seg.bound_var);  // let pass 2 create it
            continue;
        }

        // Same function — instance survives with all its state.
        // Update dt if changed.
        if (std::abs(ns.dt_ms - old_seg.dt_ms) > 0.01) {
            auto inst_it = instances_.find(old_seg.instance_id);
            if (inst_it != instances_.end() && scheduler_) {
                scheduler_->update_dt(inst_it->second->subscription_id_, ns.dt_ms);
                inst_it->second->dt_ms_ = ns.dt_ms;
                old_seg.dt_ms = ns.dt_ms;

                // Also update instance_param_exprs_ so that tick_instance
                // doesn't re-evaluate the OLD dt AST expression and revert
                // the scheduler interval back on the very next tick.
                auto extract_dt_expr = [](const parser::stmt_ptr& s) -> parser::expr_ptr {
                    if (!s || s->type_ != parser::node_t::assignment) return nullptr;
                    const auto& assign = static_cast<const parser::assignment&>(*s);
                    if (!assign.value_ ||
                        assign.value_->type_ != parser::node_t::function_call_expr)
                        return nullptr;
                    const auto& fce =
                        static_cast<const parser::function_call_expr&>(*assign.value_);
                    if (!fce.call_) return nullptr;
                    for (const auto& arg : fce.call_->arguments_)
                        if (arg && arg->name_ == "dt") return arg->value_;
                    return nullptr;
                };

                auto& pexprs = instance_param_exprs_[old_seg.instance_id];
                if (auto new_dt_expr = extract_dt_expr(ns.binding_stmt))
                    pexprs["dt"] = new_dt_expr;
                else
                    pexprs.erase("dt");
            }
        }

        // ── Hot swap the reaction set ──────────────────────────────────────
        // The scheduler closure holds a shared_ptr to old_seg.rxn.
        // Writing new content here takes effect on the very next tick —
        // no unsubscribe, no re-subscribe, no scheduling gap.
        // Lock to prevent a data race with the scheduler thread, which reads
        // these fields on every tick.
        {
            std::lock_guard<std::mutex> lk(old_seg.rxn->mutex);
            old_seg.rxn->binding_stmt = ns.binding_stmt;
            old_seg.rxn->reactions    = std::move(ns.reactions);
            // Reset fired flags on catches so re-used catch names fire again.
            old_seg.rxn->catches.clear();
        }
    }

    // Remove dead segments from lp (those not in handled and not already erased).
    lp.segments.erase(
        std::remove_if(lp.segments.begin(), lp.segments.end(),
            [&](const live_segment& s) {
                // Dead = variable removed or function changed (and not re-added yet)
                if (new_segs.find(s.bound_var) == new_segs.end()) return true;
                if (!handled.count(s.bound_var)) return true;
                return false;
            }),
        lp.segments.end());

    // ── Pass 2: create new segments ────────────────────────────────────────
    for (const auto& varname : new_order) {
        if (handled.count(varname)) continue;  // already handled in pass 1

        auto& ns = new_segs[varname];

        // Instantiate fresh in the existing process scope
        // (env_.scopes_[lp.scope_depth] is still live on the stack).
        uint64_t id_before = next_instance_id_;
        exec_stmt(ns.binding_stmt);
        if (next_instance_id_ <= id_before) continue;  // instantiation failed

        uint64_t new_id = next_instance_id_ - 1;
        auto inst_it = instances_.find(new_id);
        if (inst_it == instances_.end()) continue;
        auto inst = inst_it->second;

        auto rxn = std::make_shared<reaction_set>();
        rxn->binding_stmt = ns.binding_stmt;
        rxn->reactions    = std::move(ns.reactions);

        live_segment new_seg;
        new_seg.instance_id = new_id;
        new_seg.bound_var   = varname;
        new_seg.def_name    = inst->def_name_;
        new_seg.dt_ms       = inst->dt_ms_;
        new_seg.rxn         = rxn;

        instance_bindings_[varname] = new_id;

        // Subscribe with the same generic closure
        if (scheduler_ && inst->dt_ms_ > 0.0) {
            std::shared_ptr<parser::function_definition> def_ptr;
            if (!inst->native_update_) {
                def_ptr = inst->local_def_;
                if (!def_ptr) {
                    std::shared_lock lock(defs_mutex_);
                    auto dit = function_defs_.find(inst->def_name_);
                    if (dit != function_defs_.end()) def_ptr = dit->second;
                }
            }

            auto weak_inst = std::weak_ptr<function_instance>(inst);
            std::string var = varname;

            inst->subscription_id_ = scheduler_->subscribe(inst->dt_ms_,
                [this, def_ptr, weak_inst, var, rxn,
                 start_t = scheduler_->now_ms()]
                (double t, double) -> bool {
                    auto si = weak_inst.lock();
                    if (!si || !si->active_) return false;

                    tick_instance(si, def_ptr.get());

                    // Snapshot reactions under lock (same pattern as the
                    // process-setup closure above).
                    parser::stmt_ptr binding_snap;
                    std::vector<parser::stmt_ptr> reactions_snap;
                    std::vector<parser::stmt_ptr> shared_snap;
                    {
                        std::lock_guard<std::mutex> lk(rxn->mutex);
                        binding_snap   = rxn->binding_stmt;
                        reactions_snap = rxn->reactions;
                        shared_snap    = rxn->shared_reactions;
                    }

                    retick_instance_ = si.get();
                    if (binding_snap) exec_stmt(binding_snap);
                    retick_instance_ = nullptr;

                    for (const auto& r : reactions_snap) exec_stmt(r);

                    for (const auto& r : shared_snap) {
                        if (r && epoch_deferred_dedup_.insert(r.get()).second)
                            epoch_deferred_.push_back(r);
                    }

                    {
                        std::lock_guard<std::mutex> lk(rxn->mutex);
                        for (auto& c : rxn->catches) {
                            if (c.fired) continue;
                            auto emit_it = si->emitted_.find(c.watched_emit);
                            if (emit_it != si->emitted_.end() && emit_it->second.is_truthy()) {
                                c.fired = true;
                                for (const auto& h : c.handler) exec_stmt(h);
                            }
                        }
                    }

                    if (!var.empty()) {
                        value* bound = env_.lookup(var);
                        if (bound && bound->type_ == value_t::trigger)
                            epoch_reset_vars_.push_back(var);
                    }

                    if (!epoch_flush_pending_) {
                        epoch_flush_pending_ = true;
                        scheduler_->subscribe(0.1, [this](double, double) -> bool {
                            for (const auto& r : epoch_deferred_)
                                exec_stmt(r);
                            epoch_deferred_.clear();
                            epoch_deferred_dedup_.clear();
                            for (const auto& vname : epoch_reset_vars_) {
                                value* v = env_.lookup(vname);
                                if (v) *v = value::rest();
                            }
                            epoch_reset_vars_.clear();
                            epoch_flush_pending_ = false;
                            return false;
                        });
                    }

                    return true;
                });
        }

        lp.segments.push_back(std::move(new_seg));

        // Keep active_process_instances_ in sync
        active_process_instances_[lp.name].push_back(new_id);
    }

    // ── Pass 2.5: link catch blocks to their target segments ──────────────
    // Pass 1 already cleared catches on surviving segments; pass 2 created
    // fresh reaction_sets for new segments — so all segments start clean here.
    // Only named catches (catch var::sig) are handled; anonymous-instance
    // catches are not re-created on hot reload.
    for (auto& cb : new_catch_blocks) {
        if (!cb->instance_expr_) continue;

        std::string source_var;
        if (cb->instance_expr_->type_ == parser::node_t::identifier_expr) {
            const auto& ie =
                static_cast<const parser::identifier_expr&>(*cb->instance_expr_);
            if (ie.identifier_) source_var = ie.identifier_->name_;
        }
        if (source_var.empty()) continue;

        for (auto& seg : lp.segments) {
            if (seg.bound_var == source_var) {
                std::lock_guard<std::mutex> lk(seg.rxn->mutex);
                seg.rxn->catches.push_back(
                    {cb->signal_name_, cb->handler_, false});
                break;
            }
        }
    }

    // ── Pass 3: redistribute reactions and update metadata ─────────────────
    redistribute_reactions(lp.segments);
    lp.ast = new_ast;
}

// ════════════════════════════════════════════════════════════════════════════════
// hot_reload — parse and apply a source fragment to the running evaluator
// ════════════════════════════════════════════════════════════════════════════════

void evaluator::hot_reload(const std::string& source) {
    std::istringstream ss(source);
    auto prog = parser::parse_stream(ss, false);
    if (!prog) {
        std::cerr << "[hot_reload] parse failed\n";
        return;
    }

    for (const auto& stmt : prog->statements_) {
        if (!stmt) continue;

        switch (stmt->type_) {

        // ── Global function redefinition ───────────────────────────────────
        case parser::node_t::function_definition: {
            auto& def = static_cast<const parser::function_definition&>(*stmt);

            if (def.parameters_.empty() && !def.lambda_block_ && def.body_) {
                // Zero-param constant: re-evaluate and update env binding.
                // No defs_mutex_ needed — env_ is not read by the scheduler thread.
                value result = eval_expr(def.body_);
                env_.define(def.name_, std::move(result));
            } else {
                auto def_ptr = std::static_pointer_cast<parser::function_definition>(
                    std::const_pointer_cast<parser::node>(
                        std::shared_ptr<const parser::node>(stmt, &def)));
                {
                    std::unique_lock lock(defs_mutex_);
                    function_defs_[def.name_] = def_ptr;
                }
                env_.define(def.name_, value::function_ref(def.name_));
            }
            std::cerr << "[hot_reload] updated function '" << def.name_ << "'\n";
            break;
        }

        // ── Flow redefinition ──────────────────────────────────────────────
        case parser::node_t::flow_definition: {
            auto& def = static_cast<const parser::flow_definition&>(*stmt);

            {
                std::unique_lock lock(defs_mutex_);
                // Evict stale cache entries for this flow name
                for (auto it = flow_call_cache_.begin(); it != flow_call_cache_.end(); ) {
                    if (it->first.name == def.name_) it = flow_call_cache_.erase(it);
                    else ++it;
                }
                flow_cache_order_.remove_if(
                    [&](const auto& k) { return k.name == def.name_; });

                if (def.parameters_.empty()) {
                    // Eager zero-param flow: re-evaluate immediately.
                    // Release lock before eval (eval doesn't touch flow_defs_).
                }  else {
                    auto fp = std::static_pointer_cast<parser::flow_definition>(
                        std::const_pointer_cast<parser::statement>(stmt));
                    flow_defs_[def.name_] = fp;
                }
            }

            if (def.parameters_.empty()) {
                value fv;
                fv.type_  = value_t::flow;
                fv.flow_  = eval_flow_members(def.members_);
                env_.define(def.name_, std::move(fv));
            } else {
                env_.define(def.name_, value::function_ref(def.name_));
            }
            std::cerr << "[hot_reload] updated flow '" << def.name_ << "'\n";
            break;
        }

        // ── Named process block ────────────────────────────────────────────
        case parser::node_t::process_block: {
            auto& proc = static_cast<const parser::process_block&>(*stmt);
            if (proc.name_.empty()) {
                std::cerr << "[hot_reload] anonymous process blocks cannot be hot-reloaded\n";
                break;
            }
            if (!proc.body_) break;

            auto new_ast = std::static_pointer_cast<parser::process_block>(
                std::const_pointer_cast<parser::node>(
                    std::shared_ptr<const parser::node>(stmt, &proc)));

            auto lp_it = live_processes_.find(proc.name_);
            if (lp_it == live_processes_.end()) {
                // Not currently running — just update the stored AST.
                stored_processes_[proc.name_] = new_ast;
                std::cerr << "[hot_reload] stored process '" << proc.name_ << "' (not running)\n";
            } else {
                diff_and_apply(lp_it->second, proc, new_ast);
                stored_processes_[proc.name_] = new_ast;
                std::cerr << "[hot_reload] updated process '" << proc.name_ << "'\n";
            }
            break;
        }

        default:
            std::cerr << "[hot_reload] unsupported statement type — skipped\n";
            break;
        }
    }
}

} // --- idyl::core ---
