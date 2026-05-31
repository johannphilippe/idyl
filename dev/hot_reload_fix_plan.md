# Hot-Reload Fix Plan (ready to implement)

Companion to `hot_reload_diagnosis.md`. Each fix below is concrete and self-contained.
Implement + build + test each independently on **port 9099** (user runs live idyl on 9000).

---

## Bug 1 — Removing an unnamed driver kills all subsequent unnamed segments  (HIGH)

**Where:** `diff_and_apply` body scan, evaluator.cpp ~lines 3845–3860.

**Root cause:** New expression_stmts are classified as "unnamed drivers" by matching their
OUTER function name against `old_unnamed_expr_names[old_expr_drv_idx]` positionally. When a
driver is removed from the front/middle, the index sticks and every later driver is
misclassified as a reaction/side-effect (then exec'd once → ghost instances; the real
segments get killed in pass 0.5 because `new_unnamed_binding_stmts` is empty).

**Fix:** Replace the positional name-match with a *semantic* test: an expression_stmt is an
unnamed driver iff `find_inner_temporal_call(s)` returns an inner fn that is actually a
**temporal function**. Add a helper:

```cpp
auto is_temporal_fn = [&](const std::string& name) -> bool {
    if (name.empty()) return false;
    if (name == "sync" || name == "phasor") return true;           // native temporals
    std::shared_ptr<const parser::function_definition> fdef;
    { std::shared_lock lk(defs_mutex_);
      auto it = function_defs_.find(env_.intern(name));
      if (it != function_defs_.end()) fdef = it->second; }
    if (!fdef) return false;
    if (fdef->lambda_block_) return true;                          // has |> block
    for (const auto& p : fdef->parameters_)                        // has dt param
        if (p && (p->name_ == "dt" || p->has_default_time_)) return true;
    return false;
};
```

Then in the `bname.empty()` branch, set `is_unnamed_driver` for expression_stmts as:
```cpp
auto [inner_fn, inner_fce] = find_inner_temporal_call(s);   // reuse pass-0.5 helper, hoist it
if (is_temporal_fn(inner_fn)) is_unnamed_driver = true;
```
Drop the `old_unnamed_expr_names` / `old_expr_drv_idx` machinery entirely (no longer needed).

Rationale: `cs_note(cs, tune(...))` → inner=`tune` (pure) → NOT driver → reaction (correct).
`vapordream(euclid(...))` → inner=`euclid` (lambda) → driver (correct). `grainsharp(m,500)` →
no inner call → reaction (correct). Robust to add/remove/reorder.

**Optional follow-up (state preservation):** pass 0.5 still zips old-unnamed vs
new_unnamed_binding_stmts positionally. After the step-1 fix nothing *stops*, but a
middle removal reshuffles which instance backs which slot. A later improvement is to align
by signature (`outer_fn|inner_fn`) with a greedy LCS. Not required to fix the reported bug.

**Test:** `/tmp/test_bug1_proper.idyl` (3 unnamed drivers) → reload with first removed →
expect the other two to keep firing.

---

## Bug 2a — No-change reload silences multi-segment (shared) reactions  (HIGH)

**Where:** the "Pre-redistribute reactions" block, evaluator.cpp ~lines 4092–4125.

**Root cause:** The pre-redistribute runs `redistribute_reactions(pre_segs)` which moves
cross-segment reactions into `shared_reactions` and empties `.reactions`. Only `.reactions`
is copied back → empty. Pass 1 writes empty `.reactions` to live segments; pass 3's
redistribute then sees nothing to share and CLEARS the stale `shared_reactions` (evaluator.cpp
~line 3635). The shared `print` is lost.

**Fix (simplest, removes the whole class of bug):** DELETE the entire pre-redistribute block
(lines ~4092–4125). It is redundant: `hot_reload` holds `eval_mutex_` for the whole update, so
no scheduler tick can run until `diff_and_apply` returns, and the FINAL `redistribute_reactions
(lp.segments)` at pass 3 (~line 4828) runs before unlock. Keep `new_segs[var].reactions` as the
full (un-redistributed) list through pass 1/2; pass 3 distributes once, correctly, into both
`.reactions` and `.shared_reactions`.

Verify after deletion: pass 1 still does `old_seg.rxn->reactions = std::move(ns.reactions)`
with the FULL list, and pass 2 seeds new segments with their full lists. Then pass 3 handles
the rest.

**Test:** `/tmp/test_bug2a.idyl` (a `print(m, duree)` referencing two segments) → no-change
reload → expect prints to continue.

---

## Bug 2b — Named keyword args treated as temporal dependencies  (MEDIUM)

**Where:** `collect_expr_ids`, function_call_expr case, evaluator.cpp ~lines 58–62.

**Root cause:** Redistribution collects identifiers from ALL call arguments, including
`duration=duree`, `amp=...`, `ch=...`. A fast value param (`duree`) then wins "fastest
segment" and the reaction fires at the wrong rate.

