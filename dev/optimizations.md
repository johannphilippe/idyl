# Evaluator Performance Analysis & Optimization Roadmap

## Benchmark

```
fibonacci(5) + fibonacci(10) + fibonacci(20)   →   ~123 ms   (Idyl)
                                                     ~0.5 ms   (Python 3)
                                                     ~0.4 ms   (Ruby)
```

Total recursive calls: 22,083 (fib(5)=15, fib(10)=177, fib(20)=21,891).  
Per-call overhead: **~5.6 µs** (Idyl) vs **~0.02 µs** (Python) → ~280× slower.

fib(25) adds 242,785 calls and takes ~1,584 ms → ~6.5 µs/call.  
Overhead is linear in call count, ruling out GC pauses or memory thrashing as primary causes.

---

## Current Architecture (AST-Walker)

`eval_expr` is a large `switch` over `parser::node_t` tags. Every expression
evaluation is a virtual dispatch through a `shared_ptr<node>` plus a
`static_cast` to the concrete node type. Function calls go through
`eval_call` → `eval_user_function`, which does the following on every
invocation:

1. Resolve the function name (env `lookup` linear scan).
2. Evaluate each argument with a recursive `eval_expr`.
3. `env_.push_scope()` — appends a new `scope_frame` to a `std::vector`.
4. Bind every parameter: one `unordered_map::operator[]` per param in the new frame.
5. Evaluate the body expression with another `eval_expr`.
6. `env_.pop_scope()` — destructs the `unordered_map` and its heap allocations.

---

## Bottlenecks

### 1. `shared_ptr` reference counting on every value copy

`value` carries four `shared_ptr` fields:

```cpp
std::shared_ptr<std::string>               string_;
std::shared_ptr<flow_data>                 flow_;
std::shared_ptr<function_instance>         instance_;
std::shared_ptr<parser::function_definition> fn_def_;
std::shared_ptr<function_instance>         closure_inst_;
```

Even `number` and `trigger` values (the hot path for fib) go through copy
constructors that increment/decrement all five ref-counts atomically.
At 22,083 calls, each passing 1–2 numeric arguments, that is tens of
thousands of atomic increments/decrements on the critical path.

**Fix difficulty: Medium.**  
A tagged union (`std::variant` or a manual discriminated union) with `string_`
boxed separately would make numeric copies a plain `double` copy.
Risk: `value` is used in ~300 call sites; the struct change touches every one.

### 2. Scope allocation on every function call

Every call does `env_.push_scope()`, which `emplace_back`s a
`scope_frame { std::unordered_map<std::string, value> }` onto a
`std::vector<scope_frame>`. The `unordered_map` heap-allocates even for a
single-parameter function like `fibonacci(n)`. On `pop_scope`, the map
destructs and frees that allocation.

For 22,083 calls this is 22,083 heap alloc + free cycles just for scopes.

**Fix difficulty: Low–Medium.**  
Possible approaches in increasing complexity:
- **Small-scope optimization**: If the function has ≤ N parameters (e.g. ≤ 4),
  use a flat `std::array<std::pair<std::string_view, value>, N>` on the stack.
- **Arena-per-call**: Use a bump allocator reset between calls.
- **Scope slab**: Pre-allocate a pool of `scope_frame` objects and recycle them.

Risk: Low — scope semantics are well-isolated behind `push_scope`/`pop_scope`.

### 3. `unordered_map` lookup for every variable read

`lookup` walks `scopes_` in reverse and calls `unordered_map::find` at each
level. For a one-scope-deep function body reading `n`, that is one hash
computation plus a bucket probe. For deeply nested calls or closures the walk
is longer.

`unordered_map` is a poor fit here: keys are known at parse time, buckets
add indirection, and load-factor rehashing invalidates all iterators.

**Fix difficulty: Medium.**  
At parse time, assign each local variable an integer slot index. Replace
the `unordered_map` with a `std::vector<value>` indexed by slot. Lookup
becomes `O(1)` with no hashing. This is the standard approach in every
production interpreter (Lua, Python, V8).

