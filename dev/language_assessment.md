# Language assessment — current state

A candid review of what is working well, what is fragile, and what is missing.
Written against the pre-alpha state (May 2026).

---

## Strengths

### Time is a first-class type
The language's core bet — that time should be a value, not an afterthought — is paying off. Writing `metro(dt=500ms)` or `clock(120bpm)` reads exactly like what it means. The scheduler underneath is drift-free and accurate. No other general-purpose language makes temporal composition this readable.

### Trigger/rest duality is genuinely expressive
`!` and `_` as typed values (not booleans, not nulls) is a strong design decision. Trigger-gated logic — whether in flow members (`on`) or `on` blocks — maps cleanly onto musical and reactive thinking. The distinction forces programmers to be explicit about "live right now" vs "currently silent".

### Reactive process architecture is clean
The segment + reaction model — temporal bindings followed by reactions that re-fire on every tick — turns out to be a natural programming model for this domain. Reaction redistribution across segments works transparently. The programmer doesn't think about scheduling; they write intent.

### Functional core is consistent
No `if/else`, no mutation outside `|>` lambda blocks, no implicit state. Every binding is a definition. This is enforced structurally, not just stylistically. Code is easy to reason about locally.

### Emit/catch is a good side-channel design
Emitting named signals out of temporal functions (`emit finished = !`) and catching them (`catch timer::finished: {}`) is clean and avoids the need for callbacks, observers, or a separate event bus. It composes naturally with the reactive model.

### Flows are powerful and growing
Named members, generator expressions, parametric flows, live temporal elements per-slot, dynamic rebuilding when arguments change, and `on` gating — flows have become a real data model, not just a list wrapper. The cursor-per-member model handles polyphony and rhythm naturally.

### Clock hierarchy solves a real problem
Proportional tempo propagation — children that scale automatically when the parent changes — is something no mainstream language provides out of the box. Being able to call a clock handle as `c(2b)` to get a beat duration is elegant.

### 7-pass semantic analysis before execution
All errors are reported before anything runs. This eliminates a class of bugs (wrong arity, undefined names, scope violations) that would otherwise surface as confusing runtime crashes.

---

## Consistency analysis

### `::` overloaded across two unrelated roles
The `::` operator means two different things:

```idyl
u = import("utils")
u::my_function(42)   // (1) namespace access — function call on library u

c = counter(dt=100ms)
c::step              // (2) emit accessor — read side-channel value of instance c
```

Both produce a value from a named member of some "owner", so the surface similarity is intentional. But the operational semantics are unrelated: (1) is a static dispatch into a namespace resolved at load time; (2) is a dynamic read from live instance state. A reader seeing `x::y` cannot tell without context whether `x` is a module/library or a temporal instance.

**Options:** keep and document clearly; or split with a different accessor (e.g. `.` for module namespacing, `::` only for emit). No strong recommendation — the visual alignment of `a::b` for both "member of a" concepts is arguably an asset. But the documentation must be explicit.

---

### `on` carries two unrelated meanings

```idyl
// (1) Flow member gate — inside a flow literal
flow pattern = {
    rhythm:  [!, _, _, !]
    melody on rhythm: [60, 63, 65]   // melody only advances when rhythm fires
}

// (2) Reactive block — inside a process body
process: {
    r = rhythm[m]
    on r: { print("tick") }          // react to trigger r
}
```

Both meanings share the concept "when this signal fires, do something". Form (1) gates a flow cursor; form (2) executes a block. The grammar disambiguates them by context (inside `flow { }` vs inside `process { }`), so there is no parser conflict.

The risk is cognitive, not technical: a programmer reading `melody on rhythm` and `on r:` will correctly guess the relationship but may not realize the semantics are significantly different (cursor advance vs block execution). If a third `on` form is added — e.g. event subscription like `on "note_on": { }` — the keyword will become genuinely overloaded.

**Recommendation:** document both forms explicitly and side-by-side. Consider whether the block form should use a distinct keyword (`when`, `react`, `if`) before the API is stable. The cost of renaming before 1.0 is low; after 1.0 it is a breaking change.

---

### `flow` keyword — partially redundant and heading toward optionality

The `flow` keyword currently serves three roles, and each has a different redundancy level:

| Usage | Example | Redundant? |
|---|---|---|
| Non-parametric global sequence | `flow notes = [60, 62, 64]` | Mostly — global constants work fine |
| Parametric sequence | `flow pattern(i) = [40+i, 50+i]` | **Yes** — identical to `pattern(i) = [40+i, 50+i]` |
| Multi-member flow | `flow drum = { kick: [...], snare: [...] }` | Partly — `{name: ...}` literal is parseable without it |

