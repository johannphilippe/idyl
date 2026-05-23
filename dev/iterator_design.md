# Iterator Design for idyl

## Motivation

idyl already handles sequential temporal patterns well through `counter`, `metro`, and flow indexing.
What is missing is a way to do **parallel or immediate repetition** — firing multiple events at once
(e.g. a chord, a burst of notes to different voices, an initialization sweep) without manually
unrolling the code.

```idyl
// Currently: you have to unroll manually
cs_note(cs, 1, 1s, 0.1, c4)
cs_note(cs, 2, 1s, 0.1, e4)
cs_note(cs, 3, 1s, 0.1, g4)

// With an iterator: concise, scalable
each n in 3: cs_note(cs, n+1, 1s, 0.1, deg(c4, major, n*2))
```

Temporal iteration is also useful — firing a block N times, once every `dt`, is a common
pattern in live coding that currently requires a `counter` + reaction boilerplate.

---

## Proposed Keyword: `each`

`each` fits the language naturally: it is imperative, short, and reads like English.
It does not conflict with any existing keyword.

---

## Syntax

### 1. Simple count

```idyl
each n in N: body
```

`n` is the iteration variable, scoped to `body`. Runs `body` N times with `n = 0, 1, …, N-1`.

```idyl
// Chord: fire 4 notes immediately
each n in 4: cs_note(cs, "voice", 1s, 0.1, deg(c3, major, n*2))

// Multi-block body
each n in 3: {
    freq = mtof(c4 + n * 4)
    cs_note(cs, n+1, 0.5s, 0.1, freq)
}
```

### 2. Range

```idyl
each n in start..end: body
```

`n` goes from `start` to `end` inclusive.

```idyl
each n in 2..5: cs_note(cs, n, 0.5s, 0.1, c4 + n*2)  // n = 2, 3, 4, 5
```

### 3. Range with step

```idyl
each n in start..end..step: body
```

Step can be negative (descending). Iteration stops before exceeding `end`.

```idyl
each n in 0..12..3: cs_note(cs, 1, 0.5s, 0.1, c4 + n)   // 0 3 6 9 12
each n in 12..0..-3: cs_note(cs, 1, 0.5s, 0.1, c4 + n)  // 12 9 6 3 0
```

For `step > 0`: iterates while `n <= end`.
For `step < 0`: iterates while `n >= end`.
Default step is 1 (or -1 when `start > end`, inferred automatically).

---

## 4. Temporal iteration

```idyl
each n in N, dt=X: body
```

Instead of running all iterations immediately, fires `body` once per `dt`, advancing `n` by 1
each time. After `n = N-1`, cycles back to 0 (infinite temporal loop, like a metro).

```idyl
// Fire one note per voice every 200ms, cycling through 4 voices
each n in 4, dt=200ms: cs_note(cs, n+1, 0.3s, 0.1, deg(c4, minor, n))
```

This is equivalent to (but far more concise than):

```idyl
cnt = counter(dt=200ms)
n = cnt % 4
cs_note(cs, n+1, 0.3s, 0.1, deg(c4, minor, n))
```

The temporal form works with ranges and steps too:

```idyl
each n in 0..6..2, dt=300ms: cs_note(cs, 1, 0.5s, 0.1, c4 + n)
```

---

## 5. Expression body (without braces)

When the body is a single expression, braces are optional (consistent with ternary and catch):

```idyl
each n in 4: cs_note(cs, n+1, 1s, 0.1, c4 + n*7)
```

---

## Design alternatives considered

### A — `for` / `foreach`
`for n in 0..7: body` — familiar from many languages, clear intent.
Downside: `for` carries baggage (imperative loops, not idyl's style).

### B — `rep(N, n): body` (function-style)
```idyl
rep(4, n): { ... }
rep(0, 12, 3, n): { ... }
rep(4, n, dt=200ms): { ... }
```
Looks like the existing module function syntax. Downside: the iteration variable `n` inside the
argument list feels awkward; it is not a value but a binding name.

### C — Pipe into block
```idyl
4 |> as n: { ... }
0..7 |> as n: { ... }
```
Consistent with `|>` syntax but the `as n` binding is novel and non-obvious.

### D — `times` / `repeat`
```idyl
4 times as n: { ... }
```
Reads naturally in English but `times` is a noun, not a verb, and does not generalize
well to ranges.

**Recommendation: option A (`each … in … :`) for clarity and generality.**
It handles counts, ranges, steps, and temporal iteration with a uniform syntax.
The `in` preposition and `..` range notation are already familiar from common languages
and from idyl's own flow notation.

---

## Interaction with other features

### Inside a process block

```idyl
process chord: {
    m = metro(dt=1s)
    // Immediate: fire all voices at once when m triggers
    on m: each n in 4: cs_note(cs, n+1, 2s, 0.1, deg(c3, major, n*2))
}
```

### Nested temporal iterators

```idyl
process arp: {
    // Outer: chord root changes every 4s
    root = counter(dt=4s)
    // Inner temporal iterator: arpeggio fires one note every 200ms
    each n in 7, dt=200ms: cs_note(cs, 1, 0.3s, 0.1, deg(c3 + root*5, major, n))
}
```

### With flow indexing

```idyl
flow voices = [1, 2, 3, 4]
each n in 4: cs_note(cs, voices[n], 1s, 0.1, c4 + n*7)
```

### Iteration variable in temporal context as a flow index

```idyl
flow scale = major
each n in 7, dt=200ms: cs_note(cs, 1, 0.4s, 0.1, mtof(deg(c4, scale, n)))
```

---

## Boundary and edge cases

| Expression        | Iterates             | Notes                          |
|-------------------|----------------------|-------------------------------|
| `each n in 0`     | never                | body never runs               |
| `each n in 1`     | once, n=0            |                               |
| `each n in 0..0`  | once, n=0            | inclusive end                 |
| `each n in 5..2`  | 5, 4, 3, 2           | auto step=-1 when start>end   |
| `each n in 5..2..1` | never              | step direction contradicts range |
| `each n in 4, dt=0ms` | immediate (=no dt) | treat as non-temporal      |

---

## Implementation notes

**Static (immediate) form**: desugars to a compile-time or runtime loop. During bytecode
compilation, the body is emitted in a loop with a local slot for `n`. No scheduler involvement.

**Temporal form**: creates a synthetic instance similar to `counter(dt=X)` with a bound body.
The body is wrapped in a callback, `n` is computed from `(tick_count % range_length)`,
and the iterator owns a scheduler subscription. Internally similar to a named temporal binding
but with an implicit variable.

**Range representation** at parse time: `start..end..step` — three optional expressions.
If only `N` is given: `start=0`, `end=N-1`, `step=1`.
If `start..end` is given: `step` inferred from sign of `(end - start)`.
If `start..end..step` given: used as-is (iteration guard prevents infinite loop on step=0).