### 4. AST traversal and `shared_ptr<node>` indirection

Every `eval_expr` call follows a `shared_ptr` (pointer to the ref-count block,
then pointer to the actual node), does a `static_cast`, and reads the node
fields — all cold memory if the AST does not fit in L1/L2 cache.

For fib(20) the recursion depth is 20 levels; the call tree has 21,891 leaves.
The entire fib AST is tiny and fits in cache, so this bottleneck is smaller
here than in large programs. But in general (many distinct function bodies),
random pointer chasing will dominate.

**Fix difficulty: High if staying on AST-walker; resolved by bytecode.**

### 5. String-keyed function dispatch

`eval_call` looks up `fn_name` (a `std::string`) against:
1. `env_.lookup(fn_name)` — hash of the string.
2. `env_.lookup_builtin(fn_name)` — second hash.
3. `env_.lookup_module_fn(fn_name)` — third hash.
4. `function_defs_` map — fourth hash.

Four hash operations per call for any user-defined function.

**Fix difficulty: Low.**  
After parsing, intern all function names to integer IDs. All four maps become
`unordered_map<int, ...>` — integer hashing is an order of magnitude faster
than string hashing, and the key size drops from 16–32 bytes to 4 bytes.

### 6. `value::as_string()` formatting cost

`as_string()` on a `number` calls `std::to_string` and then trims trailing
zeros with a string search. Not relevant for fib, but `print()` calls this on
every argument every tick — relevant for real programs.

**Fix difficulty: Low.**  
Use `fmt` or `std::format` (C++20) with a custom format spec; avoid the
intermediary `std::string`.

---

## Summary Table

| Bottleneck                    | Impact | Fix Cost | Risk   |
|-------------------------------|--------|----------|--------|
| `shared_ptr` in value copies  | High   | Medium   | Medium |
| Scope heap alloc per call     | High   | Low–Med  | Low    |
| `unordered_map` var lookup    | High   | Medium   | Medium |
| AST pointer chasing           | Medium | High*    | High   |
| String-keyed function dispatch| Medium | Low      | Low    |
| `as_string()` formatting      | Low    | Low      | Low    |

\* Resolved by moving to a bytecode VM.

The first three alone (value copy cost + scope alloc + lookup) likely account
for >80% of the overhead. Fixing them without changing the overall architecture
would probably bring fib(20) from 123 ms to 5–15 ms — still not Python-speed,
but in the same order of magnitude.

---

## Proposed New Architecture: Bytecode VM

### Why bytecode

AST-walkers pay indirection costs on every node visit. A bytecode compiler
flattens the AST once into a dense array of fixed-size instructions; the VM
then loops over that array with no pointer chasing, no virtual dispatch, and
predictable branch patterns for the CPU's branch predictor.

This is how CPython, LuaJIT (before JIT), Ruby (YARV), and every other
production interpreter works. Going from AST-walker to bytecode typically
yields a **5–20× speedup** without changing any language semantics.

### Instruction set sketch

Idyl needs only ~30 opcodes for the static subset:

```
LOAD_CONST  <const_idx>      // push constant from constant pool
LOAD_LOCAL  <slot>           // push value from current frame slot
STORE_LOCAL <slot>           // pop into frame slot
LOAD_GLOBAL <name_id>        // process-level or global variable
STORE_GLOBAL <name_id>
CALL        <fn_id> <argc>   // call by interned function ID
RETURN                       // return top of stack
ADD / SUB / MUL / DIV        // binary arithmetic (in-place on stack top)
NEG                          // unary negate
LT / LE / EQ / NE            // comparisons → push 0.0 or 1.0
JUMP        <offset>         // unconditional
JUMP_IF_ZERO <offset>        // ternary false branch (cond == 0)
EMIT        <name_id>        // emit a signal (temporal)
STOP                         // self-stop
```

### Frame layout

