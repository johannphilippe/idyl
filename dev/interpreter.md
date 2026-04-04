# Idyl Interpreter Architecture

**Document Status**: Design Document (Pre-Implementation)
**Scope**: Interpreter pipeline from AST to execution — excluding Time Engine internals (covered separately)

---

## Table of Contents

1. [Overview & Philosophy](#1-overview--philosophy)
2. [Pipeline Summary](#2-pipeline-summary)
3. [Step 1: Semantic Analysis](#3-step-1-semantic-analysis)
4. [Step 2: IR Generation (Lowering)](#4-step-2-ir-generation-lowering)
5. [Step 3: Environment & Symbol Table](#5-step-3-environment--symbol-table)
6. [Step 4: Evaluation Engine](#6-step-4-evaluation-engine)
7. [Step 5: Function Instantiation & State](#7-step-5-function-instantiation--state)
8. [Step 6: Flow Runtime](#8-step-6-flow-runtime)
9. [Step 7: Module System](#9-step-7-module-system)
10. [Step 8: Process Block Execution](#10-step-8-process-block-execution)
11. [Step 9: Memory Management & Garbage Collection](#11-step-9-memory-management--garbage-collection)
12. [Step 10: Error Handling at Runtime](#12-step-10-error-handling-at-runtime)
13. [Time Engine (Stub)](#13-time-engine-stub)
14. [Implementation Order](#14-implementation-order)

---

## 1. Overview & Philosophy

### 1.1 What Kind of Interpreter Is Idyl?

Idyl is a **tree-walking interpreter with optional IR lowering** — similar in spirit to early Lua, pre-JIT Ruby (MRI), or Faust's block-diagram compiler, but with a reactive/dataflow twist closer to Max/MSP's execution model or Elm's functional-reactive architecture.

The key philosophical choices:

| Principle | Idyl Choice | Comparable Language |
|-----------|-------------|-------------------|
| Evaluation model | Lazy instantiation, eager process execution | Haskell (lazy) meets Faust (eager graph) |
| State management | Explicit, isolated per-instance | Elm (Model/Update), Faust (delay lines) |
| Type system | Implicit, inferred at analysis time | Lua, early Python |
| Execution model | Synchronous dataflow within process blocks | Faust (synchronous), Max/MSP (dataflow) |
| Memory model | Reference-counted shared_ptr with scope-based cleanup | Swift (ARC), Rust (ownership-lite) |
| Concurrency | Parallel semantics, single-threaded execution | Faust (parallel declarations, sequential exec) |

### 1.2 Why Not Compile to Bytecode?

For the alpha, a tree-walking interpreter is the right choice:

- **Simplicity**: AST nodes are already rich data structures; walking them is natural
- **Debuggability**: Errors map 1:1 to source locations (line/column already tracked in AST)
- **Rapid iteration**: Language design is still in flux; bytecode formats freeze semantics too early
- **Real-time constraints are external**: The time-critical path is in modules (Csound, audio engines), not in Idyl's expression evaluation

Later, if performance demands it, a bytecode VM (like Lua's register-based VM or Python's stack-based VM) can be introduced as an optimization pass. The IR layer described below is designed to make that transition smooth.

### 1.3 Comparable Interpreter Architectures

**Faust** (GRAME): Compiles a functional DSL to C++ signal processing code. Idyl shares Faust's philosophy of "everything is a signal" and explicit memory (`@` operator maps to Faust's `mem` / `'` delay primitive). Key difference: Faust is compiled ahead-of-time; Idyl interprets at runtime for live-coding flexibility.

**Lua** (PUC-Rio): Register-based bytecode VM with reference counting. Idyl borrows Lua's philosophy of minimal core + C module integration. The module system (`import("csound")`) mirrors Lua's `require()` with `dlopen`.

**Elm** (Evan Czaplicki): Purely functional with explicit state via The Elm Architecture (Model → Update → View). Idyl's lambda blocks mirror this: `init` = Model, update statements = Update, output expression = View. Elm's "no runtime exceptions" goal aligns with Idyl's graceful degradation philosophy.

**ChucK** (Ge Wang): Time-aware audio language with explicit time advancement (`now`). Idyl shares ChucK's first-class time semantics but differs by being purely functional rather than imperative. ChucK's `=>` (chuck operator) for connecting unit generators parallels Idyl's `|>` for lambda blocks.

**SuperCollider** (James McCartney): Client-server architecture where the language (sclang) schedules events on a real-time audio server (scsynth). Idyl's process blocks + module system mirrors this separation — Idyl schedules, modules execute.

---

## 2. Pipeline Summary

```
Source Code (.idl)
       │
       ▼
┌─────────────┐
│   Lexer     │  (Flex) → Token stream
│   Parser    │  (Bison) → AST (ast.hpp nodes)
└─────────────┘
       │
       ▼
┌─────────────────────┐
│  Semantic Analysis  │  Validate names, types, parameters, scoping
│  (see Step 1)       │  Detect errors before execution
└─────────────────────┘
       │
       ▼
┌─────────────────────┐
│  IR Generation      │  (Optional) Flatten AST into linear operations
│  (see Step 2)       │  Annotate with type info, resolve names
└─────────────────────┘
       │
       ▼
┌─────────────────────┐
│  Environment Setup  │  Build symbol tables, load modules
│  (see Step 3)       │  Register built-in functions
└─────────────────────┘
       │
       ▼
┌─────────────────────┐
│  Process Execution  │  Walk AST/IR for each process block
│  (see Steps 4-8)    │  Instantiate functions, manage state
└─────────────────────┘
       │
       ▼
┌─────────────────────┐
│  Runtime Loop       │  Time engine drives updates
│  (see Step 13)      │  GC cleans up dead instances
└─────────────────────┘
```

The pipeline is **sequential** — each step completes before the next begins. This is important because semantic analysis must catch errors *before* any process block executes (a process block with a typo should not partially run then crash).

---

## 3. Step 1: Semantic Analysis

Semantic analysis is the first post-parse pass. It walks the AST and validates correctness that the grammar cannot enforce. This step produces **no new data structures** — it annotates the existing AST and accumulates errors/warnings.

A separate detailed document exists for this step: see `semantic_analysis.md`.

### 3.1 What It Checks

#### 3.1.1 Name Resolution

Every identifier used in an expression must resolve to either:
- A parameter of the enclosing function
- A previously defined function (in top-level scope)
- A built-in function (`sin`, `cos`, `fmod`, `rnd`, etc.)
- A module binding (`cs`, `osc`, etc.)
- A lambda-local variable (defined in `init:` or update statements)
- The special identifier `dt` (only valid inside temporal functions)

**Example errors:**
```idl
foo(x) = x + y          // ERROR: 'y' is undefined
bar(a) = baz(a)          // ERROR: 'baz' is undefined (unless defined below — see ordering)
```

**Ordering rule**: Idyl uses **declaration-order semantics** (like C, unlike JavaScript hoisting). A function must be defined before it is used. This is simpler to implement and avoids confusing circular definition issues.

Comparable: C requires forward declarations. Lua resolves in declaration order. Haskell allows mutual recursion (more complex).

#### 3.1.2 Parameter Arity Checking

Function calls must match the expected number of arguments, accounting for default parameters.

```idl
add(a, b) = a + b
result = add(1)           // ERROR: add expects 2 arguments, got 1
result = add(1, 2, 3)     // ERROR: add expects 2 arguments, got 3
```

Named arguments (`dt=10ms`) must match a declared parameter name:
```idl
lfo(freq, dt=10ms) = ...
x = lfo(5hz, dt=20ms)     // OK
x = lfo(5hz, foo=20ms)    // ERROR: 'foo' is not a parameter of lfo
```

#### 3.1.3 Type Inference (Soft)

Idyl has no explicit type annotations, but the analyzer infers types to catch obvious mismatches. The type universe is small:

| Type | Description |
|------|------------|
| `number` | Float64 value |
| `time` | Duration in milliseconds (from literals like `100ms`, `2s`, `440hz`, `4b`) |
| `trigger` | Instantaneous event (`!`) |
| `rest` | Absence of event (`.`) |
| `flow` | Ordered sequence |
| `function` | Callable definition |
| `module` | External module handle |
| `unknown` | Not yet determined |

Type inference is **soft** — it warns but does not block execution. This matches Idyl's real-time philosophy: a type mismatch should produce a warning, not a fatal crash that kills a live performance.

Comparable: Lua has no static types at all. Python's mypy is optional. Faust infers signal types (mono/stereo) but is strict.

#### 3.1.4 `dt` and Trigger Parameter Validation

- `dt` as identifier is only meaningful inside temporal functions (functions with a `dt=` parameter and a `|>` block). Using `dt` outside this context produces a warning.
- Trigger parameters (`name!`) must be used in functions with `|>` blocks.
- A function with a `|>` block should have at least one time source (`dt=` or a trigger parameter).

```idl
// WARNING: dt used outside temporal function
plain_func(x) = x * dt

// ERROR: lambda block without time source
broken() = x |> {
    init: { x = 0 }
    x = x + 1
}
```

#### 3.1.5 Flow Validation

- Flow members must have consistent types (all expressions, not mixed definitions)
- Generator ranges must be valid (`start..end` where start ≤ end)
- Repetition markers must reference valid counts

#### 3.1.6 Process Block Validation

- Process blocks can only contain assignments, emit statements, catch blocks, and expressions
- Imports must not appear inside process blocks
- Function definitions must not appear inside process blocks

#### 3.1.7 Scope Validation

- Lambda `init:` variables are visible within the lambda block only (unless `emit`-marked)
- Lambda update variables shadow init variables of the same name
- `emit` variables are accessible in parent scope
- Module bindings are global

### 3.2 Implementation Strategy

The analyzer is a **recursive visitor** over the AST. It maintains:

```cpp
struct semantic_context {
    // Symbol table: name → definition info
    std::unordered_map<std::string, symbol_info> symbols_;

    // Current scope (global, function body, lambda body, init block, process block)
    scope_type current_scope_;

    // Whether we're inside a temporal function
    bool in_temporal_function_ = false;

    // Accumulated errors and warnings
    std::vector<diagnostic> diagnostics_;
};
```

Each AST node type has a corresponding `analyze()` method. The visitor descends into children, building scope as it goes.

### 3.3 Error Reporting

Errors use the line/column information already stored in AST nodes:

```
3.5: error: undefined identifier 'y' in function 'foo'
7.12: warning: 'dt' used outside temporal function
15.1: error: function 'add' expects 2 arguments, got 1
```

Severity levels:
- **Error**: Prevents execution (undefined name, arity mismatch)
- **Warning**: Logged but execution continues (type ambiguity, unused parameter)
- **Info**: Style suggestions (shadowed name, redundant parentheses)

---

## 4. Step 2: IR Generation (Lowering)

The IR (Intermediate Representation) is an **optional** step that transforms the tree-shaped AST into a flatter, more execution-friendly form. For the alpha, the interpreter can walk the AST directly. The IR becomes important when:

- Optimization passes are needed (constant folding, dead code elimination)
- A bytecode VM is introduced later
- Profiling or instrumentation is required

### 4.1 IR Design

The IR is a sequence of **typed operations** in SSA-like (Static Single Assignment) form:

```
%0 = load_param "freq"
%1 = const 2.0
%2 = const 3.14159
%3 = mul %1, %2          // 2 * pi
%4 = load_local "phase"
%5 = mul %4, %3          // phase * 2 * pi
%6 = call_builtin "sin", %5
%7 = return %6
```

Each operation:
- Has a unique result register (`%N`)
- References input registers
- Carries type information (inferred in Step 1)
- Preserves source location for error reporting

Comparable: LLVM IR (but much simpler), Lua's bytecode opcodes, Python's dis output.

### 4.2 Constant Folding

Expressions involving only literals can be evaluated at compile time:

```idl
// Source
pi_approx = 3.14159
two_pi = 2 * pi_approx

// After constant folding
two_pi = 6.28318   // Computed once, never re-evaluated
```

This is particularly valuable for Idyl because many definitions are pure constants (`silence = 0`, `concert_pitch = 440hz`).

### 4.3 When to Introduce IR

**Alpha**: Skip IR; walk AST directly. The overhead is minimal for the expected program sizes.

**Beta**: Introduce IR for:
- Constant folding (significant for flow generators that produce large tables)
- Dead code elimination (functions defined but never called in any process block)
- Type-annotated operations (avoid runtime type checks)

---

## 5. Step 3: Environment & Symbol Table

The environment is the runtime's "database" of all known names and their bindings.

### 5.1 Structure

```cpp
struct environment {
    // Global scope: function definitions, flow definitions, module bindings
    std::unordered_map<std::string, value> globals_;

    // Built-in functions (sin, cos, fmod, rnd, etc.)
    std::unordered_map<std::string, builtin_fn> builtins_;

    // Loaded modules (import("csound") → module handle)
    std::unordered_map<std::string, module_handle> modules_;

    // Library imports (lib("scales.idl") → merged definitions)
    // These are merged into globals_ after loading
};
```

### 5.2 Scope Chain

Idyl has a simple scope hierarchy:

```
Global Scope (function defs, module bindings, flow defs)
  └─ Function Scope (parameters)
       └─ Lambda Scope (init variables, update variables)
            └─ Init Block Scope (initialization-only bindings)
```

Lookup proceeds from innermost to outermost scope, like lexical scoping in Scheme or JavaScript (but without closures in alpha).

Comparable: Lua uses an upvalue chain. Python uses LEGB (Local, Enclosing, Global, Built-in). Idyl's model is closest to **Scheme's lexical scoping** — each scope is a frame on a chain, and lookup walks outward.

### 5.3 Built-in Function Registration

Built-ins are registered at startup before any user code runs:

```cpp
void register_builtins(environment& env) {
    env.builtins_["sin"]   = builtin_sin;
    env.builtins_["cos"]   = builtin_cos;
    env.builtins_["fmod"]  = builtin_fmod;
    env.builtins_["floor"] = builtin_floor;
    env.builtins_["ceil"]  = builtin_ceil;
    env.builtins_["abs"]   = builtin_abs;
    env.builtins_["min"]   = builtin_min;
    env.builtins_["max"]   = builtin_max;
    env.builtins_["pow"]   = builtin_pow;
    env.builtins_["sqrt"]  = builtin_sqrt;
    env.builtins_["rnd"]   = builtin_rnd;
    env.builtins_["rint"]  = builtin_rint;
    env.builtins_["int"]   = builtin_int;
    env.builtins_["float"] = builtin_float;
    env.builtins_["len"]   = builtin_len;
    env.builtins_["bit"]   = builtin_bit;
    // ... etc
}
```

Each built-in is a C++ function pointer with signature:
```cpp
using builtin_fn = value(*)(const std::vector<value>& args);
```

This mirrors Lua's `lua_CFunction` or Python's `PyCFunction` approach.

---

## 6. Step 4: Evaluation Engine

The evaluation engine is the core of the interpreter. It takes an expression (AST node or IR operation) and produces a **value**.

### 6.1 The Value Type

Idyl's runtime value is a tagged union (variant):

```cpp
struct value {
    enum class type { number, time, trigger, rest, flow, function, module, null };

    type tag_;

    union {
        double number_;               // number and time (time stored as ms)
        bool trigger_;                // true = fired, false = not fired
        // rest has no data (tag is sufficient)
    };

    // For complex types, use shared_ptr outside the union
    std::shared_ptr<flow_data> flow_;
    std::shared_ptr<function_instance> function_;
    std::shared_ptr<module_handle> module_;
};
```

Comparable: Lua's `TValue` (tagged union), Ruby's `VALUE` (tagged pointer), Python's `PyObject` (type pointer + refcount).

**Design choice**: Time is stored as a `double` in milliseconds, with the `type::time` tag. This means arithmetic between numbers and time values works naturally (both are doubles), but semantic analysis can warn about mixing them inappropriately.

### 6.2 Expression Evaluation (Tree-Walk)

The evaluator is a visitor that dispatches on AST node type:

```cpp
value evaluate(const expr_ptr& expr, eval_context& ctx) {
    if (auto* lit = dynamic_cast<literal_expr*>(expr.get())) {
        return eval_literal(lit);
    }
    if (auto* id = dynamic_cast<identifier_expr*>(expr.get())) {
        return ctx.lookup(id->identifier_->name_);
    }
    if (auto* binop = dynamic_cast<binary_op_expr*>(expr.get())) {
        value left = evaluate(binop->op_->left_, ctx);
        value right = evaluate(binop->op_->right_, ctx);
        return apply_binop(binop->op_->op_, left, right);
    }
    if (auto* call = dynamic_cast<function_call_expr*>(expr.get())) {
        return eval_call(call, ctx);
    }
    // ... etc for all expression types
}
```

This is the classic tree-walking approach used by early Ruby (MRI), Tcl, and early JavaScript engines.

### 6.3 Operator Semantics

Arithmetic operators work on numbers and time values:

| Operation | Left Type | Right Type | Result Type | Behavior |
|-----------|-----------|------------|-------------|----------|
| `+` | number | number | number | Addition |
| `+` | time | time | time | Duration addition |
| `*` | time | number | time | Scale duration |
| `*` | number | time | time | Scale duration |
| `/` | time | time | number | Ratio |
| `/` | time | number | time | Divide duration |
| `&` `\|` `^` `<<` `>>` `~` | number | number | number | Bitwise (integer part) |
| `<` `>` `==` etc. | any | any | number | Comparison (0 or 1) |

Trigger and rest have special semantics:
- `trigger + trigger` → `trigger`
- `trigger + rest` → `trigger`
- `rest + rest` → `rest`
- Trigger in arithmetic context is treated as `1`, rest as `0`

### 6.4 Ternary Evaluation

The ternary operator `opt0; opt1; opt2 ? condition` evaluates to:

1. Evaluate `condition` → get numeric value N
2. Compute index = `int(N) % option_count`
3. Evaluate and return `options[index]`

**Important**: Only the selected option is evaluated (lazy/short-circuit). This prevents triggering side effects in non-selected branches. Comparable to Faust's `select2`/`selectN` but with arbitrary option count.

---

## 7. Step 5: Function Instantiation & State

This is where Idyl diverges most from typical interpreters. Functions in Idyl are not just callable code — they can be **stateful instances** with their own persistent memory.

### 7.1 Pure Functions (No State)

Pure functions have no `|>` block. They are evaluated freshly on every call with no persistent state:

```idl
square(x) = x * x
```

Implementation: Create a new `eval_context` with parameters bound, evaluate the body expression, return the result. No instance creation needed.

Comparable: Every function call in Haskell, Elm, or mathematical functions in Faust.

### 7.2 Temporal Functions (Stateful Instances)

Functions with `|>` blocks are **instantiated** — each call site creates a unique instance with its own state:

```idl
lfo(freq, dt=10ms) = sin(phase * 2 * pi) |> {
    init: { phase = 0 }
    phase = fmod(phase + 1/freq, 1)
}
```

When `lfo(5hz)` is called from a process block:

1. **Allocate** a new `function_instance`
2. **Run init block**: Set `phase = 0` in the instance's local state
3. **Register with time engine**: Subscribe to updates every 10ms
4. **Return**: The instance's output expression (`sin(phase * 2 * pi)`)

The instance struct:

```cpp
struct function_instance {
    // The function definition (shared, immutable)
    std::shared_ptr<function_definition> definition_;

    // Instance-local state (mutable)
    std::unordered_map<std::string, value> state_;

    // Bound parameters (immutable after creation)
    std::unordered_map<std::string, value> params_;

    // Current output value (recomputed on each update)
    value output_;

    // Time subscription info
    double dt_ms_ = 0;      // 0 if trigger-only
    bool is_active_ = true;

    // Emitted values (accessible to parent scope)
    std::unordered_map<std::string, value> emitted_;
};
```

### 7.3 The Update Cycle

On each time tick (dt expires or trigger fires):

1. Evaluate all update statements in the lambda block, reading from current state
2. Write new values to state (double-buffered to avoid order-dependent evaluation)
3. Evaluate the output expression with updated state
4. Store the new output value
5. Propagate `emit`-marked values to parent scope

**Double buffering** is critical for Idyl's parallel semantics. Within a single update:
```idl
a = b       // reads current b, writes to next a
b = a       // reads current a (NOT the just-written a), writes to next b
```

This is how Faust handles it (all delay lines update simultaneously) and how synchronous hardware description languages (Verilog, VHDL) work.

Comparable: Faust's synchronous one-sample computation. Elm's immutable model update (new model replaces old atomically). Digital circuit simulation (all flip-flops clock on the same edge).

### 7.4 Instance Identity and Deduplication

Each call site creates a unique instance:

```idl
process: {
    a = lfo(1hz)    // Instance #1
    b = lfo(1hz)    // Instance #2 (same params, different state)
    c = a           // NOT a new instance — c references Instance #1
}
```

The interpreter tracks instances by **call site** (source location), not by function name or parameter values. This ensures two calls at different lines always produce independent instances.

---

## 8. Step 6: Flow Runtime

Flows are Idyl's structured data type — ordered, indexed sequences with named members.

### 8.1 Flow Representation

```cpp
struct flow_data {
    // Named members (empty name for single-member flows)
    struct member {
        std::string name_;
        std::vector<value> elements_;
    };

    std::vector<member> members_;
    size_t logical_length_;  // Effective length (for wrapping)
};
```

### 8.2 Flow Construction

**Literal flows** (`[1 2 3 4]`): Elements are evaluated once at construction time and stored.

**Generator flows** (`[i = 0..N : expr]`): The range is evaluated, then the body expression is evaluated for each value of the iteration variable. Results are stored as a literal vector.

```idl
sine_table(size) = [i = 0..size : sin(i / size * 2 * pi)]
// At construction: evaluate sin(0/256 * 2pi), sin(1/256 * 2pi), ... → store 256 doubles
```

Comparable: Python list comprehensions (`[sin(i/n * 2*pi) for i in range(n)]`), Haskell list generators (`[sin (fromIntegral i / n * 2 * pi) | i <- [0..n]]`).

### 8.3 Flow Indexing

Flows support three indexing modes, automatically detected by the indexer type:

| Indexer Type | Behavior | Example |
|-------------|----------|---------|
| `trigger` | Advance to next element on each trigger | `pattern(metronome(500ms))` |
| `integer` (number with no fractional part) | Direct index, wrapping | `pattern(counter(metro))` |
| `float` (number with fractional part) | Interpolated sweep, wrapping 0→1 to 0→length | `pattern(phasor(1hz))` |

Implementation:
```cpp
value flow_index(const flow_data& flow, const value& indexer, flow_read_state& state) {
    if (indexer.tag_ == value::type::trigger && indexer.trigger_) {
        // Advance internal counter
        state.position_ = (state.position_ + 1) % flow.logical_length_;
        return flow.members_[0].elements_[state.position_];
    }
    if (is_integer(indexer.number_)) {
        int idx = static_cast<int>(indexer.number_) % flow.logical_length_;
        return flow.members_[0].elements_[idx];
    }
    // Float: map [0, 1) → [0, length)
    double mapped = fmod(indexer.number_, 1.0) * flow.logical_length_;
    // Could add interpolation here for smooth sweeping
    int idx = static_cast<int>(mapped) % flow.logical_length_;
    return flow.members_[0].elements_[idx];
}
```

### 8.4 Multi-Member Flow Access

The `::` operator accesses named members:

```idl
flow drum = { kick: [! . . .], snare: [. . ! .] }
k = drum::kick(metro)
s = drum::snare(metro)
```

Each member maintains its own read position (for trigger-based indexing), but they share the flow's logical time base.

---

## 9. Step 7: Module System

Modules bridge Idyl to the outside world (Csound, OSC, MIDI, filesystem, etc.).

### 9.1 Module Interface

A module is a shared library (`.so` / `.dylib` / `.dll`) that exposes a C API:

```c
// Module entry point
idyl_module* idyl_module_init(idyl_runtime* runtime);

// Module function table
typedef struct {
    const char* name;
    idyl_value (*function)(idyl_value* args, int argc);
} idyl_module_function;

typedef struct {
    const char* name;
    idyl_module_function* functions;
    int function_count;
} idyl_module;
```

Comparable: Lua's `luaL_Reg` function registration table. Python's `PyMethodDef`. Ruby's `rb_define_method`.

### 9.2 Module Loading

```idl
cs = import("csound")
```

Steps:
1. Search module paths for `csound.so` (or platform equivalent)
2. `dlopen` the shared library
3. Call `idyl_module_init()` to get the function table
4. Register all module functions under the `cs` namespace
5. Return a module handle value

### 9.3 Module Function Calls

```idl
osc::send(sender, "/note", 60)
```

Translates to:
1. Look up `osc` in module table → module handle
2. Look up `send` in module's function table → native function pointer
3. Marshal Idyl values to C values
4. Call the native function
5. Marshal the return value back to an Idyl value

### 9.4 Safety

Module calls are wrapped in try/catch to prevent native crashes from killing the interpreter:

```cpp
value call_module_function(module_handle& mod, const std::string& func_name,
                           const std::vector<value>& args) {
    try {
        auto* fn = mod.lookup(func_name);
        if (!fn) {
            report_warning("Module function '" + func_name + "' not found");
            return value::null();
        }
        return fn->call(args);
    } catch (const std::exception& e) {
        report_warning("Module error: " + std::string(e.what()));
        return value::null();
    }
}
```

Comparable: Lua's `lua_pcall` (protected call). Erlang's supervisor model (let it crash, but contain the blast radius). This is essential for live-coding — a buggy module should not take down the whole performance.

---

## 10. Step 8: Process Block Execution

Process blocks are Idyl's entry points — the only code that actually runs.

### 10.1 Execution Model

When a file is loaded:

1. Parse and build AST
2. Run semantic analysis
3. Build environment (register functions, load modules)
4. Find all `process` blocks
5. Execute each unnamed `process:` block immediately
6. Named process blocks (`process intro, dt=8s:`) are registered for later composition

### 10.2 Inside a Process Block

Each statement in a process block is an **instantiation** or **binding**:

```idl
process: {
    my_lfo = lfo(5hz, dt=10ms)          // Instantiate lfo, bind as my_lfo
    freq = 440hz * (1 + my_lfo * 0.1)   // Expression referencing my_lfo
}
```

All statements are evaluated conceptually in parallel:
- `my_lfo` is instantiated
- `freq` depends on `my_lfo`, so it re-evaluates every time `my_lfo` updates

The interpreter builds a **dependency graph** from the process block:

```
my_lfo ──→ freq
```

When `my_lfo` updates (every 10ms), `freq` is recomputed. This is reactive dataflow, similar to a spreadsheet or an Rx Observable chain.

Comparable: Max/MSP's patch execution (data flows from outlets to inlets). Faust's signal graph (every sample, all nodes evaluate). React's component re-rendering (state change triggers dependent recalculation).

### 10.3 Named Process Sequencing

Named process blocks with duration enable composition sequencing:

```idl
process intro, dt=8s: { ... }
process verse, dt=16s: { ... }
```

The time engine manages the lifecycle: `intro` runs for 8 seconds, then `verse` starts. This is detailed in the Time Engine document.

### 10.4 Catch Blocks

Catch blocks register callbacks for lifecycle events:

```idl
process: {
    voice = some_function(args)
    voice catch end: {
        cleanup_variable = 0
    }
}
```

The interpreter:
1. Evaluates the expression before `catch`
2. Registers the catch handler with the instance's lifecycle manager
3. When the event fires (e.g., function reaches natural end), executes the handler

---

## 11. Step 9: Memory Management & Garbage Collection

### 11.1 Strategy: Reference Counting with Scope-Based Cleanup

Idyl uses **reference counting** via C++ `shared_ptr`, augmented with scope-based deterministic cleanup. This is not garbage collection in the traditional sense (no mark-and-sweep, no generational collector), but it achieves the same goal: unused memory is reclaimed.

**Why reference counting over tracing GC?**

| Concern | Reference Counting | Tracing GC (Mark-and-Sweep) |
|---------|-------------------|---------------------------|
| Latency | Deterministic, incremental | Unpredictable pauses |
| Real-time suitability | Excellent (no stop-the-world) | Poor unless concurrent |
| Implementation complexity | Low (`shared_ptr` does most work) | High (root scanning, write barriers) |
| Cyclic references | Must handle manually | Handled automatically |
| Memory overhead | Per-object counter | Metadata + free list |

For a real-time audio scheduling language, **deterministic latency is paramount**. A GC pause during a live performance is unacceptable. Reference counting guarantees that deallocation happens incrementally, at predictable times.

Comparable:
- **Swift** uses ARC (Automatic Reference Counting) — same philosophy, same trade-offs
- **Rust** uses ownership + borrowing (compile-time, zero overhead) — ideal but harder to implement in an interpreter
- **CPython** uses reference counting + cyclic GC as backup
- **Objective-C** transitioned from manual retain/release to ARC

### 11.2 Lifecycle of Values

```
Creation:     shared_ptr<T> allocated (refcount = 1)
    │
Binding:      Assigned to symbol table entry (refcount++)
    │
Reference:    Expression evaluation reads it (temporary refcount++)
    │
Scope exit:   Process block ends, function returns (refcount--)
    │
Destruction:  refcount reaches 0 → destructor called → memory freed
```

### 11.3 Handling Circular References

Circular references are unlikely in Idyl's design (functions don't capture mutable closures, state is isolated per-instance), but they can theoretically occur through:

- Two function instances that emit values to each other
- Flow elements that reference the flow itself

**Mitigation strategies (in priority order):**

1. **Design prevention**: The language semantics make most cycles impossible. Functions cannot reference themselves (no recursion in alpha), and flows are evaluated eagerly.

2. **Weak references for parent pointers**: If an instance needs to reference its parent context (e.g., for `emit` propagation), use `weak_ptr` instead of `shared_ptr`.

3. **Cycle breaker on scope exit**: When a process block ends, all its bindings are explicitly cleared, breaking any potential cycles.

4. **Fallback cycle detector** (if needed later): A simple mark-and-sweep pass that runs infrequently (e.g., between process blocks) to catch any leaked cycles. This would be non-real-time but tolerable since it only runs at composition boundaries.

### 11.4 Instance Pool and Reuse

For temporal functions that are created and destroyed frequently (e.g., polyphonic voices triggered by events), object pooling avoids repeated allocation:

```cpp
struct instance_pool {
    std::vector<std::unique_ptr<function_instance>> free_list_;

    function_instance* acquire(const function_definition& def) {
        if (!free_list_.empty()) {
            auto* inst = free_list_.back().release();
            free_list_.pop_back();
            inst->reset(def);  // Reinitialize state
            return inst;
        }
        return new function_instance(def);
    }

    void release(function_instance* inst) {
        inst->deactivate();
        free_list_.push_back(std::unique_ptr<function_instance>(inst));
    }
};
```

Comparable: Game engine entity pools. Apache APR memory pools. ChucK's shred recycling. This pattern is critical for avoiding GC pressure in real-time systems.

---

## 12. Step 10: Error Handling at Runtime

### 12.1 Philosophy: Degrade, Don't Crash

Idyl is designed for live performance. A runtime error should **never** crash the interpreter or silence the output. Instead:

1. **Log the error** with source location
2. **Substitute a safe default** (0 for numbers, rest for triggers, empty for flows)
3. **Continue execution**

This is the "show must go on" philosophy, shared by:
- **Erlang/OTP**: "Let it crash" at the process level, supervisors restart
- **Max/MSP**: Errors produce console warnings, audio continues
- **SuperCollider**: Server continues even if client language crashes
- **Web browsers**: JavaScript errors don't bring down the page

### 12.2 Error Categories

| Error | Default Value | Severity |
|-------|--------------|----------|
| Division by zero | `0` | Warning |
| NaN result | `0` | Warning |
| Infinity result | `0` | Warning |
| Undefined function call | `0` (rest for triggers) | Error (logged) |
| Module function failure | `null` | Warning |
| Flow index out of range | Wrapped index | Silent (by design) |
| Type mismatch in operation | Coerce to number | Warning |

### 12.3 Implementation

```cpp
value safe_divide(double a, double b, const source_location& loc) {
    if (b == 0.0) {
        report_warning(loc, "Division by zero, returning 0");
        return value::number(0.0);
    }
    double result = a / b;
    if (std::isnan(result) || std::isinf(result)) {
        report_warning(loc, "Invalid arithmetic result, returning 0");
        return value::number(0.0);
    }
    return value::number(result);
}
```

### 12.4 Error Accumulation

Runtime errors are collected in a ring buffer (not an unbounded list) to prevent memory growth from repeated errors:

```cpp
struct error_log {
    static constexpr size_t MAX_ERRORS = 1024;
    std::array<diagnostic, MAX_ERRORS> errors_;
    size_t write_pos_ = 0;
    size_t count_ = 0;

    void push(diagnostic d) {
        errors_[write_pos_ % MAX_ERRORS] = std::move(d);
        write_pos_++;
        count_ = std::min(count_ + 1, MAX_ERRORS);
    }
};
```

---

## 13. Time Engine (Stub)

The time engine is the heart of Idyl's runtime — it drives all temporal function updates, trigger dispatching, and process block sequencing.

**This section is intentionally minimal.** The time engine will be designed and documented separately, as it involves:

- High-resolution system clock integration
- Audio engine callback synchronization
- Trigger scheduling and jitter management
- Named process block sequencing and composition
- `dt` resolution and drift compensation
- Beat/tempo clock management

### 13.1 Interface to Interpreter

The interpreter only needs to know this about the time engine:

```cpp
struct time_engine {
    // Register a temporal function instance for periodic updates
    void subscribe(function_instance* inst, double dt_ms);

    // Register a trigger source
    void subscribe_trigger(function_instance* inst, trigger_source* src);

    // Unsubscribe (instance being destroyed)
    void unsubscribe(function_instance* inst);

    // Main loop tick (called externally by host or audio callback)
    void tick(double current_time_ms);
};
```

The interpreter creates instances and registers them. The time engine calls back into the interpreter's update mechanism when it's time to evaluate. This clean separation means the time engine can be swapped out (system clock vs. audio callback) without changing the interpreter.

### 13.2 Two Time Modes

1. **System clock mode**: A high-resolution timer (`std::chrono::high_resolution_clock` or platform equivalent) drives ticks. Good for scheduling, OSC, general-purpose use.

2. **Audio callback mode**: The audio engine (Csound, PortAudio, etc.) drives ticks via its buffer callback. Guarantees sample-accurate timing but couples to audio block size.

Both modes present the same interface to the interpreter. The choice is made at startup (CLI flag or configuration).

---

## 14. Implementation Order

A suggested order for building the interpreter, from foundations to full features:

### Phase 1: Core Evaluation (Weeks 1-2)
1. **Value type** — tagged union with number, time, trigger, rest
2. **Built-in functions** — sin, cos, fmod, rnd, basic math
3. **Expression evaluator** — tree-walk over AST, all operators
4. **Environment** — symbol table with scope chain
5. **Pure function calls** — parameter binding, body evaluation

**Milestone**: Can evaluate `square(3)` → `9`, `sin(3.14159)` → `~0`

### Phase 2: Process & State (Weeks 3-4)
6. **Process block execution** — iterate statements, build bindings
7. **Function instantiation** — create instances for temporal functions
8. **Init block execution** — initialize instance state
9. **Update cycle** — evaluate lambda body, double-buffered state
10. **Basic time engine stub** — manual tick for testing

**Milestone**: Can run `process: { x = lfo(1hz) }` and observe state changes through manual ticking

### Phase 3: Flows & Data (Weeks 5-6)
11. **Flow construction** — literal and generator
12. **Flow indexing** — trigger, integer, float modes
13. **Multi-member flows** — `::` access
14. **Repetition and restart markers**

**Milestone**: Can construct `[i = 0..8 : i * 2]` and index it with different methods

### Phase 4: Modules & Integration (Weeks 7-8)
15. **Module loader** — dlopen, function table registration
16. **Module call marshalling** — value conversion to/from C
17. **Catch blocks** — event registration and callback
18. **Error handling** — safe defaults, ring buffer logging

**Milestone**: Can `import("osc")` and send a message

### Phase 5: Semantic Analysis (Parallel with above)
19. **Name resolution pass**
20. **Arity checking pass**
21. **Type inference pass** (soft warnings)
22. **Scope validation pass**

**Milestone**: Meaningful error messages before any code runs

### Phase 6: Polish & Time Engine
23. **Time engine** (separate document)
24. **Named process sequencing**
25. **Instance pooling**
26. **Performance profiling**

---

## Appendix A: Glossary

| Term | Definition |
|------|-----------|
| **Tree-walking** | Interpreter that evaluates by recursively traversing the AST |
| **IR** | Intermediate Representation — a flattened, optimizable form between AST and execution |
| **SSA** | Static Single Assignment — IR form where each variable is assigned exactly once |
| **ARC** | Automatic Reference Counting — memory management via shared_ptr / refcount |
| **Double buffering** | Reading from current state while writing to next state, then swapping |
| **Instance** | A runtime copy of a temporal function with its own isolated state |
| **Dataflow** | Execution model where computation is driven by data availability, not sequential order |
| **Reactive** | System that recomputes dependent values when inputs change |

## Appendix B: AST Node → Interpreter Action Mapping

| AST Node | Interpreter Action |
|----------|-------------------|
| `program` | Entry point; iterate top-level statements |
| `function_definition` | Register in global environment |
| `flow_definition` | Evaluate and register in global environment |
| `process_block` | Execute immediately (or register if named) |
| `library_import` | Parse and merge imported file's definitions |
| `module_import` | dlopen and register module |
| `assignment` | Evaluate RHS, bind in current scope |
| `literal_expr` | Return corresponding value |
| `identifier_expr` | Look up in scope chain |
| `binary_op_expr` | Evaluate both sides, apply operator |
| `unary_op_expr` | Evaluate operand, apply operator |
| `ternary_op_expr` | Evaluate condition, select and evaluate chosen option |
| `function_call_expr` | Resolve function, bind args, evaluate body (or instantiate) |
| `memory_op_expr` | Access delay buffer in instance state |
| `flow_literal_expr` | Construct flow_data from elements |
| `generator_expr_node` | Iterate range, evaluate body for each, construct flow_data |
| `flow_access_expr` | Index into flow member |
| `module_access_expr` | Call native module function |
| `catch_block` | Register event handler with instance lifecycle |
| `lambda_block` | Attached to function_instance; drives update cycle |
| `init_block` | Run once at instance creation |