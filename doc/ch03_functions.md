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

## Local functions and closures

Functions can be defined inside a process block, an `init` block, or an update block. They are visible only within the enclosing scope and do not export to global scope.

### Process-scope helpers

```idyl
process: {
    semitones_to_ratio(s) = pow(2, s / 12)
    midi_to_hz(note) = 440 * semitones_to_ratio(note - 69)

    print(midi_to_hz(60))   // 261.626
}
```

`semitones_to_ratio` and `midi_to_hz` are invisible outside this process block. Multiple process blocks can each define their own `normalize`, `scale`, or `clamp` without collision.

### Init-block functions (closures)

A function defined inside an `init` block is stored as a closure on the temporal instance. It captures the instance by **reference**: when called, it always reads the instance's current params and state — not a frozen snapshot.

```idyl
make_adder(n, dt=100ms) = fn |>
{
    init: {
        fn(x) = x + n    // n captured by reference
    }
}

process: {
    add5 = make_adder(5)
    print(add5(10))   // 15
    print(add5(20))   // 25
}
```

The closure can also capture mutable state. Here `total` grows each tick; every call to `fn` sees the latest value:

```idyl
accumulator(dt=200ms) = fn |>
{
    init: { total = 0 }
    total = total + 1
    fn(x) = x + total
}

process: {
    acc = accumulator()
    print(acc(100))   // 100, 101, 102, ... (grows with total)
}
```

### Update-block tick-local aliases

A function defined in the update body is a **tick-local alias** — redefined fresh each tick from the current scope, not persisted between ticks:

```idyl
tracker(input, dt=10ms) = ratio |>
{
    init: { peak = 0 }
    peak = max(peak, abs(input))
    normalize(v) = v / max(peak, 0.001)   // defined each tick, uses current peak
    ratio = normalize(input)
}
```

`normalize` is visible to all subsequent update statements in the same tick but is not stored or emitted.

### Scoping rules

- Local functions look up names bottom-up: their own parameters → enclosing block scope → process scope → global scope.
- A local function **can** shadow a global function of the same name.
- **Mutual recursion** between local functions is not supported — each function must be declared before it is called.
- Local functions defined in update blocks cannot be emitted or stored as persistent values (they are tick-scoped). Closures from `init` blocks **can** be emitted and stored.

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
| `string(x)` | 1 | Convert value to string |
| `trigger(x)` | 1 | Convert value to trigger type |

### Time unit conversion

| Function | Arity | Description |
|----------|-------|-------------|
| `as_ms(x)` | 1 | Interpret value as milliseconds |
| `as_s(x)` | 1 | Interpret value as seconds |
| `as_hz(x)` | 1 | Interpret value as Hz (period in ms) |
| `as_bpm(x)` | 1 | Interpret value as BPM (period in ms) |

### MIDI / frequency

| Function | Arity | Description |
|----------|-------|-------------|
| `mtof(note)` | 1 | MIDI note number → frequency in Hz |
| `ftom(freq)` | 1 | Frequency → MIDI note number |

### Random

| Function | Arity | Description |
|----------|-------|-------------|
| `rnd()` | 0 | Random in [0, 1) |
| `rnd(lo, hi)` | 2 | Random in [lo, hi) |
| `rnd(lo, hi, step)` | 3 | Quantized random |
| `seed(n)` | 1 | Seed the random number generator |

### Utility

| Function | Arity | Description |
|----------|-------|-------------|
| `len(flow)` | 1 | Length of a flow |
| `bit(val, idx)` | 2 | Bit at index (0 or 1) |
| `print(...)` | 1+ | Print values to stdout |
| `printf(fmt, ...)` | 1+ | C-style formatted print |
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
| `bpm(handle)` | Alias for `tempo(handle)` — query a clock's BPM |

See [Chapter 8 — Clock & tempo](ch08_clock_tempo.md) for full details.

---

[Next: Time & temporal functions →](ch04_temporal.md)
