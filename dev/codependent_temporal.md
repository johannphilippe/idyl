# Co-dependent temporal parameters in idyl

## The problem

```idyl
flow whisper = {
    base:     [c4]
    deg:      [0, 2, -2, 4]
    duration: [8b, 16b, 8b, 12b]
}

process test: {
    tempo(60)
    speed = whisper.duration[0]   // initial period
    m = metro(speed)               // metro driven by speed

    w = whisper[m]
    on(m): {
        speed = w.duration         // reaction updates speed
    }
}
```

The intended semantics: each element of `whisper` dictates how long we stay on it before advancing. But it doesn't work as intended — the timing is always one step behind, and `sync` can't be used at all.

---

## Root cause: causal ordering in one tick

Within a single scheduler tick, the evaluation order is fixed:

```
① tick_instance(metro)
    └─ re-reads `speed` from env_   ← OLD value from previous tick
    └─ calls scheduler::update_dt(id, old_speed)
    └─ outputs trigger(true)

② exec_stmt(binding: m = metro(speed))

③ reactions run:
    w = whisper[m]              ← cursor advances
    on(m): speed = w.duration  ← NEW speed written to env_

④ scheduler fires callback, re-reads dt_ms_ for next fire
   (but update_dt was already called in ① with old_speed)
```

The scheduler's `update_dt` is called at step ① with the old speed. By step ③ the new speed is available, but the next fire is already scheduled.

Because `sync` computes `first_fire` at instantiation and uses a phase-locked grid, it has no re-read mechanism at all — it is completely static.

---

## Why Faust's `letrec` doesn't map directly

In Faust, every signal is synchronous and evaluated at the sample rate. A `letrec` block computes a fixed point over simultaneous equations — all signals in the block are evaluated as if they were simultaneous.

In idyl, time is event-driven. `metro(speed)` fires at a specific wall-clock instant; updating `speed` inside the tick that fires is structurally *after* the fire decision. True simultaneity would require a fixed-point solver and block the tick until convergence — impractical for a real-time system.

The right mental model is **causal co-recursion**: the value produced in tick N is used to schedule tick N+1. Not simultaneous, but precisely one tick of feedback.

---

## Proposal A — Post-tick dt re-read (minimal engine fix)

### Idea

After all reactions for a segment have run, re-evaluate the `dt` expression of the driving instance and call `update_dt` if it changed.

The scheduler already re-reads `dt_ms_` *after* the callback returns (drift-free scheduling design). A second `update_dt` call at the end of the tick overrides the one done in `tick_instance`, and the scheduler uses it for the next fire.

### Evaluation order with this fix

```
① tick_instance(metro)
    └─ reads speed = old_speed  (still one tick behind, but this is the CURRENT fire)
    └─ update_dt(id, old_speed)   ← schedules hypothetical next fire

③ reactions:
    w = whisper[m], speed = w.duration   ← new speed written

④ post-tick re-read:
    re-evaluate speed from env_
    → update_dt(id, new_speed)   ← OVERRIDES step ①'s scheduling

Scheduler re-reads dt_ms_ after callback → uses new_speed → next fire at T + new_speed ✓
```

### Trace with fix

| fire # | old speed used in ① | new speed from reactions | next wait |
|--------|---------------------|--------------------------|-----------|
| 1      | 8b (initial)        | duration[0] = 8b         | 8b ✓      |
| 2      | 8b                  | duration[1] = 16b        | 16b ✓     |
| 3      | 16b                 | duration[2] = 8b         | 8b ✓      |
| 4      | 8b                  | duration[3] = 12b        | 12b ✓     |

The sequence plays correctly from the second fire onwards. Fire #1 always uses the initial speed (correct — it IS the duration of element 0).

### Scope

This fixes `metro(dynamic_param)` where `dynamic_param` is updated by reactions of the same segment. It does NOT fix `sync` (see below).

### Implementation sketch

In the scheduler tick closure (after reactions run, before epoch flush):

```cpp
// Re-read dt after reactions so self-modifying rhythm patterns (metro(speed)
// where speed = w.duration) take effect on the very next fire.
if (inst->subscription_id_ != 0 && !inst->native_update_) {
    auto expr_it = instance_param_exprs_.find(si->id_);
    if (expr_it != instance_param_exprs_.end()) {
        auto dt_it = expr_it->second.find("dt");
        if (dt_it != expr_it->second.end() && dt_it->second) {
            value new_dt = eval_expr(dt_it->second);
            double new_dt_ms = new_dt.as_number();
            if (new_dt_ms > 0.0 && std::abs(new_dt_ms - si->dt_ms_) > 0.01) {
                si->dt_ms_ = new_dt_ms;
                scheduler_->update_dt(si->subscription_id_, new_dt_ms);
            }
        }
    }
}
```

