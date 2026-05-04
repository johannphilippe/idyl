# Block expressions — design analysis

**Status**: Under consideration (May 2026)

Anonymous code blocks in expression position: `{ stmt; stmt; expr }`. The block evaluates its statements in order and returns the value of the last expression.

---

## The proposal

```idyl
// Ternary where each branch is a multi-step block
(condition) ? {
    osc_send(out, "/gate", 1)
    440hz
} ; {
    osc_send(out, "/gate", 0)
    220hz
}

// Single-branch: run side effects when triggered
(trigger) ? {
    emit step = step + 1
    print("fired:", step)
}
```

---

## Grammar

The path is clear. When we introduced `flow { }` literals we deliberately put the `flow` keyword in front so that bare `{ }` in expression position would be free for exactly this purpose. No conflict exists between:

```
flow { name: [...] }    // named-member flow — flow keyword is the signal
{ stmt; stmt; expr }    // code block — bare brace is the signal
```

The grammar addition is one new `primary_expression` alternative:

```bison
| LBRACE block_body RBRACE
```

where `block_body` is a sequence of statements followed by an optional expression (or just statements returning nil). The same rule that governs process block bodies and lambda update bodies works here — assignments, bare calls, `emit`, deferred blocks.

**Return value**: the last expression in the block. If the last statement is an assignment or a bare call, the block returns `nil`. This mirrors how Rust, Kotlin, and Ruby handle block expressions.

---

## Consistency analysis

### Fits the language well

The language already has several imperative contexts — process blocks, init blocks, update blocks. A block expression is just a way to use that same imperative power inline, in expression position. It does not introduce a new paradigm; it reuses the existing one.

The ternary operator is the only control flow primitive. Today it is limited to single expressions per branch. Block expressions lift that limitation cleanly:

```idyl
// Before: awkward workarounds — either call a function or split logic
(trig) ? do_on_trigger() ; _

// After: inline multi-step branches
(trig) ? {
    emit gate = !
    send_note(out, pitch)
} ; {
    emit gate = _
}
```

### Consistent with "explicit state"

The language philosophy is: mutations are visible. Inside a block expression, all assignments and `emit` statements are explicit. Nothing changes about how state is tracked — `emit` still marks mutations, lambda blocks still own state. Blocks do not introduce hidden mutable variables.

### Scoping rule

Block expressions introduce a **new lexical scope**. Variables defined inside a block are not visible outside it. This is the standard rule for any block-scoped language and has no surprising interactions with idyl's existing scope model.

```idyl
x = (cond) ? {
    local_val = compute()   // only visible inside this block
    local_val * 2
} ; 0
// local_val is gone here
```

---

## Risk and difficulty

### Grammar: low risk

One production rule added. No ambiguity with existing syntax (resolved by the `flow` prefix decision). No shift/reduce conflicts expected.

### AST: low

New node type `block_expr` holding a list of statements and a return expression. Mirrors `lambda_block` internally — same structure, different semantics (no `init:`, no temporal context, just inline execution).

### Evaluator: low-moderate

Execute statements in order, return last value. The existing `exec_stmt` machinery handles all statement types. The only new question is: **what temporal context does a block expression run in?**

- A block expression inside a process body → same context as the process.
- A block expression inside a lambda update body → same context as the lambda tick.
- A block expression used as a ternary branch → evaluated only when the branch is taken, same context as the enclosing expression.

This is exactly right. No new engine complexity.

### Semantic analysis: low-moderate

Push/pop a scope around the block. Validate statements inside. Infer the return type from the last expression (or `nil` if the last statement has no expression value). The existing analysis passes all apply inside blocks.

### Dangerous edge cases

**`stop` inside a block expression**: If `stop` appears inside a block expression used as a ternary branch, its semantics are unchanged — it terminates the temporal instance (or process). No special handling needed.

**Temporal assignments inside blocks**: `phase = fmod(phase + dt, 1)` inside a block expression within a lambda update body is fine — it's still in temporal context. The state update happens as part of the tick.

**`emit` inside blocks**: Works fine. `emit` is a statement. Blocks can contain statements.

**Blocks inside flow literals**: `flow { name: { stmt; expr } }` — this would be a block expression as a flow member value. Semantically fine: the block evaluates on every access and returns its last value.

