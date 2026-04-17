# 2 — Language fundamentals

[← Back to index](idyl_documentation.md) · [Previous: Getting started](ch01_getting_started.md)

---

## Comments

Idƴl uses C-style comments:

```idyl
// Single-line comment — extends to end of line

/* Multi-line comment.
   Everything between the delimiters is ignored. */

x = 42  // comments can appear at end of a line
```

Comments have no semantic meaning. They are discarded during lexing.

---

## Core principles

| Principle | What it means |
|-----------|---------------|
| **Stateless by default** | Variables are immutable bindings. There are no mutable globals. |
| **Explicit state** | State only exists inside temporal lambda blocks (`\|> { ... }`), and every mutation is visible. |
| **Time is first-class** | Time literals are primitive values. Temporal functions carry their own clock. |
| **Functional** | No `if/else`, no `while`, no classes. Control is ternary selection, iteration is generator expressions. |
| **Graceful degradation** | Type mismatches and missing values produce warnings, not crashes. The show goes on. |

---

## Types

Idƴl has a small, implicit type system. There are no type annotations — the analyzer infers types and warns on mismatches.

| Type | Description | Example |
|------|-------------|---------|
| `number` | 64-bit floating point | `42`, `3.14`, `0.5` |
| `time` | Duration in milliseconds | `100ms`, `2s`, `440hz`, `120bpm`, `4b` |
| `trigger` | Instantaneous event | `!` |
| `rest` | Absence of event | `_` |
| `string` | Text | `"hello"` |
| `flow` | Ordered sequence | `[1 2 3]` |
| `function` | Callable reference | `sin`, `my_func` |
| `handle` | Opaque module resource | returned by `osc_out(...)` |
| `nil` | Nothing | default uninitialized state |

Types are **soft** — the analyzer warns about mismatches but does not prevent execution. This is intentional: in live contexts, a warning is better than a crash.

---

## Number literals

```idyl
x = 42
pi_approx = 3.14159
ratio = 0.5
negative = -1
```

Numbers are always 64-bit floating point. Integer conversion is available via `int()`.

---

## Time literals

Time is a first-class value. Five unit suffixes are recognized:

| Suffix | Meaning | Internal representation |
|--------|---------|------------------------|
| `ms` | Milliseconds | Direct |
| `s` | Seconds | × 1000 |
| `hz` | Hertz (frequency) | 1000 / value (period in ms) |
| `b` | Beats | Scaled by current tempo |
| `bpm` | Beats per minute | 60000 / value (period in ms) |

```idyl
delay = 100ms              // 100 milliseconds
duration = 2.5s            // 2500ms
pitch = 440hz              // ≈ 2.27ms period
quarter = 120bpm           // 500ms period
musical = 4.0b             // 4 beats (tempo-dependent)
```

Time arithmetic works naturally:

```idyl
total = 2s + 100ms         // 2100ms
doubled = 100ms * 2        // 200ms
half = 1s / 2              // 500ms
```

---

## Trigger and rest

```idyl
pulse = !      // trigger — an instantaneous event
silence = _    // rest — absence of event
```

Triggers are used as input events for trigger-driven temporal functions (see [Chapter 4](ch04_temporal.md)). Rest represents "nothing happened."

---

## Strings

```idyl
greeting = "hello idyl"
address = "/osc/note"

// Concatenation with +
full = "hello" + " " + "world"

// Number coercion
label = "value: " + 42          // "value: 42"
```

String comparison: `==`, `!=`, `<`, `>`, `<=`, `>=` (lexicographic).

---

## Constants

A zero-parameter definition is a constant. It is evaluated eagerly at load time:

```idyl
silence = 0
concert_pitch = 440hz
pi_approx = 3.14159
```

The predefined constants `pi`, `tau`, and `euler` are always available:

```idyl
circle = 2 * pi
full_circle = tau      // = 2π
```

---

## Scope rules

### Global scope — declaration only

Top-level (global) scope is **purely declarative**: only function definitions, constants, flow definitions, and module/library imports are allowed. Bare function calls and any other imperative statements are not valid at global scope.

```idyl
// Valid at global scope
silence = 0
square(x) = x * x
import("scales.idyl")
module("osc")

// NOT valid at global scope
print("hello")       // error — no bare calls outside process/lambda blocks
```

Top-level definitions are **hoisted** — they can reference each other regardless of declaration order:

```idyl
foo(x) = bar(x) + 1
bar(x) = x * 2           // defined after foo — still valid
```

### Process and lambda blocks — imperative context

Executable statements (assignments and bare expression calls) are only valid inside **process blocks** and **lambda blocks** (`|> { ... }`). These are the only contexts where function calls without assignment are permitted:

```idyl
process: {
    x = compute(440)        // assignment
    print("result:", x)     // bare call — valid here
}
```

Inside lambda blocks, **declaration order applies** — a variable must be defined before it is used.

Definitions are lexically scoped. Function parameters shadow outer-scope bindings. Lambda-local variables are visible only within their block.

---

[Next: Functions →](ch03_functions.md)
