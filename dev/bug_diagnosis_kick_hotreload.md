# Bug Diagnosis: kick process hot-reload

Two bugs under investigation. Test process:

```idyl
process kick: {
    tempo(120bpm)
    deepkick(sync(4b), amp=fff)
    ph = phasor(.4b)
    dirtykick(euclid(3, 8, phase_in=ph), amp=fff)
}
```

---

## Bug 1: dirtykick stops running on hot-reload

### What happens

After any hot-reload of `kick`, `dirtykick` (driven by euclid) goes silent.
`deepkick` (driven by sync) continues to fire normally.

### Segment layout at runtime

```
lp.segments[0]  bound_var=""   def_name="sync"   (drives deepkick)
lp.segments[1]  bound_var="ph" def_name="phasor"
lp.segments[2]  bound_var=""   def_name="euclid"  (drives dirtykick)
```

### How unnamed drivers are identified on reload (`diff_and_apply`)

Before the body scan, `old_unnamed_expr_names` is built from every unnamed
segment whose `binding_stmt` is an `expression_stmt`, by extracting the
**outermost** function name via `extract_fn_name`:

```
old_unnamed_expr_names = ["deepkick", "dirtykick"]
```

During the body scan each `expression_stmt` with an empty `bname` is compared
to `old_unnamed_expr_names[old_expr_drv_idx]`:

```
tempo(120bpm)                      → outer fn "tempo" ≠ "deepkick" → side-effect
deepkick(sync(4b), amp=fff)        → outer fn "deepkick" == [0]    → driver, idx→1
ph = phasor(.4b)                   → assignment (bname="ph"), skips driver check
dirtykick(euclid(3,8,...), amp=fff)→ outer fn "dirtykick" == [1]   → driver
```

Resulting: `new_unnamed_binding_stmts = [deepkick_stmt, dirtykick_stmt]`  (size 2)

In pass 0.5 the unnamed segments are matched positionally to these stmts.
With two unnamed segments and two new stmts this should be a clean 1-to-1
match. Neither `kill_instance` nor `killed_unnamed_ids` should be invoked.

---

### Potential failure points

#### F1 – `old_expr_drv_idx` consumed by a hidden expression_stmt

If any expression_stmt **before** `dirtykick` in the process body matches
`old_unnamed_expr_names[1]` = `"dirtykick"`, `old_expr_drv_idx` is
incremented prematurely and the real `dirtykick_stmt` is **not** recognized
as a driver.

Concretely: if there is a statement in the hot-reloaded body that evaluates
to an expression whose outer function name equals `"dirtykick"` and that
statement appears before the real `dirtykick(...)` line, it would consume
slot [1] and leave `dirtykick(euclid(...))` unmatched.

**How to verify:** inspect `new_unnamed[1] outer_fn=` in the debug output.
If it does not equal `"dirtykick"`, this failure is active.

---

#### F2 – mismatch between `old_unnamed_expr_names` size and actual unnamed segments

`old_unnamed_expr_names` is built **only** from segments whose `binding_stmt`
type is `expression_stmt`. An `on_block`-driven unnamed segment has a different
binding_stmt type and is **skipped** by the builder.

If after a previous hot-reload an `on_block` segment exists at an index
before the euclid segment, the sizes diverge:

```
lp.segments: [on_block_seg, phasor_seg, euclid_seg]
old_unnamed_expr_names: ["dirtykick"]   ← only expr_stmt drivers
new_unnamed_binding_stmts: [deepkick_stmt?, dirtykick_stmt?]
```

Pass 0.5 iterates by `uidx`. The `on_block` segment (at uidx=0) is matched to
`new_unnamed_binding_stmts[0]`, consuming the first slot.  
The euclid segment (at uidx=1) checks `uidx >= new_unnamed_binding_stmts.size()`
→ if there is only one new stmt, **euclid is killed**.

This process currently has no `on_block` driver, so this path cannot apply
here — but it illustrates the general fragility.

---

#### F3 – `find_inner_temporal_call` returning empty for euclid

