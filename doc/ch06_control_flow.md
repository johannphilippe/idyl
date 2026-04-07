# 6 — Control flow & operators

[← Back to index](idyl_documentation.md) · [Previous: Flows](ch05_flows.md)

---

Idƴl has no `if/else`, no `while`, no `for`. Control flow is expression-based: ternary selection replaces conditionals, generator expressions replace loops.

---

## Ternary selection

The ternary operator selects between options based on a condition.

**Syntax**: `option0; option1 ? condition`

- If `condition` is `0` (false) → returns `option0`
- If `condition` is non-zero (true) → returns `option1`

```idyl
abs_value(x) = (0 - x); x ? (x >= 0)

safe_divide(a, b) = 0; (a / b) ? (b != 0)
```

### Multi-way selection

More than two options — the condition wraps around:

```idyl
// condition % N selects the option
note(degree) = 261; 293; 329; 349; 391 ? (degree % 5)

state_out(state, idle, attack, sustain) =
    idle; attack; sustain ? state
```

Options are separated by `;` (semicolons), not commas — this avoids ambiguity with function argument separators.

---

## Arithmetic operators

| Operator | Description |
|----------|-------------|
| `+` | Addition (also string concatenation) |
| `-` | Subtraction |
| `*` | Multiplication |
| `/` | Division |
| `%` | Modulo |
| `^` | Power |

```idyl
sum = 3 + 4
product = 2 * 5
power = 2 ^ 10       // 1024
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
| `^` | Bitwise XOR (context-dependent: power on floats) |
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
3. `^` (power)
4. `*`, `/`, `%`
5. `+`, `-`
6. `<<`, `>>`
7. `<`, `>`, `<=`, `>=`
8. `==`, `!=`
9. `&`
10. `|`
11. `;` ... `?` (ternary)

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
chromatic(root) = [s = 0..11 : root * (2.0 ^ (s / 12.0))]
```

Generators are declarative, composable, and produce flows directly.

---

[Next: Emit & catch →](ch07_emit_catch.md)
