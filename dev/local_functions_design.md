# Local function definitions — design document

A pre-implementation analysis of what "function definitions in process/lambda scope" means for idyl's semantics, what it enables, and what design decisions must be made before writing a single line of code.

---

## Implementation status

| Feature | Status |
|---|---|
| Local pure functions in process blocks | ✅ Implemented |
| Local functions in init blocks | ✅ Implemented |
| Tick-local function aliases in update blocks | ✅ Implemented |
| Closures with by-reference capture (Option B) | ✅ Implemented |
| Mutual recursion between local functions | ❌ Not yet — declarations must precede use |
| Closures over temporal variables (trigger/rest) | ❌ Disallowed (temporal non-determinism) |
| Full mutable closures (block expressions, make_counter pattern) | ❌ Requires block expression syntax first |

---

## Background

Currently, every function in idyl is defined at global scope. If a process block or lambda body needs a helper, it must be hoisted to the top level, which breaks locality and pollutes the global namespace. This is the single most limiting structural constraint on code organisation right now.

The goal is to allow:

```idyl
process: {
    // helper visible only within this process
    scale(v) = v * 0.5

    m = metro(dt=100ms)
    print(scale(m))
}
```

and possibly:

```idyl
smooth(input, dt=10ms) = out |> {
    init: {
        // helper visible only within this lambda
        clamp(v) = min(max(v, 0), 1)
        out = 0
    }
    out = clamp(out + (input - out) * 0.1)
}
```

---

## 1. Function definitions inside a lambda block

A lambda block has two distinct sub-contexts:

```
f(x, dt=10ms) = output |> {
    init: {
        // runs ONCE at instantiation
    }
    // update statements — run EVERY TICK
}
```

### 1.1 In the `init` block ✅ Implemented

This is the unambiguous case. A function defined in `init` is:

- Evaluated exactly once, at instantiation time
- Stored in the lambda's local environment alongside other init-time bindings
- Visible to all subsequent update statements in the same lambda
- Stable: the function definition never changes across ticks

```idyl
envelope(attack_ms, decay_ms, dt=1ms) = level |> {
    init: {
        // helpers computed once from parameters
        attack_rate(t) = min(t / attack_ms, 1)
        decay_rate(t)  = max(1 - t / decay_ms, 0)
        phase = 0
    }
    phase = phase + dt
    level = attack_rate(phase) * decay_rate(phase - attack_ms)
}
```

The function can reference the init-time parameters (`attack_ms`, `decay_ms`) by value — those are bound when the lambda is instantiated. This is a **capture by value at instantiation time**, which is safe and deterministic.

**Implementation notes:**
- Init-block parametric functions are stored as `value::local_function` (carries AST directly, not a `function_defs_` lookup key).
- They are placed in `inst->current_` so they survive into the update block's scope.
- They are also promoted to the enclosing lambda-body scope so update statements can call them.

### 1.2 In the update block ✅ Implemented (tick-local aliases)

Functions defined in the update block re-execute on every tick as **tick-local aliases**:

```idyl
tracker(input, dt=10ms) = ratio |> {
    init: { peak = 0 }
    peak = max(peak, abs(input))
    // re-creates the function on each tick, capturing current `peak`
    normalize(v) = v / max(peak, 0.001)
    ratio = normalize(input)
}
```

On each tick, `normalize` is defined as a local function in the tick scope. It is visible to subsequent statements in the same update block. It is NOT stored between ticks (not in `inst.next_`), NOT emitted, NOT returned.

**Implementation notes:**
- In `tick_instance`, a `function_definition` statement with non-empty parameters is recognized as a tick-local alias.
- `value::local_function(name, ast_ptr)` is created and `env_.define(name, fn_val)` is called.
- The same pattern applies in the "first pass" (initial evaluation at instantiation with an init block).
- Bug fixed: the first-pass loop did not handle parametric function definitions, causing "undefined function" on the first tick only.

**Limitation:** functions defined in the update block capture the tick-scope environment via the lexical scope chain. They can read state variables as they exist in that tick's scope. They cannot mutate state (writes must go through `inst.next_`).

### 1.3 Recursion

