# Hot-Reload Bug Diagnosis

Prepared: 2026-05-30  
Investigator: analysis of `src/core/evaluator.cpp`  
Test programs: `/tmp/test_bug1_proper.idyl`, `/tmp/test_bug2a.idyl`, `/tmp/test_bug2b_v2.idyl`, `/tmp/test_bug3.idyl`, `/tmp/test_bug4.idyl`, `/tmp/test_bug4_v6.idyl`

---

## Bug 1: Removing an unnamed expression_stmt segment kills all subsequent unnamed segments

### Symptoms

When one of several "unnamed" temporal expression_stmt segments (e.g., `vapordream(euclid(...), ...)` or `tickA()`) is commented out during hot-reload, **all** other unnamed segments are killed too. Only named segments (e.g., `m = metro(1b)`) and their reactions continue.

### Reproduction

**Initial code:**
```idyl
process bug1: {
    tickA()          // unnamed segment 1
    tickB()          // unnamed segment 2
    tickC()          // unnamed segment 3
    m = metro(dt=600ms)
    print("m=", m)
}
```

**Hot-reloaded code (tickA removed):**
```idyl
process bug1: {
    // tickA() removed
    tickB()
    tickC()
    m = metro(dt=600ms)
    print("m=", m)
}
```

**Result:** tickA, tickB, AND tickC all get killed. Only the named `m` segment survives.

```
[kill_instance] id=1 def_name='tickA'
[kill_instance] id=2 def_name='tickB'
[kill_instance] id=3 def_name='tickC'
```

After reload, only `m= trigger !` continues printing.

**Secondary effect:** The unrecognized `tickB()` and `tickC()` expression_stmts are executed as pre-segment side-effects (line 3869: `exec_stmt(s)`). This creates NEW, untracked temporal instances that ARE subscribed to the scheduler but NOT recorded in `lp.segments`. These ghost instances run indefinitely and cannot be killed without a process stop/restart.

### Root Cause: evaluator.cpp, lines 3845–3860 and 4167–4171

**Phase 1 — Building `new_unnamed_binding_stmts`:**

`diff_and_apply` first builds `old_unnamed_expr_names`: a list of outer function names for each OLD unnamed expression_stmt segment, in source order. For the initial process: `["tickA", "tickB", "tickC"]`.

Then `old_expr_drv_idx = 0` steps through this list while scanning the new source body. For each expression_stmt in the new code, it checks:

```cpp
if (!new_fn.empty()
        && new_fn == old_unnamed_expr_names[old_expr_drv_idx]) {
    is_unnamed_driver = true;
    ++old_expr_drv_idx;
}
```

When tickA is removed, the first expression_stmt encountered is `tickB()`. `new_fn = "tickB"`, but `old_unnamed_expr_names[0] = "tickA"` → **mismatch**. `is_unnamed_driver = false`. `old_expr_drv_idx` stays at 0. tickB is NOT added to `new_unnamed_binding_stmts`. It gets executed as a side-effect pre-segment statement instead (line 3869), creating a rogue temporal instance.

Then `tickC()`: `new_fn = "tickC"` vs `old_unnamed_expr_names[0] = "tickA"` → mismatch again. Still not added.

Result: `new_unnamed_binding_stmts` is empty. `old_expr_drv_idx` never advances past 0.

**Phase 2 — Pass 0.5:**

```cpp
for (auto& old_seg : lp.segments) {
    if (!old_seg.bound_var.empty()) continue;
    if (uidx >= (int)new_unnamed_binding_stmts.size()) {  // size=0
        kill_instance(old_seg.instance_id);               // kills ALL unnamed
        killed_unnamed_ids.insert(old_seg.instance_id);
        ++uidx;
        continue;
    }
    ...
}
```

Since `new_unnamed_binding_stmts` is empty (size=0), **every** old unnamed segment satisfies `uidx >= 0` and gets killed immediately.

### Architectural Explanation

The matching algorithm is strictly sequential and positional: it steps through `old_unnamed_expr_names` in order (via `old_expr_drv_idx`) and matches against new stmts also in order. When the new first stmt (`tickB`) doesn't match the old first name (`tickA`), the algorithm misinterprets the new stmt as a non-driver and never recovers — the index is stuck, so no subsequent stmt can match.

The design assumes that unnamed segment stmts maintain the same relative order and only change at the tail. Removing one from the front or middle breaks this assumption.

