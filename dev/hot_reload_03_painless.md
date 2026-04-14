# Hot reload — appendix 03: making it painless

This document proposes the minimal architectural rework that makes hot reload a natural consequence of the data model rather than a surgical fight against the existing implementation. The goal: after the refactor, hot reload for both global definitions and named process blocks requires no special-casing in the evaluator — it is just a normal write to a live, mutable data structure that tick callbacks already read from.

The changes are purely internal. The idyl language, its semantics, and user-visible behavior are unchanged.

---

## The central insight

The current model: process setup builds scheduler callbacks that **capture** their behavior (reactions, binding statement, catches) as immutable lambda values. The scheduler sees opaque closures. Hot reload cannot reach inside them.

The target model: process setup builds scheduler callbacks that **dispatch through** a shared mutable record. The callback is a thin, generic function — "tick this instance, then read and execute whatever is in this record". Hot reload updates the record. The callback automatically picks up the new behavior on its next tick, with no re-subscription and no scheduling gap.

This single conceptual shift resolves pain points 1, 3, and 4 from appendix 02. Pain points 2, 5, 6, 8, and 9 are resolved by small, isolated fixes on top. Pain point 7 (delay memory) remains a known limitation.

---

## Step 1 — Two new structs

### `reaction_set` — the mutable behavior of one segment

Replaces the three captured-by-value variables (`binding`, `reactions`, `catches`) in the current scheduler closure:

```cpp
// evaluator.hpp  (or a new header included by it)

struct catch_info {
    std::string                    watched_emit;
    std::vector<parser::stmt_ptr>  handler;
    bool                           fired = false;
};

struct reaction_set {
    parser::stmt_ptr               binding_stmt;   // the "m = metro(...)" assignment
    std::vector<parser::stmt_ptr>  reactions;      // statements that follow it
    std::vector<catch_info>        catches;
};
```

### `live_segment` — everything about one temporal binding in a running process

```cpp
struct live_segment {
    uint64_t                        instance_id;
    std::string                     bound_var;    // "m", "m2", etc.
    std::string                     def_name;     // "metro", "lfo", etc.
    std::shared_ptr<reaction_set>   rxn;          // shared with the scheduler closure
};
```

### `live_process` — the complete live state of a running named process

```cpp
struct live_process {
    std::string                                        name;
    std::shared_ptr<parser::process_block>             ast;     // current AST (for diffing)
    std::vector<live_segment>                          segments;
    // Index of the env scope frame pushed for this process.
    // Kept alive as long as the process is running.
    std::size_t                                        scope_depth;
};
```

### New evaluator member

```cpp
// evaluator.hpp  (replaces active_process_instances_ for named processes)
std::unordered_map<std::string, live_process> live_processes_;
```

`active_process_instances_` can be kept for anonymous processes or gradually removed. `live_processes_` is the authoritative record for named processes.

---

## Step 2 — The generic tick callback

The process setup loop changes from:

```cpp
// Before: captures everything by value — immutable after subscribe
inst->subscription_id_ = scheduler_->subscribe(inst->dt_ms_,
    [this, def_ptr, weak_inst, var, binding, reactions, catches, ...]
    (double t, double) -> bool {
        ...
        for (const auto& r : reactions) exec_stmt(r);   // ← copy, never changes
        ...
    });
```

To:

```cpp
// After: captures rxn by shared_ptr — live, mutable
auto rxn = std::make_shared<reaction_set>();
rxn->binding_stmt = seg.binding_stmt;
rxn->reactions    = seg.reactions;
rxn->catches      = std::move(seg.catches);
// Store rxn in the live_segment too, so hot reload can write to it.
live_seg.rxn = rxn;

inst->subscription_id_ = scheduler_->subscribe(inst->dt_ms_,
    [this, weak_inst, rxn, var, dur_ms, start_t = scheduler_->now_ms()]
    (double t, double) -> bool {
        auto si = weak_inst.lock();
        if (!si || !si->active_) return false;
        if (dur_ms > 0.0 && (t - start_t) >= dur_ms) { si->active_ = false; return false; }

        // ── Epoch reset (unchanged) ──────────────────────────────────────
        if (t > epoch_time_ms_ + 0.5) {
            for (const auto& v : epoch_reset_vars_) {
                value* p = env_.lookup(v);
                if (p) *p = value::rest();
            }
            epoch_reset_vars_.clear();
            epoch_time_ms_ = t;
        }

        tick_instance(*si, lookup_def(si->def_name_));

        retick_instance_ = si.get();
        if (rxn->binding_stmt) exec_stmt(rxn->binding_stmt);    // ← live read
        retick_instance_ = nullptr;

        for (const auto& r : rxn->reactions) exec_stmt(r);      // ← live read

        for (auto& c : rxn->catches) {                          // ← live read
            if (c.fired) continue;
            auto emit_it = si->emitted_.find(c.watched_emit);
            if (emit_it != si->emitted_.end() && emit_it->second.is_truthy()) {
                c.fired = true;
                for (const auto& h : c.handler) exec_stmt(h);
            }
        }

        if (!var.empty()) epoch_reset_vars_.push_back(var);
        return true;
    });
```

