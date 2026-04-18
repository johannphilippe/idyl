# 6 — Control flow & operators

[← Back to index](idyl_documentation.md) · [Previous: Flows](ch05_flows.md)

---

Idƴl has no `if/else`, no `while`, no `for`. Control flow is expression-based: ternary selection replaces conditionals, generator expressions replace loops.

---

## Ternary selection

The ternary operator selects between options based on a condition.

**Syntax**: `condition ? option0; option1`

- If `condition` is `0` (false) → returns `option0`
- If `condition` is non-zero (true) → returns `option1`

```idyl
abs_value(x) = (x >= 0) ? (0 - x); x

safe_divide(a, b) = (b != 0) ? 0; (a / b)
```

Options are separated by `;` (semicolons), not commas — this avoids ambiguity with function argument separators.

### Single-option shorthand

When only one option is given, the implicit second option is `_` (rest):

```idyl
// (m) ? expr   is equivalent to   m ? _; expr
// → evaluates expr only when m is truthy; returns _ otherwise
(m) ? osc_send(handle, "/gate", 1)
```

This is the idiomatic way to make an expression conditional on a trigger.

### Multi-way selection

More than two options — selected by integer index:

```idyl
// condition selects by index: 0 → first, 1 → second, …
note(degree) = (degree % 5) ? 261; 293; 329; 349; 391

state_out(state, idle, attack, sustain) =
    state ? idle; attack; sustain
```

### Style note

Wrapping the condition in parentheses is recommended for clarity but not required:

```idyl
x > 0 ? 0; 1      // works
(x > 0) ? 0; 1    // clearer — recommended
```

---

## Arithmetic operators

| Operator | Description |
|----------|-------------|
| `+` | Addition (also string concatenation) |
| `-` | Subtraction |
| `*` | Multiplication |
| `/` | Division |
| `%` | Modulo |
| `pow(x, e)` | Power (use the builtin — see Chapter 3) |

```idyl
sum = 3 + 4
product = 2 * 5
power = pow(2, 10)   // 1024
modulo = 17 % 5      // 2
```

### Unary

| Operator | Description |
|----------|-------------|
| `-` | Negation |

```idyl
neg = -42
```

---

## Comparison operators

All comparisons return `1` (true) or `0` (false). There is no boolean type.

| Operator | Description |
|----------|-------------|
| `==` | Equal |
| `!=` | Not equal |
| `<` | Less than |
| `>` | Greater than |
| `<=` | Less or equal |
| `>=` | Greater or equal |

```idyl
is_loud = amplitude > 0.5
in_range = (x >= 0) * (x <= 1)    // AND via multiplication
```

Since there are no boolean operators (`&&`, `||`), use multiplication for AND and addition/max for OR:

```idyl
both = (a > 0) * (b > 0)           // 1 only if both true
either = max((a > 0), (b > 0))     // 1 if either true
```

---

## Bitwise operators

Operate on integer-converted values:

| Operator | Description |
|----------|-------------|
| `&` | Bitwise AND |
| `\|` | Bitwise OR |
| `^` | Bitwise XOR |
| `<<` | Left shift |
| `>>` | Right shift |
| `~` | Bitwise NOT |

```idyl
a = int(12)       // 1100 in binary
b = int(10)       // 1010 in binary
result = a & b    // 1000 = 8
```

Use `bit(value, index)` to extract a single bit:

```idyl
pattern = int(7)                    // 0111 in binary
b0 = bit(pattern, 0)               // 1
b3 = bit(pattern, 3)               // 0
```

---

## Operator precedence

From highest to lowest:

1. Parentheses `()`
2. Unary `-`, `~`
3. `*`, `/`, `%`
4. `+`, `-`
5. `<<`, `>>`
6. `<`, `>`, `<=`, `>=`
7. `==`, `!=`
8. `&`
9. `^` (XOR)
10. `|`
11. `?` ... `;` (ternary condition, then options)

Use parentheses when in doubt:

```idyl
// Clear
result = (x + y) * z

// Ambiguous without knowing precedence
result = x + y * z        // = x + (y * z)
```

---

## Generator expressions (iteration)

Instead of imperative loops, use generators to create flows:

```idyl
// Range
numbers = [i = 0..10 : i]

// With transform
table = [i = 0..256 : sin(i / 256 * 2 * pi)]

// Chromatic scale
chromatic(root) = [s = 0..11 : root * pow(2.0, s / 12.0)]
```

Generators are declarative, composable, and produce flows directly.

---

## `@` — deferred execution block

The `@(time_expr): { }` block schedules a block of statements to run **once**, after a specified delay. It is the "fire and forget at time T" primitive.

```idyl
process: {
    print("start")

    @(500ms): {
        print("this runs 500ms later")
    }

    @(2s): {
        print("this runs 2 seconds later")
    }
}
```

### Syntax

```
@(time_expression): {
    statement
    statement
    ...
}
```

The single-statement form (braces optional, colon required) is also valid:

```idyl
@(1s): print("one second later")
```

### Time expression

The time expression can be any value that resolves to a duration — a literal, a variable, or a computed expression:

```idyl
process: {
    delay = 800ms
    @(delay): {
        print("fired after delay")
    }
    @(delay * 2): {
        print("fired after double the delay")
    }
}
```

### Scope

The handler block sees the process block's live scope. Variables modified in the handler are visible to subsequent code — including other at-blocks that haven't fired yet:

```idyl
process: {
    x = 10
    @(300ms): {
        x = 99
        print("x is now:", x)     // prints 99
    }
    @(600ms): {
        print("x is still:", x)   // also prints 99
    }
}
```

### Use cases

- Timed releases, fade-outs, or state transitions
- Sequencing one-shot events in a musical timeline
- Triggering another process after a warm-up delay
- Scheduling cleanup after a finite process completes

```idyl
process: {
    osc = osc_out("127.0.0.1", 9000)
    osc_send(osc, "/gate", 1)

    @(500ms): {
        osc_send(osc, "/gate", 0)    // release after 500ms
    }
}
```

---

[Next: Emit & catch →](ch07_emit_catch.md)
