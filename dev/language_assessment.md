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

### ~~49 reduce/reduce + 8 shift/reduce parser conflicts~~ — **resolved (r/r), contained (s/r)**
The grammar now uses the `glr2.cc` skeleton (GLR parser). The 51 reduce/reduce conflicts were all caused by a single duplicate rule (`primary_expression: generator_expression`, unreachable because `flow_literal` already included it) — removing that rule eliminated all r/r conflicts. 16 shift/reduce conflicts remain; all are benign operator-precedence resolutions (Bison correctly resolves them as "prefer shift") and are locked in with `%expect 16` so they cannot silently accumulate. No behavioral change to the parser.

### ~~Function definitions are global-scope only~~ — **resolved**
Local functions and closures are now supported at process-block, init-block, and update-block scope. Functions defined inside a process or lambda block are visible only within that scope and do not pollute global namespace. Closures capture the enclosing temporal instance by reference (Option B): at call time the closure reads the instance's current params and state, so the caller always sees live values. Mutual recursion between local functions is not yet supported — declarations must appear before use within the same scope.

### The ternary operator ~~is unusual and hard to read~~ — **resolved**
The syntax has been changed to condition-first: `condition ? false_val; true_val`. The condition now reads left-to-right. A single-option shorthand `cond ? expr` (equivalent to `cond ? _; expr`) makes the trigger-gate pattern concise. The multi-way N-way form remains unchanged in semantics: `index ? opt0; opt1; opt2; …`.

### No explicit error handling at runtime
User programs have no mechanism to catch runtime errors (division by zero, type mismatches, bad flow access). The evaluator either silently returns 0 or crashes. For long-running temporal programs, a silent wrong value is worse than an informative error.

### ~~Hot reload is partially working~~ — **resolved**
`diff_and_apply` now handles all statement types in a named process body:
- **`on` blocks**: previously skipped entirely; now correctly added to the owning segment's reaction list and redistributed. `on` blocks added, removed, or changed during hot reload take effect on the next tick with no subscription gap.
- **`catch` blocks**: previously skipped; now collected during the scan and linked to segments after pass 2 (same logic as initial setup). Cleared on surviving segments so the new handler replaces the old one atomically.
- **`@` blocks**: previously skipped; now re-executed on hot reload so newly added deferred actions fire.
- **`shared_reactions` stale state**: fixed — `redistribute_reactions` now always clears `shared_reactions` on all segments, not only when shared reactions exist. Previously, removing a multi-segment reaction left stale entries in `shared_reactions`.
- **Newly added temporal segments**: handled by pass 2 (unchanged since before).
- **`dt` change not applying**: fixed — when a segment's temporal function is unchanged but its `dt` is changed (e.g. `counter(dt=0.3s)` → `counter(dt=0.2s)`), the scan pass now re-reads the new `dt` from the AST rather than copying the old value. Pass 1's `dt` update path then reschedules the instance at the correct interval.
- **Spurious side-effect triggers on hot reload**: fixed — a `speculative_exec_` flag suppresses module function calls (e.g. `cs_note`) during the scan pass, so reactions like `a = note(spike!, freq)` do not fire at hot-reload time.
Anonymous-instance `catch` (`catch metro(dt=500ms)::tick: {}`) is not re-created on hot reload (existing limitation — the instance is killed with the old process and not respawned).

### ~~Serial is missing~~ — **resolved**
Serial I/O is now available via `module("serial")`. Exposes `serial_open(port, baud)`, `serial_close(handle)`, `serial_write(handle, data)`, and the native temporal `serial_recv(handle, dt=10ms)` which emits `received` on each incoming chunk. Implemented as a zero-dependency header (`utilities/serial.hpp`) using POSIX `termios` on Linux/macOS and Win32 `CreateFile`/`DCB` on Windows.