**Fix:** In the function_call_expr case, only recurse into POSITIONAL args for id collection:
```cpp
case parser::node_t::function_call_expr: {
    auto& e = static_cast<const parser::function_call_expr&>(*expr);
    if (e.call_) {
        collect_expr_ids(e.call_->function_, out);
        for (const auto& arg : e.call_->arguments_)
            if (arg && arg->name_.empty())      // positional only
                collect_expr_ids(arg->value_, out);
    }
    break;
}
```
The trigger (`spike!`) is conventionally positional, so dependency detection stays correct
while value kwargs no longer accelerate the reaction. Confirm `collect_expr_ids` still
recurses into memory_op/delay nodes so `'(m,100ms)` positional triggers are detected.

**Test:** `/tmp/test_bug2b_v2.idyl` — reaction `f(m, ..., duration=duree)` should fire at m's
rate, not duree's.

---

## Bug 3 — Flow cursor resets on reload for flow access inside reactions  (MEDIUM)

**Where:** cursor keyed by AST pointer `flow_site_cursors_[fae.access_.get()]` (~line 1332);
reactions always replaced with fresh AST in pass 1 (`old_seg.rxn->reactions = std::move(...)`,
~line 4644) and pass 0.5.

**Root cause:** New reaction AST → new `flow_access` pointers → no cursor entry → restart at 0.
Binding-stmt flow access is fine (old binding_stmt pointer is preserved); only reaction flow
access loses cursors.

**Fix (cursor migration, option A):** Just before replacing a segment's reactions, capture
old flow-access pointers in order and their cursor values; after assigning new reactions,
capture new flow-access pointers in order; transfer cursors by position. Reuse the existing
`collect_flow_access_nodes` (it walks an expr; wrap to walk a stmt's exprs). Apply in BOTH
pass 1 (named) and pass 0.5 (unnamed) at the reaction-swap site, and at pass-2 new-segment
seeding if the segment existed before.

```cpp
// helper
auto migrate_cursors = [&](const std::vector<parser::stmt_ptr>& oldR,
                           const std::vector<parser::stmt_ptr>& newR){
    std::vector<const parser::node*> oldN, newN;
    for (auto& s : oldR) collect_flow_access_nodes_in_stmt(s, oldN);
    for (auto& s : newR) collect_flow_access_nodes_in_stmt(s, newN);
    if (oldN.size() != newN.size()) return;        // structural change → let reset
    for (size_t i = 0; i < oldN.size(); ++i) {
        auto it = flow_site_cursors_.find(oldN[i]);
        if (it != flow_site_cursors_.end())
            flow_site_cursors_[newN[i]] = it->second;
    }
};
```
Call `migrate_cursors(old_seg.rxn->reactions, ns.reactions)` BEFORE the
`reactions = std::move(ns.reactions)` swap.

(Option B — re-key cursors by `(proc, segment, reaction_idx, access_idx)` — is more robust but
touches every cursor read/write; defer.)

**Test:** `/tmp/test_bug3.idyl` (`fl = notes[c]` trigger-indexed) → advance a few steps →
reload → expect it to continue, not restart at index 0.

---

## Bug 4 — Time-based delay `'(expr, Tms)` buffer thrashes in epoch-flush  (LOW–MED)

**Where:** `eval_memory_op`, evaluator.cpp ~lines 1578–1603.

**Root cause:** Converting a time delay to ticks needs the driving instance's dt. In the
epoch-flush one-shot, both `proc_stop_ctx_` and `currently_ticking_inst_` are null → dt=0 →
fallback `size=1`. The buffer-reset-on-size-change then thrashes the buffer between size 3
(local context) and size 1 (flush), disabling the delay.

**Fix (option 3, smallest):** when `dt_ms == 0` and a buffer already exists, keep its size
instead of defaulting to 1:
```cpp
if (dt_ms > 0.0) {
    size = std::max(size_t(1),
        static_cast<size_t>(std::round(count_val.number_ / dt_ms)));
} else if (it != delay_memories_.end()) {
    size = it->second.buffer.size();   // preserve existing; don't thrash
}
// else: brand-new buffer with unknown dt → size stays 1 (acceptable seed)
```
(`it` = the `delay_memories_.find(key)` iterator already computed in this function.)

**Test:** `/tmp/test_bug4_v6.idyl` (delay inside a shared reaction) → expect stable N-tick
delay, not oscillating 0/1.

---

## Suggested order & verification

1. Bug 2b (tiny, isolated) → build → test rate.
2. Bug 4 (tiny, isolated) → build → test delay stability.
3. Bug 2a (delete pre-redistribute) → build → test no-change reload keeps shared reactions.
4. Bug 1 (driver detection rewrite) → build → test removal of a middle driver.
5. Bug 3 (cursor migration) → build → test trigger-indexed flow survives reload.

Build: `cmake --build build -j$(nproc)`  (success = "Built target idyl", 0 "error:").
Run server: `nohup ./build/idyl --listen 9099 <file> > /tmp/o.txt 2>&1 &`
Start proc:  `python3 tests/debug/send_start.py` adapted to port 9099, or the OSC snippet.
Reload:      send `/idyl/eval` with the v2 source string to 127.0.0.1:9099.
Measure by counting matched lines in /tmp/o.txt across a known time window.