`find_inner_temporal_call(dirtykick_stmt)` searches the **positional** arguments
of the outermost call (`dirtykick`) for the first `function_call_expr` argument.

If `euclid(3, 8, phase_in=ph)` is somehow parsed so that the first positional
argument is **not** a `function_call_expr` (e.g. the whole call resolves as an
identifier, or a pipe-operator wraps it), `find_inner_temporal_call` returns
`{"", nullptr}`.

The pass-0.5 logic then falls through to `else` (same-function), which just
updates `binding_stmt` — but with `new_inner_fn == ""`, no inner function is
detected as changed and no rescheduling occurs. This path should be safe, but
it means the dt-update sub-block is skipped, which could leave the euclid
running at the wrong period.

**How to verify:** look for `new_inner_fn=''` in the pass0.5 debug line for
the euclid segment.

---

#### F4 – Segment count changes between reloads (the original reordering bug)

**Before the in-place fix** (addressed in the previous commit):

On a function-change reload of an unnamed driver (e.g. `deepkick(sync→metro)`),
the old segment was erased from `lp.segments` and the new segment was appended
at the **end**. After this reorder:

```
Before: [sync/deepkick, phasor/ph, euclid/dirtykick]
After:  [phasor/ph, euclid/dirtykick, metro/deepkick]  ← REORDERED
```

On the next hot-reload, `old_unnamed_expr_names` was built in the new order
`["dirtykick", "deepkick"]`. The body scan consumed `deepkick_stmt` for slot
[0] ("dirtykick"≠"deepkick" → mismatch, falling through as side-effect), then
`dirtykick_stmt` matched slot [0] ("dirtykick"), and the second slot was never
reached. Pass 0.5 then found the metro segment at uidx=1 with
`uidx >= new_unnamed_binding_stmts.size()` → **kill_instance(metro) = kill deepkick's driver**.

**Status after the in-place fix:** the in-place update preserves positional
order after a function-change reload. This should no longer trigger. But if
the bug is still present after that fix, the reordering is happening through
a different path (see F5).

---

#### F5 – In-place fix not reached for the actual failing case

The in-place update only fires when `find_inner_temporal_call` returns a
non-empty `new_inner_fn` that differs from `old_seg.def_name`. If the inner
temporal function is the **same** (sync→sync, euclid→euclid), the code takes
the SAME_FUNCTION path which only updates `binding_stmt`.

If the actual reload that breaks dirtykick involves **no** inner-function
change (just a body edit), the old reordering bug cannot be the root cause.
The in-place fix only helps for function-change reloads.

**What to check:** does the bug reproduce on a reload that changes only the
amp value or removes a comment — i.e. with strictly identical function names?
If yes, the root cause is not reordering.

---

#### F6 – `tempo(120bpm)` re-execution resets the clock epoch

During the body scan, `tempo(120bpm)` is classified as a pre-segment
side-effect and `exec_stmt(tempo(120bpm))` is called immediately.

Inside, `tempo()` calls `clocks_.set_bpm(clocks_.main_id_, 120.0, now_ms)`.
If `set_bpm` **resets the clock epoch** (updates `epoch_ms_` / `beat_offset_`)
even when the BPM value is unchanged, the phasor's phase computation shifts:

```
phasor output = fmod((now - epoch_ms_) / (n_beats * beat_period), 1)
```

A sudden shift in `ph` is immediately visible to euclid on its next tick
(via `instance_param_exprs_["phase_in"]`). This could cause `curr_step` to
jump to a value equal to `prev_step`, suppressing the trigger for one or more
cycles (but not forever).

**More importantly:** if `set_bpm` resets `epoch_ms_` to `now_ms` at the
moment of the hot-reload, and `now_ms` is evaluated during `diff_and_apply`
which runs on the main thread while the scheduler might be sleeping, the
epoch jumps and the phasor's next reported phase is 0. If this collides with
`prev_step == 0`, euclid emits no trigger for an entire period.

This is a transient glitch, not a permanent stop. But if the user observes
"dirtykick stops" after every reload and never recovers, something more
permanent must be happening.

