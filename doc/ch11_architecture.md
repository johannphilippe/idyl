# 11 — Architecture

[← Back to index](idyl_documentation.md) · [Previous: Modules & libraries](ch10_modules_libraries.md)

---

This chapter describes the internal pipeline of the Idƴl implementation. It is intended for contributors and anyone interested in how the language works under the hood.

---

## Pipeline overview

```
Source (.idyl)
    │
    ▼
┌─────────┐
│  Lexer   │   Flex — tokenises input
└────┬─────┘
     │ tokens
     ▼
┌─────────┐
│  Parser  │   Bison — builds AST
└────┬─────┘
     │ AST
     ▼
┌──────────────────┐
│ Semantic Analyzer │   2 passes over the AST
└────┬─────────────┘
     │ validated AST
     ▼
┌──────────────────────────────────────────┐
│                Evaluator                  │
│                                           │
│  ┌──────────┐     ┌────────────────────┐ │
│  │ Compiler │────▶│   Bytecode VM      │ │
│  └──────────┘     │  (pure functions,  │ │
│                   │   reaction lists)  │ │
│                   └────────────────────┘ │
│  ┌────────────────────────────────────┐  │
│  │   AST-walking interpreter          │  │
│  │   (temporal, flow, control flow,   │  │
│  │    fallback for uncompiled fns)    │  │
│  └────────────────────────────────────┘  │
└──────────────────────────────────────────┘
     │
     ├── Temporal instances → Scheduler (system clock)
     ├── Module calls → OSC / Csound / external modules
     └── Process blocks → reactive execution
```

---

## Lexer (Flex)

The lexer is defined in `src/parser/idyl.l`. It recognises:

- **Keywords**: `process`, `flow`, `init`, `emit`, `catch`, `import`, `module`, `on`, `each`, `in`, `start`, `stop`, `clock`, `tempo`
- **Literals**: numbers, time suffixes (`ms`, `s`, `hz`, `b`, `bpm`), strings, triggers (`!`), rest (`_`)
- **Operators**: `|>` (lambda), `::` (namespace/emit access), `..` (range), `@` (deferred block), `'` (delay/memory), arithmetic, comparison, bitwise
- **Structural tokens**: braces, brackets, commas, colons, semicolons, equals, pipe bars `|` (repeat markers)

The lexer produces tokens consumed by the Bison parser.

---

## Parser (Bison)

The parser is defined in `src/parser/idyl.y`. It builds an **Abstract Syntax Tree** (AST) from token streams. All node types are defined in `src/parser/ast.hpp` under the `node_t` enum:

**Literals**

| Node type | Represents |
|-----------|------------|
| `number_literal` | Numeric value |
| `time_literal` | Time value with unit (`100ms`, `1s`, `440hz`, `2b`, `120bpm`) |
| `string_literal` | Quoted string |
| `trigger_literal` | `!` trigger value |
| `rest_literal` | `_` rest value |

**Expressions**

| Node type | Represents |
|-----------|------------|
| `identifier` | Variable reference |
| `binary_op` | Binary operation (`+`, `-`, `*`, `/`, `%`, bitwise, comparison) |
| `unary_op` | Unary negation or bitwise NOT |
| `ternary_op` | `cond ? a; b` — 2-way or multi-way selection |
| `memory_op` | `'(expr)` or `'(expr, N)` — sample-based circular delay |
| `flow_literal` | `[1, 2, 3]` — simple flow literal |
| `generator_expr` | `[x in N : expr]` / `[x in S..E : expr]` / `[x in S..E..ST : expr]` — flow generator (count, range, range+step) |
| `repetition_marker` | `\|N\|` — repeat bar; expands the preceding element or group N times in the physical table |
| `flow_member` | Named member inside `flow { }`: `name [on gate]: [...]` |
| `function_call` | `f(args)` |
| `flow_access` | `flow[index]` — trigger, integer, or float indexing |
| `module_access` | `lib::name` — namespace access or emit accessor `instance::event` |
| `memory_op_expr` | Wrapper expression node for `memory_op` |
| `self_stop_expr` | `stop` inside a temporal lambda body — terminates the instance |
| `block_expr` | Anonymous block `{ stmt; …; expr }` |

**Definitions**