**Nested blocks**: `{ { expr } }` — valid, the inner block returns its value, the outer block returns it. No depth limit.

---

## What this enables — future potential

### 1. Inline conditional state mutation (immediate value)

```idyl
// A common pattern becomes clean
velocity(trig) = v |> {
    init: { v = 0 }
    v = (trig) ? {
        new_v = rnd(60, 127)
        emit last_vel = new_v
        new_v
    } ; v
}
```

### 2. Multi-step deferred actions (already possible with `@`, now composable)

```idyl
process: {
    (gate) ? {
        osc_send(out, "/freq", pitch)
        osc_send(out, "/gate", 1)
        @(duration): { osc_send(out, "/gate", 0) }
    }
}
```

### 3. Inline function bodies that need multiple steps

Functions today are limited to a single expression body (`f(x) = expr`). Block expressions would allow multi-step inline definitions without a lambda block:

```idyl
// Today: need a helper function
clamp_and_scale(x, lo, hi) = scale(clamp(x, lo, hi))

// With block expressions: could be inline
clamp_and_scale(x, lo, hi) = {
    clamped = min(max(x, lo), hi)
    clamped / (hi - lo)
}
```

This is a significant quality-of-life improvement for utility functions that don't need state.

### 4. Thunks / lazy evaluation (future)

If blocks become first-class values (storable, passable), they become thunks — deferred computations. Combined with a `call(block)` or just `block()` syntax, this enables lazy evaluation and callbacks without a full closure system.

```idyl
// Hypothetical: store a block as a value and call it later
action = { osc_send(out, "/note", 60) }
(trigger) ? action()
```

This is a bigger feature and requires blocks to become first-class. Worth keeping in mind as a future direction.

### 5. Pattern-like multi-way dispatch

```idyl
// Multi-way block dispatch
result = (mode % 3) ? {
    x = sine(freq, dt=10ms)
    x * gain
} ; {
    x = tri_shape(phase, 1)
    x * gain * 0.7
} ; {
    x = square_shape(phase, 1)
    x * gain * 0.5
}
```

This replaces what would otherwise require three helper functions.

### 6. Cleaner `catch` and `on` handler bodies

Today catch and on blocks require the braces:

```idyl
catch timer::end: {
    print("done")
    stop
}
```

Block expressions are already the syntax — this feature just makes that syntax usable anywhere, not only in catch/on positions.

---

## What it does NOT enable (by design)

- **Loops**: blocks don't add iteration. Generator expressions handle that.
- **Mutable globals**: same rule — mutations require explicit `emit` or temporal context.
- **Early return from a function**: `return` doesn't exist. Block expressions evaluate fully and return the last value. This is consistent.
- **Exception handling**: `catch` is for temporal events, not exceptions. No change.

---

## Priority and recommendation

**Recommendation: implement.** The feature is low-risk, grammatically clean (the `flow { }` decision already cleared the path), consistent with the language's existing execution model, and has meaningful payoff:

| Dimension | Assessment |
|-----------|------------|
| Grammar conflict risk | None — `flow { }` prefix already disambiguates |
| Implementation effort | Low: ~3-4 days across parser, AST, evaluator, analyzer |
| Consistency | High — reuses existing imperative context model |
| Breaking changes | None |
| Payoff | High: unlocks multi-step ternary branches, inline multi-step functions |

The single most valuable immediate use is **multi-step ternary branches**. Today every non-trivial branch requires extracting a helper function. Block expressions remove that friction and keep logic co-located with its usage.

The longer-term potential (thunks, lazy eval) is worth keeping in mind when designing the block expression AST node — make the node independently storable from the start, even if first-class block values are not exposed yet.

---

## Proposed syntax summary

```idyl
// Block expression — bare braces, evaluates to last expression
{ stmt; stmt; expr }

// In ternary
(cond) ? { side_effect(); value1 } ; { value2 }

// Single branch (evaluates only when condition is truthy)
(trig) ? { emit x = x + 1 }

// As function body (multi-step without lambda block)
clamp(x, lo, hi) = {
    lo_clamped = max(x, lo)
    min(lo_clamped, hi)
}

// Inside a flow member (live, evaluated each access)
signals = flow {
    processed: [{
        raw = sensor()
        raw * gain + offset
    }]
}
```

Statements inside a block are separated by newlines or semicolons — same rule as process blocks and lambda bodies.