### ~~MIDI is missing~~ — **resolved**
MIDI I/O is now available via `module("midi")`. RtMidi is fetched automatically by CMake (`FetchContent`, always latest `master`). No manual installation required; on Linux, ALSA and JACK are auto-detected. Covers all major MIDI 1.0 channel messages: `midi_note_on`, `midi_note_off`, `midi_cc`, `midi_program`, `midi_pitch_bend`, `midi_pressure`, `midi_aftertouch`. Port discovery via `midi_ports_in()` / `midi_ports_out()`. The native temporal `midi_recv(handle, dt=1ms)` polls for incoming messages and emits typed signals (`note_on`, `note_off`, `cc`, `program`, `pitch_bend`, `pressure`) with output flow `[type, channel, data1, data2]`.

### HTML documentation is manually regenerated
`doc/html/` is generated from `doc/*.md` via `build_html.sh` and committed separately. It will drift out of sync whenever the markdown is updated. A CI step or a note in contributing docs would help.

### `process` keyword semantics
The TODO itself notes that `process` doesn't fit the language's vocabulary ("tide", "weave", etc. were considered). This is a pre-alpha window to rename it — after 1.0, renaming a keyword is a breaking change with no easy migration path.

### Flow cursors ~~are global, not per-call-site~~ — **resolved**
Cursors are now per-call-site, keyed by the stable AST node pointer of the `flow_access_expr`. `pattern[m1]` and `pattern[m2]` each own independent cursors and step through the flow at their own rate. Polyphonic use works correctly for static, dynamic, and parametric flows.

### ~~No audio-rate scheduling~~ — **resolved**
An audio-callback-driven scheduler (`audio_clock_scheduler`) is implemented via RtAudio. The audio driver callback increments an atomic tick counter and releases a counting semaphore; a worker thread fires due subscriptions at buffer rate. At 32 frames / 48 kHz this gives ~0.67 ms accuracy. Enabled with `--audio-clock` (optionally `--audio-sample-rate` / `--audio-buffer-size`). Falls back to `sys_clock_scheduler` when not requested.

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

### Temporal function returning an indexed flow

**What it would bring.** A temporal function with a `|>` block can today only
expose a single scalar output. Allowing its return expression to be a flow
literal — `[s, s::phase, amp]` — lets one binding produce multiple related
values without a wrapper:

```idyl
oscil(freq, amp, dt=50ms) = [s, s::phase, amp]
|> {
    s = sine(freq) * amp
}

o = oscil(1hz, 0.8)
send(o[0])       // signal
show(o[1])       // phase
```

The sub-values `s` and `s::phase` are internal names resolved inside the `|>`
block at each tick. The caller receives a snapshot flow indexed 0, 1, 2. No
`flow` keyword required — the `|>` block makes the temporal nature explicit, and
the `[...]` literal in return position makes the collection nature explicit.

**What is consistent about it.** The `|>` block already computes named
sub-variables and exposes one (`= out`). This change simply allows the return
expression to be a flow literal that bundles several. The `::` accessor on
instance state (`s::phase`) already exists. Indexed flow access (`o[0]`) already
exists. Nothing new is introduced — only the output type of a temporal function
is widened from scalar to optionally flow-valued.

**What it could break / open questions.**
- `read_output()` in the evaluator currently returns a scalar `value`. It must
  be widened to return `value_t::flow` when the binding expression evaluates to
  one. Callers of `read_output()` (retick path, reaction snapshot) need to
  handle both cases.
- The flow returned is a static snapshot per tick — no cursors, no per-call-site
  state needed. Each tick the `|>` block fires and the return expression is
  re-evaluated to produce a fresh snapshot. This keeps the cursor ownership
  question entirely out of scope.
- The return expression is evaluated inside the same scope as the `|>` block,
  so internal names (`s`, `phase`) are in scope. No special scoping rule needed.
- The existing temporal-element-inside-flow question (who subscribes `metro`
  inside a flow literal?) does not arise here: the `[...]` in return position
  contains references to already-computed variables, not new temporal
  instantiations.

**Verdict.** Low implementation risk. The evaluator change is contained to
`instantiate_temporal`, `tick_instance` output capture, and `read_output`.
The feature is immediately useful for multi-output signal processors (oscillator
bundles, envelope packages, analyzer outputs). Implement before 1.0.