| Node type | Represents |
|-----------|------------|
| `function_definition` | `f(params) = body` — pure or temporal (with `\|> { }` lambda) |
| `flow_definition` | `flow name = { member: [...] }` — global named-member flow |
| `init_block` | `init: { … }` inside a temporal lambda |
| `lambda_block` | `\|> { init: { } update-stmts }` — temporal update body |
| `parameter` | Single function parameter (name, optional default, trigger flag) |
| `argument` | Single call argument (positional or named) |

**Statements**

| Node type | Represents |
|-----------|------------|
| `assignment` | `x = expr` |
| `expression_stmt` | Bare function call (side-effect without assignment) |
| `catch_block` | `catch instance::event: { … }` |
| `at_block` | `@(time): { … }` — one-shot deferred execution block |
| `on_block` | `on expr: { … }` — trigger-gated reaction block |
| `each_block` | `each n in count [, dt=time]: { … }` — counted loop |
| `stop_statement` | `stop [name]` — stop current or named process |
| `start_statement` | `start name` — start a named process |
| `process_block` | `process [name] [, dur=…]: { … }` |
| `library_import` | `import("path")` |
| `module_import` | `module("name")` |

---

## Semantic analyzer

The semantic analyzer (`src/semantic/analyzer.cpp`) runs **2 passes** over the AST before any code executes.

### Pass 1 — `global_scope_pass`

A single forward scan of all top-level statements. In order:

1. **Library imports** — `import("path")` calls are resolved, the file is loaded and parsed, circular imports are detected, and library symbols are registered (namespaced if the import is named).
2. **Module imports** — `module("name")` directives are collected and validated against known built-in module names.
3. **Top-level binding** — every top-level function definition, flow definition, and process block is entered into the global scope so that mutual references resolve correctly in pass 2.

### Pass 2 — `resolve`

A recursive descent over every node in the AST. For each node type it performs the checks relevant to that construct:

- **Name resolution** — every identifier is verified against the scope chain; undefined names produce an error.
- **Arity checking** — call argument counts are checked against the declared parameter list.
- **Temporal validation** — temporal functions must have a `dt=` or trigger parameter; `stop` may only appear inside a lambda update body; `emit` names are recorded.
- **Flow validation** — flow member names are unique; gated members (`on`) reference a member that appears before them.
- **Scope structure** — `catch`, `each`, `on`, and `@` blocks are validated for correct nesting and consistent emit/catch pairing.

Both passes use the **scope system** (`src/semantic/scope.hpp`) and **symbol table** (`src/semantic/symbol.hpp`). All diagnostics are collected and reported before evaluation begins — no code runs if the program contains errors.

---

## Evaluator

The evaluator (`src/core/evaluator.cpp`) is a **tree-walking interpreter**. It traverses the AST and executes nodes directly.

### Key components

| Component | File | Purpose |
|-----------|------|---------|
| `Environment` | `src/core/environment.hpp` | Runtime scope chain for variable lookup |
| `function_defs_` | `src/core/evaluator.hpp` | Map of interned name ID → AST function definition |
| `fn_library_scope_` | `src/core/evaluator.hpp` | Map of qualified name → library-local scope (for namespaced imports) |
| `instances_` | `src/core/evaluator.hpp` | Map of instance ID → live `function_instance` (temporal functions) |
| `live_processes_` | `src/core/evaluator.hpp` | Map of process name → `live_process` (segments, reactions, catch blocks) |
| `stored_processes_` | `src/core/evaluator.hpp` | Map of process name → AST node (for listen mode) |
| `flow_cache_` | `src/core/evaluator.hpp` | Cache of (flow name, args) → built flow value (for parametric flows) |
| `Scheduler` | `src/time/scheduler.hpp` | Drift-free timer management |
| `ClockRegistry` | `src/core/core.hpp` | Clock hierarchy and tempo propagation |
| `ModuleRegistry` | `src/include/module.hpp` | Lazy-loaded module catalog and handles |
| `vm_` | `src/vm/vm.hpp` | Bytecode VM for compiled pure functions and reaction lists |

---

### The scope system

At runtime, the environment holds a **stack of scope frames** (`std::vector<scope_frame>`). Each frame is a flat name → value map. Lookup walks the stack from innermost (top) to outermost (bottom).

```
┌─────────────────────────────┐  ← innermost (current function / tick)
│  phase = 0.0                │
│  out   = 0.309              │
├─────────────────────────────┤
│  [library-local scope]      │  ← pushed for namespaced library functions
│  sine_shape → std::sine_shape│
│  lfo        → std::lfo      │
│  ...                        │
├─────────────────────────────┤
│  [process block scope]      │
│  l = <temporal instance>    │
├─────────────────────────────┤
│  [global scope]             │  ← outermost, always present
│  pi, tau, euler, ...        │
│  std::sine, std::lfo, ...   │
└─────────────────────────────┘
```

