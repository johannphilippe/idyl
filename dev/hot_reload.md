# Hot reload — design notes

## Goal

Allow live-coding: an OSC message (`/idyl/eval`) sends a fragment of idyl source to a running instance, which re-evaluates it without restarting the process. Temporal instances that were not changed keep their state; new or structurally different ones are started fresh.

---

## How idyl currently works (relevant parts)

Understanding what hot reload has to change requires understanding what evaluation does now.

**Global scope evaluation** (`evaluator::run` → `exec_stmt` per statement):
- A `function_definition` stores the AST in `function_defs_[name]` and a `function_ref` in `env_`.
- A zero-param pure definition evaluates eagerly and stores its value in `env_`.
- A `flow_definition` stores the AST in `flow_defs_[name]` or evaluates eagerly.

**Process block execution** (`exec_stmt`, `node_t::process_block`):
- The body is walked once (the "setup loop"). Each temporal call encountered creates a `function_instance` with isolated `current_` / `next_` state and subscribes it to the scheduler.
- All statements after a temporal binding are collected as "reactions" — closures re-executed on every tick of that instance.
- Each instance lives in `instances_[id]` and each named process's instance IDs are tracked in `active_process_instances_[name]`.

**State that matters for hot reload**:
- `function_defs_`, `flow_defs_` — the definition registry
- `env_` — the global binding environment  
- `instances_` — all live temporal instances (hold state in `current_`, `output_`, `emitted_`)
- `active_process_instances_` — maps process name → list of instance IDs it owns
- `stored_processes_` — AST nodes for named process blocks (used by `start_process`)
- `instance_bindings_` — maps variable name → instance ID (for `::` accessor)

---

## OSC entry point

Extend the existing listen-mode OSC dispatcher in `idyl.cpp`. Add a new address:

```
/idyl/eval    s   "<idyl source code>"
```

The body of the message is a string containing valid idyl code (one or more top-level statements). The main thread receives it, queues it, and evaluates it on the main thread (same thread as all other evaluator access — no concurrency concern).

Parsing and evaluation happen on the main thread inside the keep-alive poll loop, exactly like `start`/`stop` commands today.

---

## Categories of incoming code

The incoming fragment can contain three distinct kinds of statements, each with different semantics.

### Category 1 — Global definitions (pure functions, constants, flows)

Examples:
```idyl
square(x) = x * x
freq = 440
flow scale = [60, 62, 64, 67, 69]
```

**New definition** — name is not in `function_defs_` / `env_` yet:
- Simply register as normal. No running instances reference it yet.
- Cost: trivial. Just insert into `function_defs_` / `flow_defs_` / `env_`.

**Redefinition** (name already exists):
- Update `function_defs_[name]` to point at the new AST node.
- Update `env_` binding.
- Existing temporal instances that call this function will pick up the new AST on their **next tick** automatically — `tick_instance` looks up `function_defs_` by `def_name_` on every tick. This is already the correct behavior.
- State inside existing instances is in `current_` / `next_`, keyed by variable name. As long as the variable names in the lambda block are unchanged, state survives. If the user renamed a variable, that slot gets a zero/default on the next tick — acceptable for live coding.
- **Difficulty: low**. The architecture already separates AST lookup from instance state.

**What to avoid**: eagerly re-instantiating temporal instances whose definitions changed. That would reset state and break continuity. The correct approach is: update only `function_defs_`; let running instances pick up the change organically.

### Category 2 — Named process block, not currently running

```idyl
process bass: {
    m = metro(dt=250ms)
    print(m)
}
```

If `stored_processes_` does not have `"bass"`, or `active_process_instances_` does not have `"bass"`:
- Store/overwrite the AST in `stored_processes_["bass"]`.
- If it was not running: no instances to manage. Done.
- Optionally auto-start it (configurable behavior).
- **Difficulty: low**.

### Category 3 — Named process block, currently running (the hard case)

```idyl
process bass: {
    m  = metro(dt=250ms)
    m2 = metro(dt=125ms)   // ← new instance added
    // m3 removed compared to previous version
    print(m, m2)
}
```

This is the core of hot reload for live coding. The goal is:
- Instances that are **unchanged** keep their state (no reinit, no phase reset).
- Instances that are **new** are started fresh.
- Instances that are **removed** are killed (unsubscribed).
- Instances that are **changed** (same variable name, different `dt` or different function) are restarted — the state is lost for that instance only.

**Difficulty: high**. See the diffing section below.

---

## Process diffing

To implement Category 3, the reload mechanism must compare the incoming process AST against the currently running one.

### What defines an instance identity?

The key question: how do we match a running instance to a statement in the new AST?

**Option A — Variable name as identity**
The bound variable name (`m = metro(...)`) uniquely identifies the instance within the process. This is simple, human-readable, and maps directly to `instance_bindings_[varname] → instance_id`.

