# 5 — Flows

[← Back to index](idyl_documentation.md) · [Previous: Temporal functions](ch04_temporal.md)

---

Flows are **ordered sequences** — the data structure of Idƴl. They hold numbers, time values, triggers, strings, or even nested flows. Flows wrap around when indexed past their length.

**Flows are literals.** Both flow forms are expressions — they can appear on the right-hand side of any assignment, as function return values, inside ternary branches, and inside temporal function bodies.

---

## Simple flows

A bracketed list `[...]` is a flow literal. It needs no keyword:

```idyl
notes = [60, 62, 64, 67, 69]
```

A name at global scope can use `flow` as an optional prefix for readability — both lines below are identical:

```idyl
flow notes = [60, 62, 64, 67, 69]   // same as above
notes      = [60, 62, 64, 67, 69]
```

---

## Generator expressions

Create flows programmatically with the `[var in range : expression]` syntax. Three forms are supported:

```idyl
// Count form — var runs 0 … N-1
fives = [i in 5 : i * 2]            // [0, 2, 4, 6, 8]

// Range form — ascending (exclusive end)
squares = [i in 1..5 : i * i]       // [1, 4, 9, 16, 25]

// Range form — descending (direction inferred automatically)
countdown = [i in 5..1 : i]         // [5, 4, 3, 2, 1]

// Range + step (float step allowed)
thirds = [i in 0..1..0.25 : i]      // [0, 0.25, 0.5, 0.75, 1.0]

// Using functions
harmonics(root) = [h in 1..8 : root * h]

// Sine table
sine_table(size) = [i in 0..size - 1 : sin(i / size * 2 * pi)]
```

For the two-value range form, `start..end` is inclusive of both ends. Direction is inferred: ascending when `start ≤ end`, descending otherwise. The step form `start..end..step` walks by `step` (which may be a float).

---

## Named-member flows

A **named-member flow** is written as `flow { name: [...] }`. The `flow` keyword is required here to distinguish the brace block from a future code block:

```idyl
drum_pattern = flow {
    kick:  [!, _, _, _]
    snare: [_, _, !, _]
    hat:   [!, !, !, !]
}
```

At global scope you can write `flow name = { }` as shorthand — the two forms are exactly equivalent:

```idyl
// Both declare the same named-member flow:
flow drum_pattern = {
    kick:  [!, _, _, _]
    snare: [_, _, !, _]
    hat:   [!, !, !, !]
}

drum_pattern = flow {
    kick:  [!, _, _, _]
    snare: [_, _, !, _]
    hat:   [!, !, !, !]
}
```

Each member is a named sequence. Members are accessed with dot notation: `drum_pattern.kick`, `drum_pattern.snare`.

Because `flow { }` is an expression, named-member flows can be returned from functions:

```idyl
// A function returning a named-member flow
rhythm_pair(a, b) = flow {
    fast: [a, _, a, _]
    slow: [b, _, _, _]
}

process: {
    rp = rhythm_pair(!, !)
    m  = metro(dt=200ms)
    print(rp.fast[m], rp.slow[m])
}
```

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
import("stdlib")

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

Since flows are expressions, any function can return one. Simple lists need no special syntax; named-member flows use `flow { }`:

```idyl
// Function returning a simple flow — no keyword needed
scale(root) = [root, root * 1.125, root * 1.25, root * 1.5]

// Function returning a named-member flow
sustain_lvl = 0.7

envelope_data(attack_time, decay_time, release_time) = flow {
    attack:  [i in 0..100 : i / 100]
    decay:   [i in 0..50 : sustain_lvl + (1 - sustain_lvl) * (1 - i / 50)]
    sustain: [sustain_lvl]
    release: [i in 0..100 : sustain_lvl * (1 - i / 100)]
}
```

Parameters used inside generators must be resolvable at evaluation time.

---

## Flows with temporal elements

Flow slots can hold **live temporal expressions** — their values update on every tick of the temporal instance:

```idyl
import("stdlib")

// Each slot is a running oscillator — values change each time they are read
flow oscs = [sine(1hz, dt=100ms), sine(3hz, dt=100ms)]

process: {
    m = metro(dt=300ms)
    print(oscs[m])    // alternates between the two live sines
}
```