---

### Temporal function returning a named-member flow

**What it would bring.** Same motivation as indexed flow output, but with
named fields instead of positional indices. A temporal function whose return
expression is a record literal exposes its outputs by name:

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

lfo = multilfo(2hz, 0.5)
send(lfo.sine_lfo)
send(lfo.sq_lfo)
```

Again, no `flow` keyword. The record literal `{ name: expr, ... }` in return
position is the same syntax already used in `flow` definition bodies; it just
appears as the return expression of a temporal function.

**What is consistent about it.** Member access with `.` already works on flow
values. A named-member flow snapshot from a temporal function is accessed the
same way as any other flow member — the caller's code doesn't know or care
whether the flow came from a `flow` definition or from a temporal function's
output.

**What it could break / open questions.**
- The `{ name: expr, ... }` syntax currently only appears inside `flow { }`
  definition bodies and as flow literals. Allowing it in the return position of
  a temporal function definition requires the parser to accept it after `=` when
  a `|>` block follows. This is a grammar extension but should be unambiguous:
  `= { ... } |>` is only parseable one way.
- If the parser currently uses `flow` as the discriminator for "this brace block
  is a record literal and not a process body", that discriminator needs a new
  trigger (e.g., detect `{ name: expr }` by the presence of `name:` at the
  start, which is already how flow literals are distinguished from block scopes).
- The same `read_output()` widening from indexed flow output applies here.
  Named-member flow is a superset of indexed flow from the evaluator's
  perspective — both are `value_t::flow`, just with or without member names.

**Verdict.** Slightly higher parser complexity than indexed output, but
semantically cleaner — field names are self-documenting. Implement after indexed
flow output is stable. Together, the two features cover the full proposal from
`flow_syntax_change.md` without touching the `flow` keyword or affecting
existing code.

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
| ✅ | Fix parser conflicts | Switched to GLR (`glr2.cc`), eliminated all 51 r/r; 16 s/r locked with `%expect 16` |
| ✅ | Flow cursor instancing per call-site — resolved | Each call site owns its cursor; polyphony works correctly |
| ✅ | Catch syntax — resolved | `catch a::sig: {}` implemented; anonymous-instance `catch expr::sig: {}` experimental |
| 🟣 | Functional flow | allow functions to return flows with standard flow instance properties (flow is recomputed if any parameter changes in the function call, flow can be dynamic, and contain temporal elements) |
| 🟢 | Temporal function → indexed flow output | `f(dt=...) = [a, b, c] \|> {}` — `read_output()` widening only, no keyword, no cursor complexity |
| 🟡 | Temporal function → named-member flow output | `f(dt=...) = { x: a, y: b } \|> {}` — same evaluator change, minor parser extension for `{ name: expr }` in return position |
| 🟣 | Code block feature | Allow block of code (expression bloc, like in @ blocs or catch blocks, that can be used to create anonymous scopes, and can be used in ternary expressions `(condition) ? {/*expressions A*/}; {/*Expressions B*/}`|
| ✅ | Serial module | `module("serial")` — zero-dependency, POSIX + Win32 |
| ✅ | MIDI module | `module("midi")` — RtMidi via FetchContent, all major channel messages |
| 🟠 | Runtime error handling | Silent failures in long-running programs are dangerous |
| ✅ | Function definitions in process/lambda scope | Quality of life, code locality |
| ✅ | Hot reload edge cases | `on`/`catch`/`@` blocks, stale `shared_reactions`, `dt` change propagation, spurious module-call triggers — all fixed |
| 🟡 | Consider renaming `process` before 1.0 | Breaking change window is closing |
| ✅ | Audio-rate scheduler mode | `--audio-clock` CLI flag, RtAudio driver callback, sub-ms at small buffers |
| ✅ | Local functions and closures | Implemented: process/init/update scope, by-reference capture |
| 🟢 | Traits | flow traits allowing changing behavior of flow (operator overloading, flow methods etc) |
| ✅ | Ternary syntax — resolved | Condition-first `cond ? a; b` implemented |