### Proposed Fix Approach

1. **Lenient prefix skip:** If `new_fn != old_unnamed_expr_names[old_expr_drv_idx]`, scan forward through `old_unnamed_expr_names` to find a match before giving up. This allows removal from the front/middle.

2. **Name-based matching (if outer function name is reliable):** Instead of strictly positional matching, try to match `new_fn` against ANY remaining entry in `old_unnamed_expr_names` (starting from `old_expr_drv_idx`). If found, advance `old_expr_drv_idx` to `found_idx + 1` and skip the intervening entries (killing those old segments).

3. **Simpler: two-pointer scan:** Walk both lists simultaneously, killing old segments that have no match in the new list (like a merge/diff algorithm).

---

## Bug 2a: No-change hot-reload stops shared reactions

### Symptoms

Hot-reloading a process with NO changes causes all reactions that reference variables from multiple segments (shared_reactions) to stop firing, effectively silencing the process.

### Reproduction

**Code (hot-reloaded with no changes):**
```idyl
process bug2a: {
    m = metro(dt=500ms)
    duree = rndt(100, 800, dt=250ms)
    print("tick m=", m, "duree=", duree)
}
```

`print` references both `m` and `duree`, so after `redistribute_reactions` it becomes a shared_reaction (epoch-deduped). Before hot-reload: prints every 500ms. After no-change hot-reload: silent.

### Root Cause: evaluator.cpp, lines 4092–4124 and 4828

**Step 1 — Pre-redistribute on `pre_segs` (lines 4092–4124):**

A temporary `pre_segs` vector is built from `new_segs`. `redistribute_reactions(pre_segs)` is called. This correctly redistributes: `print` goes to `pre_segs[*].rxn->shared_reactions`. But the copy-back (lines 4116–4123) only copies `.reactions`:

```cpp
nit->second.reactions = pre_segs[idx].rxn->reactions;  // copies ONLY local reactions
// shared_reactions from pre_segs are silently discarded here
```

`new_segs["m"].reactions = []` and `new_segs["duree"].reactions = []` (both empty, because print went to shared_reactions).

**Step 2 — Pass 1 (lines 4641–4653):**

```cpp
old_seg.rxn->reactions = std::move(ns.reactions);  // = [] (empty!)
// old_seg.rxn->shared_reactions is NOT updated — stale from original run
```

**Step 3 — Pass 3 / final redistribute (line 4828):**

```cpp
redistribute_reactions(lp.segments);
```

This operates on the actual `lp.segments`. Since `reactions = []` for all segments, there is nothing to redistribute. `shared_set` is empty → triggers the explicit clear:

```cpp
// No shared reactions — clear any stale entries from a prior run
segments[si].rxn->shared_reactions.clear();  // ← KILLS the print!
```

The `shared_reactions` that held `print` is now empty. The scheduler callbacks see `shared_snap = []` and stop firing the print.

### Architectural Explanation

The hot-reload path has two redistribution steps:
1. `pre_segs` redistribution: produces correct per-segment assignments, but only `.reactions` is harvested; `.shared_reactions` is discarded.
2. Pass 3 redistribution: operates on the now-empty `.reactions` in `lp.segments`, sees nothing to share, and clears the stale `shared_reactions`.

The `shared_reactions` field on `live_segment.rxn` is never populated during hot-reload — it can only be set by `redistribute_reactions`, which requires the reactions to already be in `.reactions` first.

### Proposed Fix Approach

After the pre_segs redistribution (lines 4116–4123), also copy `.shared_reactions` back to `new_segs`:

```cpp
nit->second.reactions        = pre_segs[idx].rxn->reactions;
nit->second.shared_reactions = pre_segs[idx].rxn->shared_reactions;  // add this
```

Then in pass 1, apply both:

```cpp
old_seg.rxn->reactions        = std::move(ns.reactions);
old_seg.rxn->shared_reactions = std::move(ns.shared_reactions);
```

And skip pass 3 (or make it aware that `.reactions` was already redistributed). This requires adding a `shared_reactions` field to `new_seg_info`.

---

## Bug 2b: `collect_stmt_ids` treats non-trigger named parameters as temporal dependencies

### Symptoms

