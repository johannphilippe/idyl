# Hot reload — appendix 02: pain points and architectural fixes

This document analyses the specific implementation details in the current codebase that make hot reload difficult, and proposes targeted architectural changes that would reduce the cost from "very hard" to "medium" without requiring a rewrite.

References are to `src/core/evaluator.cpp` and related files.

---

## Pain point 1 — Reactions are baked into scheduler lambda captures

**Where**: `evaluator.cpp`, process block case, lines ~413–482.

```cpp
inst->subscription_id_ = scheduler_->subscribe(inst->dt_ms_,
    [this, def_ptr, weak_inst, var, binding, reactions, catches, ...]
    (double t, double) -> bool {
        ...
        for (const auto& r : reactions)   // ← captured by value at subscribe time
            exec_stmt(r);
        ...
    });
```

The `reactions` vector (a `vector<stmt_ptr>`) and the `catches` shared_ptr are captured **by value** into the scheduler callback closure at subscription time. There is no handle to update them after the fact.

**Consequence for hot reload**: To change what a running instance reacts to, you must unsubscribe and re-subscribe. That is safe but causes a brief gap (up to one tick) where the instance ticks without reactions. More subtly, if the instance is unsubscribed and re-subscribed, the drift-free scheduling restarts from `now`, breaking rhythmic continuity.

**Fix**: Introduce a thin indirection layer — a shared mutable reaction record:

```cpp
struct reaction_set {
    parser::stmt_ptr             binding_stmt;
    std::vector<parser::stmt_ptr> reactions;
    std::shared_ptr<std::vector<catch_info>> catches;
};

// Per-segment, created once, pointed to by the closure:
auto rxn = std::make_shared<reaction_set>(...);

inst->subscription_id_ = scheduler_->subscribe(inst->dt_ms_,
    [this, ..., rxn](double t, double) -> bool {
        ...
        for (const auto& r : rxn->reactions)   // ← live pointer, not a copy
            exec_stmt(r);
        ...
    });
```

Hot reload then writes new AST into `rxn->reactions` and `rxn->catches` under a mutex. The next tick picks up the new code automatically — no re-subscribe needed, no scheduling gap.

**Cost to implement**: medium. Requires wrapping 4–5 captured variables into the struct and adding a mutex. The subscription machinery is untouched.

---

## Pain point 2 — `epoch_resets_` holds raw pointers into the environment

**Where**: `evaluator.hpp` member `std::vector<value*> epoch_resets_`; written in the scheduler callback:

```cpp
if (!var.empty()) {
    value* bound = env_.lookup(var);
    if (bound && bound->type_ == value_t::trigger)
        epoch_resets_.push_back(bound);
}
```

`env_.lookup(var)` returns a raw pointer into the environment's current scope frame. The frame is heap-allocated and kept alive by the process scope push (never popped for temporal processes). The assumption is that the frame is stable for the lifetime of the subscription.

**Consequence for hot reload**: If a hot reload stops the old process (`stop_process`) and starts a new one (`start_process`), a new scope frame is pushed. But stale raw pointers from the previous frame may still be in `epoch_resets_` when the next scheduler tick fires. This is a **use-after-free**.

Even without a crash, a hot reload that changes `dt` on a surviving instance via `update_dt` will keep the existing subscription alive — but if the scope frame was replaced, `epoch_resets_` now holds dangling pointers.

**Fix — option A (simplest)**: Flush `epoch_resets_` as part of `stop_process`. Since `stop_process` already marks instances as inactive and unsubscribes them, no more callbacks will add to the list for those instances. Clearing the list during stop is safe:

```cpp
bool evaluator::stop_process(const std::string& name) {
    ...unsubscribe loop...
    epoch_resets_.clear();   // ← add this
    active_process_instances_.erase(it);
    return true;
}
```

**Fix — option B (correct for partial-diff hot reload)**: Store the variable name in the epoch reset entry rather than a raw pointer, and resolve it on flush:

```cpp
std::vector<std::string> epoch_reset_vars_;   // names, not pointers

// In callback:
epoch_reset_vars_.push_back(var);

// At epoch transition:
for (const auto& v : epoch_reset_vars_) {
    value* p = env_.lookup(v);
    if (p) *p = value::rest();
}
epoch_reset_vars_.clear();
```

This is slightly slower (one map lookup per trigger per epoch) but is correct even if the scope frame changes between tick and flush.

**Cost to implement**: low. A handful of lines changed.

---

## Pain point 3 — `instance_bindings_` is a flat global map

**Where**: `evaluator.hpp` member `std::unordered_map<std::string, uint64_t> instance_bindings_`.

