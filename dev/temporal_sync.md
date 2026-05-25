# Temporal Synchronisation — Design Notes

> Status: decided / ready to implement  
> Context: `sync()` gives us phase-locked triggers but leaves oscillators, euclidean rhythms,
> and all other temporal functions phase-blind.

---

## 1. The Problem

Every temporal function in Idyl accumulates its own phase from the moment it subscribes to
the scheduler:

```
phase += dt / period   (on every tick)
```

Two processes running the same `lfo(2hz)` but starting 300 ms apart stay permanently
300 ms out of phase. The same applies to `euclid`, `phasor`, `sine`, `counter`, etc.

`sync()` solves this for pure triggers by choosing the first scheduler fire time. It does
not help any function that computes output from accumulated internal state.

---

## 2. Why `dt` Is the Wrong Driver for Phase

`dt` currently plays two unrelated roles in temporal functions:

| Role | What it controls |
|------|-----------------|
| **Scheduling rate** | How often the scheduler calls the function (timing resolution) |
| **Phase increment** | How much phase advances per tick: `Δphase = dt / period` |

These are entangled. In `phasor(freq, dt=10ms)`:

```idyl
phase = fmod(phase + dt / freq, 1)
```

`dt` is both the tick interval and the phase step. Changing `dt` for accuracy reasons
changes the output frequency (they cancel exactly only if the scheduler is perfectly on
time, which it isn't under load).

More fundamentally: this formulation is *stateful relative to start time*. Two instances
starting at different times stay forever out of phase regardless of how accurate the
scheduling is.

---

## 3. The Right Foundation: Phase from Engine Time

### What is engine time?

When Idyl starts, the scheduler's internal clock begins at `t = 0`. "Engine time" (or
scheduler time) is the number of milliseconds elapsed since that moment. It is:

- **The same for every process** at any given instant — it does not depend on when a
  process was started, restarted, paused, or resumed.
- **Monotonically increasing** from 0, giving a clean, shared reference.
- **Different from wall-clock time** — wall-clock time is a large absolute value (system
  uptime in ms); engine time starts fresh each run.

A phasor computed from engine time is stateless:

```
phase = fmod(engine_time_ms / period_ms, 1)
```

Two processes evaluating this at the same moment will always produce the *same* phase
value, regardless of when they started. This is the universal sync reference.

`sync()` already relies on this insight for triggers: `first_fire = ceil(now / period) * period`
snaps to the next absolute boundary of engine time. The missing piece is extending the
same mechanism to continuous functions.

---

## 4. Design Decisions

### 4a. Single-argument form uses master clock

```idyl
sync(4b)          // syncs to master clock, period = 4 beats of master tempo
phasor(4b)        // phasor synced to master clock, 1 cycle per 4 master beats
```

When only a period is given, both primitives use the master clock as the phase reference.
This is the most common case (one global tempo) and avoids having to retrieve the master
clock handle explicitly.

### 4b. Two-argument form uses a specific clock, and interprets beat units in that clock's tempo

```idyl
clk = clock(60bpm)
sync(clk, 4b)     // period = 4 beats of clk = 4000ms, NOT 4 beats of master clock
phasor(clk, 4b)   // same: 4b is resolved using clk's BPM
```

The beat-unit (`b`) in the second argument is evaluated relative to the given clock's BPM,
not the master clock. This eliminates the redundancy of `sync(clk, clk(4b))`. The
implementation evaluates the clock argument first, then re-evaluates the period expression
with that clock's BPM as the beat context.

---

## 5. New Primitives

### 5a. `phasor` as an engine primitive

`phasor` is promoted from a stdlib function to a primitive. The current stdlib `phasor`
accumulates phase internally; the new primitive reads engine time directly:

```idyl
// 1-arg: master clock
ph = phasor(4b)          // 0→1 over 4 master beats, engine-time-locked

// 2-arg: specific clock
clk = clock(120bpm)
ph  = phasor(clk, 4b)   // 0→1 over 4 beats of clk
```

The phasor is a **temporal function** (subscribes to the scheduler, updates every `dt`)
rather than a continuously-readable value, so it works in reaction blocks where the
evaluator samples the bound value on each tick. Its `dt` parameter controls update
accuracy (default 10ms) without affecting the output frequency, because the output is
computed from engine time, not from accumulated `dt` steps.

Implementation:

```
beat_ms  = 60000 / clock_bpm(clk)   // or master bpm for 1-arg form
period_ms = period_beats * beat_ms
phase    = fmod(engine_time_ms / period_ms, 1)
```

No `init` block, no phase accumulation, no start-time dependency.

### 5b. `scheduler_time()` builtin (internal implementation detail)

The engine exposes `scheduler_time()` as a builtin returning ms since scheduler start.
This is what `phasor` uses internally and is also available for user-defined stateless
temporal functions:

```idyl
// User can build arbitrary stateless patterns from engine time:
my_ramp(period_ms, dt=10ms) = fmod(scheduler_time() / period_ms, 1)
```

This is primarily an implementation mechanism for the primitives; `phasor(clk, N)` is
the idiomatic user-facing API.

---

## 6. stdlib Changes