Can a locally-defined function call itself? For init-block functions, yes — with the usual idyl restriction that recursion in static context must terminate. For update-block tick-local functions, recursion is technically possible but practically useless (they can't hold state between calls). The implementation ensures the function name is in scope before the function body is evaluated.

---

## 2. Function definitions inside a process block ✅ Implemented

A process block is a sequence of segments (temporal bindings) and reactions. A function defined in a process block is:

- Visible to all code that follows it in the process block
- NOT visible outside the process block (not globally exported)
- Re-evaluated if placed as a reaction (i.e., after a temporal binding) — which would make it a tick-local alias, same as in the update block case

```idyl
process: {
    // Helper visible only within this process
    db_to_linear(db) = pow(10, db / 20)

    m = metro(dt=100ms)
    gain = db_to_linear(-6)     // uses helper
    osc_send(handle, "/gain", gain)
}
```

The function here is pure (no mutable state captured), so there is no temporal complexity — it behaves identically to a global function but with limited visibility.

**Functions placed as reactions (after a temporal binding) are re-created every tick** — same rule as lambda update block. If the function is pure, this is efficient and correct. If it captures mutable process-local state, the snapshot-vs-reference question arises.

**Implementation notes:**
- `exec_stmt(function_definition)`: if `env_.scopes_.size() > 1` (i.e., inside any block), stores the AST in a `value::local_function` and defines it in the current scope — does NOT write to `function_defs_`.
- `eval_call` checks for `fn_def_` on the looked-up value before falling back to `function_defs_`.
- The semantic analyser pre-collects local function names in process/init/update scopes so forward-references within the same block work.

### 2.1 Scope rule

The natural scope rule:

```
global scope
└── process-local scope (one per running process)
    └── lambda-local scope (one per instance)
        └── init scope (sub-scope of lambda-local, populated once)
```

A local function first looks up names in its own body, then in the enclosing scope (lambda-local → process-local → global). This means a local function CAN see global functions (to call them), but a global function cannot see process-local definitions.

This is standard lexical scoping. The evaluator already has an environment stack — local functions just add another frame.

---

## 3. Closures ✅ Implemented (by-reference, single-expression body)

Closures are local functions that carry their enclosing context across scope boundaries — stored in a flow, emitted, passed as an argument, or returned as a function's output.

### 3.0 What is implemented

**By-reference capture (Option B):** when a local function is created in an init or update block, the resulting `value` carries a `shared_ptr<function_instance>` pointing to the owning temporal instance. At call time, the instance's current `params_` and `current_` state are pushed as a scope frame below the function's own parameter frame. The closure always sees the instance's most-recently-committed state, not a snapshot taken at definition time.

```idyl
// Closure returned as the function's output
make_adder(n, dt=100ms) = fn |>
{
    init: {
        fn(x) = x + n    // n is captured by reference
    }
}

process: {
    add5 = make_adder(5)
    print(add5(10))   // 15
    print(add5(20))   // 25
}

// Closure over mutable state
accumulator(dt=200ms) = fn |>
{
    init: { total = 0 }
    total = total + 1
    fn(x) = x + total   // total is captured by reference — sees current value each call
}

process: {
    acc = accumulator()
    print(acc(100))   // 100, 101, 102, ... (total grows each tick)
}
```

**Process-scope closures** work through the lexical scope chain: the process scope stays alive for the duration of the process, so free variable lookups traverse the live chain automatically.

**Mutual recursion is not supported.** Local function declarations must appear before use within the same scope. Forward references between two local functions in the same block will fail at runtime with "undefined function". This is a declaration-order requirement, not a fundamental limit.

### 3.1 What is NOT yet implemented (requires block expressions)

The `make_counter` pattern — a closure with fully mutable private state — requires block-as-expression syntax that does not yet exist:

```idyl
// Pseudocode only — block expressions not yet in the language
make_counter(start) = fn
fn(x) = {
    n = n + 1    // n is a captured mutable variable
    x + n
}
```

This requires (a) block expressions `{ stmt; result_expr }` and (b) a heap-allocated shared environment for captured mutable variables. Both are future work.

### 3.1 What closures would unlock

```idyl
// Process-local: a function that remembers its own creation-time calibration
process: {
    calibration_offset = read_sensor()

    // calibrated_read closes over calibration_offset
    calibrated_read(raw) = raw - calibration_offset

    m = metro(dt=100ms)
    raw = adc_read()
    print(calibrated_read(raw))
}
```

Without closures, `calibrated_read` must be a global function and `calibration_offset` must be passed as a parameter every call. With closures, the process owns a private calibration context.

More powerfully:

```idyl
// Returns a function that counts calls to itself
make_counter(start) = {
    n = start
    fn(x) = {
        n = n + 1
        x + n
    }
    fn
}
```

This requires:
1. Blocks as expressions (separate feature, see language_assessment.md)
2. Mutable captured variables (`n` must be writable across calls)
3. A heap-allocated closure environment (the function carries a pointer to `n`)

### 3.2 The capture semantics question

This is the hardest design decision. Three options:

**A. Capture by value (snapshot at definition time)**

```idyl
x = 10
f() = x + 1   // captures x=10, never sees updates to x
x = 20
print(f())    // always prints 11
```

Pros: deterministic, no temporal non-determinism, easy to implement (copy the value).  
Cons: closed-over state never updates — limits the closure's usefulness for reactive code.

**B. Capture by reference (always sees current value)**

```idyl
x = 10
f() = x + 1   // holds a reference to the binding x
x = 20
print(f())    // prints 21
```

Pros: full power, functions see the latest state.  
Cons: calling `f()` at different points in a tick can give different results. For temporal programs where multiple segments can call the same closure, the output depends on call order — subtle non-determinism.

**C. Capture by temporal snapshot (per-tick consistent)**

Each tick, the closure's captured variables are frozen at the START of that tick's evaluation. Within a single tick, all calls to the closure see the same snapshot. After the tick, the snapshot is updated.

Pros: deterministic within a tick, reactive across ticks.  
Cons: requires the evaluator to freeze and restore the closure's environment at the tick boundary — implementation complexity. Semantics are non-obvious to users.

**Recommendation: start with capture by value for the init-block case (already done). Defer mutable capture until the implications for the scheduler are fully understood.**

### 3.3 Temporal closures

A closure that closes over a temporal variable (a metro, a counter) is a special case:

```idyl
process: {
    m = metro(dt=500ms)

    // Does this make sense?
    fire_when_m(handler) = {
        m ? handler()
    }
}
```

If `m` is captured by reference, calling `fire_when_m` at different scheduler times would give different results based on whether `m` has fired yet in that tick. This is exactly the kind of temporal non-determinism that makes the language hard to reason about.

**Recommendation: closures that capture temporal variables (trigger/rest typed) are disallowed until the semantics are worked out. The type system would need to track this.**

---

## 4. Scoping rules — is local still a descendant of global?

Yes — this is the only design that composes cleanly. The scope chain is:

1. **Function body** (local bindings created by that function's parameters + body)
2. **Enclosing lambda's update scope** (if defined in update block) OR **lambda's init scope** (if defined in init block)
3. **Process-local scope** (if defined in process block, or the lambda belongs to a process)
4. **Global scope** (top-level functions and definitions)

Lookup proceeds bottom-up (local → enclosing → process → global). This means:
- A local function CAN call global functions (e.g., `sin`, `metro`)
- A local function CAN shadow a global function by redefining the same name locally
- The global scope does NOT see local definitions (one-way transparency)

This is standard lexical scoping and matches programmer expectations.

### 4.1 Name shadowing

```idyl
// Global
scale(x) = x * 2

process: {
    // Local shadow — this scale(x) hides the global one
    scale(x) = x * 0.5

    m = metro(dt=100ms)
    print(scale(m))   // uses the local 0.5 version
}
```

The global `scale` is unaffected. This should work exactly like it does in most languages.

### 4.2 Mutual recursion

```idyl
process: {
    is_even(n) = n == 0 ? 0; is_odd(n - 1)
    is_odd(n)  = n == 0 ? 0; is_even(n - 1)
}
```

Mutual recursion between local functions requires both names to be in scope before either body is evaluated. This is a forward-reference problem. Options:
1. **Require declaration order** — `is_even` can't call `is_odd` unless `is_odd` is defined first. Simple but limiting.
2. **Allow forward references within the same scope** — scan the scope for all local function names first, then evaluate bodies. Cleaner but requires a two-pass approach in the evaluator.

**Recommendation: start with declaration order (simpler). Forward references within the same scope can be added later.**

---

## 5. Implementation path

The features decompose into three independently releasable steps:

### Step 1 — Local pure functions in process and init blocks (no capture) ✅ Done

Functions that don't close over anything from their enclosing scope. They act like global functions but with visibility limited to the enclosing scope.

```idyl
process: {
    clamp(v, lo, hi) = max(min(v, hi), lo)   // pure, no capture
    m = metro(dt=100ms)
    print(clamp(m, 0, 1))
}
```

Implementation: the evaluator checks `env_.scopes_.size() > 1` in `exec_stmt(function_definition)`. If inside any block, stores AST in `value::local_function` rather than writing to `function_defs_`. The semantic analyser pre-collects local function names.

### Step 2 — Init-block functions with value capture ✅ Done

Functions defined in a lambda's init block can capture init-time values by snapshot. Since init runs once and all captured values are immutable (they are fixed at instantiation), there is no temporal non-determinism.

```idyl
filter(cutoff, dt=1ms) = out |> {
    init: {
        coeff = dt / (dt + 1 / (2 * pi * cutoff))
        apply(x, prev) = prev + coeff * (x - prev)
        out = 0
    }
    out = apply(input, out)
}
```

Implementation: init-block parametric definitions are stored as `value::local_function` in `inst->current_`. The lexical scope chain makes init-time variables visible inside the function body without explicit capture.

### Step 2b — Tick-local aliases in update blocks ✅ Done

```idyl
tracker(input, dt=10ms) = ratio |> {
    init: { peak = 0 }
    peak = max(peak, abs(input))
    normalize(v) = v / max(peak, 0.001)
    ratio = normalize(input)
}
```

Implementation: `tick_instance` and the first-pass loop both recognize parametric `function_definition` statements as tick-local aliases and `env_.define` them in the current scope. They are not persisted in `inst.next_`.

### Step 3 — By-reference closures ✅ Done

Closures with by-reference capture are implemented. A `value::closure(name, ast_ptr, inst_ptr)` carries the owning instance; `eval_user_function` pushes `inst->params_` and `inst->current_` as a scope frame before the function's parameter frame.

### Step 4 — Full mutable closures with block expressions ❌ Future

The `make_counter` pattern requires block-as-expression syntax and heap-allocated shared environments for mutable captured variables. Deferred until block expressions are designed.

---

## 6. Open questions / future work

**Closures with mutable captured state** — the big one. Currently, local functions in update blocks can read enclosing scope variables via the lexical chain, but cannot write back to them. Making writes persist (true mutable closure) requires a shared heap-allocated environment.

**Closures over temporal variables** — disallowed until the type system can track temporal types and the scheduler semantics are fully specified.

**Hot reload** — when a process is hot-reloaded, its local scope is rebuilt from scratch. Local function definitions are re-evaluated in the new scope. This is correct behavior and is already handled.

**First-class local functions** — local functions can be passed as arguments and stored in variables, but passing them OUT of their defining scope (e.g., emitting a tick-local function) is undefined behavior. The type system should eventually enforce this.

---

## 7. Recommended design decisions (current)

| Question | Decision |
|---|---|
| Local functions in init block | ✅ Yes — full support, value capture |
| Local functions in update block | ✅ Yes — tick-local alias only, no escape |
| Local functions in process block | ✅ Yes — process-local visibility, no capture |
| Scope: local is descendant of global | ✅ Yes — standard lexical scoping |
| Name shadowing | ✅ Yes — local shadows global |
| Mutual recursion | Declaration-order requirement (simpler) |
| Closures with value capture | After Step 1 lands (init block already works) |
| Closures with mutable capture | After temporal semantics are resolved |
| Closures over temporal variables | Disallowed until type system tracks it |
| First-class local functions (no capture) | ✅ Yes — they are just function values |

---

## 8. Example: what is now enabled

```idyl
// Process-local helpers — no global pollution
process synth: {
    semitones_to_ratio(s) = pow(2, s / 12)
    midi_to_hz(note) = 440 * semitones_to_ratio(note - 69)

    m = metro(dt=100ms)
    note = 60
    print(midi_to_hz(note))
}

// Init-block helpers that capture parameters
filter(cutoff, dt=1ms) = out |> {
    init: {
        coeff = dt / (dt + 1 / (2 * pi * cutoff))
        apply(x, prev) = prev + coeff * (x - prev)
        out = 0
    }
    out = apply(input, out)
}

// Tick-local helper that reads state
tracker(input, dt=10ms) = ratio |> {
    init: { peak = 0 }
    peak = max(peak, abs(input))
    normalize(v) = v / max(peak, 0.001)
    ratio = normalize(input)
}
```
