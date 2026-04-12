# 3 — Functions

[← Back to index](idyl_documentation.md) · [Previous: Fundamentals](ch02_fundamentals.md)

---

## Definition syntax

A function is defined with `name(params) = body`. The body is an expression — there is no `return` keyword.

```idyl
square(x) = x * x
add(a, b) = a + b
```

Zero-parameter definitions are constants (evaluated eagerly):

```idyl
silence = 0
concert_pitch = 440hz
```

Definitions are **immutable**. Once bound, a name cannot be reassigned at the same scope level.

---

## Parameters

Functions accept positional parameters:

```idyl
blend(sig_a, sig_b, factor) =
    sig_a * (1 - factor) + sig_b * factor

normalize(value, min_val, max_val) =
    (value - min_val) / (max_val - min_val)
```

---

## Default parameters

Parameters can have default values. The special `dt=` parameter sets the tick interval for temporal functions.

```idyl
// dt as temporal tick rate
lfo(freq, amplitude, dt=10ms) = modulation |> { ... }

// Regular default parameter
scale(input, factor=1.0) = input * factor
```

Default parameters can be passed in three ways:

```idyl
// 1. Omit — uses default
result = scale(42)             // factor = 1.0

// 2. Positional
result = scale(42, 2.0)       // factor = 2.0

// 3. Named
result = scale(42, factor=2.0)
```

---

## Named arguments

Any parameter can be passed by name at the call site:

```idyl
blend(sig_a, sig_b, factor) = ...

// Positional
x = blend(a, b, 0.5)

// Named (order-independent)
x = blend(factor=0.5, sig_a=a, sig_b=b)
```

Named arguments are particularly useful for temporal functions with `dt` and for `clock()` with `parent=`:

```idyl
c = clock(60bpm, parent=main_clock)
```

---

## Composition

Functions compose by nesting calls:

```idyl
scale_and_offset(input, scale, offset) = (input * scale) + offset

// Compose
chain(x) = scale_and_offset(scale_and_offset(x, 2, 10), 0.5, -5)
```

Higher-order patterns emerge naturally: functions that take flow inputs and produce flow outputs can be chained together.

---

## First-class functions

Functions are values. A function name used as an expression (without parentheses) returns a function reference:

```idyl
// Select between sin and cos
wave_select(selector) = sin; cos ? selector
```

The selected function can be called indirectly:

```idyl
process: {
    func = wave_select(1)       // func is now cos
    result = func(pi / 4)      // calls cos(pi/4)
    print(result)               // 0.707107
}
```

---

## Built-in functions

The following functions are always available:

### Math

| Function | Arity | Description |
|----------|-------|-------------|
| `sin(x)` | 1 | Sine |
| `cos(x)` | 1 | Cosine |
| `tan(x)` | 1 | Tangent |
| `log(x)` | 1 | Natural logarithm |
| `exp(x)` | 1 | Exponential |
| `pow(x, e)` | 2 | Power |
| `sqrt(x)` | 1 | Square root |
| `abs(x)` | 1 | Absolute value |
| `floor(x)` | 1 | Floor |
| `ceil(x)` | 1 | Ceiling |
| `fmod(x, m)` | 2 | Floating-point modulo |
| `min(a, b)` | 2 | Minimum |
| `max(a, b)` | 2 | Maximum |

### Conversion

| Function | Arity | Description |
|----------|-------|-------------|
| `int(x)` | 1 | Truncate to integer (for bitwise ops) |
| `float(x)` | 1 | Ensure float |
| `rint(x)` | 1 | Round to nearest integer |

### Random

| Function | Arity | Description |
|----------|-------|-------------|
| `rnd()` | 0 | Random in [0, 1) |
| `rnd(lo, hi)` | 2 | Random in [lo, hi) |
| `rnd(lo, hi, step)` | 3 | Quantized random |

### Utility

| Function | Arity | Description |
|----------|-------|-------------|
| `len(flow)` | 1 | Length of a flow |
| `bit(val, idx)` | 2 | Bit at index (0 or 1) |
| `print(...)` | 1+ | Print values to stdout |
| `now()` | 0–1 | Current time (`"ms"` or `"s"`) |

### Temporal intrinsics

These are handled specially by the evaluator (not plain builtins):

| Function | Description |
|----------|-------------|
| `clock(bpm)` | Create a child clock |
| `clock(bpm, parent=h)` | Create clock with specific parent |
| `tempo()` | Query main clock BPM |
| `tempo(handle)` | Query a specific clock's BPM |
| `tempo(bpm)` | Set main clock BPM |
| `tempo(handle, bpm)` | Set specific clock BPM |
| `handle(Nb)` | Duration of N beats at that clock's BPM |

See [Chapter 8 — Clock & tempo](ch08_clock_tempo.md) for full details.

---

[Next: Time & temporal functions →](ch04_temporal.md)