The callback now reads `rxn->reactions` and `rxn->catches` on every tick from the shared pointer. There is no copy inside the closure. The closure itself never changes — only what `rxn` points to changes.

**Note**: `epoch_resets_` becomes `epoch_reset_vars_` (a `vector<string>`, not `vector<value*>`), resolving pain point 2 from appendix 02. See the flush loop above.

---

## Step 3 — Thread safety for definitions

Add one `shared_mutex` to the evaluator:

```cpp
// evaluator.hpp
mutable std::shared_mutex defs_mutex_;
```

In `tick_instance`, wrap the `function_defs_` lookup:

```cpp
std::shared_lock lock(defs_mutex_);
auto it = function_defs_.find(inst.def_name_);
```

In the hot-reload handler (main thread), wrap writes with an exclusive lock:

```cpp
{
    std::unique_lock lock(defs_mutex_);
    function_defs_[name] = new_def;
    // also invalidate flow_call_cache_ entries for this name
    std::erase_if(flow_call_cache_, [&](auto& kv){ return kv.first.name == name; });
    flow_cache_order_.remove_if([&](auto& k){ return k.name == name; });
}
```

This is the complete thread safety story for global definitions.

---

## Step 4 — Always store named process ASTs

In `exec_stmt` process block case, store the AST unconditionally before the `listen_mode_` branch:

```cpp
if (!proc.name_.empty()) {
    stored_processes_[proc.name_] = std::static_pointer_cast<parser::process_block>(...);
}
```

Two lines added. No behavior change.

---

## Step 5 — Extract reaction redistribution

Move the redistribution loop (~35 lines, currently inlined at lines 333–366) into a standalone function:

```cpp
static void redistribute_reactions(
    std::vector<temporal_segment>& segments,
    const std::unordered_map<uint64_t, std::shared_ptr<function_instance>>& instances);
```

Called from the process setup loop and from the hot-reload diff path. No semantic change.

---

## Step 6 — The hot reload algorithm

With the above in place, the `/idyl/eval` handler becomes:

```cpp
void evaluator::hot_reload(const std::string& source) {
    // 1. Parse
    auto prog = parser::parse_string(source);
    if (!prog) { warn("hot_reload: parse failed"); return; }

    for (const auto& stmt : prog->statements_) {
        switch (stmt->type_) {

        // ── Global function / constant / flow redefinition ─────────────────
        case node_t::function_definition: {
            auto& def = static_cast<const parser::function_definition&>(*stmt);
            std::unique_lock lock(defs_mutex_);
            if (def.parameters_.empty() && !def.lambda_block_ && def.body_) {
                // Pure constant: re-evaluate and update env
                env_.define(def.name_, eval_expr(def.body_));
            } else {
                function_defs_[def.name_] = make_shared_from(stmt, def);
                env_.define(def.name_, value::function_ref(def.name_));
            }
            break;
        }
        case node_t::flow_definition: {
            auto& def = static_cast<const parser::flow_definition&>(*stmt);
            std::unique_lock lock(defs_mutex_);
            // Evict stale cache entries
            std::erase_if(flow_call_cache_, [&](auto& kv){ return kv.first.name == def.name_; });
            flow_cache_order_.remove_if([&](auto& k){ return k.name == def.name_; });
            if (def.parameters_.empty()) {
                // Eager: re-evaluate and store
                value fv; fv.type_ = value_t::flow; fv.flow_ = eval_flow_members(def.members_);
                env_.define(def.name_, std::move(fv));
            } else {
                flow_defs_[def.name_] = make_shared_from(stmt, def);
                env_.define(def.name_, value::function_ref(def.name_));
            }
            break;
        }

        // ── Named process block ────────────────────────────────────────────
        case node_t::process_block: {
            auto& proc = static_cast<const parser::process_block&>(*stmt);
            if (proc.name_.empty()) {
                warn("hot_reload: anonymous process blocks cannot be hot-reloaded");
                break;
            }
            auto it = live_processes_.find(proc.name_);
            if (it == live_processes_.end()) {
                // Not currently running: just update stored AST, optionally start
                stored_processes_[proc.name_] = make_shared_from(stmt, proc);
            } else {
                diff_and_apply(it->second, proc, stmt);
            }
            break;
        }

        default:
            warn("hot_reload: only definitions and named process blocks are supported");
            break;
        }
    }
}
```

### The diff algorithm (`diff_and_apply`)

