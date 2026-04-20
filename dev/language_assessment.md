# Language assessment — current state

A candid review of what is working well, what is fragile, and what is missing.
Written against the pre-alpha state (April 2026).

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
Emitting named signals out of temporal functions (`emit finished = !`) and catching them (`timer catch finished: { }`) is clean and avoids the need for callbacks, observers, or a separate event bus. It composes naturally with the reactive model.

### Flows are powerful and growing
Named members, generator expressions, parametric flows, live temporal elements per-slot, dynamic rebuilding when arguments change, and now `on` gating — flows have become a real data model, not just a list wrapper. The cursor-per-member model handles polyphony and rhythm naturally.

### Clock hierarchy solves a real problem
Proportional tempo propagation — children that scale automatically when the parent changes — is something no mainstream language provides out of the box. Being able to call a clock handle as `c(2b)` to get a beat duration is elegant.

### 7-pass semantic analysis before execution
All errors are reported before anything runs. This eliminates a class of bugs (wrong arity, undefined names, scope violations) that would otherwise surface as confusing runtime crashes. For a time-based language where programs run "forever", this is especially valuable.

### Editor support exists early
Vim syntax, VSCode extension with tmLanguage grammar, and a Vim live-coding plugin that sends code via OSC. Most languages don't get editor support until much later.

---

## Weaknesses

### 49 reduce/reduce + 8 shift/reduce parser conflicts
These are pre-existing and unresolved. Most are benign (generator expressions being matched by two rules, operator precedence ambiguities), but they represent real grammar technical debt. Bison resolves them heuristically; the resolution may not always match user intent. A clean rewrite of the grammar with explicit precedence declarations for every operator would fix this, but it's a full day of careful work.

### ~~Function definitions are global-scope only~~ — **resolved**
Local functions and closures are now supported at process-block, init-block, and update-block scope. Functions defined inside a process or lambda block are visible only within that scope and do not pollute global namespace. Closures capture the enclosing temporal instance by reference (Option B): at call time the closure reads the instance's current params and state, so the caller always sees live values. Mutual recursion between local functions is not yet supported — declarations must appear before use within the same scope.

### The ternary operator ~~is unusual and hard to read~~ — **resolved**
The syntax has been changed to condition-first: `condition ? false_val; true_val`. The condition now reads left-to-right. A single-option shorthand `cond ? expr` (equivalent to `cond ? _; expr`) makes the trigger-gate pattern concise. The multi-way N-way form remains unchanged in semantics: `index ? opt0; opt1; opt2; …`.

### No explicit error handling at runtime
User programs have no mechanism to catch runtime errors (division by zero, type mismatches, bad flow access). The evaluator either silently returns 0 or crashes. For long-running temporal programs, a silent wrong value is worse than an informative error.

### Hot reload is partially working
Hot reload for named process blocks works, but the diff-and-apply mechanism has known edge cases (newly added temporal segments, changed `on` blocks). Using it for live coding requires caution. The TODO marks it `in_progress`.

### ~~Serial is missing~~ — **resolved**
Serial I/O is now available via `module("serial")`. Exposes `serial_open(port, baud)`, `serial_close(handle)`, `serial_write(handle, data)`, and the native temporal `serial_recv(handle, dt=10ms)` which emits `received` on each incoming chunk. Implemented as a zero-dependency header (`utilities/serial.hpp`) using POSIX `termios` on Linux/macOS and Win32 `CreateFile`/`DCB` on Windows.

### MIDI is missing
For a language targeting musical and embedded use, the absence of MIDI is a significant gap. OSC and Csound cover a lot, but MIDI is unavoidable for hardware integration.

### HTML documentation is manually regenerated
`doc/html/` is generated from `doc/*.md` via `build_html.sh` and committed separately. It will drift out of sync whenever the markdown is updated. A CI step or a note in contributing docs would help.

### `process` keyword semantics
The TODO itself notes that `process` doesn't fit the language's vocabulary ("tide", "weave", etc. were considered). This is a pre-alpha window to rename it — after 1.0, renaming a keyword is a breaking change with no easy migration path.

### Flow cursors ~~are global, not per-call-site~~ — **resolved**
Cursors are now per-call-site, keyed by the stable AST node pointer of the `flow_access_expr`. `pattern[m1]` and `pattern[m2]` each own independent cursors and step through the flow at their own rate. Polyphonic use works correctly for static, dynamic, and parametric flows.

### No audio-rate scheduling
The scheduler is system-clock based, accurate to ~1ms. For sample-accurate scheduling (synthesis, tight rhythmic quantization below 1ms), a separate audio-buffer-rate mode is needed. The TODO acknowledges this as "later". Until then, the language cannot replace an audio-rate environment.