Built-in math functions and module functions are not in the scope stack — they are looked up via separate indices (`builtin_index_` and `module_registry_->lookup()`) only after the scope chain is exhausted.

#### When scopes are pushed and popped

| Event | Scope pushed | Scope popped |
|---|---|---|
| Process block execution | once at entry | when block ends (or never, if temporal) |
| Pure function call | once for parameters | when call returns |
| Temporal init block | once for parameters | after init statements run |
| Temporal first-pass | once for params + init state | after output expression |
| Scheduler tick (`tick_instance`) | once for params + current state | after output expression |
| Namespaced library function (any of the above) | **also** one extra scope for library-local names | paired with the one above |

#### Library-local scopes for namespaced imports

When a library is loaded under a namespace (e.g. `std = import("stdlib")`), two things happen:

1. **Global registration**: every function is stored in `function_defs_` under its qualified name (`"std::lfo"`, `"std::sine"`, ...). Bare names are never added to the global scope.

2. **Library-local scope construction**: a shared `lib_scope` map is built, containing every original (unqualified) name in the library mapped to a `function_ref` pointing at its qualified counterpart:

   ```
   lib_scope = {
       "lfo"        → function_ref("std::lfo"),
       "sine_shape" → function_ref("std::sine_shape"),
       "tri_shape"  → function_ref("std::tri_shape"),
       ...
   }
   ```

   This scope is stored in `fn_library_scope_` keyed by the qualified name, and attached to each `function_instance` when a temporal function from that library is instantiated.

3. **Injection at call time**: before evaluating a namespaced function's body or tick, the library-local scope is pushed as an extra frame. The function body (e.g. `lfo`'s update block calling `sine_shape`) finds `"sine_shape"` in that frame, gets back `function_ref("std::sine_shape")`, and the call resolves correctly. The frame is popped immediately after.

This design means:
- Internal library cross-calls work without qualification.
- Bare names never appear in the caller's global scope.
- Two libraries loaded under different namespaces can have overlapping internal names without conflict.

---

### Evaluation model

1. **Constants and functions** are evaluated once and stored in `function_defs_` and the global scope.
2. **Temporal lambdas** are instantiated: their init blocks run, and they are registered with the scheduler at their `dt` interval.
3. **Reactive bindings** subscribe to the temporal function's output — each scheduler tick re-evaluates the binding expression with the updated value.
4. **Process blocks** orchestrate the above — each block is an independent execution context.

### Temporal instantiation

When the evaluator encounters a temporal function call (e.g. `std::lfo(1hz, dt=50ms)`):

1. Resolve the call to a `function_definition` and its **qualified key** in `function_defs_`.
2. Create a `function_instance`. Set `def_name_` to the qualified key (e.g. `"std::lfo"`) so the process-block subscription lookup finds the right definition.
3. Attach `library_scope_` from `fn_library_scope_["std::lfo"]` (if present).
4. Bind parameters (positional, named, defaults). Extract `dt_ms`.
5. Run the `init` block in a temporary scope (with library-local scope if present) — initialise `current_` state.
6. Run a **first pass** of the update statements, skipping variables that were set by `init`. This seeds the output (e.g. `out = sin(0) = 0`) without overwriting init values.
7. Store the instance in `instances_`. Return its initial output.

The process block then subscribes the instance to the scheduler. On each tick, `tick_instance` is called, which:

1. Pushes the library-local scope (if present) as an extra frame.
2. Pushes a scope with the instance's current parameters and state.
3. Evaluates each update statement in order; writes go to `next_`.
4. Evaluates the output expression against the updated scope.
5. Pops both scopes.
6. Commits `next_` → `current_` (double-buffer swap).

### The scheduler

The scheduler is **drift-free**: it tracks elapsed time against the system clock rather than accumulating intervals. This prevents timing drift over long runs.

Each tick:

1. Compute actual elapsed time since start.
2. For each registered lambda, check if its next fire time has passed.
3. If so, call `tick_instance`, re-evaluate the bound variable expression, then run any downstream reactions.
4. Advance the next fire time by exactly `dt`.

