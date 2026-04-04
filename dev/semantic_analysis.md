# Idyl Semantic Analysis

**Document Status**: Design Document (Pre-Implementation)
**Scope**: Post-parse semantic validation — from raw AST to annotated, validated AST ready for interpretation.

---

## Table of Contents

1. [Overview](#1-overview)
2. [Role in the Pipeline](#2-role-in-the-pipeline)
3. [Design Principles](#3-design-principles)
4. [Data Structures](#4-data-structures)
5. [Pass 1: Symbol Collection & Import Resolution](#5-pass-1-symbol-collection--import-resolution)
6. [Pass 2: Name Resolution](#6-pass-2-name-resolution)
7. [Pass 3: Arity & Argument Validation](#7-pass-3-arity--argument-validation)
8. [Pass 4: Type Inference (Soft)](#8-pass-4-type-inference-soft)
9. [Pass 5: Temporal Validation](#9-pass-5-temporal-validation)
10. [Pass 6: Flow Validation](#10-pass-6-flow-validation)
11. [Pass 7: Scope & Structure Validation](#11-pass-7-scope--structure-validation)
12. [Dispatch via `node_t`](#12-dispatch-via-node_t)
13. [Error Reporting](#13-error-reporting)
14. [Implementation Strategy](#14-implementation-strategy)
15. [Walkthrough Example](#15-walkthrough-example)

---

## 1. Overview

Semantic analysis is the **first post-parse pass** in the idyl interpreter pipeline. Its purpose is to validate correctness properties that the grammar (Bison/Flex) cannot enforce. The parser guarantees syntactic well-formedness — semantic analysis guarantees that identifiers resolve, function calls match signatures, types are compatible, temporal constructs are used appropriately, and scoping rules are respected.

This step produces **no new data structures**. It operates on the existing AST (defined in `ast.hpp`) and accumulates diagnostics (errors, warnings, info). If fatal errors are found, execution does not proceed to the evaluation engine.

### 1.1 What It Does

- Verifies every identifier resolves to a known definition (function, parameter, built-in, module, or local variable)
- Checks that function calls supply the correct number of arguments and that named arguments match parameter names
- Infers types (number, time, trigger, rest, flow, function, module) and warns on mismatches
- Validates temporal constructs (`dt`, `|>`, trigger parameters `!`, `@` memory operator) are used in legal contexts
- Validates flow structure (member consistency, generator ranges, repetition markers)
- Enforces structural rules (imports at global scope only, no definitions inside process blocks, etc.)

### 1.2 What It Does Not Do

- It does not evaluate expressions or compute values
- It does not produce IR or bytecode
- It does not modify the AST (though future versions may annotate nodes with inferred types)
- It does not enforce strict type errors — type mismatches produce warnings, not fatal errors (see §8)

---

## 2. Role in the Pipeline

```
Source Code (.idl)
       │
       ▼
   Lexer/Parser (Flex + Bison)
       │
       ▼
   Raw AST (ast.hpp nodes, with node_t tags)
       │
       ▼
┌──────────────────────────────┐
│     SEMANTIC ANALYSIS        │  ◄── This document
│                              │
│  Pass 1: Symbol Collection   │  Gather all top-level definitions, resolve imports
│                              │  Detect circular imports, deduplicate
│  Pass 2: Name Resolution     │  Resolve every identifier reference
│  Pass 3: Arity Validation    │  Check function call arguments
│  Pass 4: Type Inference      │  Infer and propagate types (soft)
│  Pass 5: Temporal Validation │  Validate dt, |>, !, @ usage
│  Pass 6: Flow Validation     │  Validate flow structure
│  Pass 7: Scope Validation    │  Enforce structural rules
│                              │
│  Output: diagnostics list    │
└──────────────────────────────┘
       │
       ▼
   If errors → report and halt
   If clean  → proceed to Environment Setup / Evaluation
```

Semantic analysis must complete **before** any process block executes. A program with an unresolved identifier should never partially run and then crash — the user should see all errors upfront.

---

## 3. Design Principles

### 3.1 Warn, Don't Block (Where Possible)

Idyl is a live-coding language. The philosophy is "degrade gracefully, don't crash." Semantic analysis follows the same spirit:

- **Errors** (prevent execution): undefined identifiers, arity mismatches, structural violations
- **Warnings** (log, continue): type ambiguities, `dt` used outside temporal context, shadowed names
- **Info**: unused parameters, redundant parentheses, style suggestions

This mirrors the runtime's "show must go on" philosophy described in interpreter.md §12.

### 3.2 Top-Level Hoisting, Local Declaration-Order

Top-level declarations (functions, flows, imports) are **collected in a preliminary pass** (Pass 1) before any body is analyzed. This means forward references between top-level definitions are perfectly valid — order doesn't matter:

```idl
foo(x) = bar(x)    // OK: 'bar' is collected in Pass 1 before bodies are resolved
bar(x) = x * 2
```

This is similar to how languages like Haskell or OCaml treat top-level bindings: all names are visible everywhere at the same scope level.

**Inside function/flow bodies**, declaration-order applies. Local variables (assignments in lambda blocks) must be defined before use — no hoisting of locals:

```idl
f(x) = x + y       // ERROR inside body: 'y' is not yet defined
    |> y = x * 2
```

Built-in functions (`sin`, `cos`, `fmod`, etc.) and module bindings are always available because they are registered before any user code is analyzed.

### 3.3 Single-Pass Where Possible, Multi-Pass Where Needed

The passes are conceptually ordered but some can be merged in implementation. Symbol collection (Pass 1) must come first. Name resolution (Pass 2) and arity checking (Pass 3) naturally interleave. Type inference (Pass 4) depends on resolved names. The implementation may combine passes 2–4 into a single recursive walk for efficiency.

---

## 4. Data Structures

### 4.1 The `node_t` Enum

Every AST node carries an immutable `node_t type_` tag, set at construction. This enables fast dispatch without `dynamic_cast` or RTTI:

```cpp
enum class node_t {
    // Literals
    number_literal, time_literal, trigger_literal, rest_literal, string_literal,
    // Identifiers
    identifier,
    // Operators
    binary_op, unary_op, ternary_op, memory_op,
    // Flow/generators
    generator_expr, flow_literal, repetition_marker, flow_member, flow_definition,
    // Calls/access
    argument, function_call, flow_access, module_access,
    // Blocks
    init_block, lambda_block, parameter,
    // Expressions (wrappers)
    literal_expr, identifier_expr, binary_op_expr, unary_op_expr, ternary_op_expr,
    memory_op_expr, generator_expr_node, flow_literal_expr, function_call_expr,
    flow_access_expr, module_access_expr, parenthesized_expr,
    // Statements
    expression_stmt, assignment, catch_block,
    function_definition, process_block_body, process_block,
    library_import, module_import,
    // Top-level
    program,
};
```

The analyzer switches on `node->type_` and `static_cast`s to the concrete type. This is safe because the tag is set by each struct's constructor and is `const` — it cannot be mutated after construction.

### 4.2 Symbol Information

Each resolved name is tracked with metadata:

```cpp
enum class symbol_kind {
    function,       // function_definition
    flow,           // flow_definition
    parameter,      // function parameter
    local_variable, // assignment inside lambda block
    emit_variable,  // emit assignment (visible in parent scope)
    builtin,        // built-in function (sin, cos, etc.)
    module,         // module binding (module("path") or ns = module("path"))
    library,        // library namespace (ns = lib("path"))
};

struct symbol_info {
    symbol_kind kind_;
    std::string name_;
    int line_ = 0;
    int column_ = 0;

    // For functions/flows: parameter count and names
    int arity_ = 0;                           // total params
    int required_arity_ = 0;                  // params without defaults
    std::vector<std::string> param_names_;     // ordered parameter names
    std::vector<bool> param_has_default_;      // which params have defaults

    // For temporal functions
    bool is_temporal_ = false;                 // has |> block
    bool has_dt_param_ = false;                // has dt=... parameter
    std::vector<std::string> trigger_params_;  // parameters with !

    // For type inference
    inferred_type type_ = inferred_type::unknown;

    // For imports (library/module)
    std::string import_path_;  // resolved file path
};
```

### 4.2.1 Import Tracking

The analyzer maintains a global import registry to detect duplicates and circular imports:

```cpp
struct import_record {
    std::string resolved_path_;  // canonical absolute path
    std::string namespace_;      // empty = global import
    bool is_library_;            // true = lib(), false = module()
    int line_ = 0;
    int column_ = 0;
};

struct import_registry {
    // All imported paths (both lib and module), for dedup
    std::unordered_map<std::string, import_record> imported_;

    // Stack of files currently being processed (for circular detection)
    std::vector<std::string> import_stack_;

    bool is_already_imported(const std::string& path) const;
    bool is_circular(const std::string& path) const;
    void push_file(const std::string& path);
    void pop_file();
};
```

### 4.3 Scope Stack

The analyzer maintains a stack of scopes. Each scope maps names to their `symbol_info`:

```cpp
enum class scope_type {
    global,         // top-level definitions
    function_body,  // inside a function's body expression
    lambda_body,    // inside a |> { ... } block
    init_block,     // inside init: { ... }
    process_block,  // inside process: { ... }
};

struct scope {
    scope_type type_;
    std::unordered_map<std::string, symbol_info> symbols_;

    // Context: what function/process are we inside?
    std::string enclosing_function_;
    bool is_temporal_ = false;
};

struct scope_stack {
    std::vector<scope> scopes_;

    void push(scope_type type);
    void pop();
    void define(const std::string& name, symbol_info info);
    symbol_info* lookup(const std::string& name);  // walks outward
};
```

Lookup proceeds from the innermost scope outward, matching standard lexical scoping:

```
Global Scope (function defs, flow defs, module bindings, built-ins)
  └─ Function Scope (parameters)
       └─ Lambda Scope (init variables, update variables)
            └─ Init Block Scope (initialization-only bindings)
```

### 4.4 Inferred Types

```cpp
enum class inferred_type {
    unknown,    // not yet determined
    number,     // float64
    time,       // duration in ms
    trigger,    // instantaneous event (!)
    rest,       // absence of event (.)
    flow,       // ordered sequence
    function_,  // callable
    module,     // external module handle
};
```

### 4.5 Diagnostics

```cpp
enum class severity {
    error,    // prevents execution
    warning,  // logged, execution continues
    info,     // style/advice
};

struct diagnostic {
    severity level_;
    int line_;
    int column_;
    std::string message_;
};
```

### 4.6 The Analyzer Context

All state for the analysis pass is bundled into a single context struct:

```cpp
struct analyzer {
    scope_stack scopes_;
    std::vector<diagnostic> diagnostics_;
    import_registry imports_;

    // Pre-populated with built-in function signatures
    void register_builtins();

    // Entry point
    void analyze(const program& prog);

    // Per-node dispatch (see §12)
    void analyze_node(const node* n);
    void analyze_statement(const statement* stmt);
    void analyze_expression(const expression* expr);

    // Helpers
    void error(int line, int col, const std::string& msg);
    void warning(int line, int col, const std::string& msg);
    void info(int line, int col, const std::string& msg);

    bool has_errors() const; // any severity::error in diagnostics_
};
```

---

## 5. Pass 1: Symbol Collection & Import Resolution

### 5.1 Purpose

Walk the top-level statements of the `program` node, resolve all imports, and register every definition into the global scope. This builds the "table of contents" before deeper analysis begins.

### 5.2 Import Syntax

Idyl has two import keywords with four syntax forms:

| Syntax | AST Node | Behavior |
|--------|----------|----------|
| `lib("path.idl")` | `library_import` (namespace empty) | Parse .idl file, merge all definitions into global scope |
| `ns = lib("path.idl")` | `library_import` (namespace = "ns") | Parse .idl file, definitions accessible via `ns::` |
| `module("path")` | `module_import` (namespace empty) | Load binary, register functions at global scope |
| `ns = module("path")` | `module_import` (namespace = "ns") | Load binary, functions accessible via `ns::` |

Both libraries and modules can be imported globally or into a namespace. The `::` access syntax works identically for both.

### 5.3 What Is Collected

| AST Node Type                | Registered As          |
|------------------------------|------------------------|
| `function_definition`        | `symbol_kind::function` with arity, param names, temporal info |
| `flow_definition`            | `symbol_kind::flow` with arity, param names |
| `module_import` (namespaced) | `symbol_kind::module` under namespace name |
| `module_import` (global)     | Each module function registered as `symbol_kind::builtin` in global scope |
| `library_import` (namespaced)| `symbol_kind::library` under namespace name |
| `library_import` (global)    | Each library definition merged into global scope |

### 5.4 Algorithm

```
for each stmt in program.statements_:
    switch stmt->type_:
        case node_t::function_definition:
            extract name, parameters, check for lambda_block
            register in global scope as symbol_kind::function
            if lambda_block exists: mark is_temporal_ = true
            check for dt= param: mark has_dt_param_
            check for ! params: record trigger_params_

        case node_t::flow_definition:
            extract name, parameters
            register in global scope as symbol_kind::flow

        case node_t::library_import:
            resolve_library_import(stmt)

        case node_t::module_import:
            resolve_module_import(stmt)

        case node_t::process_block:
            skip — process blocks are analyzed in later passes

        default:
            warning: unexpected top-level statement
```

### 5.5 Import Resolution Algorithm

#### 5.5.1 Library Import

```
resolve_library_import(lib_stmt):
    1. Resolve path to canonical absolute path
    2. Check deduplication:
       - If already imported globally → skip (no-op, with info message)
       - If already imported into a namespace → warning: "re-importing already loaded library"
    3. Check circular imports:
       - If path is on the import_stack_ → ERROR: circular import detected
    4. Push path onto import_stack_
    5. Parse the .idl file → get its AST
    6. Recursively run Pass 1 on the imported AST (handles transitive imports)
    7. Pop path from import_stack_
    8. Register symbols:
       - If namespace_ is empty: merge all definitions into global scope
       - If namespace_ is set: register namespace as symbol_kind::library,
         store imported definitions as sub-symbols accessible via ::
```

#### 5.5.2 Module Import

```
resolve_module_import(mod_stmt):
    1. Resolve path (search module paths for .so/.dylib/.dll)
    2. Check deduplication:
       - If already imported globally → skip (no-op, with info message)
       - If already imported into a different namespace → warning
    3. Record in import_registry
    4. Register symbols:
       - If namespace_ is empty: register at global scope
         (module function names will be available directly)
       - If namespace_ is set: register namespace as symbol_kind::module,
         functions accessible via ns::function_name()
```

Note: Module function signatures are not known at analysis time (they come from dynamically loaded binaries). For namespaced modules, the analyzer registers the namespace name and skips arity checks on module function calls. For global modules, function registration happens at runtime.

### 5.6 Circular Import Detection

Circular imports are detected by maintaining an **import stack** — the chain of files currently being processed:

```
main.idl imports scales.idl
  scales.idl imports utils.idl
    utils.idl imports scales.idl  → ERROR: circular import
```

The import stack at the point of error: `[main.idl, scales.idl, utils.idl]`

When `utils.idl` tries to import `scales.idl`, the analyzer finds `scales.idl` already on the stack and reports:

```
3.1: error: circular import detected: utils.idl → scales.idl → utils.idl
```

Circular imports are **always errors** — there is no way to resolve mutual dependencies between files in Idyl.

### 5.7 Import Deduplication

The same file may be imported multiple times (e.g., two different files both `lib("utils.idl")`). The analyzer deduplicates by canonical path:

| Scenario | Behavior |
|----------|----------|
| Same file imported globally twice | Second import is silently skipped (info message) |
| Same file imported globally and into namespace | Both valid — global gets merged, namespace gets scoped access |
| Same file imported into two different namespaces | Both valid — creates two namespace aliases for the same definitions |
| Same file imported into the same namespace twice | Second import is silently skipped (info message) |

Deduplication prevents re-parsing and re-registering symbols, which matters for transitive imports (A imports B, A imports C, both B and C import D — D should only be processed once).

### 5.8 Transitive Import Example

```idl
// utils.idl
clamp(x, lo, hi) = min(max(x, lo), hi)

// scales.idl
lib("utils.idl")
major_scale(root) = [root, root+2, root+4, root+5, root+7, root+9, root+11]

// main.idl
lib("scales.idl")      // also transitively imports utils.idl
lib("utils.idl")       // already imported — silently skipped

process: {
    notes = major_scale(60)    // from scales.idl
    clamped = clamp(x, 0, 127) // from utils.idl (via transitive import)
}
```

### 5.9 Shadowing Detection

If a name is already defined in the global scope, the new definition shadows it. This produces a **warning** (not an error), matching Idyl's "later definitions override earlier ones (with warning)" rule from the spec:

```
3.5: warning: function 'lfo' shadows previous definition at 1.1
```

### 5.10 Built-in Registration

Before processing user code, the global scope is pre-populated with built-in function signatures:

```
sin(x)         → arity=1, type=number
cos(x)         → arity=1, type=number
fmod(x, m)     → arity=2, type=number
rnd()           → arity=0, type=number
rnd(a, b)       → arity=2, type=number (overloaded)
rnd(a, b, step) → arity=3, type=number (overloaded)
metronome(dt)  → arity=1, type=trigger
counter(trig)  → arity=1, type=number
phasor(freq)   → arity=1, type=number
len(flow)      → arity=1, type=number
bit(val, idx)  → arity=2, type=number
int(x)         → arity=1, type=number
float(x)       → arity=1, type=number
... etc
```

Built-ins are marked with `symbol_kind::builtin` and cannot be shadowed without a warning.

---

## 6. Pass 2: Name Resolution

### 6.1 Purpose

Walk every expression and identifier in the AST and verify that it resolves to a known symbol. This is the most critical pass — an unresolved name is always a fatal error.

### 6.2 Resolution Rules

An identifier resolves by looking up the scope stack from innermost to outermost:

1. **Init block scope** (if inside `init: { ... }`): variables defined in the init block
2. **Lambda scope** (if inside `|> { ... }`): update variables and init variables
3. **Function scope**: parameters of the enclosing function
4. **Global scope**: previously defined functions, flows, module bindings
5. **Built-in scope**: `sin`, `cos`, `metronome`, etc.

If not found in any scope → **error: undefined identifier**.

### 6.3 Special Identifiers

- **`dt`**: Only valid inside a temporal function (one that has a `dt=` parameter). If used elsewhere, produces a warning: `'dt' used outside temporal function`.
- **Trigger parameters** (e.g., `spike!`): The `!` is part of the parameter declaration syntax, not the identifier. Inside the function body, the parameter is referenced as `spike` (without `!`), and its value is a trigger type.

### 6.4 Node-by-Node Resolution

The resolver descends recursively. For each node type:

| Node Type            | Resolution Action                                                                                     |
|----------------------|------------------------------------------------------------------------------------------------------|
| `identifier_expr`    | Look up `identifier_->name_` in scope stack. Error if not found.                                     |
| `binary_op_expr`     | Recurse into `op_->left_` and `op_->right_`.                                                        |
| `unary_op_expr`      | Recurse into `op_->operand_`.                                                                        |
| `ternary_op_expr`    | Recurse into `op_->condition_` and all `op_->options_`.                                              |
| `function_call_expr` | Resolve the function name, then recurse into all argument values.                                    |
| `flow_access_expr`   | Resolve the flow name, validate the member name exists.                                              |
| `module_access_expr` | Resolve the module binding name. (Module function names cannot be validated statically.)             |
| `memory_op_expr`     | Recurse into `op_->expr_` and `op_->delay_count_`.                                                  |
| `literal_expr`       | No resolution needed (literals are self-contained).                                                  |
| `parenthesized_expr` | Recurse into `expr_`.                                                                                |
| `generator_expr_node`| Push scope for generator variable, recurse into range and body, pop scope.                           |
| `flow_literal_expr`  | Recurse into all elements.                                                                           |

### 6.5 Scope Transitions

When the analyzer enters a new syntactic context, it pushes a new scope:

```
analyze function_definition "lfo":
    push scope(function_body)
    define parameters: freq, dt
    if has lambda_block:
        analyze body expression (with function scope)
        push scope(lambda_body, is_temporal=true)
        if has init_block:
            push scope(init_block)
            analyze init statements → define local vars
            pop scope
        analyze update statements → define local vars
        pop scope
    else:
        analyze body expression
    pop scope
```

For process blocks:
```
analyze process_block:
    push scope(process_block)
    analyze all statements:
        assignments create process-local bindings
        expression_stmts are analyzed in place
        catch blocks analyzed with their handler bodies
    pop scope
```

### 6.6 Forward References Between Top-Level Definitions

Because Pass 1 collects **all** top-level declarations before Pass 2 resolves bodies, forward references between top-level functions and flows are valid:

```idl
foo(x) = bar(x)    // OK: 'bar' was collected in Pass 1
bar(x) = x * 2     // definition of bar
```

This means mutual recursion between top-level functions is allowed:

```idl
is_even(n) = n == 0 ? 1 : is_odd(n - 1)      // OK
is_odd(n)  = n == 0 ? 0 : is_even(n - 1)     // OK
```

Forward references are only errors for **local variables** inside function/flow bodies (see §3.2). If an identifier is not found in any scope (local or global), the error message should indicate it is undefined — not that it is a forward reference.

---

## 7. Pass 3: Arity & Argument Validation

### 7.1 Purpose

Every function call must supply the correct number of arguments, and named arguments must reference valid parameter names.

### 7.2 Algorithm

For each `function_call_expr` node:

1. Resolve the function name → get `symbol_info`
2. Count **positional arguments** and **named arguments** in the call
3. Validate:
   - Total arguments ≥ `required_arity_` (params without defaults)
   - Total arguments ≤ `arity_` (total params)
   - Each named argument matches a parameter name in `param_names_`
   - No parameter is bound twice (once positionally and once by name)

### 7.3 Positional vs Named Arguments

Idyl allows three calling conventions for functions with defaults:

```idl
fade_out(duration, curve=linear) = ...

fade_out(500ms)                     // OK: positional only, curve uses default
fade_out(500ms, exponential)        // OK: positional for both
fade_out(500ms, curve=exponential)  // OK: named argument for curve
```

The analyzer must track which parameters have been bound:

```
given: fade_out(500ms, curve=exponential)
  - param 0 "duration" ← bound positionally to 500ms
  - param 1 "curve" ← bound by name to exponential
  → valid

given: fade_out(500ms, exponential, curve=linear)
  - param 0 "duration" ← bound positionally to 500ms
  - param 1 "curve" ← bound positionally to exponential
  - param 1 "curve" ← bound by name to linear → ERROR: duplicate binding
```

### 7.4 Error Examples

```
5.1: error: function 'add' expects 2 arguments, got 1
7.15: error: function 'add' expects 2 arguments, got 3
9.20: error: 'foo' is not a parameter of function 'lfo' (parameters are: freq, dt)
11.5: error: parameter 'curve' bound twice in call to 'fade_out'
```

### 7.5 Built-in Overloads

Some built-ins accept variable arity (e.g., `rnd()`, `rnd(a, b)`, `rnd(a, b, step)`). These are registered with multiple valid arities. The analyzer checks that the call matches at least one valid signature.

### 7.6 Module Calls

Module function calls (`cs::play(...)`) cannot be arity-checked statically because module signatures are not known until runtime (they come from dynamically loaded `.so` files). The analyzer skips arity validation for module calls but still resolves the namespace binding name.

Note: This applies equally to namespaced library calls (`scales::minor(root)`) — however, for libraries, the imported definitions *are* known at analysis time, so arity checking is possible on library namespace calls.

---

## 8. Pass 4: Type Inference (Soft)

### 8.1 Purpose

Infer the type of every expression and warn (but do not block) on mismatches. Idyl has no explicit type annotations — types are inferred from literals, operators, and function signatures.

### 8.2 Type Universe

| Type       | Sources                                                        |
|------------|---------------------------------------------------------------|
| `number`   | Number literals (`42`, `0.5`), arithmetic results, comparisons |
| `time`     | Time literals (`100ms`, `2s`, `440hz`, `4b`)                  |
| `trigger`  | Trigger literal (`!`), trigger parameters, `metronome()`       |
| `rest`     | Rest literal (`.`)                                            |
| `flow`     | Flow literals (`[1 2 3]`), flow definitions, generators        |
| `function` | Function definitions (when referenced, not called)             |
| `module`   | Module bindings (`module(...)`)                                |
| `unknown`  | Cannot be determined (propagates through expressions)          |

### 8.3 Inference Rules

#### 8.3.1 Literals

Straightforward — the literal's `node_t` directly determines its type:

| `node_t`            | Inferred Type |
|---------------------|---------------|
| `number_literal`    | `number`      |
| `time_literal`      | `time`        |
| `trigger_literal`   | `trigger`     |
| `rest_literal`      | `rest`        |
| `string_literal`    | `number` (strings are not a value type in idyl; they only appear in `import`/`lib` paths) |

#### 8.3.2 Identifiers

Look up the symbol → return its `inferred_type`. If the symbol's type is `unknown`, propagate `unknown`.

#### 8.3.3 Binary Operators

| Operator          | Left     | Right    | Result    | Notes                                    |
|-------------------|----------|----------|-----------|------------------------------------------|
| `+` `-`           | number   | number   | number    |                                          |
| `+` `-`           | time     | time     | time      | Duration arithmetic                      |
| `*`               | time     | number   | time      | Scale duration                           |
| `*`               | number   | time     | time      | Scale duration                           |
| `/`               | time     | time     | number    | Ratio                                    |
| `/`               | time     | number   | time      | Divide duration                          |
| `%`               | number   | number   | number    |                                          |
| `<` `>` `==` etc. | any      | any      | number    | Comparison returns 0 or 1                |
| `&` `\|` `^` `<<` `>>`| number | number | number  | Bitwise (integer)                        |
| any               | trigger  | trigger  | trigger   | Trigger algebra                          |
| any               | trigger  | rest     | trigger   |                                          |
| any               | rest     | rest     | rest      |                                          |

If the combination doesn't match any rule, the result is `unknown` and a **warning** is emitted:

```
4.10: warning: operator '+' applied to time and trigger — result type is ambiguous
```

#### 8.3.4 Unary Operators

| Operator | Operand | Result |
|----------|---------|--------|
| `-`      | number  | number |
| `-`      | time    | time   |
| `~`      | number  | number |

#### 8.3.5 Ternary Operator

The result type is the type of the options. If all options have the same type, that is the result type. If options have mixed types, the result is `unknown` with a warning.

The condition must be `number` (0/1/N selector). If the condition is `trigger`, it's valid (trigger in numeric context is 0 or 1).

#### 8.3.6 Function Calls

The result type is the function's declared return type (inferred from the function's body expression). If the function hasn't been analyzed yet or its return type is `unknown`, propagate `unknown`.

#### 8.3.7 Memory Operator (`@`)

`@(expr)` returns the same type as `expr` (it's the previous value of the expression). The delay count, if present, must be `number`.

#### 8.3.8 Flow Access

Accessing a flow member (`flow::member(index)`) returns the element type of that member. If the flow contains number elements, the result is `number`. Mixed-type flows produce `unknown`.

### 8.4 Type Propagation

Types propagate bottom-up through the expression tree. The analyzer attaches inferred types to expression nodes (or maintains a parallel map keyed by node identity). When a parent expression's children have known types, the parent's type is determined by the rules above.

### 8.5 Why Soft?

Type mismatches produce warnings, not errors, because:

1. **Live coding**: blocking on a type warning during a performance is unacceptable
2. **Runtime coercion**: the interpreter coerces types at runtime (trigger → 1, rest → 0), so type mismatches are often intentional
3. **Implicit conversion**: time values are internally numbers (milliseconds), so most arithmetic "just works"
4. **Progressive strictness**: the warning system lets users opt into stricter checking later without breaking existing code

---

## 9. Pass 5: Temporal Validation

### 9.1 Purpose

Validate that temporal constructs (`dt`, `|>` lambda blocks, trigger parameters `!`, `@` memory operator) are used in legal contexts. Temporal functions are Idyl's most distinctive feature, so they get dedicated validation.

### 9.2 Rules

#### 9.2.1 Lambda Blocks Require a Time Source

A function with a `|>` block must have at least one time source:
- A `dt=value` parameter (clock-driven), or
- At least one trigger parameter (`name!`, trigger-driven), or
- Both (hybrid)

If neither is present → **error**:
```
5.1: error: function 'broken' has a lambda block but no time source (add dt= or trigger parameter)
```

#### 9.2.2 `dt` Identifier Requires Temporal Context

The identifier `dt` (when used inside a function body, not as a parameter declaration) is only meaningful inside a temporal function — one that has a `dt=` parameter. Using `dt` in a non-temporal function produces a **warning**:

```
3.15: warning: 'dt' used in non-temporal function 'plain_func' — value is undefined
```

Implementation: when resolving the identifier `dt`, check if the current scope stack contains a `lambda_body` scope with `is_temporal_ = true` and the enclosing function has a `dt=` parameter.

#### 9.2.3 Trigger Parameters Require Lambda Block

A trigger parameter (`name!`) is syntactically valid in any parameter list, but semantically it only makes sense in a temporal function with a `|>` block. A trigger parameter on a pure function produces a **warning**:

```
2.10: warning: trigger parameter 'spike!' in non-temporal function 'add' — triggers are ignored in pure functions
```

#### 9.2.4 Memory Operator Requires Temporal Context

The `@(expr)` memory operator accesses the previous value of an expression, which only has meaning inside a temporal function (where "previous" means "at the last dt tick"). Using `@` outside a temporal function produces a **warning**:

```
8.5: warning: memory operator '@' used outside temporal function — no previous value exists
```

#### 9.2.5 `emit` Requires Lambda Block

The `emit` keyword is only valid inside a `|>` lambda block (either in `init:` or in the update section). Using `emit` elsewhere produces an **error**:

```
12.5: error: 'emit' used outside lambda block — emit is only valid inside |> { ... }
```

#### 9.2.6 `init:` Block Placement

The `init:` block can only appear as the first construct inside a `|>` lambda block. It cannot appear:
- At top level
- Inside a process block
- Inside a non-temporal function
- More than once per lambda block

### 9.3 Temporal Context Tracking

The analyzer tracks temporal context via the scope stack. When entering a `function_definition` with a `lambda_block_`:

```
set is_in_temporal_function = true
set has_dt = (any param with has_default_time_)
set trigger_params = [params with is_trigger_parameter_]

analyze body expression → temporal identifiers may appear here
push lambda scope:
    analyze init block (if present)
    analyze update statements
pop lambda scope

reset is_in_temporal_function = false
```

---

## 10. Pass 6: Flow Validation

### 10.1 Purpose

Validate that flow definitions and flow literals are well-formed.

### 10.2 Rules

#### 10.2.1 Flow Member Names Must Be Unique

In a multi-member flow, each member must have a distinct name:

```idl
flow bad = {
    kick: [! . . .],
    kick: [. . ! .]     // ERROR: duplicate member name 'kick'
}
```

#### 10.2.2 Generator Range Validity

Generator expressions `[var = start..end : body]` must have valid ranges. The analyzer cannot always evaluate ranges statically (they may contain identifiers), but it can check:

- Both `range_start_` and `range_end_` are present
- If both are number literals, `start ≤ end`
- The generator variable is defined in the generator scope (it should always be, as the parser creates it)

When evaluable statically:
```
7.15: warning: generator range 10..5 — start > end, resulting flow will be empty
```

#### 10.2.3 Flow Access Member Validation

When a flow is accessed with `::`, the member name must exist in the flow definition. For statically known flows:

```idl
flow drum = { kick: [! . . .], snare: [. . ! .] }
x = drum::hat(metro)  // ERROR: 'hat' is not a member of flow 'drum' (members: kick, snare)
```

This check is only possible when the flow is defined with named members. Single-member flows accessed with `::` produce an error.

#### 10.2.4 Repetition Marker Placement

Repetition markers (`|N||` and `||`) must appear inside flow literals. They must have valid counts (if the count is a literal, it must be > 0).

---

## 11. Pass 7: Scope & Structure Validation

### 11.1 Purpose

Enforce structural rules about what constructs can appear where.

### 11.2 Rules

#### 11.2.1 Imports at Global Scope Only

`lib(...)` and `module(...)` statements must appear at the top level of the program — never inside function definitions or process blocks:

```idl
process: {
    cs = module("csound")  // ERROR: import must be at global scope
}
```

The analyzer checks in Pass 1 that `library_import` and `module_import` nodes only appear as direct children of the `program` node.

#### 11.2.2 No Definitions Inside Process Blocks

Process blocks can only contain assignments, expression statements, and catch blocks — not function or flow definitions:

```idl
process: {
    add(a, b) = a + b   // ERROR: function definitions not allowed inside process blocks
    x = add(1, 2)
}
```

The analyzer verifies that no `function_definition` or `flow_definition` appears as a child of `process_block`.

#### 11.2.3 Process Block Statement Types

Valid statement types inside a process block:
- `assignment` — bind a value (`x = expr`)
- `expression_stmt` — evaluate an expression (for its side effects)
- `catch_block` — register an event handler

Invalid statement types:
- `function_definition` → error
- `flow_definition` → error
- `library_import` → error
- `module_import` → error

#### 11.2.4 Lambda Block Statement Types

Valid statement types inside a `|>` block (update section):
- `assignment` — update state variable
- `assignment` with `emit` — update and propagate state variable
- `expression_stmt` — evaluate expression

Invalid:
- Everything else (function definitions, imports, process blocks, etc.)

#### 11.2.5 Catch Block Placement

`catch` blocks can only appear inside process blocks, attached to an expression:

```idl
expr catch end: { ... }
```

The `catch` target (`end` in alpha) must be a recognized event type. In alpha, only `end` is supported.

#### 11.2.6 Unused Parameter Warning

If a function parameter is never referenced in the function body (or lambda block), emit an **info** diagnostic:

```
4.1: info: parameter 'unused_param' is never used in function 'my_func'
```

---

## 12. Dispatch via `node_t`

The `node_t` enum enables clean, RTTI-free dispatch in the analyzer. The core dispatch function:

```cpp
void analyzer::analyze_node(const node* n) {
    switch (n->type_) {
        // Literals — no analysis needed (type is self-evident)
        case node_t::number_literal:
        case node_t::time_literal:
        case node_t::trigger_literal:
        case node_t::rest_literal:
        case node_t::string_literal:
            break;

        // Identifier — resolve name
        case node_t::identifier:
            analyze_identifier(static_cast<const identifier*>(n));
            break;

        // Operators — recurse into children
        case node_t::binary_op:
            analyze_binary_op(static_cast<const binary_op*>(n));
            break;
        case node_t::unary_op:
            analyze_unary_op(static_cast<const unary_op*>(n));
            break;
        case node_t::ternary_op:
            analyze_ternary_op(static_cast<const ternary_op*>(n));
            break;
        case node_t::memory_op:
            analyze_memory_op(static_cast<const memory_op*>(n));
            break;

        // Flows and generators
        case node_t::generator_expr:
            analyze_generator(static_cast<const generator_expr*>(n));
            break;
        case node_t::flow_literal:
            analyze_flow_literal(static_cast<const flow_literal*>(n));
            break;
        case node_t::flow_member:
            analyze_flow_member(static_cast<const flow_member*>(n));
            break;

        // Calls and access
        case node_t::function_call:
            analyze_function_call(static_cast<const function_call*>(n));
            break;
        case node_t::flow_access:
            analyze_flow_access(static_cast<const flow_access*>(n));
            break;
        case node_t::module_access:
            analyze_module_access(static_cast<const module_access*>(n));
            break;

        // Blocks
        case node_t::init_block:
            analyze_init_block(static_cast<const init_block*>(n));
            break;
        case node_t::lambda_block:
            analyze_lambda_block(static_cast<const lambda_block*>(n));
            break;

        // ... and so on for all node types
    }
}
```

For expression wrapper nodes (`literal_expr`, `binary_op_expr`, etc.), the dispatch unwraps them and recurses into the inner node:

```cpp
void analyzer::analyze_expression(const expression* expr) {
    switch (expr->type_) {
        case node_t::literal_expr:
            analyze_node(static_cast<const literal_expr*>(expr)->literal_.get());
            break;
        case node_t::identifier_expr:
            analyze_node(static_cast<const identifier_expr*>(expr)->identifier_.get());
            break;
        case node_t::binary_op_expr:
            analyze_node(static_cast<const binary_op_expr*>(expr)->op_.get());
            break;
        case node_t::unary_op_expr:
            analyze_node(static_cast<const unary_op_expr*>(expr)->op_.get());
            break;
        case node_t::ternary_op_expr:
            analyze_node(static_cast<const ternary_op_expr*>(expr)->op_.get());
            break;
        case node_t::memory_op_expr:
            analyze_node(static_cast<const memory_op_expr*>(expr)->op_.get());
            break;
        case node_t::generator_expr_node:
            analyze_node(static_cast<const generator_expr_node*>(expr)->generator_.get());
            break;
        case node_t::flow_literal_expr:
            analyze_node(static_cast<const flow_literal_expr*>(expr)->flow_.get());
            break;
        case node_t::function_call_expr:
            analyze_node(static_cast<const function_call_expr*>(expr)->call_.get());
            break;
        case node_t::flow_access_expr:
            analyze_node(static_cast<const flow_access_expr*>(expr)->access_.get());
            break;
        case node_t::module_access_expr:
            analyze_node(static_cast<const module_access_expr*>(expr)->access_.get());
            break;
        case node_t::parenthesized_expr:
            analyze_expression(static_cast<const parenthesized_expr*>(expr)->expr_.get());
            break;
        default:
            break;
    }
}
```

### 12.1 Why `node_t` + `static_cast` Instead of `dynamic_cast`?

| Approach           | Cost            | Safety                            | Readability |
|--------------------|-----------------|-----------------------------------|-------------|
| `dynamic_cast`     | RTTI overhead, O(depth) per cast | Safe — returns nullptr on failure | Medium (null checks everywhere) |
| `node_t` + `static_cast` | Zero overhead (enum comparison + pointer cast) | Safe — tag is const, set at construction | High (clean switch) |
| Visitor pattern    | Virtual dispatch overhead | Safe — compile-time completeness | High but verbose |

The `node_t` approach is the best fit for Idyl:
- The enum is exhaustive and `const`, so the tag always matches the actual type
- `switch` gives compile-time warnings for unhandled cases (with `-Wswitch`)
- No RTTI overhead — important for a real-time-oriented language
- Easily extensible: add a new enum value + a new case in the switch

---

## 13. Error Reporting

### 13.1 Format

Errors use the `line_` and `column_` fields stored on every AST node:

```
<line>.<column>: <severity>: <message>
```

Examples:
```
3.5: error: undefined identifier 'y' in function 'foo'
7.12: warning: 'dt' used outside temporal function
15.1: error: function 'add' expects 2 arguments, got 1
22.10: warning: operator '+' applied to time and trigger — result type is ambiguous
30.1: info: parameter 'unused' is never used in function 'my_func'
45.5: error: import must be at global scope (found inside process block)
48.1: error: circular import detected: main.idl → utils.idl → main.idl
```

### 13.2 Severity Levels

| Level     | Blocks Execution? | Examples                                          |
|-----------|-------------------|--------------------------------------------------|
| **error** | Yes               | Undefined name, arity mismatch, structural violation |
| **warning** | No              | Type ambiguity, dt outside temporal, shadowed name |
| **info**  | No                | Unused parameter, redundant parentheses           |

### 13.3 Error Collection

All diagnostics are accumulated in a single vector. After all passes complete, the analyzer reports:
1. All errors, warnings, and info messages (sorted by line/column)
2. A summary: `N error(s), M warning(s)`
3. If any error exists → execution halts

### 13.4 Helpful Error Messages

Where possible, the analyzer provides context and suggestions:

```
1.10: error: undefined identifier 'bar'
8.5: error: function 'lfo' expects 1-2 arguments, got 3 (parameters: freq, dt=10ms)
12.1: warning: function 'helper' shadows previous definition at line 3
```

---

## 14. Implementation Strategy

### 14.1 File Structure

```
src/
  semantic/
    analyzer.hpp      — analyzer struct, scope_stack, symbol_info, diagnostic
    analyzer.cpp      — analyze() entry point, per-node methods
```

Or, for simplicity in alpha, a single header:
```
src/
  semantic/
    analyzer.hpp
```

### 14.2 Entry Point

```cpp
#include "parser/ast.hpp"
#include "semantic/analyzer.hpp"

// After parsing:
auto program = parse(source);

// Run semantic analysis:
idyl::semantic::analyzer analyzer;
analyzer.analyze(*program);

if (analyzer.has_errors()) {
    analyzer.print_diagnostics();
    return 1;  // halt
}

// Proceed to evaluation...
```

### 14.3 Implementation Order

The passes should be built incrementally, each one adding value independently:

1. **Symbol collection** — enables forward-reference detection
2. **Name resolution** — catches the most common errors (typos, missing definitions)
3. **Arity checking** — catches argument count errors
4. **Scope validation** — enforces structural rules (imports at global scope, etc.)
5. **Temporal validation** — catches dt/lambda/trigger misuse
6. **Flow validation** — catches flow member errors
7. **Type inference** — the most complex pass; implement last, start with literal types and propagate upward incrementally

Each pass can be tested independently with small `.idl` files that exercise its rules.

### 14.4 Testing Strategy

For each pass, write test cases that cover:
- **Positive cases** (valid programs that should produce no diagnostics)
- **Negative cases** (programs with specific errors that should produce expected diagnostics)

Example test pattern:
```cpp
TEST(NameResolution, UndefinedIdentifier) {
    auto prog = parse("foo(x) = x + y");
    analyzer a;
    a.analyze(*prog);
    ASSERT_TRUE(a.has_errors());
    ASSERT_EQ(a.diagnostics_[0].message_, "undefined identifier 'y' in function 'foo'");
}
```

---

## 15. Walkthrough Example

A complete example showing what each pass does on a small program.

### Source Code

```idl
lib("scales.idl")
cs = module("csound")

base_freq = 440hz

lfo(freq, dt=10ms) = sin(phase * 2 * pi) |> {
    init: { phase = 0 }
    phase = fmod(phase + 1/freq, 1)
}

flow melody = [60, 62, 64, 65, 67]

process: {
    metro = metronome(500ms)
    vibrato = lfo(5hz)
    note = melody::value(metro)
    freq = base_freq * pow(2, (note - 69) / 12)
    cs::play(freq + vibrato * 10)
}
```

### Pass 1: Symbol Collection & Import Resolution

Process imports first:
- `lib("scales.idl")`: resolve path, check not circular, parse file, merge definitions into global scope
- `cs = module("csound")`: register `cs` as `symbol_kind::module` in global scope

Global scope after collection:
```
base_freq   → symbol_kind::function, arity=0, type=time
lfo         → symbol_kind::function, arity=2, required=1, is_temporal=true, has_dt=true
melody      → symbol_kind::flow, arity=0
cs          → symbol_kind::module, path="csound"
(builtins)  → sin, cos, fmod, metronome, pow, etc.
(library)   → scales.idl definitions merged in (e.g. major_scale, minor_scale, etc.)
```

### Pass 2: Name Resolution

Walk `base_freq` body: `440hz` → literal, no resolution needed. ✓

Walk `lfo` body:
- Push function scope: `{freq, dt}`
- `sin(phase * 2 * pi)`:
  - `sin` → resolves to builtin ✓
  - `phase` → not in function scope, not in global... push lambda scope first
- Push lambda scope:
  - `init:` defines `phase` in lambda scope ✓
  - Update: `fmod(phase + 1/freq, 1)`:
    - `fmod` → builtin ✓
    - `phase` → lambda scope ✓
    - `freq` → function scope ✓
- The output expression `sin(phase * 2 * pi)`:
  - `phase` → lambda scope ✓
  - `pi` → NOT FOUND → **error: undefined identifier 'pi' in function 'lfo'** (unless `pi` is a built-in)
- Pop scopes

Walk `melody`: `[60, 62, 64, 65, 67]` → all literals ✓

Walk process block:
- Push process scope
- `metro = metronome(500ms)`: `metronome` → builtin ✓, bind `metro` in process scope
- `vibrato = lfo(5hz)`: `lfo` → global scope ✓, bind `vibrato`
- `note = melody::value(metro)`: `melody` → global scope (flow) ✓, `::value` member check (Pass 6)
- `freq = base_freq * pow(2, (note - 69) / 12)`: all identifiers resolve ✓
- `cs::play(freq + vibrato * 10)`: `cs` → module ✓, module function not validated ✓
- Pop scope

### Pass 3: Arity Checking

- `metronome(500ms)` → arity=1, expects 1 ✓
- `lfo(5hz)` → arity=2, required=1, got 1 ✓ (dt uses default)
- `pow(2, ...)` → arity=2, expects 2 ✓
- `melody::value(metro)` → flow access, not a regular call

### Pass 4: Type Inference

- `base_freq`: body is `440hz` → type `time`
- `lfo` returns `sin(...)` → type `number`
- `metro`: `metronome(500ms)` → type `trigger`
- `vibrato`: `lfo(5hz)` → type `number`
- `note`: flow access → type `number` (flow elements are number literals)
- `freq`: `time * number` → type `time` (base_freq is time, pow returns number)
  - Potential warning: using time as a frequency value (this is actually intentional in music — 440hz *is* a frequency stored as time)
- `cs::play(...)` → module call, type `unknown`

### Pass 5: Temporal Validation

- `lfo` has `|>` block and `dt=10ms` parameter → valid temporal function ✓
- `base_freq` has no `|>` but no temporal constructs used → ✓
- Process block has no `@`, `dt`, or `emit` usage → ✓

### Pass 6: Flow Validation

- `melody` has single unnamed member with 5 elements → ✓
- `melody::value` — single-member flow accessed with `::value`. Depends on naming convention: if single-member flows don't have a `value` member, this is an error. (Implementation decision: single-member flows might support `::value` as a built-in alias or might require index-only access.)

### Pass 7: Scope Validation

- `lib()` and `module()` at top level → ✓
- No definitions inside process block → ✓
- No `emit` outside lambda block → ✓
- No circular imports → ✓

### Final Output

```
6.40: error: undefined identifier 'pi' in function 'lfo'
1 error(s), 0 warning(s)
Execution halted.
```

(Fixing this requires either defining `pi` as a constant or adding it as a built-in.)