Compound expressions involving temporals also stay live:

```idyl
import("stdlib")

// scaled[0] oscillates between 0 and 128; scaled[1] is always 0
flow scaled = [sine(2hz, dt=100ms) * 64 + 64, 0]

process: {
    m = metro(dt=300ms)
    print(scaled[m])
}
```

Multi-member flows support live slots in any member:

```idyl
import("stdlib")

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

## Temporal functions returning flows

Because `flow { }` is a literal expression, temporal functions can return named-member flows. This is the idiomatic way to expose multiple synchronized outputs from a single temporal instance:

```idyl
import("stdlib")

// Temporal function whose output is a named-member flow
dual_osc(freq1, freq2, dt=100ms) = flow {
    a: [sine(freq1, dt=dt)]
    b: [sine(freq2, dt=dt)]
} |> { }

process: {
    pair = dual_osc(1hz, 3hz)
    m    = metro(dt=300ms)
    print("a:", pair.a[m], "b:", pair.b[m])
}
```

The `|> { }` is the minimal lambda block — the body runs on every tick. The `flow { }` literal in the return position is re-evaluated each tick, giving live values per member.

---

## Dynamic parametric flows

Parametric flows can be called with **temporal arguments**. When a parameter changes value (because it is driven by a temporal source), the flow is automatically rebuilt with the new arguments on the next access:

```idyl
import("stdlib")

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
    scale = [i in 0..11 : 440 * pow(2.0, i / 12.0)]
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

All three modes address the **physical table** — the expanded representation including any repeat-bar copies.

| Index type | Behavior |
|------------|----------|
| Integer | Direct physical slot at that index (wraps at physical length) |
| Float | Selects proportionally across physical length, between 0 and 1 (wraps if exceeds 1) |
| Trigger | Advance the cursor one physical slot per tick |

---

## Repeat bars

The `|N|` bar attached to an element — or a bracketed group — repeats that element N times in the **physical table**: the expanded representation that all indexing modes operate against.

**Single-element repeat** — `|N|` follows a single value and repeats it:

```idyl
flow seq = {
    degree: [1, 2, 3 |3|, 13, 8]
    //  physical: [1, 2, 3, 3, 3, 13, 8]   len = 7
}
```

**Group repeat** — `|N|` follows a bracketed sub-list and repeats the whole group:

```idyl
flow seq2 = {
    degree: [[1, 2, 3] |3|, 13, 8]
    //  physical: [1, 2, 3, 1, 2, 3, 1, 2, 3, 13, 8]   len = 11
}
```

All three index modes address the same physical table:

| Index type | Physical table behaviour |
|------------|--------------------------|
| Integer n | Accesses physical slot n directly (wraps at physical length) |
| Float [0, 1) | Selects proportionally across physical length |
| Trigger | Advances the cursor one physical slot per tick |

Use `each n in len(flow)` to iterate every physical slot, including repeated ones:

```idyl
process: {
    each n in len(seq): {
        print(seq[n].degree)   // 1 2 3 3 3 13 8
    }
    each n in len(seq2): {
        print(seq2[n].degree)  // 1 2 3 1 2 3 1 2 3 13 8
    }
}
```

---

## Flow length

Use `len()` to get the number of elements. For flows with repeat bars, `len()` returns the **physical length** — total slots including all repeated copies:

```idyl
flow notes = [60, 62, 64, 67, 69]
flow seq   = { degree: [1, 2, 3 |3|, 13, 8] }

process: {
    print("length:", len(notes))   // 5
    print("length:", len(seq))     // 7  (physical: 1,2,3,3,3,13,8)
}
```

---

## Combining flows

Flows can be transformed with generator expressions that reference other flows:

```idyl
flow_a = [1, 2, 3, 4]
flow_b = [10, 20, 30, 40]

// Element-wise combination
combined = [i in 0..len(flow_a) : flow_a[i] + flow_b[i]]

// Scale every element
scaled = [i in 0..len(flow_a) : flow_a[i] * 2]
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
