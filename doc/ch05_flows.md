# 5 — Flows

[← Back to index](idyl_documentation.md) · [Previous: Temporal functions](ch04_temporal.md)

---

Flows are **ordered sequences** — the data structure of Idƴl. They hold numbers, time values, triggers, strings, or even nested flows. Flows wrap around when indexed past their length.

---

## Simple flows

A flow literal is a bracketed list of values separated by spaces:

```idyl
flow notes = [60 62 64 67 69]
```

Values are not comma-separated — spaces are the delimiter.

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
    kick:  [! _ _ _]
    snare: [_ _ ! _]
    hat:   [! ! ! !]
}
```

Each member is a named sequence. Members are accessed with dot notation: `drum_pattern.kick`, `drum_pattern.snare`.

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
flow seq = [10 20 30]
// seq[0] = 10, seq[1] = 20, seq[2] = 30
// seq[3] = 10  (wraps), seq[4] = 20, ...
```

### Access modes

| Index type | Behavior |
|------------|----------|
| Integer | Direct element at that index (wraps) |
| Float | Element at `floor(index)` (wraps) |
| Trigger | Advance to next element on trigger |

---

## Flow length

Use `len()` to get the number of elements:

```idyl
flow notes = [60 62 64 67 69]

process: {
    print("length:", len(notes))   // 5
}
```

---

## Combining flows

Flows can be transformed with generator expressions that reference other flows:

```idyl
flow_a = [1 2 3 4]
flow_b = [10 20 30 40]

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
    result = add([0 3 5], [5 2 0])
    print(result)    // flow: [5 5 5]
}
```

This broadcasting principle lets the same function work on scalars and flows without special syntax.

---

[Next: Control flow & operators →](ch06_control_flow.md)