When a reaction like `grainsharp(m, freq, duration=duree)` references a non-trigger named parameter (`duration=duree`) where `duree` is a FASTER temporal function than `m`, `redistribute_reactions` moves the reaction to `duree`'s segment (or makes it shared). The reaction fires at `duree`'s rate instead of only at `m`'s rate.

Although the `spike!` check in `grainsharp` prevents the Csound note from sounding on rest ticks, it still causes:
- Unnecessary reaction calls on every `duree` tick
- Side-effecting expressions (`rnd()`, `'(v, N)` delay operators) evaluated at wrong rate
- Trigger reset/flush overhead for every `duree` tick instead of every `m` tick

### Root Cause: evaluator.cpp, lines 65–72 (collect_expr_ids) and 3550–3595 (one_pass in redistribute_reactions)

`collect_stmt_ids` calls `collect_expr_ids` which recurses into ALL arguments of function calls, including named arguments:

```cpp
case parser::node_t::function_call_expr: {
    auto& e = static_cast<const parser::function_call_expr&>(*expr);
    if (e.call_) {
        collect_expr_ids(e.call_->function_, out);
        for (const auto& arg : e.call_->arguments_)
            if (arg) collect_expr_ids(arg->value_, out);  // includes duration=duree
    }
    break;
}
```

`duree` is collected indiscriminately alongside `m`. The redistribution then uses `duree`'s segment (faster, shorter dt) as the target segment for the reaction, treating `duration` as if it were a temporal trigger.

### Architectural Explanation

`collect_stmt_ids` is designed to find ALL temporal dependencies of a reaction so it fires whenever any of its inputs change. This is correct for trigger parameters but wrong for value parameters like `duration`, `amp`, `ch`, etc.

The fix requires distinguishing temporal triggers (parameters marked `!` in function signatures) from pure value parameters. Without this distinction, any fast-updating value parameter causes the reaction to be incorrectly accelerated.

### Proposed Fix Approach

1. **Semantic annotation:** When parsing `grainsharp(m, freq, duration=duree)`, look up the function definition for `grainsharp` and identify which arguments correspond to `spike!` (trigger) parameters. Only collect identifiers from trigger-parameter arguments.

2. **Simpler heuristic:** Do not collect identifiers from named keyword arguments at all — only collect from positional arguments. Named keyword arguments (`duration=`, `amp=`, `ch=`) are almost always pure value parameters.

3. **Partial fix:** Add an exclusion list of known non-trigger parameter names (duration, amp, ch, etc.) to `collect_expr_ids`.

---

## Bug 3: Flow cursor resets on hot-reload for reactions containing flow access

### Symptoms

A trigger-indexed flow access in a reaction (`fl = notes[c]` where `c` is a metro trigger) resets its cursor to 0 on every hot-reload, losing its current position in the sequence.

### Reproduction

```idyl
flow notes = [10, 20, 30, 40, 50]

process bug3: {
    c = metro(dt=500ms)
    fl = notes[c]
    print("fl=", fl)
}
```

Run: sees `10, 20, 30, 40, 50, 10, 20, 30` (cursor at 3). Hot-reload with minor change. After reload: sees `10, 20, 30...` again (cursor reset to 0).

### Root Cause: evaluator.cpp, line 1332 (cursor key) and lines 4641–4644 (reaction hot-swap)

**Cursor keying (line 1332):**
```cpp
auto& site_cursor_ptr = flow_site_cursors_[fae.access_.get()];
```

The cursor is keyed by the raw pointer to the `flow_access` AST node. This pointer is stable across hot-reloads ONLY for the `binding_stmt` (which is explicitly preserved, see lines 4002–4016):

```cpp
// Preserve the old binding_stmt so that flow_site_cursors_
// (keyed by AST node pointer) survives hot-reload.
nsi.binding_stmt = old_binding ? old_binding : s;
```

**Reaction hot-swap (lines 4641–4644):**
```cpp
old_seg.rxn->binding_stmt = ns.binding_stmt;   // OLD pointer preserved ✓
old_seg.rxn->reactions    = std::move(ns.reactions);  // NEW AST nodes ✗
```

Reactions always get new AST nodes from the freshly parsed hot-reload source. The `flow_access` nodes inside reactions (e.g., `fl = notes[c]`) have NEW pointer values. When the reaction runs post-reload, `flow_site_cursors_[new_ptr]` is not found → a new cursor is created starting at 0.

### Architectural Explanation