Matching rule:
- Old `m = metro(dt=500ms)`, new `m = metro(dt=500ms)` → **same**: keep instance, keep state.
- Old `m = metro(dt=500ms)`, new `m = metro(dt=250ms)` → **changed dt**: update `dt_ms_` via `scheduler_->update_dt(id, new_dt)`. State survives.
- Old `m = metro(dt=500ms)`, new `m = lfo(1hz, dt=100ms)` → **different function**: kill old, create new.
- Old `m = metro(...)` exists, new AST has no `m = ...` → **removed**: kill.
- New `m2 = metro(...)` exists, old has no `m2` → **added**: create fresh.

**Option B — Structural hashing**
Hash the call expression AST. Two identical expressions map to the same instance. Fragile to any textual change and harder to implement.

**Recommendation: Option A**. Variable name as identity is natural for a language where every temporal binding has a name, and it matches how users think about live coding ("I renamed my LFO, so it reset — that's expected").

### dt-only changes

A special and common case: only `dt` changes. This is already supported by `scheduler_->update_dt(id, new_dt_ms)` — no state loss, just a reschedule interval change. This should be detected and handled without restarting the instance at all.

### Reaction re-wiring

After diffing instances, the reactions (statements following temporal bindings) must be rebuilt for all surviving and new instances. The old closures captured the old AST and old env scope — they must be replaced by closures over the new AST.

This is a significant structural challenge: reactions are currently baked into scheduler callbacks as captured lambdas. There is no clean way to "re-wire" them without rebuilding the subscription.

**Options**:

1. **Indirect dispatch**: instead of capturing the reaction AST directly in the lambda, store a pointer to a `shared_ptr<vector<stmt_ptr>>` (an indirection layer). Hot reload then replaces the content of that vector. The scheduler callback picks up the new reactions on the next tick.

2. **Rebuild subscriptions**: unsubscribe and re-subscribe all instances in the process with new callbacks. Instances that survived keep their state (`current_`, `output_`) — only the callback closure changes. State continuity is maintained; there is a brief gap (one tick) where the instance runs without reactions.

Option 2 is simpler and more correct for now. The instance's state is preserved across the re-subscription; only the scheduler callback is replaced.

### Process scope / environment

Each process currently runs in a pushed scope (`env_.push_scope()`). On hot reload, the scope for the old process is live (it was kept alive for tick callbacks). The new code needs to run in the same or a compatible scope.

Two sub-options:
- **Reuse the scope**: eval the new statements into the existing scope. Variables that existed carry over; new variables are added. Old variables that no longer appear in the new code linger but are harmless.
- **Fresh scope**: pop the old scope, push a new one. Cleaner but loses any non-instance variable state the user may have set.

For live coding, reusing the scope is more useful. Orphaned variables are a small price.

---

## Implementation plan (phased)

### Phase 1 — Global definition hot swap (low cost, high value)

**What**: handle `/idyl/eval` messages that contain only function/flow/constant definitions (no process blocks).

**Steps**:
1. In `idyl.cpp` listen loop: add `/idyl/eval` OSC command type.
2. On receipt: parse the string (`parse_stream` on a `stringstream`).
3. Run the analyzer on the fragment (optional but recommended — catches arity errors before they corrupt state).
4. Walk the AST statements; for each `function_definition` or `flow_definition`: update `function_defs_` / `flow_defs_` / `env_` in place.
5. For zero-param pure redefinitions: re-evaluate the RHS and update `env_`.

Running instances automatically pick up the new AST on their next tick. No instance management needed.

**Estimated cost**: 1–2 days.

### Phase 2 — Named process reload (medium cost)

**What**: handle `/idyl/eval` with a named process block whose definition is in `stored_processes_`.

**Steps**:
1. Parse and analyze the incoming fragment.
2. For each process block in the fragment:
   a. Store/overwrite the AST in `stored_processes_`.
   b. If NOT currently running → done (will use new AST next time it starts).
   c. If currently running → run the diff algorithm (Phase 3).

**Estimated cost**: depends on Phase 3.

### Phase 3 — Live process diffing (high cost)

**What**: given an old running process and a new process AST, apply minimal changes.

**Steps**:
1. Build a map `old_bindings: varname → (instance_id, call_expr AST, dt_ms)` from `instance_bindings_` + the stored old AST.
2. Walk the new process body to extract `new_bindings: varname → (call_expr AST, dt_ms)`.
3. For each `varname` in `old_bindings`:
   - Present in `new_bindings`, same function, same dt → **keep**: do nothing.
   - Present in `new_bindings`, same function, different dt → **update dt**: call `scheduler_->update_dt(id, new_dt)`.
   - Present in `new_bindings`, different function → **replace**: kill old instance, create new one.
   - Not present in `new_bindings` → **remove**: unsubscribe and erase from `instances_`.