The map is `varname → instance_id` across ALL processes. Two processes that both use `m = metro(...)` collide: the second overrides the first's binding.

**Consequence for hot reload**: The diff algorithm for a named process needs to know "which instance ID corresponds to variable `m` in process `bass`". With a flat map, that lookup is ambiguous if multiple processes share variable names. More critically, when an instance in process `bass` is replaced during hot reload, writing a new ID to `instance_bindings_["m"]` corrupts any other process that also uses `m`.

**Fix**: Scope the map per process:

```cpp
// Before:
std::unordered_map<std::string, uint64_t> instance_bindings_;

// After:
std::unordered_map<std::string,
    std::unordered_map<std::string, uint64_t>> instance_bindings_;
// key: process name → (var name → instance id)
// Anonymous processes use "" as the process name key.
```

The `::` accessor (`a::step` → lookup instance for `a`) currently uses the flat map. It would change to look up by current process name, which the evaluator can track as a context variable (already done partially via `process_filter_`).

**Cost to implement**: medium. All call sites of `instance_bindings_` need updating (~10 places). The `::` accessor in `eval_expr` needs a current-process-name context.

---

## Pain point 4 — `active_process_instances_` stores IDs without variable names

**Where**: `evaluator.hpp` member `std::unordered_map<std::string, std::vector<uint64_t>> active_process_instances_`.

The map stores `process_name → list of instance IDs`. Variable names are not stored alongside IDs. To diff old vs. new, the hot-reload code must reconstruct the `varname → id` mapping by cross-referencing `instance_bindings_` — which (per pain point 3) is a flat global map.

**Consequence for hot reload**: The diff algorithm becomes:

```
for each varname in old_process_body:
    id = instance_bindings_[varname]   // global, may be wrong
    old_instance = instances_[id]
```

This is fragile and requires keeping the old process AST available for walking.

**Fix**: Change the value type to a richer struct that records the variable name:

```cpp
struct process_instance_entry {
    uint64_t    instance_id;
    std::string bound_var;
    double      dt_ms;            // snapshot for dt-only change detection
    std::string def_name;         // function name, for function-change detection
};

std::unordered_map<std::string,
    std::vector<process_instance_entry>> active_process_instances_;
```

The diff algorithm then becomes a straightforward join on `bound_var` between the old entries and the new process AST.

**Cost to implement**: low-medium. Mechanical changes to the 4–5 call sites that build or read `active_process_instances_`.

---

## Pain point 5 — No thread safety on `function_defs_` and `flow_defs_`

**Where**: `evaluator.hpp` members `function_defs_` and `flow_defs_` are plain `unordered_map`s.

The scheduler thread reads from `function_defs_` on every tick (in `tick_instance`, which calls `function_defs_.find(inst->def_name_)`). The main thread, during hot reload, would write to `function_defs_` (insert or replace entries). `unordered_map` is not thread-safe for concurrent reads and writes to different keys.

**Consequence for hot reload**: Phase 1 of hot reload (global function redefinition) is immediately a data race. It will occasionally corrupt the map or crash.

**Fix**: Add a `std::shared_mutex` protecting both maps:

```cpp
mutable std::shared_mutex defs_mutex_;
```

- Scheduler thread ticks: `std::shared_lock lock(defs_mutex_)` before `function_defs_.find(...)`.
- Hot reload (main thread): `std::unique_lock lock(defs_mutex_)` before insert/replace.

`flow_call_cache_` (also read on scheduler thread) needs the same protection, or can be cleared under the exclusive lock on hot reload.

**Cost to implement**: low. Add the mutex and wrap the two access patterns. The shared lock on the tick path adds a tiny overhead (nanoseconds) on every tick — negligible.

---

## Pain point 6 — Reaction redistribution is inlined, not a reusable function

**Where**: `evaluator.cpp`, lines ~333–366. The algorithm that moves reactions to the fastest-ticking segment they depend on is written inline inside the `process_block` case of `exec_stmt`.

**Consequence for hot reload**: Phase 3 (live process diffing) must redo this redistribution after any structural change to the process (instance added, removed, or changed). Currently the logic cannot be called from anywhere else without copy-pasting it.

**Fix**: Extract into a standalone function:

```cpp
static void redistribute_reactions(
    std::vector<temporal_segment>& segments,
    const std::unordered_map<uint64_t, std::shared_ptr<function_instance>>& instances);
```

Hot reload calls this after completing the diff and before rebuilding subscription closures. This is purely a refactor — zero semantic change.

**Cost to implement**: very low. Mechanical extraction, ~5 minutes.

---

## Pain point 7 — `delay_memories_` keyed by AST node pointer