The scheduler runs in a tight loop with a configurable sleep granularity to balance CPU usage against timing precision.

---

## Bytecode VM

The bytecode VM (`src/vm/`) accelerates execution of pure functions and process-block reaction lists. It sits alongside the AST-walking evaluator: the evaluator attempts to compile eligible code at definition time and dispatches to the VM at call time, falling back to AST-walking for anything the compiler does not support.

### Components

| File | Purpose |
|------|---------|
| `src/vm/instruction.hpp` | `opcode` enum and `instruction` struct |
| `src/vm/chunk.hpp` | `bytecode_fn` — one compiled unit (instruction array + constant pool) |
| `src/vm/compiler.hpp/cpp` | AST → bytecode compiler |
| `src/vm/vm.hpp/cpp` | Stack-based VM execution engine |

### Instruction format

Each `instruction` is a fixed-width 8-byte struct:

```
┌────────┬────────┬────────┬──────────────────────┐
│  op    │   a    │   b    │          c            │
│ uint8  │ uint16 │ uint16 │        int32          │
└────────┴────────┴────────┴──────────────────────┘
 opcode   slot /   builtin   fn_id (CALL) /
          argc /   idx       jump offset (JUMP) /
          const-             reserved
          idx
```

### Instruction set

| Category | Opcodes |
|----------|---------|
| Loads / stores | `LOAD_CONST`, `LOAD_LOCAL`, `STORE_LOCAL`, `LOAD_NIL`, `POP`, `LOAD_GLOBAL`, `STORE_GLOBAL` |
| Arithmetic | `ADD`, `SUB`, `MUL`, `DIV`, `MOD` |
| Unary | `NEG`, `NOT_OP` |
| Comparison | `LT`, `LE`, `GT`, `GE`, `EQ`, `NE` |
| Logical | `AND`, `OR` |
| String | `CONCAT` |
| Flow | `FLOW_INDEX` — pop index and flow, push the element at the wrapped physical slot |
| Control | `JUMP` (unconditional, relative offset), `JUMP_IF_FALSY` (pop condition, branch) |
| Calls | `CALL` (compiled user function by fn_id), `CALL_NATIVE` (built-in by index) |
| Return | `RETURN` |

### Compilation scope

**What gets compiled:**

- **Pure user functions** (`function_definition` with no lambda block, no named arguments, supported expression types) — compiled at the moment the function is registered, before any process block runs.
- **Reaction lists** — the statement lists inside `on` blocks that fire on every temporal tick. Variables are accessed via `LOAD_GLOBAL`/`STORE_GLOBAL` because reactions operate on the live process environment.

**What falls back to AST-walking:**

- Temporal functions (lambda blocks, `init`, state update)
- `each` loops, `@` deferred blocks, `catch` blocks, `start`/`stop`
- Functions with named arguments or unsupported AST nodes (flow literals, module calls, etc.)
- Any call made while the VM is already active (re-entrancy guard — the VM is not re-entrant)

### Integration with the evaluator

```
evaluator::register_function(def)
    └── try_compile(def, fn_id)          ← compiler returns bytecode_fn or nullptr
            └── vm_.store(fn_id, chunk)

evaluator::eval_user_function(key, args)
    ├── if vm_.has_compiled(fn_id)
    │       └── vm_.run(fn_id, args)     ← fast path
    └── else
            └── ast_walk(def, args)      ← fallback

scheduler tick → reaction fires
    ├── if rxn.compiled_reactions
    │       └── vm_.run_reactions(chunk) ← fast path
    └── else
            └── ast_walk(reactions)      ← fallback
```

On hot-reload, affected function chunks and reaction chunks are recompiled automatically before the next tick.

---

## Trace mode

The `--trace` flag enables diagnostic output during evaluation:

```bash
idyl program.idyl --trace
```

This prints:
- Each scheduler tick with timestamps
- Temporal function evaluations and their outputs
- OSC messages sent
- Clock tempo changes

Useful for debugging timing issues and understanding program behavior.

---

## Comparable systems

Idƴl draws inspiration from several domains:

| System | Shared concept |
|--------|---------------|
| **Csound** | The most fully featured musical programming language |
| **Faust** | Functional signal processing |
| **Cordelia** | Live coding tenderness |
| **Lua** | Flexible and elegant general purpose language  |
| **Antescofo** | Timed, reactive programming language |
| **ChucK** | Strongly-timed audio programming |

---

[← Back to index](idyl_documentation.md)
