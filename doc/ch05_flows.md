# 5 — Flows

[← Back to index](idyl_documentation.md) · [Previous: Temporal functions](ch04_temporal.md)

---

Flows are **ordered sequences** — the data structure of Idƴl. They hold numbers, time values, triggers, strings, or even nested flows. Flows wrap around when indexed past their length.

---

## Simple flows

A flow literal is a bracketed list of values separated by commas:

```idyl
flow notes = [60, 62, 64, 67, 69]
```

---

## Generator expressions

Create flows programmatically with the `[var = range : expression]` syntax:

```idyl
// 0 through 9
numbers = [i = 0..9 : i]

// Doubled
doubled = [i = 0..9 : i * 2]

// Squares
squares = [i = 1..5 : i * i]

// Using functions
harmonics(root) = [h = 1..8 : root * h]

// Sine table
sine_table(size) = [i = 0..size - 1 : sin(i / size * 2 * pi)]
```

The range `start..end` is inclusive of `start`, exclusive of `end`.

---

## Multi-member flows

Flows can have multiple named members — like a struct of parallel sequences:

```idyl
flow drum_pattern = {
    kick:  [!, _, _, _]
    snare: [_, _, !, _]
    hat:   [!, !, !, !]
}
```

Each member is a named sequence. Members are accessed with dot notation: `drum_pattern.kick`, `drum_pattern.snare`.

---

## Gated members (`on`)

A multi-member flow can declare that one member **only advances when another member's current value is a live trigger**. This is written with the `on` keyword between the member name and the colon:

```idyl
flow pattern = {
    rhythm  : [!, _, _, !, !, _]
    melody on rhythm : [60, 63, 65]
}
```

When indexed with a trigger, `rhythm` advances on every tick as normal. `melody` only advances on the ticks where `rhythm` produces `!` — it stays frozen on `_` ticks.

```idyl
lib("stdlib")

flow pattern = {
    rhythm  : [!, _, _, !, !, _]
    melody on rhythm : [60, 63, 65]
}

process: {
    m = metro(dt=200ms)
    p = pattern[m]
    print(p.rhythm, p.melody)
    // rhythm advances each tick; melody only advances when rhythm is !
}
```

The gate member must appear **before** the gated member in the flow body — members are processed in order and the gate is read from the current tick's already-resolved values.

---

## Parameterized flows

Flows are functions. They can take parameters:

```idyl
sustain_lvl = 0.7

flow envelope_data(attack_time, decay_time, release_time) = {
    attack:  [i = 0..100 : i / 100]
    decay:   [i = 0..50 : sustain_lvl + (1 - sustain_lvl) * (1 - i / 50)]
    sustain: [sustain_lvl]
    release: [i = 0..100 : sustain_lvl * (1 - i / 100)]
}
```

Parameters used inside generators must be resolvable at evaluation time.

---

## Flows with temporal elements

Flow slots can hold **live temporal expressions** — their values update on every tick of the temporal instance:

```idyl
lib("stdlib")

// Each slot is a running oscillator — values change each time they are read
flow oscs = [sine(1hz, dt=100ms), sine(3hz, dt=100ms)]

process: {
    m = metro(dt=300ms)
    print(oscs[m])    // alternates between the two live sines
}
```

Compound expressions involving temporals also stay live:

```idyl
lib("stdlib")

// scaled[0] oscillates between 0 and 128; scaled[1] is always 0
flow scaled = [sine(2hz, dt=100ms) * 64 + 64, 0]

process: {
    m = metro(dt=300ms)
    print(scaled[m])
}
```

Multi-member flows support live slots in any member:

```idyl
lib("stdlib")

flow osc_bank = {
    slow: [sine(1hz, dt=100ms), sine(2hz, dt=100ms)]
    fast: [sine(5hz, dt=100ms), sine(7hz, dt=100ms)]
}

process: {
    m = metro(dt=300ms)
    row = osc_bank[m]
    print("slow:", row.slow, "fast:", row.fast)
}
```

---

## Dynamic parametric flows

Parametric flows can be called with **temporal arguments**. When a parameter changes value (because it is driven by a temporal source), the flow is automatically rebuilt with the new arguments on the next access:

```idyl
lib("stdlib")

flow mult = [1, 2, 4]
flow mixed(i) = [60 * i, sine(1hz, dt=100ms) * 12 + 60, 72]

process: {
    m0 = metro(dt=750ms)
    m  = metro(dt=250ms)
    mlt = mult[m0]          // advances through [1, 2, 4] every 750ms
    res = mixed(mlt)[m]     // mixed is rebuilt whenever mlt changes
    print(mlt, res)
}
```

The flow cursor is preserved across re-evaluations with the same arguments. When the argument changes, a fresh flow is built starting from index 0.

---

## Flow access

Flows are indexed with brackets:

```idyl
process: {
    scale = [i = 0..11 : 440 * (2.0 ^ (i / 12.0))]
    first_note = scale[0]
    fifth_note = scale[4]
    print(first_note, fifth_note)
}
```

### Wrapping

Flows wrap automatically when indexed past their length:

```idyl
flow seq = [10, 20, 30]
// seq[0] = 10, seq[1] = 20, seq[2] = 30
// seq[3] = 10  (wraps), seq[4] = 20, ...
```

### Access modes

| Index type | Behavior |
|------------|----------|
| Integer | Direct element at that index (wraps) |
| Float | Nearest element of index, proportionally, between 0 and 1 (wraps if exceeds 1) |
| Trigger | Advance to next element on trigger |

---

## Flow length

Use `len()` to get the number of elements:

```idyl
flow notes = [60, 62, 64, 67, 69]

process: {
    print("length:", len(notes))   // 5
}
```

---

## Combining flows

Flows can be transformed with generator expressions that reference other flows:

```idyl
flow_a = [1, 2, 3, 4]
flow_b = [10, 20, 30, 40]

// Element-wise combination
combined = [i = 0..len(flow_a) : flow_a[i] + flow_b[i]]

// Scale every element
scaled = [i = 0..len(flow_a) : flow_a[i] * 2]
```

---

## Flows in functions

Functions applied to flows operate element-wise:

```idyl
add(a, b) = a + b

process: {
    result = add([0, 3, 5], [5, 2, 0])
    print(result)    // flow: [5, 5, 5]
}
```

This broadcasting principle lets the same function work on scalars and flows without special syntax.

---

[Next: Control flow & operators →](ch06_control_flow.md)