Replace `scope_frame { unordered_map }` with:

```cpp
struct call_frame {
    const bytecode_fn* fn;   // pointer to compiled function
    size_t             ip;   // instruction pointer (index into fn->code)
    size_t             base; // base index into value_stack
};
```

All locals for a call live in a contiguous slice of a pre-allocated
`std::vector<value> value_stack_`. Accessing slot N is
`value_stack_[frame.base + N]` — one addition and one array index,
no hashing.

### Compiler pipeline

```
AST (idyl.y)
  └─ existing parser (keep as-is)
        └─ Bytecode compiler  (new: src/vm/compiler.cpp)
              └─ function_chunk { vector<instruction>, vector<value> consts }
                    └─ VM interpreter  (new: src/vm/vm.cpp)
                          └─ runtime result
```

The compiler is a single-pass AST visitor. It can reuse the existing
`collect_expr_ids` / scope logic to assign slot indices at compile time
(slot assignment = `unordered_map<string, int>` built once per function,
discarded after compilation).

### Temporal layer

The temporal scheduler, `redistribute_reactions`, process instances, and
`on`/`at` machinery can stay exactly as they are. A `reaction` becomes a
compiled `function_chunk` instead of a `vector<stmt_ptr>`; `tick_instance`
calls `vm.run(chunk, frame)` instead of iterating statements.

This is the lowest-risk migration path: replace the hot evaluation loop
without touching any scheduler, segmentation, or timing logic.

### Implementation phases

**Phase 1 — VM + compiler for pure functions only** (2–4 weeks)

- Write `function_chunk` struct and `instruction` enum.
- Write the compiler for the expression subset used by pure functions
  (literals, binary ops, unary ops, ternaries, function calls, identifiers).
- Write a minimal stack VM (`vm::run`).
- Route `eval_user_function` to the VM when the function has been compiled;
  fall back to the AST-walker for anything not yet handled.
- Test: `fib.idyl` should run in under 10 ms.

**Phase 2 — Compiler for process body statements** (2–3 weeks)

- Extend compiler to cover `assignment`, `emit`, `stop`, `on`, `at`.
- Each compiled reaction is a chunk; the scheduler calls `vm::run(chunk)`.
- Keep the AST-walker as fallback for uncompiled constructs.

**Phase 3 — Value representation cleanup** (1–2 weeks)

- Change `value` to a tagged union where `number` and `trigger` hold no
  `shared_ptr` at all.
- String and flow values box their payload with a reference-counted wrapper
  only when needed.
- This eliminates the atomic ref-count traffic on the numeric hot path.

**Phase 4 — Optional: JIT** (months, high risk)

- A JIT (LLVM or copy-and-patch style like CPython 3.13) can push numeric
  loops to near-native speed.
- Not worth doing until Phase 1–3 land; the 5–20× VM speedup is usually
  sufficient for an embedded audio/music language.

### Risk assessment

| Phase | Risk | Mitigation |
|-------|------|-----------|
| 1 (pure fn VM) | Low | AST-walker fallback; pure fns have no side effects |
| 2 (process body) | Medium | Keep AST-walker for temporal constructs until chunk path is verified |
| 3 (value repr) | Medium | Large struct change; good test coverage required |
| 4 (JIT) | High | Requires deep platform expertise; skip unless needed |

The existing test suite (`tests/static/`, `tests/temporal/`) already covers
the cases that matter most. Running both the old and new paths in parallel
during Phase 1–2 and diffing outputs is the safest validation strategy.

---

## Recommended Implementation Sequence

The steps below are ordered so that each one delivers standalone value and
lays the groundwork for the next. None of them require throwing anything away:
the AST-walker stays alive as a fallback throughout.

---

### Step 1 — Fix the `value` representation ✅ *(done)*

**Bottleneck addressed:** `shared_ptr` ref-count traffic on every value copy.

**What was implemented:**