The `functional_flow` and `functional_flow_member` tests demonstrate the redundancy directly: `func2() = [5, 4, 3, 2]` and `func() = a: [...] b: [...]` already work without the `flow` keyword. The semantic difference between `flow pattern(i)` and `pattern(i)` is zero.

See the dedicated section below for a full analysis.

---

### The ternary `?`/`;` is internally consistent but unusual

```idyl
// Selection by index
note(deg) = (deg % 5) ? 261; 293; 329; 349; 391

// Boolean guard (0 = false → first option; 1 = true → second option)
x = (age > 0) ? _; !

// Single-option shorthand (cond ? expr ≡ cond ? _; expr)
(trig) ? cs_note(handle, "first", 0.1s, 0.3, mtof(note))
```

The semantics are consistent: the condition evaluates to an integer index selecting among the options. This unifies boolean guards and multi-way dispatch under one operator.

However, `;` as an option separator (instead of `:`) catches every newcomer. The reason `:` is unavailable is that it is already used for flow member names (`kick: [...]`). This is a legitimate grammar constraint, not a design error. The `;` is slightly uncomfortable but not wrong.

One internal inconsistency: the shorthand `(m) ? expr` implies "do expr when m is truthy", which is index=1 → the second slot. The absence of a slot-0 value is filled with `_`. This is logical but easy to misread as "if m, do expr" in a zero-indexed way, since the intent IS "do expr when m fires" but the index is 1, not 0. The single-option form could instead be read as `(m) ? _; expr` where `_` is implicit. That's exactly what it is — just ensure the documentation shows the equivalence.

---

### `process` naming does not fit the language vocabulary

The existing assessment already noted this. `process` is a computing term; the language's domain is musical and temporal composition. `flow`, `tempo`, `metro`, `phasor` all come from music/signal vocabulary. `process` stands out.

Candidates: `part`, `voice`, `phrase`, `pattern`, `score`, `run`, `patch`. The word `part` reads naturally: `part drums: { ... }` / `part bass, dur=8b: { ... }`. It also avoids collision with common programming meanings.

**Priority:** rename before 1.0. This is the last zero-cost window.

---

### `stop` semantics split across contexts

```idyl
// (1) Self-stop inside a lambda block
age >= limit ? stop; _

// (2) Stop another process from a process block
stop process_name

// (3) Stop in a ternary expression slot (stops the current instance if branch taken)
remaining == 0 ? stop; remaining - 1
```

Forms (1) and (3) are the same concept in two syntactic positions (statement vs expression). Form (2) is a completely different operation. A reader writing `stop` inside a lambda expects it to mean "I'm done". Writing `stop drums` inside a process means "kill another block". The two share a keyword but have no operational relationship.

**Recommendation:** consider separating these: `halt` for self-termination (inside lambda/ternary), `stop name` for process termination (inside process blocks). Or keep `stop` for both and accept the dual semantics — they are already grammatically distinct by position.

---

### `module()` and `import()` — similar calls, different systems

```idyl
import("stdlib")         // load an .idyl file; functions merged into global scope
u = import("utils")      // load an .idyl file into a namespace

module("osc")            // load a compiled module; functions merged globally
osc = module("osc")      // load a compiled module into a namespace
```

The two calls look identical syntactically but load completely different things (an interpreted file vs a compiled binary module). They also have different namespace behaviors: a bare `import("stdlib")` merges into global scope; a bare `module("osc")` also merges. Both support namespaced loading (`u = import(...)`). So they are consistent in that respect.

The redundancy is the call syntax itself — why not a single `load("stdlib")` that detects whether the target is a file or compiled module? The split is currently justified by the need to signal to the programmer what they are loading. Worth keeping for now, but the distinction should be noted in documentation.

---

### Naming in stdlib

Several pairs overlap or could be confusing:

| Pair | Issue |
|---|---|
| `counter` vs `accum` | Both count up on trigger. `counter` is free-running (no wrap); `accum` wraps and uses `trig_in` naming. Inconsistent parameter naming conventions. |
| `rcounter` vs `accum` | `rcounter` has a named `reset!` trigger; `accum` implicitly resets at `wrap_at`. Different abstractions but the naming doesn't signal this. |
| `gate` (temporal) vs `gate_in` (param name) | `gate` as a function name collides with the conventional meaning of "gate signal" used as a parameter name throughout the library. |
| `oneshot` | Fires `!` on tick 0 (age=0, condition false → first option), then `_`. The name is accurate but the behavior feels backwards to read: `(age > 0) ? !; _` fires the trigger when age is NOT > 0, i.e. the first tick only. |