The binding_stmt preservation (lines 4002–4016 and 4643) is designed to handle this for the binding expression. However, reactions are not preserved — they're always replaced with new AST nodes. Any flow access inside a reaction (not the driving binding expression) loses its cursor on every hot-reload.

The fix needs to either:
1. Preserve old reaction AST nodes when the source is unchanged, or
2. Transfer cursor state from old reaction AST pointers to new ones on reload.

### Proposed Fix Approach

**Option A (cursor migration):** Before replacing reactions, walk the old reactions to collect all `flow_access` node pointers and their cursor states. Walk the new reactions to map new AST pointers to old cursor values by structural position (reaction index × flow-access position within that reaction).

**Option B (stable reaction keys):** Key flow cursors by a stable identifier rather than AST pointer — e.g., `(process_name, segment_name, reaction_index, flow_access_index_within_reaction)`. This survives AST replacement as long as the structure doesn't change.

**Option C (reaction fingerprint):** Only reset the cursor when the reaction's source text actually changed (compare old vs new reaction AST structure or source span). If unchanged, keep old cursor.

Option B is the most robust and requires changing only the cursor lookup/store logic.

---

## Bug 4: Time-based delay `'(expr, Tms)` uses wrong buffer size when evaluated in epoch-flush context

### Symptoms

`'(m, 300ms)` should delay `m` by 300ms (= N ticks where N = 300ms / tick_dt). When this expression is part of a **shared_reaction** (epoch-flushed), the buffer is incorrectly sized to 1 tick and is reset on every tick that uses a different segment's dt.

### Reproduction

```idyl
process bug4: {
    m = counter(dt=100ms)
    slow = counter(dt=250ms)
    // References both m and slow → shared_reaction (epoch-flushed)
    print("slow=", slow, "m=", m, "d=", '(m, 300ms))
}
```

Expected: `d` = `m` delayed by 3 ticks (300ms / 100ms).  
Observed: `d` is inconsistent — sometimes 0, sometimes 1 tick delay. The delay oscillates due to buffer size thrashing.

In isolation (local reaction context where proc_stop_ctx_ = m instance, dt=100ms):
```idyl
process bug4: {
    m = counter(dt=100ms)
    d_ticks = '(m, 3)
    d_300ms = '(m, 300ms)
    print("m=", m, " d3=", d_ticks, " d300ms=", d_300ms)
}
```
Both `d_ticks` and `d_300ms` correctly show 3-tick delay (behavior is correct).

### Root Cause: evaluator.cpp, lines 1578–1592 (eval_memory_op) and lines 1599–1603 (buffer reset on size change)

**Size computation:**
```cpp
if (moe->op_->delay_count_) {
    value count_val = eval_expr(moe->op_->delay_count_);
    if (count_val.type_ == value_t::time) {
        double dt_ms = 0.0;
        if (proc_stop_ctx_ && proc_stop_ctx_->dt_ms_ > 0.0)
            dt_ms = proc_stop_ctx_->dt_ms_;
        else if (currently_ticking_inst_ && currently_ticking_inst_->dt_ms_ > 0.0)
            dt_ms = currently_ticking_inst_->dt_ms_;
        if (dt_ms > 0.0)
            size = std::max(size_t(1),
                static_cast<size_t>(std::round(count_val.number_ / dt_ms)));
        // If dt_ms == 0.0: size stays at 1 (default)
    }
}
```

**Buffer reset on size change:**
```cpp
if (it == delay_memories_.end() || it->second.buffer.size() != size) {
    memory_buffer buf(size);
    for (auto& slot : buf.buffer) slot = v; // reset with current value!
    delay_memories_[key] = std::move(buf);
}
```

**Epoch flush context:**

When a shared_reaction containing `'(m, 300ms)` fires during the epoch flush (0.1ms one-shot scheduler callback), both `proc_stop_ctx_` and `currently_ticking_inst_` are null (they were cleared after the segment callbacks finished):

```cpp
// In segment callback (line 698):
proc_stop_ctx_ = si.get();  // set ✓
// ... reactions run ...
proc_stop_ctx_ = nullptr;   // cleared ✓

// In epoch flush (line 767-784):
scheduler_->subscribe(0.1, [this](double, double) -> bool {
    // proc_stop_ctx_ == nullptr here
    // currently_ticking_inst_ == nullptr here
    exec_stmt(r);  // evaluates '(m, 300ms) with dt_ms=0 → size=1
```