The original `value` struct held five separate `shared_ptr` fields
(`string_`, `flow_`, `instance_`, `fn_def_`, `closure_inst_`) — 80 bytes of
ref-counted pointers on top of the scalar fields, for a total of **112 bytes**
per `value`.

Replaced the five fields with a single `std::shared_ptr<void> payload_` that
type-erases the heap box. For complex types:

- `string` → payload points to a `std::string`
- `flow` → payload points to `flow_data`
- `function` → payload points to a new `function_payload { name_, fn_def_, closure_inst_ }`
- `instance_ref` → payload points to `function_instance`

Scalar types (`number`, `time`, `trigger`, `handle`, `nil`) have
`payload_ == nullptr` — zero heap traffic on copy.

Result: `sizeof(value) == 48` bytes (enforced with a `static_assert`), down
from 112. Smaller structs fit better in cache lines, reducing memory bandwidth
on every value copy. Typed accessors (`str()`, `fn()`, `flow()`, `inst()`) use
`static_cast<T*>(payload_.get())` for zero-overhead recovery.

**Measured gain:** ~15% on the fib benchmark (debug build: 123 ms → 105 ms).
The remaining overhead is scope heap allocation and string-keyed map lookups
(addressed in Steps 2–3).

---

### Step 2 — Intern function and variable names ✅ *(done)*

**Bottleneck addressed:** string hashing in function dispatch and variable lookup.

**What was implemented:**

Added a `string_intern` table inside `environment` that maps each distinct
string to a `uint32_t` ID (starting from 1; 0 is the "not yet interned"
sentinel). Two maps: `unordered_map<string, uint32_t>` forward lookup and a
`vector<string>` reverse table.

Added `mutable uint32_t name_id_ = 0` to `struct identifier` in the AST.
The first time any identifier node is evaluated, `env_.intern(name)` is called
and the returned ID is cached on the node. All subsequent evaluations — which
is the common case for temporal loops executing thousands of times — use the
cached integer directly, paying zero string hashing.

Changed the four hot-path maps from `unordered_map<std::string, …>` to
`unordered_map<uint32_t, …>`:

- `scope_frame::bindings_` (variable lookup on every identifier read)
- `builtin_index_` (checked on every function call)
- `function_defs_` (user-function dispatch)
- `flow_defs_` (parametric-flow dispatch)

In `eval_call`, `fn_name` is interned once per call site (lazily cached on the
AST node too); all four subsequent lookups use the cached `fn_id`.
In `eval_expr` identifier case, `env_.lookup(nid)` replaces `env_.lookup(name)`.

This step also produces the intern-table infrastructure the VM compiler needs:
`CALL fn_id` and `LOAD_GLOBAL name_id` instructions will use IDs from this
same table.

**Measured gain:** the release build was already at ~7 ms for fib(20)
(ms-resolution `now()` makes sub-ms improvements invisible here). The gain
shows up in programs with many distinct identifiers and long names, and in
debug builds where string hashing is a larger fraction of total cost.
The primary value is as a low-risk prerequisite for Step 3.

---

### Step 3 — VM core: pure function compilation ✅ *(done)*

**Bottlenecks addressed:** scope heap allocation, `unordered_map` var lookup,
AST pointer chasing — all at once, for pure functions.

**What was implemented:**

New `src/vm/` directory with four files:

- `instruction.hpp` — `opcode` enum (25 opcodes) + 12-byte `instruction {op, a, b, c}` struct
- `chunk.hpp` + `compiler.hpp/.cpp` — single-pass AST→bytecode compiler; assigns each param a slot index at compile time (slot map discarded after compilation); falls back silently (returns `nullptr`) for unsupported constructs (lambda blocks, flows, N≠2 ternary, module access)
- `vm.hpp/.cpp` — iterative dispatch loop over a shared `vector<value> stack_` and `vector<call_frame> frames_`; each frame holds `{fn*, ip, base}`; local access is `stack_[base + slot]` — no hashing, no heap allocation per call