**Recommendation:** rename `rcounter` to `counter_r` or add it as `counter(reset!)`. Rename `accum` to `step_counter`. Review parameter names for trigger-vs-signal consistency.

---

## Feature collisions / gaps

### No type distinction between a number and a boolean
`1 > 0` returns `value_t::number(1.0)`. The ternary uses this as an index. Bitwise operators (`&`, `|`) work on numbers. There is no `bool` type. This is pragmatic — it matches signal-processing conventions (1=on, 0=off) — but it means no static protection against accidentally using a number as a condition when a trigger is expected.

### `emit` is a secondary return value dressed as a side effect
`emit x = ...` is semantically a second output channel from a temporal function. But it is syntactically a statement inside a `|>` block, which makes it look like a side effect rather than an output. The `::` accessor then reads this "side effect" value. The design works but fights the functional framing: if outputs are values, the second output should look like a second value, not an emission.

This is the strongest argument for the "temporal function returning a flow" proposal — `f() = [primary, secondary] |> { ... }` makes both outputs explicit and symmetric.

### No runtime error handling
Programs have no mechanism to catch runtime errors (division by zero, undefined flow access, type mismatches). The evaluator silently returns 0 or `_`. For long-running temporal programs, a silent wrong value is worse than an informative error. This is the most dangerous omission for production use.

---

## Conciseness

### Excellent
- Time units inline: `100ms`, `1hz`, `4b`, `120bpm`
- Trigger/rest literals: `!`, `_`
- Generator expressions: `[i = 0..9 : i * 2]`
- Ternary multi-way: `(x) ? a; b; c; d` replaces switch entirely
- `flow` member gating: `melody on rhythm: [...]` — highly expressive per character
- Named process duration: `process intro, dur=8b: { }` — one line

### Acceptable but slightly verbose
- Lambda block header: `output_var |> { init: { ... } ... }` — the `init:` block inside the `|>` adds one nesting level. For simple stateful functions (counter, etc.) this feels heavy.
- `emit finished = (remaining == 0) ? _; !` — the `emit` keyword and the inverted ternary (for "fire when condition true") requires a mental double-step.

### Genuinely awkward
- `(age > 0) ? !; _` for oneshot behavior — reads as "if age>0 then !, else _" but since index 0 = `!` it actually fires on the FIRST tick (age=0). First-time readers get this backwards.
- Parametric flow definition with `flow` keyword alongside equivalent function syntax creates needless surface area: `flow pattern(i) = [...]` vs `pattern(i) = [...]` — identical semantics, two spellings.

---

## The `flow` keyword and the `{ }` grammar collision

### The core problem

The grammar distinguishes three kinds of `{ }` blocks, all unambiguous today because each is preceded by a distinct keyword or operator:

| Syntax | What precedes `{` | Meaning |
|---|---|---|
| `flow drum = { kick: [...] }` | `flow name =` | named-member flow literal |
| `f(dt=..) = expr \|> { ... }` | `\|>` | lambda (update) block |
| `process: { ... }` | `process ...:` | process body |
| `@(t): { ... }` | `@(expr):` | timed callback |
| `catch x::sig: { ... }` | `catch ...:` | catch handler |
| `on x: { ... }` | `on expr:` | reaction block |
| `init: { ... }` | `init:` | init block |

There is currently **no `{ }` in expression position** — specifically, a function body is always a single `expression`, never a brace block.

This creates two problems simultaneously:

**Problem 1 — Multi-member flows from functions don't work.**
`functional_flow_member.idyl` attempts:
```idyl
func() = 
    a: [1, 2, 3]
    b: [4, 5, 6]
```
The parser grammar has no rule for `function_definition` with a `flow_members` body. The bare `IDENTIFIER COLON flow_literal` sequence is not a valid `expression`. This test is **not currently supported** — there is no grammar rule for it.

**Problem 2 — Future code blocks need that same expression slot.**
The planned code block feature `(cond) ? { expr; expr }; { expr }` requires `{ }` in expression position. If named-member flows also claim `{ }` in expression position (e.g. `func() = { a: [...], b: [...] }`), the two are syntactically indistinguishable without lookahead:

```idyl
func() = { a: [1, 2, 3] }     // flow member? or code block with label 'a'?
func() = { a + b }             // unambiguously a code block expression
```