```cpp
void evaluator::diff_and_apply(live_process& lp,
                                const parser::process_block& new_proc,
                                const parser::stmt_ptr& new_stmt) {
    // Build a map of new segments by variable name
    auto new_segs = extract_segments(new_proc);   // same logic as process setup loop

    // ── Pass 1: update or remove existing segments ─────────────────────────
    for (auto& old_seg : lp.segments) {
        auto nit = new_segs.find(old_seg.bound_var);

        if (nit == new_segs.end()) {
            // Variable removed → kill the instance
            kill_instance(old_seg.instance_id);
            continue;
        }

        auto& new_seg = nit->second;

        if (new_seg.def_name != old_seg.def_name) {
            // Different function → kill old, will be created in pass 2
            kill_instance(old_seg.instance_id);
            continue;
        }

        // Same function: the instance survives with its state intact
        auto inst_it = instances_.find(old_seg.instance_id);
        if (inst_it == instances_.end()) continue;
        auto& inst = inst_it->second;

        if (new_seg.dt_ms != old_seg.def_name /* compare dt */) {
            // dt changed → update scheduler interval, no state loss
            scheduler_->update_dt(inst->subscription_id_, new_seg.dt_ms);
        }

        // ── The key operation: swap the reaction set ──────────────────────
        // No re-subscribe. The existing callback reads rxn on every tick.
        // Writing new reactions here takes effect on the very next tick.
        // Protected by defs_mutex_ already held, or a dedicated rxn_mutex_.
        old_seg.rxn->binding_stmt = new_seg.rxn->binding_stmt;
        old_seg.rxn->reactions    = std::move(new_seg.rxn->reactions);
        // Preserve fired state on surviving catches; add new ones
        merge_catches(*old_seg.rxn, std::move(new_seg.rxn->catches));
        old_seg.def_name = new_seg.def_name;

        nit->second.handled = true;  // mark as seen
    }

    // ── Pass 2: create new segments ────────────────────────────────────────
    for (auto& [varname, new_seg] : new_segs) {
        if (new_seg.handled) continue;
        // New variable or replacement after kill: instantiate fresh
        instantiate_segment(new_seg, lp);
    }

    // ── Pass 3: update redistribution & clean up lp.segments ───────────────
    remove_dead_segments(lp.segments);
    redistribute_reactions(lp.segments, instances_);

    // Update stored AST for future hot reloads
    lp.ast = make_shared_from(new_stmt, new_proc);
}
```

This is the entire hot reload algorithm for a running process. The critical line is:

```cpp
old_seg.rxn->reactions = std::move(new_seg.rxn->reactions);
```

This is a write to a `shared_ptr<reaction_set>` that the scheduler callback is already holding. The next tick reads the new reactions. No unsubscribe, no re-subscribe, no scheduling gap, no state loss.

---

## Step 7 — Scope continuity

When `diff_and_apply` runs, the env scope frame pushed for `lp` (recorded in `lp.scope_depth`) is still on the scope stack. Surviving instances still write their outputs into it each tick. New variables introduced by the updated process just need to be `define`d into the existing frame:

```cpp
// In instantiate_segment, during hot reload:
env_.scopes_[lp.scope_depth].bindings_[new_seg.bound_var] = value::rest();
```

No scope pop, no scope push. The existing frame accumulates new variables. Removed variables become orphaned entries in the frame — they are unreachable from the new AST and cause no harm.

---

## Step 8 — OSC entry point

In `idyl.cpp`, the listen loop gains one new command type:

```cpp
struct listen_command {
    enum class type { start, stop, list, eval } type_;
    std::string name_;   // process name for start/stop; source code for eval
};
```

```cpp
// In the OSC listener thread:
} else if (addr == "/idyl/eval") {
    auto code = msg.try_get<std::string>(0);
    if (code) {
        std::lock_guard<std::mutex> lk(cmd_mutex);
        cmd_queue.push_back(listen_command{listen_command::type::eval, *code});
    }
}
```

```cpp
// In the main-thread dispatch loop:
case listen_command::type::eval:
    eval.hot_reload(cmd.name_);   // cmd.name_ carries the source code
    break;
```

The dispatch happens on the main thread, serialized with all other evaluator access. No new threading concerns.

---

## Step 9 — `kill_instance` helper

A small helper used by `diff_and_apply` that encapsulates what `stop_process` currently does per-instance:

```cpp
void evaluator::kill_instance(uint64_t id) {
    auto it = instances_.find(id);
    if (it == instances_.end()) return;
    it->second->active_ = false;
    if (scheduler_ && it->second->subscription_id_ != 0)
        scheduler_->unsubscribe(it->second->subscription_id_);
    // instance stays in instances_ map until the callback's weak_ptr expires
}
```

---

## What the full refactor looks like

### New / changed data structures