### 6a. Oscillators — optional `phase` input

All oscillators (`lfo`, `sine`, `cosine`, `tri`, `square`, `saw`, `pulse`) gain an
optional `phase_in` parameter. When supplied, it drives the oscillator directly;
internal accumulation is bypassed. `dt` reverts to being purely an accuracy/rate control.

```idyl
lfo(freq, amplitude=1, waveform=0, dt=10ms, phase_in=-1) = out |> {
    init: { phase = 0 }
    phase = (phase_in >= 0) ? phase_in; fmod(phase + dt / freq, 1)
    out   = sine_shape(phase, amplitude)    // (or other waveform)
}
```

Usage:

```idyl
clk = clock(120bpm)
ph  = phasor(clk, 4b)        // one cycle per 4 beats
osc = lfo(2hz, phase_in=ph)  // synced, dt=10ms is just accuracy
```

Same pattern for all other waveform functions.

### 6b. Euclidean rhythm — optional phasor and rotation

`euclid` gains two optional parameters:

- `phase_in` (default `-1`): external phasor input. When `-1`, the rhythm drives itself
  using a built-in phasor synced to the master clock at a default 4-beat cycle. When
  provided, the step position is derived directly from the given phasor value.
- `rotation` (default `0`): integer step offset that rotates the Bjorklund pattern.
  `rotation=1` shifts all onsets one step forward.

```idyl
euclid(hits, steps, dt=10ms, phase_in=-1, rotation=0) = trig |> {
    init: { prev_step = -1; trig = _; auto_phase = 0 }

    // Use external phasor if provided, else accumulate internally
    ph        = (phase_in >= 0) ? phase_in; fmod(auto_phase + dt / (steps * dt), 1)
    auto_phase = (phase_in >= 0) ? auto_phase; ph

    curr_step = int(ph * steps)
    rot_step  = int(fmod(curr_step + rotation, steps))
    onset     = fmod(rot_step * hits, steps) < hits
    trig      = trigger((curr_step != prev_step) * onset)
    prev_step = curr_step
}
```

Usage:

```idyl
clk = clock(120bpm)

// Explicitly synced to a 4-beat phasor:
ph = phasor(clk, 4b)
e1 = euclid(3, 8, phase_in=ph)

// With rotation:
e2 = euclid(3, 8, phase_in=ph, rotation=2)

// Free-running (legacy mode, unchanged behaviour):
e3 = euclid(3, 8, clk(0.5b))
```

When `phase_in` is not provided, euclid uses an internal default phasor at master tempo
4b, giving reasonable default sync without requiring the caller to wire up a phasor
explicitly.

---

## 7. BPM Changes During Runtime

When `tempo()` changes mid-run, the stateless phasor (`fmod(engine_time / period_ms, 1)`)
will produce a phase discontinuity because `period_ms` changes while `engine_time` keeps
running. **Option (a) is chosen**: record a `(time_of_change, phase_at_change)` anchor
pair each time BPM changes, then compute:

```
phase = fmod(phase_at_anchor + (engine_time - time_of_anchor) / new_period_ms, 1)
```

This carries the phase smoothly through tempo changes with no jump — the phasor
continues from wherever it was at the moment of change, just with a new rate. The
anchor is cheap to store (two doubles) and cheap to apply (one extra subtraction before
the fmod).

The `clock_registry` already tracks BPM per clock; it will also track the latest
`(time_of_change, phase_at_change)` anchor per clock node, updated whenever `set_bpm()`
is called.

---

## 8. `sync` and `phasor` Grid Alignment

`sync(clk, 4b)` fires a trigger at each 4b boundary of engine time.
`phasor(clk, 4b)` outputs `0.0` at exactly those same moments, `0.5` halfway between,
and `1.0` just before the next trigger.

They share the same grid by construction: both compute from `engine_time / period_ms`,
so `sync`'s fire boundary corresponds exactly to `phasor = 0`. This makes them naturally
complementary — `sync` is the discrete event form and `phasor` is the continuous form of
the same clock reference:

```idyl
clk = clock(120bpm)
t   = sync(clk, 4b)    // fires at 0ms, 2000ms, 4000ms ...
ph  = phasor(clk, 4b)  // 0.0   at 0ms, 2000ms, 4000ms ...
                        // 0.5   at 1000ms, 3000ms ...
                        // 0.999 just before each trigger
```

---

## 9. Summary: What Needs to Change

| Item | Change required | Effort |
|------|----------------|--------|
| `scheduler_time()` builtin | New C++ builtin (~5 lines) | XS |
| `phasor(clk, N)` primitive | C++ native temporal, reads engine time | S |
| `sync` / `phasor` beat-arg uses clock BPM | Ordered arg eval in `eval_call_expr` | S |
| `lfo`, `sine`, etc. with `phase_in=` | Optional parameter in stdlib | S |
| `euclid` with `phase_in=`, `rotation=` | Refactor in stdlib | S |
| BPM-change anchor in `clock_registry` | Add anchor fields, hook `set_bpm()` | M |
| Remove accumulated phasor from stdlib | Deprecate old `phasor` | L (later) |