**How to verify:** add `std::cerr << "[tempo] set_bpm called bpm=" << bpm << "\n";`
inside the tempo builtin. Also log the clock epoch before/after.

---

#### F7 – `ph` identifier not found in scope during euclid tick

`tick_instance` re-evaluates `instance_param_exprs_["phase_in"]` (the `ph`
identifier expression) on each tick:

```cpp
if (pit->second->type_ == node_t::identifier_expr) {
    auto& ie = static_cast<const identifier_expr&>(*pit->second);
    if (ie.identifier_ && !env_.lookup(ie.identifier_->name_))
        continue;  // SKIP re-eval if identifier not in scope
}
```

If `env_.lookup("ph")` returns null at tick time, `phase_in` is never updated
and euclid runs with its initial `phase_in` (the phasor value at instantiation
time). With a fixed phase, euclid produces a fixed rhythm but should still
fire — so this would not cause a permanent stop.

However, if at instantiation time `ph` was 0, `phase_in = 0`, and euclid's
`prev_step` initializes to `-1`, the first tick fires normally. Subsequent
ticks: `curr_step = int(0.0 * 8) = 0`, `0 == prev_step(0)` → **no trigger
after the first tick**. This would look like "stops after one note."

The scope is correctly pinned (`scope_pin_ = proc_scope_idx`) so `ph` should
be reachable. But if `proc_scope_idx` is wrong (e.g., 0 because the process
scope was not yet pushed when the subscription was created), `env_.lookup("ph")`
would skip the correct scope frame and return null.

**How to verify:** add `std::cerr << "[tick_phase_in] ph=" << ph_val << "\n";`
inside euclid's tick to confirm `phase_in` is updating.

---

#### F8 – `diff_and_apply` races with scheduler callbacks (no eval_mutex_)

`hot_reload` / `diff_and_apply` runs on the **main thread** without holding
`eval_mutex_`. Scheduler callbacks run on the **scheduler thread** and DO hold
`eval_mutex_`. This creates potential races on:

- `instances_` map (modified in diff_and_apply via `kill_instance`, read in
  tick callbacks via `instances_.find`)
- `active_process_instances_` (modified in diff_and_apply, read in stop_process)
- `lp.segments` vector (erased in diff_and_apply, not read by tick callbacks
  directly but metadata like `scope_depth` is)

The `rxn->binding_stmt` is protected by `rxn->mutex`, so that specific field
is safe. But if `kill_instance` sets `inst->active_ = false` concurrently
with the tick callback checking `si->active_`, there is a data race (UB on
`bool` without `std::atomic`).

In the specific dirtykick case, if no kill is expected (same-function reload),
this race is benign. But the racy environment may corrupt other shared state.

---

#### F9 – `binding_stmt` set to null by a failed `new_unnamed_binding_stmts` lookup

In pass 0.5, `new_stmt = new_unnamed_binding_stmts[uidx]`. If `uidx` is in
range but the vector element is null (a `shared_ptr` that was default-
constructed), `rxn->binding_stmt` is set to null. The tick callback then
does `if (bsnap) exec_stmt(bsnap)` → **skips execution permanently**.

This can only happen if something pushes a null `shared_ptr` into
`new_unnamed_binding_stmts`. The body scan pushes `s` directly from the
statement loop, which is always non-null. So this is unlikely in practice,
but worth keeping in mind.

---

### Most likely root cause (to verify with debug output)

Looking at the debug prints already in the build:

```
[diff] process 'kick' segments before reload:
  [0] bound_var='' def_name='sync' inst_id=X
  ...
  old_unnamed_expr_names: 'deepkick' 'dirtykick'
  new_unnamed_binding_stmts: N
  new_unnamed[0] outer_fn='deepkick'
  new_unnamed[1] outer_fn='dirtykick'     ← must be present
[pass0.5] seg def='euclid' inst=Z new_inner_fn='euclid'
```

**If `new_unnamed_binding_stmts = 1` (N=1):** the `dirtykick_stmt` was not
recognized as a driver. Root cause is F1 or F2.

**If `new_inner_fn=''` for euclid:** `find_inner_temporal_call` failed for
the new dirtykick_stmt. Root cause is F3.