### The `on` keyword could accumulate ambiguity
Currently `on` means two distinct things: flow member gate (`melody on rhythm: [...]`) and reaction block (`on m: { ... }`). Both are syntactically unambiguous today, but if a future event-handler form is added (`on "note_on": { ... }`), the keyword will carry three senses. Worth monitoring as the language grows.

---

## Proposed features — analysis

### Catch syntax: `catch a::sig: { }` instead of `timer catch sig: { }`

**What it would bring.** The proposed form reuses the `::` accessor already present in the language (`timer::finished` reads the emitted value; `catch timer::finished: {}` would subscribe to it). This makes the two operations — reading and catching — visually parallel. It also opens the door to catching from anonymous or inline expressions without pre-naming the binding, e.g. `catch metro(dt=500ms)::tick: {}`, which the current form cannot express at all.

**What is consistent about it.** The `a::sig` notation already implies "signal `sig` from instance `a`". Overloading it in `catch` position is a natural extension: `catch` tells the parser "subscribe, don't just read". No new operator or keyword is needed.

**What it could break / open questions.**
- All existing `catch` code is a straightforward mechanical rename — `x catch y: {}` → `catch x::y: {}`. Low risk, high churn.
- The semantic overlap between `a::sig` as a read (returns the current value) and `catch a::sig` as a subscription (installs a handler) needs to be clearly documented. Both are valid but do different things; the difference is the leading `catch` keyword, which is unambiguous in the grammar.
- `catch` currently belongs to a specific reaction-set context (inside process blocks, after a temporal binding). The new form needs to clarify whether `catch` can appear anywhere or only in those same positions.
- If anonymous-instance catching (`catch metro(dt=500ms)::tick: {}`) is supported, the scheduler needs to own the anonymous instance's lifetime — defining where it is destroyed.

**Verdict.** Worth doing before 1.0. The symmetry gain is real, the mechanical breakage is contained, and the anonymous-instance form unlocks a genuinely new pattern. The open-question around lifetime ownership of anonymous instances should be resolved in the design before implementation.

---

### Functional flow — functions returning flows

**What it would bring.** Today, parameterized flows (`flow pattern(key) = { melody: [...], rhythm: [...] }`) are first-class at the definition level but are a special syntactic form. If ordinary functions could return flows, the distinction disappears: any function that evaluates to a flow value is a parameterized flow. Computed flows become possible — flows whose structure is derived from runtime arithmetic, not just literal arrays. Composition becomes natural: `f(g(x))` where `g` returns a flow that `f` sequences or merges.

**What is consistent about it.** The language already has the concept of a flow as a typed value (`value_t::flow` presumably exists internally). Making it returnable from a function is the same as making any other value type returnable. The "recomputed when parameters change" behavior already exists for parametric flows — it would just be triggered by argument-change detection at the function-call level instead of at the flow-definition level.

**What it could break / open questions.**
- **Type system gap.** The evaluator currently doesn't know at call time whether a function returns a flow. A caller writing `pattern[m]` needs to know `pattern` is a flow, not a number. Either functions need return-type annotations, or the evaluator needs to do a runtime check on every indexed access — both are significant changes.
- **Cursor ownership.** A flow returned from a function call needs its own cursor state. Currently cursors live inside `flow_site_cursors_` keyed by AST node. If the same function is called at two sites and both return flows, each return value needs independent cursors — the ownership model becomes recursive.
- **Temporal elements inside returned flows.** If the flow contains `metro(dt=100ms)` as an element, who subscribes it to the scheduler? The subscription must be tied to the lifetime of the flow value, not the function call frame.
- **Recomputation on parameter change.** Implementing "recompute when arguments change" requires tracking which call sites have which argument values and diffing them on each tick — essentially a memoization layer on top of the evaluator. This is non-trivial to implement correctly for temporal elements.
- **Interaction with hot reload.** If a function body changes during hot reload and it returns a flow, all existing flow instances from previous calls need to be invalidated.

**Verdict.** High impact, high complexity. The semantic payoff is large — it would unify parameterized flows with regular functions and enable fully computed flows. However, it touches cursor ownership, the type system, the scheduler, and hot reload simultaneously. This is a 1.0+ feature unless scope is narrowed (e.g., restrict to stateless flows — no temporal elements, no cursors — as a first step).

---

### Code block feature — anonymous expression blocks in ternary

**What it would bring.** `(condition) ? { expr_a; expr_b }; { expr_c; expr_d }` allows multi-statement branches without hoisting them to a named function. It partially addresses the "functions are global-scope only" weakness: local helper logic stays local. It also reads naturally for anyone coming from a C-style background. As a generalization, named `{...}` scopes inside expressions would be the foundation closures need.

**What is consistent about it.** `{...}` already denotes scoped blocks in process bodies, lambda bodies (`|> { ... }`), and `@` deferred blocks. Making a block an expression that evaluates to its last statement is a direct extension of the existing convention.