### Caveats

- Applies only to user-defined temporals with a `dt` parameter. `sync` uses a phase-grid and needs a separate treatment.
- For `sync` to support dynamic periods, it would need to re-snap to the new period's grid each time the period changes — semantics TBD (re-snap from last fire? from clock origin?).

---

## Proposal B — Self-adjusting period inside `|>` blocks

### Idea

Allow a `|>` update block to assign to `dt` — a special assignment that changes the instance's own scheduling interval at the end of the tick.

```idyl
seq(flow, dt=10ms) = out |> {
    init: {
        cursor = 0
        out    = flow[0]
    }
    out    = flow[cursor]
    dt     = out.duration          // ← self-adjusting: sets own period
    cursor = fmod(cursor + 1, len(flow))
}
```

`dt = out.duration` inside the update block would be treated as a directive to call `update_dt` for this instance at the end of the tick, using the assigned value.

### Usage

```idyl
process test: {
    tempo(60)
    w = seq(whisper)   // auto-advances with whisper.duration as period

    on(w): {
        print(w.deg, w.duration)
        lushpad(!, mtof(deg(w.base, minor, w.deg, just)), duration=w.duration)
    }
}
```

`seq` would be a stdlib function, not a built-in. The engine only needs to support the `dt = expr` directive inside `|>` blocks.

### How it works internally

At the end of `tick_instance`, after the update block runs, check if `inst.next_["dt"]` was assigned. If so, treat it as an `update_dt` call:

```cpp
// Inside tick_instance, after commit():
auto dt_it = inst.next_.find("dt");  // or inst.current_ after commit
if (dt_it != inst.next_.end()) {
    double new_dt = dt_it->second.as_number();
    if (new_dt > 0.0 && std::abs(new_dt - inst.dt_ms_) > 0.01) {
        inst.dt_ms_ = new_dt;
        if (scheduler_ && inst.subscription_id_ != 0)
            scheduler_->update_dt(inst.subscription_id_, new_dt);
    }
}
```

The `dt` parameter is effectively a reserved name inside `|>` blocks with a semantic of "change my period."

### Trace

`seq` starts with dt=10ms (initial), first tick fires immediately, reads element 0 and sets dt = 8b. Second tick fires 8b later, reads element 1, sets dt = 16b. Etc. The period is self-consistent — no lag.

### Advantages over Proposal A

- No lag at all: the period update is internal to the instance, happening inside the same tick as the cursor advance.
- Enables a clean library function `seq(flow)` without any new built-ins.
- Generalises to other self-modifying patterns (e.g. `step_env` that modulates its own rate based on output).
- Works as a user-land pattern, not special engine magic.

### The `seq` library function

With Proposal B, `seq` can be written in idyl itself:

```idyl
// seq(flow, dt=10ms) — fires according to flow.duration, outputs current element
// flow must have a `duration` member.
seq(flow, dt=10ms) = out |> {
    init: {
        cursor = 0
        out    = flow[0]
    }
    out    = flow[cursor]
    dt     = out.duration
    cursor = fmod(cursor + 1, len(flow))
}
```

If `flow` doesn't have a `duration` member, `dt` would be undefined/nil, causing an error. A safer version:

```idyl
seq(flow, duration_key=0, dt=10ms) = out |> {
    init: {
        cursor = 0
        out    = flow[0]
    }
    out    = flow[cursor]
    dt     = (out.duration > 0) ? out.duration ; dt  // fallback to current dt
    cursor = fmod(cursor + 1, len(flow))
}
```

---

## Sync and co-dependence

`sync` fires on a phase-locked grid relative to the clock origin. Dynamic periods are fundamentally ambiguous for `sync`: does a period change mean "re-snap to a new grid from now" or "re-snap from the clock origin"?

Two options for a dynamic-period `sync`:

**Option 1 — Relative re-snap**: when the period changes, the next fire is `new_period` ms after the last fire. This gives continuous rhythmic sequences without grid alignment.

**Option 2 — Absolute re-snap**: when the period changes, find the next multiple of `new_period` from the clock origin (same as current `sync` logic, but with the new period). This preserves grid alignment when switching between related periods (e.g., 4b → 8b stays on the beat grid).