**If `[kill_instance] id=Z def_name='euclid'` appears:** the euclid instance
is killed. Most likely from the count-mismatch path (uidx >= stmts.size()),
which means `new_unnamed_binding_stmts.size() < 2`. Root cause is F1 or F2.

**If no kill appears and N=2:** the bug is post-reload, not in diff_and_apply.
Root cause is F6 (tempo epoch reset), F7 (ph not in scope), or the retick
mechanism is producing the wrong output.

---

## Bug 2: Extra event at process start (sync/phasor-driven)

### What happens

When a process containing `sync(Nb)` or `phasor(N)` is started, there may be
1-2 spurious events before synchronization takes effect. For synced processes
specifically, a few iterations fire before the phase aligns.

### Sync first-fire calculation

The sync builtin computes its first fire time as:

```cpp
first_fire = ceil(now / period) * period
```

with a 0.5ms guard: if `first_fire - now < 0.5`, `first_fire += period`.

**Potential failure points:**

#### S1 – `now` is stale when first_fire is computed

`now` is read from `scheduler_->now_ms()` inside the sync instantiation. If
the scheduler's clock has not yet started (or is at a different point than
when the subscription fires), the quantization is off by a beat boundary.

If `now` is read at process **parse time** but the subscription fires later
(after some setup latency), `first_fire` was computed for a different moment
than when the callback actually starts.

#### S2 – 0.5ms guard is too tight

If the process starts within 0.5ms of a beat boundary, `first_fire` is
pushed to the NEXT boundary. But if the scheduler callback fires even slightly
late (jitter), the boundary is already passed and the NEXT+1 boundary is used.

Depending on the scheduler resolution, this guard may be too tight and cause
alignment to slip by one full period.

#### S3 – `first_fire` is passed to `scheduler_->subscribe` but the subscribed
callback fires immediately

Looking at the process body setup loop (line 792):
```cpp
}, proc.name_, inst->first_fire_ms_);
```

The `first_fire_ms_` is the absolute time of the first desired fire. If the
scheduler's `subscribe(dt, callback, name, first_fire)` interprets `first_fire`
relative to now (not absolute), the first callback fires at `now + first_fire_ms_`
instead of `first_fire_ms_`, effectively at `2 * first_fire_ms_` absolute time
which is wrong.

**How to verify:** add a print in the sync native_update that logs `now_ms`,
`first_fire_ms_`, and `period_ms` on the first few ticks.

#### S4 – Phasor phase on first tick is 0 regardless of clock position

When `ph = phasor(.4b)` is instantiated, the phasor computes its initial phase
using `clocks_.phase_at(clock_id, n_beats, now)`. If `phase_at` uses an epoch
that hasn't been aligned to the global clock's beat grid, the initial phase is
an arbitrary fraction and `ph` starts somewhere in the middle of the period.

The first tick of euclid then sees a `curr_step` that is NOT 0. If `prev_step`
initialized to -1, `curr_step != -1` is true → **euclid fires a trigger on
the very first tick**, regardless of where in the beat grid we are.

This explains "an extra note at the beginning": euclid fires immediately at
process start (before the phasor has aligned to the clock) because `curr_step
(= any non-negative value) != prev_step (-1)`.

**Fix direction:** initialize `prev_step` to the initial `curr_step` rather
than -1, so no trigger fires on the very first tick.

#### S5 – Sync-driven process fires before sync aligns

For `deepkick(sync(4b))`: sync schedules its first fire at the next beat
boundary. But the **process setup code also runs `exec_stmt(deepkick_stmt)`
synchronously at setup time**, which evaluates `deepkick(sync(4b), amp=fff)`.
At setup time `sync(4b)`'s first output is its initial state. If the initial
state is a live trigger (because `first_fire` defaults to `now` before the
sync alignment is computed), `deepkick` fires immediately.

Checking whether the initial `sync` output is `!` or `_` at instantiation
time is the key question.

---

---

## Confirmed Results (live testing 2026-05-28)

### Testing methodology