**What it could break / open questions.**
- **Grammar ambiguity.** `{...}` is currently unambiguous because it only appears after specific keywords or operators (`:`, `|>`). In expression position — especially inside ternary options — the parser would need to distinguish a block expression from a flow literal (`{...}` with named members) and from a process body. This adds grammar complexity to a grammar already carrying 50+ conflicts.
- **Return value semantics.** What does a block evaluate to? Options: (a) the value of the last expression, (b) a tuple of all expression values, (c) `_` (side-effects only). Option (a) is most natural and consistent with how `|>` lambda bodies work.
- **Scope and capture.** Variables defined inside a block (`{ x = ...; x * 2 }`) must not escape. But the block needs to read from enclosing scope. If the enclosing scope contains temporal variables, capturing them is effectively a closure — which means the block-as-expression feature and closures are tightly coupled design decisions.
- **Temporal bindings inside blocks.** `{ m = metro(dt=100ms); m }` in a ternary branch — does `m` subscribe if the branch is evaluated? What if the other branch is taken next tick? This requires careful semantics around conditional scheduler subscription, which does not exist today.
- **Interaction with the ternary evaluator.** Currently `eval_expr` on ternary options is lazy (only the selected branch is evaluated). Block expressions would need the same lazy semantics — and temporal subscriptions inside the unevaluated branch must not fire.

**Verdict.** Very useful ergonomically; the multi-statement branch case is a genuine pain point. But the grammar, scope, and temporal-subscription problems make this non-trivial. A safe first step: allow blocks only in non-temporal contexts (static functions, outside process blocks), deferring the temporal-subscription question. That subset is implementable without touching the scheduler and would address the most common use case (local helper logic in pure functions).

---

## Open design questions

**Should `dt` be per-function or per-instance?**
Right now `dt` is a parameter — you pass it at the call site. This means the same function definition can run at multiple rates, which is flexible. But it also means the function definition alone doesn't encode its expected rate, making it harder to reason about at a glance.

**Should flows be immutable values or mutable streams?**
The current model is a hybrid: flows are defined immutably but their cursors are mutable state. Formalizing this distinction — flows as pure sequences, streams as advancing views over them — would clarify the semantics and make cursor ownership unambiguous.

**Is `emit` a side effect or a return value?**
Semantically, `emit` is an out-of-band signal, but in practice it is used as a secondary return value. Unifying the two concepts (a temporal function that returns multiple named values) might be cleaner and make the `::` accessor feel more natural.

**What happens to scoping rules when closures are added?** — **resolved**
Closures use by-reference capture: the function value carries a `shared_ptr<function_instance>` to its owning temporal instance. At call time the instance's `params_` and `current_` state are pushed as a scope frame below the function's own parameter frame. This means a closure always sees the instance's most-recently-committed state — temporal by-reference semantics without observer callbacks. Process-scope closures (functions defined at process level capturing process-local variables) work through the lexical scope chain, which stays alive for the duration of the process.

---

## Priority ranking (subjective)

🟣 - Means dangerous, or not sure

| | Item | Impact |
|---|---|---|
| 🔴 | Fix parser conflicts | Technical debt, potential latent parse bugs |
| ✅ | Flow cursor instancing per call-site — resolved | Each call site owns its cursor; polyphony works correctly |
| ✅ | Catch syntax — resolved | `catch a::sig: {}` implemented; anonymous-instance `catch expr::sig: {}` experimental |
| 🟣 | Functional flow | allow functions to return flows with standard flow instance properties (flow is recomputed if any parameter changes in the function call, flow can be dynamic, and contain temporal elements) |
| 🟣 | Code block feature | Allow block of code (expression bloc, like in @ blocs or catch blocks, that can be used to create anonymous scopes, and can be used in ternary expressions `(condition) ? {/*expressions A*/}; {/*Expressions B*/}`|
| ✅ | Serial module | `module("serial")` — zero-dependency, POSIX + Win32 |
| 🟠 | MIDI module | Missing for hardware use |
| 🟠 | Runtime error handling | Silent failures in long-running programs are dangerous |
| ✅ | Function definitions in process/lambda scope | Quality of life, code locality |
| 🟡 | Finish hot reload edge cases | Required for live coding to be reliable |
| 🟡 | Consider renaming `process` before 1.0 | Breaking change window is closing |
| 🟢 | Audio-rate scheduler mode | Longer term, needed for synthesis |
| ✅ | Local functions and closures | Implemented: process/init/update scope, by-reference capture |
| 🟢 | Traits | flow traits allowing changing behavior of flow (operator overloading, flow methods etc) |
| ✅ | Ternary syntax — resolved | Condition-first `cond ? a; b` implemented |

