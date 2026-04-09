# 4 — Time & temporal functions

[← Back to index](idyl_documentation.md) · [Previous: Functions](ch03_functions.md)

---

Temporal functions are the heart of Idƴl. They carry state across time, driven by a clock or by trigger events. Where a pure function maps input to output once, a temporal function *evolves*.

---

## The `|>` operator

The pipe-arrow `|>` introduces a **lambda block** — a stateful body that executes repeatedly on each tick. The expression before `|>` is the **output variable** — the value the function yields on each tick.

```idyl
name(params, dt=interval) = output_var |> {
    init: { ... }
    // update statements
}
```

A temporal function must have at least one time source:
- A `dt=` parameter (clock-driven), or
- A trigger parameter with `!` suffix (event-driven), or
- Both (hybrid)

---

## Clock-driven temporal functions

The `dt` parameter sets the tick interval. The function updates at that rate.

```idyl
// Counts up by 1 every 10ms
clock_counter(dt=10ms) = n |> {
    init: { n = 0 }
    n = n + 1
}
```

### LFO example

```idyl
lfo(freq, amplitude, dt=10ms) = modulation |> {
    init: { phase = 0  modulation = 0 }
    modulation = amplitude * sin(phase * 2 * pi)
    phase = fmod(phase + 1 / (freq * (dt / 1000)), 1)
}
```

- `init:` runs once at instantiation. It sets initial state.
- Update statements run on every tick, in order.
- The output variable (`modulation`) is what the caller sees.

### Smooth control

```idyl
smooth(input, factor, dt=10ms) = smoothed |> {
    init: { state = 0 }
    smoothed = state
    state = state + (input - state) * factor
}
```

---

## Trigger-driven temporal functions

Instead of a clock, a trigger parameter (suffixed with `!`) fires the function when the trigger event occurs.

```idyl
// Increments on each trigger pulse
trigger_counter(spike!) = count |> {
    init: { count = 0 }
    count = count + 1
}

// Captures input when trigger fires
sample_hold(input, capture!) = held |> {
    init: { held = 0 }
    held = input
}
```

The function body executes once per trigger event, not on a periodic clock.

---

## Hybrid: trigger + clock

A function can accept both `dt` and trigger parameters. It updates on both:

```idyl
// Trigger sets to 1, clock decays over time
decay_on_trigger(trig!, decay_rate, dt=50ms) = level |> {
    init: { level = 0 }
    level = (0; 1 ? trig) + level * (1 - decay_rate)
}
```

Use the ternary operator to distinguish which event fired.

---

## The `init` block

The `init:` block runs **once** when the function is instantiated. It sets initial state.

```idyl
counter(dt=100ms) = n |> {
    init: { n = 0 }
    n = n + 1
}
```

Rules:
- Variables defined in `init` persist across ticks (they are the function's state).
- `init` is optional. Without it, the first update runs immediately (no dt delay).
- With `init`, the function's first output is the init value; the first update runs after one `dt`.

---

## State variables

Variables inside the lambda block are local state. They persist across ticks but are not visible outside the function — unless emitted (see [Chapter 7](ch07_emit_catch.md)).

```idyl
sawtooth(freq, dt=10ms) = phase |> {
    init: { phase = 0 }
    phase = fmod(phase + 1 / (freq * (dt / 1000ms)), 1)
}
```

`phase` is updated every tick. The caller sees only the output (`phase` in this case, since it is the output variable).

---

## Instantiation

Temporal functions are **instantiated** when called. Each call creates an independent instance with its own state:

```idyl
process: {
    slow = lfo(0.2hz, 1.0, dt=10ms)   // instance 1
    fast = lfo(1.5hz, 1.0, dt=10ms)   // instance 2
    combined = slow + fast              // two independent LFOs
}
```

The two LFOs have separate `phase` state — they don't interfere.

---

## Reactive chains

Inside a process block, statements that follow a temporal binding are **reactions** — they re-execute on every tick of the temporal source:

```idyl
process: {
    osc = lfo(5hz, 1.0, dt=10ms)
    modulated = 440hz * (1 + osc * 0.1)   // re-evaluated every 10ms
    print("freq:", modulated)               // prints every 10ms
}
```

The `print` call is not a one-shot — it runs every time `osc` ticks.

---

## Delay operator — `'`

The prime operator `'` introduces a **sample delay**: it returns the value of an expression from a previous tick rather than the current one. This is useful for feedback, differencing, and basic memory.

```idyl
process: {
    a = counter(dt=300ms)
    b = '(a)        // one-sample delay: value of a from the previous tick
    c = '(a, 3)     // three-sample delay: value of a from three ticks ago
    print(a, b, c)
}
```

### Syntax

| Form | Meaning |
|------|---------|
| `'(expr)` | One-sample delay — returns the previous tick's value |
| `'(expr, N)` | N-sample delay — returns the value from N ticks ago |

### Semantics

- On the first tick(s), delayed slots are filled with `0` until the buffer is populated.
- The delay is measured in **ticks**, not in time. A two-sample delay on a `dt=100ms` function returns the value from 200ms ago.
- Each delay expression has its own independent circular buffer, keyed to that specific expression in the AST. Multiple delays in the same function do not interfere.

### Example: differentiate a signal

```idyl
velocity(x, dt=50ms) = dx |> {
    dx = x - '(x)    // rate of change per tick
}
```

### Inside temporal functions

The delay operator is legal anywhere an expression is valid — inside lambda blocks, reactions, or the process body itself. When used inside a lambda block, the buffer is tied to that specific expression position.

```idyl
envelope(attack, dt=10ms) = level |> {
    init: { level = 0  target = 1 }
    level = level + (target - level) * attack
    // detect when movement slows (approaching target)
    delta = abs(level - '(level))
}
```

---

## Summary

| Aspect | Clock-driven | Trigger-driven | Hybrid |
|--------|-------------|----------------|--------|
| Time source | `dt=interval` | `param!` | Both |
| Update rate | Periodic | On event | Both |
| Use case | LFOs, counters, smoothers | Counters, sample-hold | Envelopes, duckers |

---

[Next: Flows →](ch05_flows.md)