For rhythmic sequences like `whisper`, Option 1 is natural. For tempo-synchronised transitions, Option 2 is better. A flag `resync=true/false` could select between them.

---

## Recommendation

**Short term**: implement Proposal A (post-tick dt re-read). It is a small, safe change that fixes the existing `metro(speed)` pattern without any syntax additions. It also makes the engine more robust for any dynamic-parameter temporal function.

**Medium term**: implement Proposal B (`dt = expr` in `|>` blocks) alongside a `seq(flow)` stdlib function. This gives idyl a clean, composable, zero-lag primitive for rhythm-as-data patterns. It is the idiomatic answer to the co-dependence problem for the common case.

**Long term**: consider a `sync` variant with a `resync` policy for grid-aware dynamic periods.

The `feedback { }` block (Faust-style letrec) is probably unnecessary if `seq` covers the main use case. True simultaneous co-recursion would require a fixed-point evaluator, adds significant complexity, and is hard to reason about in an event-driven context. The causal one-tick feedback model is more predictable and sufficient for practical music patterns.

---

## Implementation status (2026-06)

**Proposal B — implemented.**  `dt = expr` inside a `|>` update block now updates
the instance's scheduling period for the next fire.  Decided to use the plain
name `dt` (not a new keyword): the update block already treats `phase`, `level`,
etc. as mutable state, so writing `dt` is consistent.  When the body assigns `dt`
and the caller also passed a dynamic `dt=` expression, the caller's expression is
disconnected (erased from `instance_param_exprs_`) and a one-time warning is
printed — the body wins.  Implemented in both the `assignment` and
`function_definition` branches of `tick_instance`.

A companion fix pre-initializes update-block LHS variables (e.g. `cursor`) to 0
before the seed/first-tick pass, so counters absent from the init block don't
trip an "undefined identifier" warning.

`seq(in_flow, dt=10ms)` shipped in stdlib §6.  It reads `out.duration` each step
with a `(out.duration > 0) ? out.duration ; dt` fallback for flows without a
duration member.  Tests: `tests/temporal/seq_basic`, `seq_no_duration`,
`seq_dt_body`, `seq_codependent`.

**Inline dynamic dt — implemented.**  `metro(dt=lfo(0.1hz, dt=50ms))` and similar:
a temporal call in an argument position is created once, scheduled independently
on its own clock (so it evolves in wall-clock time), and the parent samples its
current output each tick instead of re-instantiating it (which previously leaked
a new instance every tick and froze the value).  Mechanism: eval_call captures
arg sub-instances; instantiate_temporal adopts them into
`function_instance::param_subinstances_` and calls `schedule_instance`; the
scheduler pool skips re-ticking anything with its own subscription but still
exposes it via `retick_pool_`; `tick_instance` exposes `param_subinstances_` via
`retick_pool_` during param re-evaluation; `kill_instance` cascades teardown.
Tests: `tests/temporal/dyn_dt_inline`, `dyn_dt_seq_compat`.

The two features compose without interference (verified by `dyn_dt_seq_compat`).

## Resolved

- **seq triggering — settled on option (b): value output + emitted trigger.**
  `seq`'s output is the current element, so `s.note` / `s.duration` read fields
  directly, and `emit tick = !` provides a per-advance trigger used as
  `on(s::tick): { ... }` (or passed to an instrument as `s::tick`).  Chosen over:
  (a) trigger-as-output with `s::elem.note` access — works but reads awkwardly and
  has a t=0 emit artifact; (c) bare `!` firing once per advance — nicest at the
  call site but needs an engine change to `!` semantics in reaction context, so
  deferred.  Shipped in stdlib §6; test `tests/temporal/seq_trigger`.
- **seq wraps automatically.**  Flow indexing wraps by design, so the cursor just
  increments (`cursor = cursor + 1`) with no `fmod`/`len` needed.  One-shot would
  be an opt-in variant later.

## Open questions

- **phasor dynamic period — adopt at end of cycle.**  Decided: a dynamic-period
  phasor latches the pending period at the phase wrap (1→0) so each ramp uses one
  consistent slope ("change at the bar").  The native grid-anchored `phasor` is
  unchanged; this is a distinct mode (e.g. a stdlib `var_phasor`).  Not yet
  implemented.
- **sync dynamic period.**  Still grid-locked (period fixed at instantiation); a
  dynamic-period `sync` needs the relative-vs-grid re-snap policy above.  Proposal
  A (post-tick dt re-read) was NOT implemented — Proposal B covered what mattered.