4. For each `varname` in `new_bindings` not in `old_bindings` → **add**: instantiate and subscribe.
5. Rebuild reactions for the whole process (Option 2 above: re-subscribe with new callbacks while keeping surviving instance state).
6. Update `stored_processes_[name]` with new AST.
7. Update `active_process_instances_[name]` with the new set of instance IDs.

**Estimated cost**: 3–5 days. Main complexity is in step 5 (reaction re-wiring) and edge cases (flows referencing instances, `::` accessors, deferred blocks).

---

## Known hard cases and limitations

### Anonymous process blocks

`process: { ... }` has no name, so it can't be targeted by identity. Hot reload cannot apply to anonymous process blocks. They must be named.

### The old AST must be available

The diff in Phase 3 requires walking the old process body to extract variable→call mappings. The old AST is currently available via `stored_processes_[name]` for blocks defined in listen mode. For blocks that were started directly (not via OSC), the AST is not stored in `stored_processes_`. A small addition: store ALL named process ASTs in `stored_processes_` unconditionally during `eval.run()`, even outside listen mode.

### Flow access expressions (`flow[metro]`)

If a temporal instance is used as a flow index (`m = metro(dt=250ms) / print(scale[m])`), the reaction depends on the instance's value not the instance itself. Diffing still works at the variable level; the reaction re-wiring covers the flow access correctly.

### `::` emit accessors

`timer::count` is resolved via `instance_bindings_["timer"] → instance_id → instances_[id].emitted_["count"]`. If the `timer` instance is preserved across a hot reload, the accessor keeps working. If the instance is replaced, `instance_bindings_["timer"]` is updated to the new ID.

### Deferred blocks (`@(500ms): { ... }`)

A `@` block creates a one-shot scheduler subscription. Already-fired blocks are gone; pending ones have captured the old AST. Hot reload cannot retroactively update pending deferred blocks — they will fire with the old code. This is acceptable for live coding.

### Catch blocks

If a catch block's handler changes in the new AST, the re-wiring in Phase 3 updates it (catches are part of the reaction closures). If the emitted variable name changes, the catch is effectively removed — the old catch was keyed on the old variable name.

### Semantic analysis on fragments

Running the 7-pass analyzer on a fragment that references names only defined in the running evaluator is problematic — the analyzer has its own scope and won't see live bindings. Options:
- Skip analysis for hot-reload fragments (risk: arity errors only surface at runtime).
- Run only the passes that don't require global scope (syntax/arity check only).
- Accept that live coding is inherently more permissive.

For Phase 1, skip analysis entirely and let runtime errors surface as warnings. For Phase 3, a lightweight arity check is worthwhile to avoid corrupting the running process.

### Thread safety

All evaluator state (`function_defs_`, `instances_`, `env_`, etc.) is currently accessed from the scheduler thread (for tick callbacks) AND from the main thread (for command dispatch). This is safe today only because the OSC command dispatch (start/stop) happens on the main thread sequentially, and ticker callbacks run on the scheduler thread — these two do not overlap.

Hot reload writes to `function_defs_` from the main thread while ticks may be reading from it. This is currently race-condition-free only because ticks on the scheduler thread look up `function_defs_` via `def_name_`, and map lookups on `unordered_map` are safe to interleave with non-mutating accesses on different keys. However, _inserting or replacing_ entries while a tick reads the map is a data race.

**Required fix for Phase 1**: wrap `function_defs_` and `flow_defs_` updates in a mutex that tick callbacks also hold during lookup. Alternatively, use a `std::shared_mutex` (reader/writer lock) — ticks take a shared read lock, hot reload takes an exclusive write lock.

For Phase 3, the same mutex covers instance creation/destruction.

---

## Summary table

| Case | Difficulty | State preserved | Notes |
|------|-----------|----------------|-------|
| New global definition | Very low | N/A | Just insert |
| Redefine global function | Low | Yes (running instances) | Ticks pick up new AST next tick |
| Redefine global constant | Low | N/A | Re-evaluate RHS |
| Redefine zero-param flow | Low | N/A | Re-evaluate |
| Named process, not running | Low | N/A | Replace stored AST |
| Named process, running — add instance | Medium | Existing kept | Instantiate new |
| Named process, running — remove instance | Medium | N/A | Unsubscribe |
| Named process, running — change dt only | Medium | Yes | `update_dt()` |
| Named process, running — change function | Medium | No (that instance) | Kill + recreate |
| Named process, running — change reactions | High | Yes | Re-wire callbacks |
| Anonymous process | Not supported | — | Requires name |
| Pending `@` deferred blocks | Not supported | — | Fire with old code |