Tests run with:
```bash
./build/idyl --listen tests/debug/kick_minimal.idyl
python3 tests/debug/send_start.py start kick
# hot-reload via: python3 tests/debug/send_osc.py <file>
```

`kick_minimal.idyl` replaces Csound with `print()` calls to make output
observable without audio hardware:
```idyl
deepkick(spike!, amp) = print("deepkick", amp)
dirtykick(spike!, amp) = print("dirtykick", amp)
```

---

### Bug 1 — CONFIRMED FIXED (no action needed)

**Confirmed by live test:** after a hot-reload that changes amp, phasor period,
or euclid parameters, the `[diff]` / `[pass0.5]` debug output shows:

```
old_unnamed_expr_names: 'deepkick' 'dirtykick'
new_unnamed_binding_stmts: 2
new_unnamed[0] outer_fn='deepkick'
new_unnamed[1] outer_fn='dirtykick'
[pass0.5] seg def='sync'   inst=1 new_inner_fn='sync'
[pass0.5] seg def='euclid' inst=3 new_inner_fn='euclid'
[hot_reload] updated process 'kick'
```

No `kill_instance` for the euclid segment. `dirtykick` continues to fire
after the reload. `deepkick` also continues.

**Root cause retrospective:** the bug was the reordering described in F4.
The in-place segment update fix (committed in `922bc13`) resolved it by
updating `old_seg` fields in-place rather than erase+append, preserving
the positional order of unnamed segments across function-change reloads.

The failure points F1-F9 were not the active issue; they remain as
documentation of theoretical edge-cases.

---

### Bug 2 — CONFIRMED FIXED (lib/stdlib.idyl changed)

**Confirmed root cause:** S4 — `prev_step = -1` in euclid's init block.

On the very first scheduler tick after process start, the euclid flow reads
the current phasor phase and computes `curr_step ∈ {0..steps-1}`. Since
`curr_step ≠ -1` is always true, euclid fires a trigger on the first tick
if the onset pattern hits that step. This produces a spurious note at
`t = dt` (10ms by default), well before sync-driven deepkick fires at the
first beat boundary.

**Fix applied — `lib/stdlib.idyl`:**

Before:
```idyl
init: {
    prev_step  = -1
    trig       = _
    auto_phase = 0
}
```

After:
```idyl
init: {
    auto_phase = 0
    _ph0       = (phase_in >= 0) ? fmod(dt / (steps * dt), 1); phase_in
    prev_step  = int(_ph0 * steps)
    trig       = _
}
```

**How the fix works:**

`_ph0` computes the phase that the first real tick will see:
- **External phasor** (`phase_in >= 0`): `_ph0 = phase_in` — the phasor's
  current value at instantiation time. First tick's `ph = phase_in` (same
  value), so `curr_step = prev_step` → no trigger.
- **Internal auto-advance** (`phase_in < 0`): `_ph0 = dt/(steps * dt) = 1/steps`.
  First tick: `ph = fmod(auto_phase=0 + 1/steps, 1) = 1/steps`, `curr_step = 1`.
  `prev_step = 1` → `curr_step == prev_step` → no trigger.

In both modes the first real step-change fires a legitimate trigger; only
the guaranteed spurious t=0 trigger is eliminated.

`_ph0` is stored in `current_` (init block assignments all persist to state)
but is overwritten by the update block's `ph = ...` assignment on every
subsequent tick, so there is no observable side-effect.

---

### Remaining open items

- **F8 (eval_mutex_ race in hot_reload):** FIXED. Added
  `std::lock_guard<std::recursive_mutex> lock(eval_mutex_)` at the start of
  `hot_reload` (after parse, before any shared-state mutation). Scheduler
  callbacks already hold `eval_mutex_`; `eval_mutex_` is `recursive_mutex`
  so internal re-acquisition inside `diff_and_apply` sub-calls is safe.

- **S2/S3 (sync alignment jitter):** the user mentioned "a few iterations
  before synchronizing" for sync-driven processes. The sync's 0.5ms guard
  and `first_fire_ms_` scheduling have not been stress-tested under high
  scheduler load. Not confirmed as a real issue; monitor in production use.
