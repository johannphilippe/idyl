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

---

# Temporal elements inside flows — case map and design

*Added 2026-04-12 — addresses the TODO item:*
> `flow temporal = [sine(5hz, dt=200ms), square(5hz, dt=100ms)]` flows with
> temporal elements should update (they don't).

---

## Root cause of the current bug

Two separate failures occur when a temporal function appears as a flow element:

**1. The flow element captures only the initial output value.**
`eval_flow_literal` calls `eval_expr(elem)` for each element.
`eval_expr` for a temporal function call reaches `instantiate_native_temporal`,
which stores the new `function_instance` in `instances_[id]` and returns
`inst->read_output()` — the initial output as a plain `value`. That value
(a number) lands in `fm.elements_[i]`. The instance pointer is not retained
by the flow.

**2. The temporal instance is never subscribed to the scheduler.**
Scheduler subscription happens only inside the process block reactive chain
setup (evaluator.cpp ~line 369). A temporal call inside a flow literal is
invisible to that code path. The instance sits in `instances_` forever with
`subscription_id_ == 0` — it computes its init value and never ticks again.

Both problems must be solved together.

---

## Complete case map

### Case A — Fully static flow

```idyl
flow melody = [60, 62, 64, 67]
flow scale  = [i = 0..7 : 60 + i * 2]
```

**Elements:** literal numbers or pure arithmetic expressions.
**When computed:** once at definition (top-level) or first evaluation.
**Status:** works today. No changes needed.

---

### Case B — Static parametric flow

```idyl
flow pattern(root) = [root, root + 2, root + 4, root + 7]
p = pattern(60)          // computed once, cached
q = pattern(60)          // cache hit — same flow_data, same cursors
```

**Elements:** expressions over scalar parameters.
**When computed:** once per unique argument combination (flow_call_cache).
**Status:** works (phase 1 from previous plan).

---

### Case C — Dynamic parametric flow (functional arg)

```idyl
flow pattern(i) = [30 + i, 32 - i, 36]
process dyn: {
    m1 = metro(dt=3s)
    m2 = metro(dt=300ms)
    i  = incr[m1]            // i changes every 3s
    print(pattern(i)[m2])    // flow rebuilt when i changes
}
```

**Elements:** expressions over a parameter that is itself a temporal output.
**When computed:** rebuild the entire flow on each tick of `m1` (when `i` changes).
  The rebuilt `flow_data` is stored; `m2` indexing reads the latest version.
**Status:** phase 2 from previous plan — not yet implemented.
**answer** It actually works : see tests/01_pre-alpha/dynamic_simple_flow.idyl or dynamic_flow.idyl
---

### Case D — Direct temporal elements in a flow (NEW)

```idyl
flow oscs = [sine(5hz, dt=200ms), square(5hz, dt=100ms)]
process: {
    m = metro(dt=50ms)
    print(oscs[m])    // should print the current sine or square output
}
```

**Elements:** the element IS a temporal function call — the flow slot is a
live oscillator, not a static number.
**When computed:**
- Each temporal element (sine, square) ticks at its own `dt` independently
  of when the flow is indexed.
- Accessing `oscs[m]` reads the *current output* of whichever instance
  lives at that slot.
- The slots never need rebuilding (assuming no dynamic parameters).
**Status:** BROKEN — initial value only, instance never ticks.
**answer** I think as an implementation note : this kind of flow elements should not "run" when defined, but only when called (instanciated at each different call)
---

### Case E — Multi-member flow with temporal elements

```idyl
flow osc_bank = {
    saw:   [saw(110hz, dt=100ms),  saw(220hz, dt=100ms)]
    pulse: [square(110hz, dt=100ms), square(220hz, dt=100ms)]
}
```

**Same as Case D** but across named members.
**When computed:** each instance ticks independently; member access reads
current outputs of the instances in that member's slots.
**Status:** BROKEN for the same reasons.
**answer**
---

### Case F — Flow element is an *expression* that involves a temporal

```idyl
flow scaled = [sine(2hz, dt=100ms) * 127 + 64, 60]
```

The slot is not directly a temporal instance — it is a compound expression
(`sine(...) * 127 + 64`) whose value changes every tick of sine.

**When computed:** the expression must be re-evaluated every time the sine
ticks (or every time the slot is accessed — see pull vs push below).
**Status:** BROKEN. More complex than case D because the slot cannot simply
hold an instance reference; it holds a computed result that depends on one.
**answer** Should behave similar way as process expressions involving temporal 
---

### Case G — Dynamic parametric flow whose elements are temporal

```idyl
flow lfo_bank(base_freq) = [
    sine(base_freq, dt=100ms),
    square(base_freq * 1.5, dt=100ms)
]
process: {
    m  = metro(dt=2s)
    bf = freq_table[m]        // base_freq changes every 2s
    lf = lfo_bank(bf)         // new temporal instances at new frequencies
    print(lf[metro(dt=50ms)])
}
```

**Double dynamism:**
- When `bf` changes → rebuild the flow, which means *destroying the old
  temporal instances and creating new ones* at the new frequency.
- Between rebuilds, the live instances tick independently.
**When computed:**
- Rebuild + re-instantiate on each tick of `m`.
- Between rebuilds, each instance ticks at its own `dt`.
**Status:** BROKEN on both axes (neither parametric dynamic rebuild nor
temporal elements work).

---

### Case H — Mixed flow: static and temporal slots in the same flow

```idyl
flow mixed = [60, sine(2hz, dt=100ms) * 127, 67, square(3hz, dt=200ms) * 127]
```

Slots 0 and 2 are static numbers. Slots 1 and 3 are live compound
expressions (case F variant).
**When computed:** static slots: never (stored value). Live slots: on each
tick of their respective temporal sources.
**Status:** BROKEN for the live slots.

---

### Case I — Temporal elements inside parametric flow with mixed members

```idyl
flow instrument(offset) = {
    notes: [60 + offset, 62 + offset, 64 + offset]    // static per call
    lfo:   [sine(2hz, dt=50ms), square(3hz, dt=50ms)]  // always live
}
process: {
    m1 = metro(dt=1s)
    o  = offset_table[m1]
    inst = instrument(o)
    print(inst.notes[metro(dt=200ms)])  // rebuilt when o changes
    print(inst.lfo[metro(dt=50ms)])     // always current
}
```

`notes` member: static values that must be rebuilt when `offset` changes.
`lfo` member: live temporal instances that should *not* be rebuilt on `offset`
changes (the frequency is not a parameter).
**When computed:**
- `notes`: rebuilt on each tick of `m1`.
- `lfo`: instances tick continuously; access reads current output.
**Status:** BROKEN for `lfo` member; `notes` member also broken (case C).

---

### Case I — Inline anonymous flow with temporal elements

```idyl
process: {
    m = metro(dt=50ms)
    v = [sine(2hz, dt=100ms), square(3hz, dt=200ms)][m]
}
```

The flow literal is not bound to a name. The `[...][m]` indexing must not
re-instantiate sine/square on every evaluation of `v`.
**When computed:** instances created once; each ticks independently; access
reads current output.
**Status:** BROKEN. Potentially doubly broken if the anonymous flow's
expression is re-evaluated on every tick of `m`.
**answer** Inline should be left for later : we will see if it has sense later, except if you think there is a simple way to integrate it with the rest of flow improvements. 

---

## Evaluation timing summary

| Case | When to build flow | When elements update | When to read |
|------|--------------------|----------------------|--------------|
| A | once at definition | never | static read |
| B | once per arg set (cached) | never | static read |
| C | rebuild on each tick of dynamic param | never | read rebuilt flow |
| D | once | each instance ticks at its own `dt` | read current instance output |
| E | once | same as D | same as D |
| F | once | expr re-eval per tick of temporal in expr | re-eval on access or push |
| G | rebuild on param tick | new instances tick after rebuild | read current output |
| H | once | live slots: per temporal tick; static: never | mixed |
| I | once | lfo member: per tick; notes: on rebuild | mixed |
| I (inline) | once (must be cached / flow-owned) | per instance tick | read current output |

---

## The two sub-problems

Every case above decomposes into exactly these two orthogonal problems:

**P1 — Scheduling:** temporal instances that live inside flow elements must
be registered with the scheduler so they actually tick. Currently only
instances bound as process-block segments get subscriptions.

**P2 — Live reads:** when a flow slot is indexed, the returned value must
reflect the instance's *current* output, not the value captured at flow
creation time. Currently `elements_[i]` is a plain `value` snapshot.

The three design options below address both.

---

## Design Option 1 — `instance_ref` value type (pull, per-slot)

Add a new variant `value_t::instance_ref` that wraps a
`shared_ptr<function_instance>`. Reading this value type (via `as_number()`,
`as_string()`, or any consuming expression) transparently calls
`instance->read_output()`.

```cpp
// core.hpp addition
case value_t::instance_ref:
    // holds: shared_ptr<function_instance> instance_
    // read: delegates to instance_->read_output()
```

**P1 (scheduling):** Introduce an *auto-schedule* flag. When
`instantiate_native_temporal` is called outside a process-block reactive
chain (detected via a `inside_process_block_` flag or by checking whether
`retick_instance_` is null and `current_seg_` is null), automatically
subscribe the instance to the scheduler using its `dt_ms_`. The flow owns
the instance via the `instance_ref` value, keeping it alive.

**P2 (live reads):** At flow indexing (`eval_index_op`), if the element at
the chosen index is `value_t::instance_ref`, return `instance->read_output()`
instead of the stored value. For all other code paths (arithmetic, print),
`as_number()` on an `instance_ref` also delegates — transparent to callers.

**Works for:** D, E, G (after parametric rebuild), H (direct instance slots),
I (anonymous flow must cache itself to avoid re-instantiation — can be keyed
by expression pointer identity).

**Does NOT handle:** Case F (`sine(...) * 127` — the arithmetic collapses the
instance_ref into a plain number at the expression level). Case F requires
Option 2 or a stored expression.

**Pros:**
- Minimal change to `flow_data` — no new data structure, just a new value type.
- Ownership is natural: `flow_data` element vector holds the
  `shared_ptr<function_instance>` through the value.
- Transparent reads: all consumers of `value` work without modification for
  direct-instance slots.
- Scheduling is localised to `instantiate_*`.

**Cons:**
- Every place that switches on `value_t` (print, binops, comparisons) must
  add the `instance_ref` case — a significant but mechanical change.
- Does not solve case F without a companion mechanism.
- Auto-scheduling creates instances that tick even if never accessed.
  (Acceptable — same behaviour as a named temporal binding in a process block.)

---

## Design Option 2 — Live-expression slots (pull, per-slot)

Extend `flow_member` with a parallel vector of optional AST expressions:

```cpp
struct flow_member {
    std::string name_;
    std::vector<value>               elements_;       // static values
    std::vector<parser::expr_ptr>    live_exprs_;     // null = static slot
    // live_exprs_[i] != null → element i is re-evaluated on each access
};
```

When building the flow, detect slots that contain temporal references (same
analysis already used in semantic/analyzer for reactive chain detection). For
those slots, store the AST expression pointer instead of (or alongside) the
evaluated value; `elements_[i]` holds the last-evaluated snapshot as a cache.

At indexing time: if `live_exprs_[i] != null`, call `eval_expr(*live_exprs_[i])`
and return the result. The evaluator's environment must still be in scope —
which means the flow must capture a reference to the evaluator scope at
creation time.

**P1 (scheduling):** temporal instances referenced from live expressions still
need to tick. They must be auto-scheduled (same as Option 1) or the flow
expression must live inside a process block where scheduling happens normally.

**Handles:** all cases including F and H (arbitrary expressions).

**Pros:**
- Handles compound expressions (case F) that Option 1 cannot.
- The `flow_data` does not need a new value type.
- Evaluation is lazy: a slot is only re-evaluated when accessed.
- Naturally handles mixed flows (static slots are zero-cost).

**Cons:**
- `flow_data` now carries AST pointers — it is no longer a pure data
  structure. This breaks the clean separation between parser/AST and runtime.
- Environment capture: the scope at flow-definition time must be kept alive
  so that `eval_expr` can resolve free variables inside the expression.
  The flow must hold a `shared_ptr` to the environment frame.
- Re-evaluation cost: every access to a live slot re-evaluates the expression
  (including calling `instantiate_native_temporal` again). The retick guard
  prevents double-instantiation only during an active scheduler tick — NOT
  during a general `eval_expr` call. A different memoisation mechanism is
  needed to return the existing instance rather than creating a new one.
  **This is the main risk** — getting this wrong causes instance explosion.
- The AST must outlive the flow, which requires the flow to hold shared
  ownership of AST nodes (already `shared_ptr` in the parser, so feasible).

---

## Design Option 3 — Push subscriptions on flow slots (push, background)

When building a flow with temporal elements, register a scheduler subscription
for each temporal instance. On each tick of that instance, update the
corresponding `elements_[i]` value in-place (under a mutex).

```cpp
struct flow_member {
    std::string name_;
    std::vector<value> elements_;          // updated in-place on tick
    mutable std::mutex slots_mutex_;       // guards updates to elements_
};
```

The flow subscribes each temporal element:
```
scheduler->subscribe(dt_ms, [flow_weak, member_idx, slot_idx, inst_weak](...) {
    auto fd = flow_weak.lock(); auto si = inst_weak.lock();
    if (!fd || !si) return false;
    std::lock_guard lock(fd->members_[member_idx].slots_mutex_);
    fd->members_[member_idx].elements_[slot_idx] = si->read_output();
    return true;
});
```

Reading `flow[i]` now just reads `elements_[i]` as today — no change at the
access site. The slots are simply kept current by background pushes.

**Handles:** D, E, H (direct temporal slots), case F with a wrapper lambda
that re-evaluates the compound expression on each tick of its temporal source.

**Pros:**
- Zero cost at read time — flow indexing is unchanged.
- No new value types.
- Natural decoupling: the temporal source pushes at its own rate regardless
  of how often the flow is accessed.

**Cons:**
- The flow is *partially stale*: between ticks, some slots are more up-to-date
  than others. This is philosophically fine but may surprise users.
- Concurrency: the scheduler thread writes, the main thread reads. Requires a
  mutex per member (or per flow). Currently `flow_data` has no locking.
- Each element that is temporal requires a separate scheduler subscription,
  multiplying the number of subscriptions in complex programs.
- For case F (compound expressions), a subscription lambda must close over the
  expression and evaluator — the evaluator is not thread-safe for concurrent
  reads and writes during a scheduler tick.

---

## Recommended approach: staged

### Stage 1 — Option 1 (`instance_ref`) for direct temporal slots

Implement `value_t::instance_ref` for cases D, E, G, and I. This covers the
most common and most requested case (`[sine(...), square(...)]`) with a clean,
additive change.

Mechanical steps:

1. Add `value_t::instance_ref` with `shared_ptr<function_instance> instance_`
   to `value` in `core.hpp`.
2. Make `as_number()`, `as_string()`, `is_truthy()` on `instance_ref` delegate
   to `instance_->read_output()`.
3. Add an `evaluator::last_created_instance_` side-channel: set it to the
   newly created instance inside `instantiate_native_temporal` and
   `instantiate_temporal`. Clear it after `eval_expr` returns in
   `eval_flow_literal`. If non-null, store `value::instance_ref(inst)` instead
   of the plain return value.
4. Auto-schedule: in `instantiate_native_temporal` /`instantiate_temporal`,
   if `scheduler_` is set and `dt_ms > 0` and we are not inside a
   process-block segment (guard via an `inside_segment_ = false` flag), call
   `scheduler_->subscribe(dt_ms, ...)` immediately, exactly as the process
   block code does for its segments. This keeps the instance ticking as long
   as it is alive.
5. Flow indexing: in `eval_index_op`, when a `flow_data` element is
   `instance_ref`, return `instance->read_output()` instead of the element
   value directly.
6. Add `instance_ref` cases to `print`, comparison, and arithmetic helpers.

### Stage 2 — Option 2 (live-expression slots) for compound temporal expressions

Add `live_exprs_` to `flow_member` for cases F and H. Key safeguards:

- Introduce `evaluator::instance_by_call_key_` — a map from
  `(def_name, serialised_args)` to `weak_ptr<function_instance>`. Before
  creating a new instance in `instantiate_*`, check this map. If a live
  instance with the same key exists, return its `instance_ref` (or current
  output). This prevents case F from spawning a new sine on every re-eval.
- An expression is marked as live if the semantic analyser finds any temporal
  function call in its subtree (reuse `analyzer`'s existing temporal-detection
  logic).
- The flow stores a `shared_ptr<environment_frame>` to the scope at creation
  time so that free variables in live expressions resolve correctly.

### Stage 3 — Dynamic parametric flows with temporal elements (Case G)

When a parametric flow is rebuilt because a dynamic parameter changed:
- Unsubscribe and deactivate the old temporal instances from the previous build.
- Instantiate new instances for the new parameter values.
- Store the new `flow_data` (with fresh `instance_ref` elements) as the new
  cached value.

This requires the dynamic-rebuild machinery from phase 2 of the parametric
flow plan, extended to manage instance lifetimes across rebuilds.

---

## Interaction with existing dynamic flow (phase 2)

The phase 2 plan (parametric dynamic flows, Case C) works on the `flow_data`
object as a whole — rebuilding it when the parameter ticks. Temporal elements
(cases D–I) work at the *slot* level within a fixed `flow_data`. The two
mechanisms are orthogonal and compose naturally:

- For Case G: first rebuild the `flow_data` (phase 2 mechanism), then
  populate slots using stage 1 (`instance_ref`) or stage 2 (live expressions).
- The rebuild must destroy old temporal instances (unsubscribe from scheduler,
  clear `instances_[id]`) to avoid accumulating orphaned ticking oscillators.

The clearest invariant: **a `flow_data` object owns its temporal instances**.
When the `flow_data` is replaced (rebuild), the old instances die with it
because the `shared_ptr` in each `instance_ref` value loses its last reference.
The scheduler subscription should hold only a `weak_ptr<function_instance>`,
so the subscription naturally becomes a no-op and returns `false` (causing
the scheduler to unsubscribe) as soon as the `flow_data` drops the instance.
This gives automatic lifetime management without explicit cleanup.

---

## Summary of trade-offs

| | Option 1 (instance_ref) | Option 2 (live-expr) | Option 3 (push) |
|---|---|---|---|
| Cases covered | D, E, G, I | All including F, H | D, E, H (F needs extra work) |
| Read cost | O(1) mutex lock | O(expr eval) | O(1) |
| Write / tick cost | same as any temporal | none extra | per-slot subscription |
| Concurrency | instance has own mutex | evaluator not thread-safe | needs flow mutex |
| AST leaks into runtime | no | yes | no |
| New value types | yes (instance_ref) | no | no |
| Handles compound exprs | no | yes | with extra lambda |
| Ownership clarity | high (flow owns inst) | medium | medium |
| Risk of instance explosion | low | high without key-map | low |
| Recommended stage | 1 | 2 | not recommended |




# Broken still 