**Integration:** `evaluator::try_compile()` attempts compilation on every function definition; compiled chunks are stored in `vm_` (keyed by intern ID). `eval_user_function` fast-paths to `vm_.run()` when a chunk exists and the VM is not already active (re-entrancy guard via `active_` flag). Uncompilable functions fall back transparently to the AST-walker.

**Key subtlety — Idyl ternary semantics:** `cond ? A ; B` uses `cond.as_number()` as an index selector (0→A, 1→B), not a boolean branch. The compiler emits `JUMP_IF_FALSY` to skip `options_[1]` (truthy path) and fall through to `options_[0]` (falsy path), matching the evaluator's `eval_expr(options_[idx])` semantics exactly.

**Measured gain:** release build fib(20) dropped from ~7 ms → **1–2 ms** (~5–7× speedup). All 99 tests pass.

---

### Step 4 — Extend compiler to the full expression set *(1–2 weeks, Low risk)*

**Bottleneck addressed:** remaining AST-walker overhead for non-trivial expressions.

With the VM infrastructure from Step 3 in place, add the remaining expression
types one at a time, each verified by a static test:

- Block expressions (`BLOCK_ENTER`/`BLOCK_EXIT` scope markers, or just inline
  since slots are pre-assigned).
- Flow access (`FLOW_INDEX` opcode backed by the existing `flow_data` machinery).
- Memory operator (`DELAY` — can delegate to the existing delay buffer logic).
- Lambda bodies inside temporal function definitions.

At the end of this step every pure function and every static expression
compiles. The AST-walker is only invoked for anything in a process body.

---

### Step 5 — Compile process body reactions *(2–3 weeks, Medium risk)*

**Bottleneck addressed:** AST-walker overhead in the temporal hot path (per-tick
reaction evaluation).

Extend the compiler to cover process body statement types:
`STORE_GLOBAL` (assignment), `EMIT`, `STOP`, `LOAD_GLOBAL`.
Each reaction list (a `vector<stmt_ptr>`) becomes a compiled
`function_chunk`; the scheduler calls `vm::run(chunk)` instead of
iterating statements.

The temporal layer — `redistribute_reactions`, `live_segment`, `tick_instance`,
the scheduler thread — stays completely unchanged. Only the inner evaluation
call is swapped.

**Risk:** process body reactions read and write process-scoped variables that
live in the global env, not in a local frame. `LOAD_GLOBAL`/`STORE_GLOBAL`
still do a name-ID keyed lookup (using the intern table from Step 2), but now
into a flat `unordered_map<name_id_t, value>` instead of the layered scope
chain — one lookup instead of N-deep chain walk.

**Validation:** run the full `tests/temporal/` suite under both paths.
The AST-walker fallback makes it safe to flip reactions to the VM one process
block at a time.

---

### Expected cumulative speedup

Note: baseline was measured in a Debug build; the Release build compiles away
much of the interpreter overhead already. Release numbers are given where known.

| After step | fib(20) Debug est. | fib(20) Release measured | Dominant remaining cost |
|------------|--------------------|--------------------------|------------------------|
| Baseline   | ~123 ms            | ~10–12 ms (est.)         | everything              |
| Step 1     | ~105 ms ✅          | ~7–8 ms ✅               | scope alloc + lookup    |
| Step 2     | ~90–100 ms         | ~6–7 ms ✅               | scope alloc + lookup    |
| Step 3     | ~10–20 ms          | ~1–2 ms ✅               | AST-walker for processes|
| Step 4     | ~10–20 ms          | ~1–3 ms                  | (same — fib is pure)   |
| Step 5     | ~10–20 ms          | ~1–3 ms                  | (same — fib is pure)   |

For temporal workloads (counters, reactions firing every tick), Steps 4–5
matter most: they remove the per-tick AST-traversal cost that accumulates over
thousands of ticks.

The natural stopping point is after Step 3: fib-class code is fast, the
architecture is clean, and Steps 4–5 can be done opportunistically as the
language grows.
