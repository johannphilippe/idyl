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
│ Semantic Analyzer │   7 passes over the AST
└────┬─────────────┘
     │ validated AST
     ▼
┌───────────┐
│ Evaluator  │   Tree-walking interpreter + scheduler
└───────────┘
```

---

## Lexer (Flex)

The lexer is defined in `src/parser/idyl.l`. It recognises:

- Keywords: `process`, `init`, `emit`, `catch`, `import`, `module`, `clock`, `tempo`
- Literals: numbers, time suffixes (`ms`, `s`, `hz`, `b`, `bpm`), strings, triggers (`!`), rest (`_`)
- Operators: `|>`, `::`, arithmetic, comparison, bitwise
- Structural tokens: braces, brackets, commas, colons, equals

The lexer produces tokens consumed by the Bison parser.

---

## Parser (Bison)

The parser is defined in `src/parser/idyl.y`. It builds an **Abstract Syntax Tree** (AST) from token streams. Key AST node types (defined in `src/parser/ast.hpp`):

| Node type | Represents |
|-----------|------------|
| `NumberLiteral` | Numeric value |
| `TimeLiteral` | Time value with unit |
| `StringLiteral` | Quoted string |
| `TriggerLiteral` | `!` trigger value |
| `RestLiteral` | `_` rest value |
| `Identifier` | Variable reference |
| `BinaryExpr` | Binary operation |
| `UnaryExpr` | Unary operation |
| `TernaryExpr` | 2-way ternary |
| `MultiTernaryExpr` | Multi-way ternary |
| `FunctionCall` | `f(args)` |
| `FunctionDef` | `f(params) = body` |
| `TemporalLambda` | `f(dt=...) = v \|> { ... }` |
| `FlowLiteral` | `[1, 2, 3]` |
| `GeneratorExpr` | `[x = 0..n : expr]` |
| `ProcessBlock` | `process name: { ... }` |
| `InitBlock` | `init: { ... }` |
| `EmitStmt` | `emit name = expr` |
| `CatchBlock` | `instance catch name: { ... }` |
| `EmitAccess` | `instance::name` |
| `Assignment` | `x = expr` |
| `LibImport` | `import("path")` |
| `ModuleImport` | `module("name")` |

---

## Semantic analyzer

The semantic analyzer (`src/semantic/analyzer.cpp`) performs **7 ordered passes**:

### Pass 1 — Library collection
Collects all `import()` calls, resolves paths, detects circular dependencies.

### Pass 2 — Module collection
Collects `module()` imports, validates module names.

### Pass 3 — Top-level binding
Registers all top-level names (functions, constants, process blocks) into the global scope.

### Pass 4 — Function analysis
Validates function definitions: parameter counts, default values, return types. Checks for recursion and undefined references.

### Pass 5 — Temporal analysis
Validates temporal lambdas: ensures `dt=` or trigger parameters are present, checks init blocks, validates state variable usage.

### Pass 6 — Flow analysis
Validates flow literals and generators: checks bounds, member names, access patterns.

### Pass 7 — Process block analysis
Validates process block contents: ensures temporal functions are properly instantiated, checks emit/catch consistency, validates duration expressions.

Each pass uses the **scope system** (`src/semantic/scope.hpp`) and **symbol table** (`src/semantic/symbol.hpp`) to track declarations and detect errors before evaluation begins.

---

## Evaluator

The evaluator (`src/core/evaluator.cpp`) is a **tree-walking interpreter**. It traverses the AST and executes nodes directly.

### Key components

| Component | File | Purpose |
|-----------|------|---------|
| `Environment` | `src/core/environment.hpp` | Runtime scope chain for variable lookup |
| `function_defs_` | `src/core/evaluator.hpp` | Map of function name → AST definition node |
| `fn_library_scope_` | `src/core/evaluator.hpp` | Map of qualified name → library-local scope |
| `Scheduler` | `src/time/scheduler.hpp` | Drift-free timer management |
| `ClockRegistry` | `src/core/core.hpp` | Clock hierarchy and tempo propagation |
| `ModuleRegistry` | `src/include/module.hpp` | Lazy-loaded module catalog and handles |
| `ProcessStore` | `src/core/evaluator.hpp` | Named process blocks for listen mode |

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

When a library is loaded under a namespace (e.g. `std = lib("stdlib")`), two things happen:

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
