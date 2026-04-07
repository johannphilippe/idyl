# 11 — Architecture

[← Back to index](idyl_documentation.md) · [Previous: Modules & libraries](ch10_modules_libraries.md)

---

This chapter describes the internal pipeline of the Idƴl implementation. It is intended for contributors and anyone interested in how the language works under the hood.

---

## Pipeline overview

```
Source (.idl)
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

- Keywords: `process`, `init`, `emit`, `catch`, `lib`, `module`, `clock`, `tempo`
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
| `LibImport` | `lib("path")` |
| `ModuleImport` | `module("name")` |

---

## Semantic analyzer

The semantic analyzer (`src/semantic/analyzer.cpp`) performs **7 ordered passes**:

### Pass 1 — Library collection
Collects all `lib()` imports, resolves paths, detects circular dependencies.

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

The evaluator (`src/core/core.hpp`) is a **tree-walking interpreter**. It traverses the AST and executes nodes directly.

### Key components

| Component | Purpose |
|-----------|---------|
| `Environment` | Scope chain for variable lookup |
| `Scheduler` | Drift-free timer management |
| `ClockRegistry` | Clock hierarchy and tempo propagation |
| `ModuleRegistry` | Loaded module handles |
| `ProcessStore` | Named process blocks for listen mode |

### Evaluation model

1. **Constants and functions** are evaluated once and stored in the environment
2. **Temporal lambdas** are instantiated: their init blocks run, then they are registered with the scheduler at their `dt` interval
3. **Reactive bindings** (`osc_send(osc, addr, temporal_fn)`) subscribe to the temporal function's output — each time the scheduler ticks the function, downstream consumers see the new value
4. **Process blocks** orchestrate the above — each block is an independent execution context

### Temporal instantiation

When the evaluator encounters a temporal lambda:

```idyl
freq(dt=10ms) = v |> {
    init: { v = 440 }
    v = v + 1
}
```

It performs these steps:

1. Evaluate the `dt` expression → 10ms
2. Run the `init` block → set `v = 440` in the lambda's local scope
3. Register the lambda with the scheduler at 10ms intervals
4. Return a **handle** that downstream expressions can read

### The scheduler

The scheduler is **drift-free**: it tracks elapsed time against the system clock rather than accumulating intervals. This prevents timing drift over long runs.

Each tick:

1. Compute actual elapsed time since start
2. For each registered lambda, check if its next fire time has passed
3. If so, execute the lambda body and advance its next fire time
4. Collect outputs for reactive consumers

The scheduler runs in a tight loop with a configurable sleep granularity to balance CPU usage against timing precision.

---

## Trace mode

The `--trace` flag enables diagnostic output during evaluation:

```bash
idyl program.idl --trace
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
| **ChucK** | Strongly-timed audio programming |
| **SuperCollider** | OSC integration, server architecture |
| **Tidal Cycles** | Pattern-based temporal sequencing |
| **Faust** | Functional signal processing |
| **Max/MSP** | Dataflow, real-time scheduling |

The key differentiator is Idƴl's approach to **temporal functions as first-class values** — time is embedded in the function definition itself, not managed externally.

---

[← Back to index](idyl_documentation.md)
