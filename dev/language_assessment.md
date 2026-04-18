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

### Function definitions are global-scope only
You cannot define a function inside a process block or a lambda body. This forces programmers to hoist all helper functions to the top level, which creates noise and breaks locality. Closures are also absent. This is the single most limiting structural constraint on expressiveness right now.

### The ternary operator ~~is unusual and hard to read~~ — **resolved**
The syntax has been changed to condition-first: `condition ? false_val; true_val`. The condition now reads left-to-right. A single-option shorthand `cond ? expr` (equivalent to `cond ? _; expr`) makes the trigger-gate pattern concise. The multi-way N-way form remains unchanged in semantics: `index ? opt0; opt1; opt2; …`.

### No explicit error handling at runtime
User programs have no mechanism to catch runtime errors (division by zero, type mismatches, bad flow access). The evaluator either silently returns 0 or crashes. For long-running temporal programs, a silent wrong value is worse than an informative error.

### Hot reload is partially working
Hot reload for named process blocks works, but the diff-and-apply mechanism has known edge cases (newly added temporal segments, changed `on` blocks). Using it for live coding requires caution. The TODO marks it `in_progress`.

### Serial and MIDI are missing
For a language targeting musical and embedded use, the absence of serial I/O and MIDI is a significant gap. OSC and Csound cover a lot, but MIDI is unavoidable for hardware integration.

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

## Open design questions

**Should `dt` be per-function or per-instance?**
Right now `dt` is a parameter — you pass it at the call site. This means the same function definition can run at multiple rates, which is flexible. But it also means the function definition alone doesn't encode its expected rate, making it harder to reason about at a glance.

**Should flows be immutable values or mutable streams?**
The current model is a hybrid: flows are defined immutably but their cursors are mutable state. Formalizing this distinction — flows as pure sequences, streams as advancing views over them — would clarify the semantics and make cursor ownership unambiguous.

**Is `emit` a side effect or a return value?**
Semantically, `emit` is an out-of-band signal, but in practice it is used as a secondary return value. Unifying the two concepts (a temporal function that returns multiple named values) might be cleaner and make the `::` accessor feel more natural.

**What happens to scoping rules when closures are added?**
The current scope model is simple because functions cannot close over process-local variables. Adding closures will require a clear decision about capture semantics (by value, by reference, by temporal snapshot). Getting this wrong creates subtle non-determinism in temporal code.

---

## Priority ranking (subjective)

| | Item | Impact |
|---|---|---|
| 🔴 | Fix parser conflicts | Technical debt, potential latent parse bugs |
| ✅ | Flow cursor instancing per call-site — resolved | Each call site owns its cursor; polyphony works correctly |
| 🟠 | Serial + MIDI modules | Missing for hardware use |
| 🟠 | Runtime error handling | Silent failures in long-running programs are dangerous |
| 🟡 | Function definitions in process/lambda scope | Quality of life, code locality |
| 🟡 | Finish hot reload edge cases | Required for live coding to be reliable |
| 🟡 | Consider renaming `process` before 1.0 | Breaking change window is closing |
| 🟢 | Audio-rate scheduler mode | Longer term, needed for synthesis |
| 🟢 | Closures | Longer term, expressiveness |
| ✅ | Ternary syntax — resolved | Condition-first `cond ? a; b` implemented |
