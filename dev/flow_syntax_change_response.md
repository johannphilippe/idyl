# Response: Flow Syntax Change Proposal

## Short answer

Don't remove `flow`. Instead, extend temporal functions to optionally return a
flow value. That gets the useful part of the proposal (multi-output oscillators,
bundled state exports) without the cost.

---

## What the proposal is actually about

There are two separate ideas mixed together here, and they have different
merit:

1. **Unify flows and functions** — remove `flow`, let any function return a
   collection as its output.
2. **Let temporal functions (`|>`) export multiple values** — `oscil` returns
   `[s, s::phase, amp]` so callers get a bundle.

Idea 2 is genuinely useful and relatively clean to add. Idea 1 is the
mechanism that makes it possible, but full unification brings real costs.

---

## Problems with full unification

### 1. Type information is pushed to runtime

Right now `flow f(i) = ...` tells the parser, the evaluator, and the reader
that `f` produces a collection. With unification, you only know that at
evaluation time, by inspecting the return value. That means:

- The evaluator can no longer cache/dispatch on definition kind at parse time.
- Tools (editor highlight, error messages) lose static information.
- A reader skimming the file can't tell from the header whether `f` is a
  scalar function or a flow producer.

The `flow` keyword is cheap to write and carries real semantic weight. Removing
it trades explicitness for a marginal reduction in vocabulary.

### 2. Non-parametric flows become ambiguous

`flow octaver = [12, 7, 0, 3, 5]` currently reads unambiguously as a
named data constant. Without `flow` it becomes `octaver = [12, 7, 0, 3, 5]`,
which looks identical to a regular assignment. In a process body that would
be a reaction, not a global data definition. The keyword is doing real work
here by scoping the meaning.

### 3. The named-member return syntax is a parsing hazard

```idyl
multilfo(freq, amp, dt=50ms) =
    sine_lfo: [si, si::phase]
    sq_lfo:   [sq, sq::phase]
    ...
```

The `name: expr` pattern has to be unambiguously parseable as "return a
named-field record" and not as some other construct (label, on-block, future
syntax). This is workable but adds a grammatical production that currently
only lives inside `flow { ... }` literals. Moving it to the return position of
any function body is a non-trivial parser change and a potential future
conflict.

The `flow { ... }` variant (second proposal) is cleaner because it keeps the
keyword as a marker: the parser sees `flow` and knows what follows. It just
moves the keyword from the declaration line to the body. That's better but
still adds a second meaning of `flow` (declaration vs. return-type hint).

### 4. Documentation and tooling churn for low gain

Every existing example, tutorial, and test that uses `flow f(...) = ...` would
need updating. The behavior is unchanged — you're paying the migration cost
purely for a keyword reduction.

---

## What is genuinely worth doing

### Temporal functions returning flow values

The `oscil` example is the compelling case:

```idyl
oscil(freq, amp, dt=50ms) = [s, s::phase, amp]
|> {
    s = sine(freq) * amp
}
```

This lets a single temporal binding expose multiple related outputs — the
signal, its phase, its amplitude — so callers can do:

```idyl
o = oscil(1hz, 0.8)
send(o[0])        // signal
visualize(o[1])   // phase
```

This is useful. It maps naturally onto the existing type system because
`value_t::flow` is already a valid output type for a function instance —
the evaluator just needs to allow `read_output()` to return a flow value
instead of asserting scalar. The `|>` block can compute sub-values and the
binding expression packs them into a flow literal that becomes the output.

Importantly, this does not require removing `flow`. You keep:

```idyl
flow melody(i) = { rhythm: [...], notes: [...] }  // unchanged static flow
```

And add:

```idyl
oscil(freq, amp, dt=50ms) = [s, s::phase, amp]    // temporal, returns flow
|> { s = sine(freq) * amp }
```

The distinction is structural: `oscil` has a `|>` block (temporal), `melody`
does not. No ambiguity.

### Named-member temporal output

If you want the `multilfo` named syntax, limit it to temporal functions:

```idyl
multilfo(freq, amp, dt=50ms) = {
    sine_lfo: si
    sq_lfo:   sq
    tri_lfo:  tri
}
|> {
    si = sine(freq) * amp
    sq = square(freq) * amp
    tri = triangle(freq) * amp
}
```

Here `{ name: expr, ... }` in the return position of a function definition
is clearly a record literal (same syntax as flow member literals). It's only
parsed in return position, the `|>` block makes it clearly temporal, and
callers access fields the same way as any flow: `lfo.sine_lfo`.

---

## Answering the specific questions

**1. Is it a good idea overall?**
Partial: yes for multi-output temporal functions, no for removing `flow`.

**2. Implement for flow-maker functions only, keep `flow` for primitives?**
Yes, this is the right split. The zero-parameter and parametric static flows
(`flow name = ...`, `flow name(x) = ...`) stay exactly as they are.
Temporal functions gain the ability to return a flow-typed value as a natural
extension of what they already do. No keyword removal needed.

**3. Another idea?**
The `::` accessor on instance state (`s::phase`) combined with flow output
covers most of what `multilfo` is trying to do. If `oscil` can return
`[s, s::phase, amp]`, the caller already gets phase without needing a
named-field record. Named fields are nicer ergonomically but the value is
incremental. I'd do indexed flow output first (simpler evaluator change),
and named field output only if there's a real use case that indexed access
handles badly.

---

## Implementation cost

| Change | Cost | Risk |
|---|---|---|
| Remove `flow` keyword entirely | High | High (parser, evaluator, stdlib, tests) |
| `flow { }` as return-type marker | Medium | Medium (second meaning of keyword) |
| Temporal function → indexed flow output | Low–Medium | Low |
| Temporal function → named-field output | Medium | Low–Medium |

The indexed flow output path touches `instantiate_temporal`, `tick_instance`,
and `read_output` in the evaluator. The `|>` body already computes
sub-variables; the binding expression just needs to be evaluated as the output
instead of having a single scalar return.