| Item | Change |
|------|--------|
| `reaction_set` struct | New. Holds binding, reactions, catches. |
| `live_segment` struct | New. Holds instance id, bound var, def name, `shared_ptr<reaction_set>`. |
| `live_process` struct | New. Holds name, AST, segments, scope depth. |
| `live_processes_` map | New evaluator member: `name → live_process`. |
| `epoch_resets_` | Renamed to `epoch_reset_vars_`, type `vector<string>` instead of `vector<value*>`. |
| `defs_mutex_` | New `shared_mutex` on evaluator. |
| `active_process_instances_` | Kept for anonymous processes; named ones migrate to `live_processes_`. |

### New / changed functions

| Function | Change |
|----------|--------|
| `redistribute_reactions(...)` | Extracted from process block case (was inlined). |
| `kill_instance(id)` | New helper. |
| `extract_segments(proc)` | New helper. Parses a process body into a map of `{varname → live_segment}`. |
| `instantiate_segment(seg, lp)` | Extracted from process setup. Creates instance + subscribes. |
| `diff_and_apply(lp, proc, stmt)` | New. The hot reload core. |
| `hot_reload(source)` | New public API. Parse → categorize → apply. |
| `exec_stmt`, process block case | Scheduler closure changed to use `shared_ptr<reaction_set>`. Also populates `live_processes_`. |
| `stop_process(name)` | Calls `kill_instance` per segment and erases from `live_processes_`. |
| `start_process(name)` | Uses `live_processes_` instead of `active_process_instances_` for named processes. |

### Functions unchanged

`tick_instance`, `eval_expr`, `eval_call`, all flow logic, all builtin dispatch, the scheduler itself, all modules.

---

## Implementation phases

These can be done incrementally, each leaving the runtime fully functional:

**Phase A — Purely mechanical refactors (no behavior change, ~2 days)**

1. Extract `redistribute_reactions` into a standalone function.
2. Always store named process ASTs in `stored_processes_`.
3. Change `epoch_resets_` to `epoch_reset_vars_` (string names, not pointers). Flush loop updated.
4. Add `defs_mutex_` and wrap `function_defs_` lookups in tick path with shared lock. No other changes.
5. Evict `flow_call_cache_` entries in `exec_stmt` when a flow is redefined (i.e. name already in `flow_defs_` before the update).
6. Introduce `reaction_set` and `live_segment` structs. Change the process setup loop to populate `live_processes_` and use `shared_ptr<reaction_set>` in the closure. All existing behavior preserved — `active_process_instances_` can remain in parallel for now.
7. Introduce `kill_instance`, `extract_segments`, `instantiate_segment` helpers by extracting from existing code.

**Phase B — Hot reload for global definitions (~1 day)**

8. Add `hot_reload(source)` with only the `function_definition` and `flow_definition` cases.
9. Wire `/idyl/eval` into the OSC dispatch.
10. Test: redefine a function used by a running instance. The next tick uses the new AST.

**Phase C — Hot reload for running process blocks (~2 days)**

11. Implement `diff_and_apply` using the helpers from Phase A.
12. Add the `process_block` case to `hot_reload`.
13. Implement scope continuity in `instantiate_segment` for the hot-reload path.
14. Test the four sub-cases: reaction-only change, dt-only change, function change, add/remove segment.

**Total: ~5 days of focused work**, mostly spent on the Phase A refactors. Phase B and C are straightforward given Phase A.

---

## What is still not fixed

**Delay buffer continuity** (`delay_memories_` keyed by AST node pointer): when a function that contains `'(x)` is hot-reloaded, the delay buffer for that expression is lost because the new AST has a new node pointer. There is no clean fix at this time. The buffer resets to zeros on next use — audible as a glitch on the tick following the reload, then stable. Accept as a known limitation.

---

## What hot reload looks like from the user side

```bash
idyl song.idyl --listen 9000
```

```bash
# Change a function definition in real time:
oscsend localhost 9000 /idyl/eval s "lfo(freq, amp, dt=10ms) = out |> {
    init: { phase = 0  out = 0 }
    out = amp * sin(phase * tau)
    phase = fmod(phase + dt / (1000 / freq), 1)
}"
# Running instances of lfo pick up the new body on their very next tick.
# Phase state is preserved — the oscillator continues from where it was.

# Change what a running process does:
oscsend localhost 9000 /idyl/eval s "process bass: {
    m = metro(dt=250ms)   // was 500ms — dt updated, no restart
    print(m)
}"
# The metro instance keeps its phase. Only its tick rate changes.

# Add a new instance to a running process:
oscsend localhost 9000 /idyl/eval s "process bass: {
    m  = metro(dt=250ms)
    m2 = metro(dt=125ms)   // new — started fresh
    print(m, m2)
}"
# m survives with state. m2 is a new instance.
```