So in the epoch flush: `dt_ms = 0.0` → `if (dt_ms > 0.0)` guard leaves `size = 1`.

**Buffer thrashing:** If the buffer was previously created with `size = 3` (from a local reaction context where dt_ms=100ms), `it->second.buffer.size() = 3 != 1 = size` → the buffer is RESET to size=1 and initialized with the current value. On the next local reaction call, `size = 3 != 1` → reset again. The buffer oscillates between size=1 and size=3, effectively disabling the delay.

### Architectural Explanation

The time-to-ticks conversion requires knowing the tick period of the RELEVANT temporal instance (the one driving `m`, not the one running the reaction). When evaluated in the epoch-flush context, no "current instance" is available, so the dt is unknown and the fallback to size=1 is incorrect.

The buffer-reset-on-size-change mechanism (designed to handle dt changes like metro(100ms) → metro(200ms)) is correct in isolation, but becomes destructive when size oscillates between valid (3) and fallback (1) on every tick.

### Proposed Fix Approach

1. **Carry segment dt through epoch flush:** Store the triggering segment's dt_ms in a member variable before queuing to epoch_deferred_, and restore it during the epoch flush execution. Something like: annotate each epoch_deferred_ entry with the dt_ms of the triggering segment.

2. **Instance-aware delay size:** Compute size from the INNER expression's driving instance rather than the outer proc_stop_ctx_. If `'(m, 300ms)`, look up `m`'s bound instance in `instance_bindings_` and use that instance's dt_ms.

3. **Stable buffer size:** Do not reset the buffer if size changes due to an ambiguous dt (dt_ms=0). Instead, preserve the existing buffer and use `size=1` only when the buffer is first created.

Option 3 is the simplest fix: in `eval_memory_op`, when `dt_ms == 0.0` and a buffer already exists, use the existing buffer's size rather than defaulting to 1. This prevents the size-thrashing while still correctly initializing new buffers.

```cpp
// Proposed: if dt_ms==0 and buffer already exists, keep existing size
if (dt_ms > 0.0) {
    size = std::max(size_t(1), static_cast<size_t>(std::round(count_val.number_ / dt_ms)));
} else if (it != delay_memories_.end()) {
    size = it->second.buffer.size();  // preserve existing
}
// else: new buffer, size=1 (acceptable fallback)
```

---

## Summary Table

| Bug | Location | Root Cause | Severity |
|-----|----------|------------|----------|
| **1** | `diff_and_apply`, lines 3845–3860 | Positional matching of unnamed segments uses sequential index that gets stuck when first segment is removed | High — kills all unnamed segments on removal |
| **2a** | `diff_and_apply`, lines 4092–4124 + line 4828 | Pre-redistribute copies only `.reactions`, not `.shared_reactions`; pass 3 then clears stale shared_reactions | High — silences multi-segment reactions on any hot-reload |
| **2b** | `collect_expr_ids`, lines 65–72 | Named keyword arguments (duration=, amp=) collected as temporal deps, causing wrong redistribution target | Medium — extra calls, wrong firing rate |
| **3** | `eval_expr` flow_access, line 1332 + pass 1, line 4644 | Flow cursor keyed by AST pointer; reaction AST is always replaced on hot-reload, new pointers have no cursor entry | Medium — flow sequence restarts on every hot-reload |
| **4** | `eval_memory_op`, lines 1578–1603 | `proc_stop_ctx_` is null in epoch-flush context; fallback size=1 + buffer-reset-on-mismatch causes buffer thrashing | Low–Medium — delay operators unreliable in multi-segment reactions |

## Test Programs (for reproduction)

All test programs use `build/idyl --listen 9099 -p <name>` and the OSC sender at `/tmp/send_osc.py`.

- Bug 1: `/tmp/test_bug1_proper.idyl` + `/tmp/test_bug1_reload_proper.idyl`
- Bug 2a: `/tmp/test_bug2a.idyl` (no-change reload reproduces it)
- Bug 2b: `/tmp/test_bug2b_v2.idyl` (demonstrates firing-rate acceleration)
- Bug 3: `/tmp/test_bug3.idyl` + `/tmp/test_bug3_reload.idyl`
- Bug 4: `/tmp/test_bug4_v6.idyl` (shared-reaction context required)
