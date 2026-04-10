# Parametric flows — design & implementation plan

## Current state

`flow_definition` in the AST already stores `parameters_`. The grammar parses
`flow pattern(i) = [...]` correctly. The evaluator ignores parameters entirely —
it evaluates all elements at definition time and stores the result as a static
`flow_data` value. Parameters are never pushed into scope before evaluation.

---

## Two cases to implement

### 1. Static parametric flows

`pattern(12)` — the argument is a plain number (or any non-temporal expression).

The flow does not exist as a pre-built value. When called, it must build and
return a `flow_data`. Required changes:

- `flow_definition` **with parameters** must be stored as a **callable** (a
  function reference pointing at the definition), not a pre-built flow value.
- `flow_definition` **without parameters** keeps current behavior (evaluate
  immediately, store as flow value).
- At call sites, `function_call_expr` evaluation must recognise that the callee
  is a flow definition and build the flow with the supplied arguments pushed
  into scope.

Difficulty: **Low** (~20–30 lines in the evaluator).
Grammar changes: **none**.
Semantic analysis: arity check for flow calls (same path as function arity).

### 2. Dynamic parametric flows

`pattern(i)` where `i` is the output of a temporal function.

**Semantic decision (settled):** the flow is recomputed at **`i`'s rate**. If
`m` (the index into the flow) ticks but `i` has not changed, the same
`flow_data` object is still valid — no rebuild needed. The result seen by the
indexing expression (`pattern(i)[m]`) is always the most recently built flow,
indexed by the current value of `m`.

Required changes beyond the static case:

- Detect that a flow-call argument references a temporal value (same detection
  path used for reactive chain assignment in process blocks).
- On each tick of the temporal source driving `i`, rebuild the `flow_data` and
  update the binding in the environment.
- The rebuilt flow must be available to downstream expressions (e.g. `[m]`
  indexing) that may tick at a different rate.

Difficulty: **Medium–High**. The main complexity is hooking flow rebuilds into
the tick machinery without creating a full temporal instance. A lightweight
"reactive binding" — a callback registered on the source instance that updates
a flow value in scope — is the natural extension point.

---

## Pros

- Flows become genuinely data-driven, not just lookup tables.
- Grammar is already valid — no parser changes at any stage.
- Static case unblocks most practical use cases immediately (transposing,
  scaling, inverting patterns via parameters).
- Dynamic case eliminates the need for lambda workarounds to rebuild patterns
  on the fly.

## Cons / risks

- Dynamic case: `flow_data` is rebuilt on every tick of `i`. For large flows at
  a fast `dt` this can be expensive — worth documenting as a user concern.
- Implicit distinction between static and dynamic: the same syntax `pattern(i)`
  behaves differently depending on whether `i` is temporal. This is consistent
  with how temporal function calls already work in the language, but should be
  documented clearly.
- `pat.notes[m]` vs `pat[m].notes` — two valid syntaxes for member-then-index
  and index-then-member. Both must stay consistent after dynamic rebuilds.

---

## Implementation plan

### Phase 1 — static (implement now)

1. In `exec_stmt` for `flow_definition`: if `parameters_` is non-empty, store
   the definition as a callable (`value::function_ref`) rather than evaluating
   immediately. Register the `flow_definition` node in the function table under
   its name (same table used for `function_definition`).

2. In `eval_expr` for `function_call_expr`: when the resolved callee is a
   `flow_definition` node, push arguments into scope (matching parameters by
   position/name, same logic as `eval_function_call`), evaluate the flow
   members, pop scope, and return the resulting `flow_data` as a `value`.

3. Semantic analysis: the flow arity checker should treat parametric flow calls
   the same as function calls (already partially wired — verify and extend if
   needed).

### Phase 2 — dynamic (later)

4. In the process block reactive chain builder: detect flow-call expressions
   whose arguments contain temporal references. For each such expression,
   register a lightweight reactive binding on the source instance that rebuilds
   the flow and updates the environment binding on each tick of `i`.

5. The binding stores the last-built `flow_data` so that expressions ticking at
   `m`'s rate (which may be faster than `i`'s rate) always read the current
   value without triggering a rebuild.

---

## Open questions (phase 2 only)

- Should a dynamic flow call in a reactive chain be written identically to a
  static one, or should there be a visual cue? Current answer: identical — the
  evaluator detects temporality automatically, consistent with the rest of the
  language.
- Default value when `i` has not yet produced its first tick: return a nil flow
  or an empty flow? Probably an empty flow to avoid crashing indexing.