At one token of lookahead (`{`, then `IDENTIFIER`, then `:`) the parser can technically discriminate — `identifier:` is only a flow member, not a valid expression start — but this adds a fragile context-sensitive disambiguation to an already GLR grammar.

---

### The solution: migrate `flow` from definition-level to literal-level

Instead of `flow` being a definition-prefix keyword (`flow drum = { ... }`), make it a **literal-level type tag** that can appear in any expression position:

```idyl
flow { name: expr, ... }   // named-member flow literal — anywhere an expression is valid
```

This disambiguates perfectly with zero lookahead:
- `flow { ... }` → named-member flow literal (the `flow` keyword is the signal)
- `{ ... }` → code block (no `flow` prefix)

The `[...]` list form needs no tag — it is already unambiguous in all positions.

---

### What this looks like in practice

**Simple flows — unchanged:**
```idyl
notes = [60, 62, 64]             // [..] is unambiguous everywhere
pattern(i) = [40+i, 50+i]        // function returning simple flow — works today
```

**Named-member flows — `flow { }` as a literal:**
```idyl
// Global constant (definition-level `flow` becomes optional)
drum = flow { kick: [!, _, _, _], snare: [_, _, !, _] }

// Function returning named-member flow (NOW WORKS)
rhythm() = flow {
    kick:  [!, _, _, _]
    snare: [_, _, !, _]
}

// Parametric named-member flow (replaces `flow complex(i) = { }`)
pattern(i) = flow {
    notes:  [40+i, 50+i, 60+i]
    rhythm: [!, _, !, !]
}

// Temporal function returning named-member flow (new capability)
multilfo(freq, amp, dt=50ms) = flow {
    sine_lfo: si
    sq_lfo:   sq
}
|> {
    si = sine(freq) * amp
    sq = square(freq) * amp
}
```

**Code blocks in ternary — no collision:**
```idyl
(cond) ? { print("a"); 42 }; { print("b"); 0 }    // plain { } = code block
(cond) ? flow { x: [1,2], y: [3,4] }; default_flow // flow { } = named-member flow
```

**`on` gating works naturally inside `flow { }`:**
```idyl
rhythm() = flow {
    kick:           [!, _, _, _]
    melody on kick: [60, 63, 65]
}
```

---

### Grammar change summary

One new production is needed — a `flow_record_literal` expression form:

```
expression: ...
    | FLOW LBRACE flow_members RBRACE   → flow_record_literal_expr
    ;
```

The existing `flow_definition` rules can be rewritten as sugar:
```
flow_definition:
    FLOW name = flow_record_literal  →  name = flow_record_literal  (just a global constant)
    FLOW name(params) = expr         →  name(params) = expr          (already works without 'flow')
```

Eventually, the `flow_definition` grammar rule can be deprecated entirely. `flow` at the definition level becomes optional, which is consistent with the assessment's recommendation.

---

### What `flow` becomes

The keyword migrates from **"I am declaring a flow"** (definition-level) to **"this brace block is a sequence, not a code block"** (literal-level). The migration is:

| Before | After | Notes |
|---|---|---|
| `flow notes = [60, 62]` | `notes = [60, 62]` | `[...]` is self-evident, no `flow` needed |
| `flow pattern(i) = [...]` | `pattern(i) = [...]` | already works, `flow` was always redundant |
| `flow drum = { kick: [...] }` | `drum = flow { kick: [...] }` | `flow` moves to the literal |
| `flow complex(i) = { notes: [...] }` | `complex(i) = flow { notes: [...] }` | same migration |
| *(new)* | `f(dt=..) = flow { a: x, b: y } \|> {...}` | temporal multi-output |

The `flow` keyword at definition level (`flow drum = ...`) can be kept as accepted sugar indefinitely — it is simply redundant, not harmful.

### Does this make `flow` useless at the definition level?

For simple flows (`[...]`) — yes, the definition-level `flow` is redundant and can be omitted.
For named-member flows — the `flow` keyword migrates INTO the literal, where it is load-bearing (it disambiguates `flow { }` from code blocks).

The net result: **`flow` is never useless**, but its role shifts from a statement-level declaration keyword to a literal-level type tag. It says "this brace block is a sequence" rather than "this definition produces a sequence". That is a more precise and consistent meaning.

---

## Proposed features — analysis

### Temporal function returning an indexed flow

**What it would bring.** A temporal function with a `|>` block today exposes a single scalar output. Allowing the return expression to be a flow literal lets one binding produce multiple related values without a wrapper:

```idyl
oscil(freq, amp, dt=50ms) = [s, s::phase, amp]
|> {
    s = sine(freq) * amp
}

o = oscil(1hz, 0.8)
send(o[0])       // signal
show(o[1])       // phase
```

**Verdict.** Low implementation risk. The evaluator change is contained to `tick_instance` output capture and `read_output`. The flow is a static snapshot per tick — no cursors, no per-call-site state. Implement before 1.0.

---

### Temporal function returning a named-member flow

**What it would bring.** Same motivation but with named fields:

```idyl
multilfo(freq, amp, dt=50ms) =
    sine_lfo: [si, si::phase]
    sq_lfo:   [sq, sq::phase]
|> {
    si = sine(freq) * amp
    sq = square(freq) * amp
}

lfo = multilfo(2hz, 0.5)
send(lfo.sine_lfo)
```

**Verdict.** Minor parser extension on top of indexed output (accept `{ name: expr }` or bare `name: expr` in return position after `=`). Implement after indexed output is stable. Together, these two features complete the `flow_syntax_change.md` proposal without removing the `flow` keyword or breaking anything.

---

### Catch syntax: `catch a::sig: { }` instead of `timer catch sig: { }`

**Verdict.** Worth doing before 1.0. The symmetry gain is real (`a::b` reads and `catch a::b:{}` subscribes — same accessor, two actions). The mechanical breakage is a search-and-replace. The anonymous-instance form (`catch metro(dt=500ms)::tick: {}`) unlocks a genuinely new pattern. Lifetime ownership of anonymous instances needs a design decision before implementation.

---

### Code block feature — anonymous expression blocks

**What it would bring.** `(cond) ? { expr_a; expr_b }; { expr_c; expr_d }` for multi-statement branches without hoisting to a named function.

**Verdict.** Useful ergonomically; the multi-statement branch case is a genuine pain point. Grammar, scope, and temporal-subscription semantics make this non-trivial. A safe first step: allow blocks in non-temporal contexts only. Defer to post-1.0.

---

## Open design questions

### Should flows be immutable values or mutable streams?
The current model is a hybrid: flows are defined immutably, cursors are mutable state. Formalizing the split — flows as pure sequences, streams as advancing views — would clarify semantics and make cursor ownership unambiguous.

### Is `emit` a side effect or a return value?
Semantically, `emit` is an out-of-band signal. In practice it is a secondary return value. The "temporal function returning a flow" proposal unifies the two concepts: instead of `emit x = ...`, the second output is an element of the returned flow. This is cleaner; `::` then becomes just "index a named member of the returned flow", which is already what it does for named flows elsewhere.

### `dt` per-function vs per-instance
`dt` is currently a parameter — the same function can run at multiple rates. This is flexible but means the function definition alone doesn't encode its expected rate. No strong recommendation; the current behavior is correct for a multi-rate composition language.

---

## Priority ranking

🟣 Uncertain / risky
🟢 High value, low risk
🟡 Medium value or medium risk
🟠 Important but harder

| Status | Item | Impact |
|---|---|---|
| ✅ | Parser conflicts resolved (GLR, 16 s/r locked with `%expect 16`) | |
| ✅ | Flow cursor instancing per call-site (polyphony works) | |
| ✅ | `catch a::sig:{}` syntax implemented | |
| ✅ | Serial module | |
| ✅ | MIDI module | |
| ✅ | Audio-rate scheduler (`--audio-clock`) | |
| ✅ | Local functions and closures | |
| ✅ | Hot reload edge cases fixed | |
| ✅ | Ternary condition-first syntax (`cond ? a; b`) | |
| 🟢 | Temporal function → indexed flow output `f(dt=..) = [a,b] \|> {}` | Low risk, high payoff |
| 🟡 | Temporal function → named-member flow output | Minor parser extension |
| 🟢 | Migrate `flow` to literal-level: `flow { }` as named-member flow literal | One grammar rule; unlocks functional multi-member flows and reserves `{ }` for code blocks |
| 🟡 | Rename `process` before 1.0 | Breaking; window is closing |
| 🟡 | Rename `on` block to `when`/`react` | Pre-1.0 window only |
| 🟠 | Runtime error handling | Silent failures dangerous in production |
| 🟣 | Full functional flow (recomputed, temporal elements, cursors) | High impact, high complexity |
| 🟣 | Code block feature (anonymous blocks in ternary) | Grammar complexity |
| 🟢 | Flow traits (operator overloading, flow methods) | Long term |