**Where**: `evaluator.hpp` member `std::unordered_map<const parser::node*, memory_buffer> delay_memories_`.

The delay/sample-memory operator `'(expr)` is keyed to the specific `parser::node*` pointer of that expression in the AST. This gives each delay site its own independent circular buffer.

**Consequence for hot reload**: When a function is hot-reloaded, a fresh AST is parsed from the new source string. Even if the expression `'(x)` is textually identical in the new code, the new AST has a different `node*` pointer. The old memory buffer is orphaned; the new code starts a fresh buffer (all zeros). This breaks any sample-delay continuity the running instance had.

This is a fundamental mismatch between "stability of the pointer" (which the current key relies on) and "stability of the logical expression" (which the user expects after a minor edit).

**Fix options**:

- **Accept the limitation**: delay buffers reset on hot reload of the containing function. Document it. For live coding this is usually acceptable — the user is making a deliberate change.
- **Key by (function qualified name, source line, source column)**: stable across re-parses of textually unchanged code. Requires the AST to carry source position on every node (already done — idyl's AST has `line_` and `col_` on every node). Implement a secondary index `map<tuple<string,int,int>, memory_buffer>`. But this breaks if the user adds a line above (shifts line numbers), which is a common live-coding operation.
- **Key by (function name, expression text hash)**: hash the printed form of the expression subtree. More robust than line numbers but has its own edge cases (two identical delay expressions in the same function would collide).

**Recommendation**: accept the limitation for now. Document that delay buffers reset when the containing function is hot-reloaded. This is the only pain point with no clean cheap fix.

---

## Pain point 8 — `flow_call_cache_` becomes stale on flow redefinition

**Where**: `evaluator.hpp` member `flow_call_cache_` (LRU cache, keyed by flow name + args).

When a parametric flow definition is hot-reloaded, the cache may still return the old `flow_data` for previously-seen arguments.

**Fix**: On flow redefinition during hot reload, iterate the cache and evict all entries whose key's `name` field matches the redefined flow:

```cpp
// In the hot-reload handler, after updating flow_defs_[name]:
std::erase_if(flow_call_cache_, [&name](const auto& kv) {
    return kv.first.name == name;
});
// Also clear the insertion-order list:
flow_cache_order_.remove_if([&name](const auto& k) {
    return k.name == name;
});
```

**Cost to implement**: very low. 5 lines.

---

## Pain point 9 — `stored_processes_` not populated in non-listen mode

**Where**: `evaluator.cpp` process block case, lines ~212–224:

```cpp
if (listen_mode_) {
    ...
    stored_processes_[proc.name_] = ...;  // ← only stored in listen mode
    break;
}
```

In normal mode (no `--listen`), named process blocks are executed immediately and their ASTs are never stored. Hot reload's diff algorithm requires the old AST to determine which variables existed before.

**Fix**: Always store named process blocks, regardless of listen mode:

```cpp
// At the top of the process_block case, before the listen_mode_ branch:
if (!proc.name_.empty()) {
    stored_processes_[proc.name_] =
        std::static_pointer_cast<parser::process_block>(...);
}

if (listen_mode_) {
    ...
    break;
}
// else fall through to execute
```

**Cost to implement**: very low. 3 lines moved.

---

## Summary: which changes to make first

Ordered by impact/cost ratio:

| # | Change | Enables | Cost |
|---|--------|---------|------|
| 6 | Extract reaction redistribution function | Phase 3 implementation | Trivial |
| 9 | Always store named process ASTs | Phase 3 diffing | Trivial |
| 8 | Invalidate flow cache on hot reload | Phase 1 correctness | Trivial |
| 2A | Flush `epoch_resets_` in `stop_process` | Prevents use-after-free | Very low |
| 5 | `shared_mutex` on `function_defs_` | Phase 1 thread safety | Low |
| 1 | Reaction indirection (`shared_ptr<reaction_set>`) | Phase 3 without re-subscribe | Medium |
| 4 | Rich `active_process_instances_` entries | Phase 3 diffing | Low–medium |
| 3 | Per-process `instance_bindings_` | Correctness with multiple processes | Medium |
| 7 | Delay buffer continuity | — | No good fix now |

Changes 6, 9, 8, 2A, and 5 are small, safe, isolated refactors that can be done before any hot-reload work begins and make everything else easier. They do not change any observable behavior of the existing runtime.

Change 1 (reaction indirection) is the single biggest enabler for Phase 3: it allows reaction code to be updated without touching the scheduler subscription, preserving drift-free timing continuity. Without it, every hot reload of a running process causes a scheduling reset.
