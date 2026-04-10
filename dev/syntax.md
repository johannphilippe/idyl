# Idyl Language Specification - Attempt 04

**Status**: Pre-Alpha Specification (Ready for Initial Parser & Engine Implementation)

**Language Purpose**: Idyl is a **functional, time-aware domain-specific language** designed for scheduling, composition, and real-time processing. It bridges the gap between live coding expressiveness and system reliability, emphasizing:
- **First-class temporal semantics** (time literals, triggers, clock-based scheduling)
- **Purely functional evaluation** with explicit state management
- **Synchronous data flow** enabling both clock-driven and trigger-driven computation
- **Pattern composition** through flows and higher-order functions
- **Modular integration** with external engines (Csound, OSC, etc.) via functional APIs

---

## Part I: Core Specification (Alpha Target)

This section describes the stable, implementable features for the initial pre-alpha release.

---

## Table of Contents (Core Specification)

1. [Language Fundamentals](#1-language-fundamentals)
   - [Comments](#11-comments)
   - [Core Principles](#12-core-principles)
2. [Function Definitions](#2-function-definitions)
3. [Primitive Types & Literals](#3-primitive-types--literals)
4. [Temporal Functions & State](#4-temporal-functions--state)
5. [Operators & Expressions](#5-operators--expressions)
6. [Control Flow](#6-control-flow)
7. [Flows & Sequences](#7-flows--sequences)
8. [Modules & Integration](#8-modules--integration)
9. [Process Blocks](#9-process-blocks)
10. [Execution Model](#10-execution-model)
11. [Built-in Functions](#11-built-in-functions)
12. [Event Handling & Lifecycle](#12-event-handling--lifecycle)

---

## 1. Language Fundamentals

### 1.1 Comments

Idyl uses **C++ style comments**:

- **Single-line comments**: `// comment text` - extends to end of line
- **Multiline comments**: `/* comment text */` - can span multiple lines

Comments are completely ignored by the parser and have no semantic meaning.

**Examples:**
```idyl
// This is a single-line comment
flow main = [1, 2, 3]

/* This is a multiline comment
   that spans multiple lines */
process: {
    x = 1  // can appear at end of line too
}

/* Nested-style comments are not differentiated;
   the first */ closes the comment block */
```

### 1.2 Core Principles

**Idyl is:**
- **Stateless by default**: Variables do not exist; only immutable function definitions
- **Explicitly stateful when needed**: The `emit` keyword makes state mutations visible and intentional
- **Temporally aware**: Time is a first-class concept, not an afterthought
- **Functional at the core**: Higher-order functions, composition, and declarative evaluation
- **Synchronous at runtime**: No invisible asynchrony; all timing is explicit

**Key Contrast**:
- Traditional imperative languages hide time and state
- Idyl makes both explicit and central to the language design

### 1.3 Compilation Model

- A file is **parsed and compiled** into function models
- **Function definitions** are eagerly compiled but lazily instantiated (on first use)
- **Process blocks** are the only executable entry points; they run when the file loads
- **No global state**: Each file compilation is independent and side-effect-free until a process block executes
- **Global scope is declaration-only**: only function/constant definitions, flow definitions, and `lib()`/`module()` imports are valid at the top level. Bare expression calls are a parse error outside process blocks and lambda blocks.

### 1.4 Namespace & Module Resolution

- **Idyl libraries** (`.idyl` files): Loaded globally via `lib()` keyword (import statements only in global scope)
- **External modules** (compiled binaries): Loaded into namespace via `module("path")` (import statements only in global scope)
- **Function shadowing**: Later definitions override earlier ones (with warning)
- **Lexical scoping**: Function parameters shadow outer scope naturally

---

## 2. Function Definitions

### 2.1 Basic Function Definition

Functions are defined using `name(parameters) = expression` syntax. All definitions are immutable.

**Syntax**:
```
function_name(param1, param2, ...) = expression
```

**Examples**:
```idl
// Constants
silence = 0
base_frequency = 440hz

// Single parameter
square(x) = x * x

// Multiple parameters
blend(sig_a, sig_b, blend_factor) = sig_a * (1 - blend_factor) + sig_b * blend_factor

// Composition
chain_transform(input, scale, offset) = 
    (input * scale) + offset
```

### 2.2 Default Parameters

Functions can specify default values for parameters.

**Syntax**:
```
function_name(param1, param2=default_value) = expression
```

**Examples**:
```idl
// With defaults
fade_out(duration, curve=linear) = 
    envelope(duration, 1, 0, curve)

// Calling with and without defaults
env1 = fade_out(500ms)
env2 = fade_out(500ms, exponential)
```

Functions expecting default parameters can be called in three ways :
- Not setting default argument (which values is set to default)
- Specifying it explicitely : `fade_out(500ms, curve=exponential)` 
- Setting it implicitely, as a normal positional argument : `fade_out(500ms, exponential)`

In the third case, `exponential` is automatically setting `curve` parameter, because this parameter has 2nd position in parameter list.

### 2.3 Function Behavior on Values vs. Flows

**Broadcasting Principle**: Functions automatically work on both scalar values and flows by applying element-wise:

```idl
add(a, b) = a + b

// Scalar operation
result = add(1, 2)  // result = 3

// Flow operation (element-wise)
flow sum = add([0, 3, 5], [5, 2, 0])  // sum = [5, 5, 5]
```

This eliminates the need for separate map/fold operations; operators are polymorphic by default.

---

## 3. Primitive Types & Literals

Idyl recognizes the following primitive types and literals:

### 3.1 Time Literals

| Literal | Meaning | Conversion |
|---------|---------|-----------|
| `Xms` | Milliseconds | Direct (X ms) |
| `Xs` | Seconds | X × 1000 ms |
| `Xhz` | Frequency (Hertz) | 1000 / X ms (period) |
| `Xb` | Beats (relative to tempo) | Scaled by current tempo |

**Rule**: All time values are internally normalized to milliseconds.

**Examples**:
```idl
short_delay = 100ms
duration = 2.5s
note_freq = 440hz
musical_timing = 4.0b
combined = 100ms + 2s  // 2100ms
```

### 3.2 Numbers

64-bit floating-point values. Integer conversion via `int()` function enables bitwise operations.

```idl
amplitude = 0.5
frequency = 440.0
```

### 3.3 Triggers & Rest

**Trigger** (`!`): Represents an instantaneous event/pulse.
**Rest** (`_`): Represents silence or lack of event (opposite of trigger).

```idl
pulse = !           // A trigger
silence = _         // Rest/no event
combined = pulse + silence  // Just the pulse
```

### 3.4 Flows

Ordered sequences with indexed access. Defined in dedicated Flows section (Section 7).

---

## 4. Temporal Functions & State

### 4.1 Temporal Functions (Stateful)

Functions requiring internal state use the **lambda block** (`|>`) syntax. The `|>` operator must immediately follow the expression (on the same line or on the next line).

**Syntax**:
```
function_name(param1, param2, ..., dt=time_unit) = output_value |> {
    init: { initialization_code }
    update_code_here
}
```

**Time Basis**:
- `dt=10ms`: Clock-driven update every 10 milliseconds
- Trigger parameter (marked with `!` postfix): Trigger-driven (updates whenever that trigger fires)
- Both `dt=` and trigger parameter: Hybrid (updates on both; inspect trigger to distinguish)

**Parameter Syntax**:
- Normal parameter: `name`
- Time-based parameter: `dt=value` (clock-driven)
- Trigger parameter: `name!` (postfixed `!` with no space indicates trigger input; context distinguishes from standalone `!` trigger literal)

**Operator Note**: `|>` (pipe-arrow) represents flow through time.

**Examples**:

#### Clock-Driven Updates
```idl
// LFO (Low-Frequency Oscillator) for parameter modulation
lfo(freq, dt=10ms) = sin(phase * 2 * pi) |> {
    init: { phase = 0 }
    phase = fmod(phase + 1/freq, 1)
}

// Exponential smoother for control signals
smooth_control(input, smoothing_factor, dt=10ms) = smoothed |> {
    init: { state = 0 }
    smoothed = state
    state = state + (input - state) * smoothing_factor
}
```

#### Pure Trigger-Driven Updates
```idl
// Counter increments on each trigger
counter(spike!) = count |> {
    init: { count = 0 }
    count = count + 1
}

// Sample-and-hold (captures input when trigger fires)
sample_hold(input, capture!) = held_value |> {
    init: { held_value = 0 }
    held_value = input
}
```

#### Multiple Triggers
```idl
// Counter increments on spike, resets on reset trigger
super_counter(spike!, reset!) = count |> {
    init: { count = 0 }
    count = count + (0; 1 ? spike)
    count = count; 0 ? reset
}
```

#### Hybrid: Trigger + Clock Updates
```idl
// Trigger increments, clock provides steady decay
smart_counter(spike!, dt=100ms) = count |> {
    init: { count = 0 }
    count = count + (0; 1 ? spike) - 0.01
}
```

**Semantic Distinction**: 
- **Clock-driven** (`dt=10ms`): Updates at fixed intervals; execute all update code
- **Pure trigger-driven** (trigger parameter with `!`): Updates only when trigger fires; execute all update code unconditionally
- **Hybrid** (both trigger param + dt): Updates on both events; inspect trigger value `(_; 1 ? trigger)` to distinguish which event fired

### 4.2 Initialization Block

Runs once when the function is first instantiated.

**Behavior**:
- Executed immediately at instantiation
- Sets up initial state variables
- Optional (omit if no initialization needed)
- Bare expression calls (without assignment) are valid inside `init` and in the lambda update body — they execute for side effects (e.g. logging, external calls)

**Example**:
```idl
delay_line(input, delay_time, dt=10ms) = delayed |> {
    init: {
        buffer_size = int(delay_time / dt)
        buffer = [i = 0..buffer_size : 0]
        write_pos = 0
    }
    delayed = buffer[wrap(write_pos - delay_time/dt, buffer_size)]
    buffer[wrap(write_pos, buffer_size)] = input
    write_pos = write_pos + 1
}
```

### 4.3 State Emission

The `emit` keyword marks variables for persistence and propagates them outside the function body to parent scope. Variables defined inside the lambda block are only visible within that function; `emit` makes them accessible externally.

**Syntax**:
```
emit variable_name = value
```

**Behavior**:
- Variables inside lambda block without `emit` are local (visible only within the function)
- Variables with `emit` retain their value across updates and are accessible in parent scope
- Makes state mutation and scope propagation explicit and auditable
- Currently reserved for future use (not yet fully integrated in alpha)

**Examples**:
```idl
// Accumulator
running_sum(input, dt=10ms) = total |> {
    init: { emit total = 0 }
    total = total + input
}

// Differentiator
differentiate(input, dt=10ms) = derivative |> {
    init: { emit prev = 0 }
    derivative = (input - prev) / (dt / 1000)
    prev = input
}
```

### 4.4 Memory Operators (`@`)

Compact delay syntax inspired by Faust:

**Syntax**:
```
@(expression)        // One-sample delay (previous value)
@(expression, N)     // N-sample delay
```

**Examples**:
```idl
// One-sample delay
previous = @(signal)

// N-sample delay
delayed = @(signal, 8)

// Differentiator using @
diff(signal, dt=10ms) = (signal - @(signal)) / (dt / 1000)
```

---

## 5. Operators & Expressions

### 5.1 Arithmetic

`+`, `-`, `*`, `/`, `%` (modulo)

```idl
sum = 3 + 4
product = 2 * 5
modulo = 17 % 5
```

### 5.2 Comparison

`<`, `>`, `<=`, `>=`, `==`, `!=`

**Return**: `1` (true) or `0` (false). No boolean type.

```idl
is_loud = amplitude > 0.5
in_range = (x > 0) * (x < 1)
```

### 5.3 Ternary & Multi-Way Selection

**Syntax**:
```
option0; option1; option2; ... ? condition
```

**Note**: Options are separated by `;` (semicolon), not `,`, to avoid ambiguity with function argument separators.

**Behavior**:
- If `condition = 0`, returns `option0`
- If `condition = N`, returns `optionN`
- **Out-of-range behavior**: Wraps around (modulo) to stay within valid options

**Examples**:
```idl
// Two-way
envelope_type(is_fast) = linear; exponential ? is_fast

// Multi-way (wraps)
play_note(degree) = 
    note_c; note_d; note_e; note_f; note_g ? (degree % 5)

// State selection
state_output(state) = 
    idle_out; attack_out; decay_out; sustain_out ? state
```

### 5.4 Bitwise Operations

For `int()`-converted values: `&` (AND), `|` (OR), `^` (XOR), `<<`, `>>`, `~` (NOT)

```idl
a = int(12)
b = int(10)
result = a & b
```

---

## 6. Control Flow

### 6.1 Conditional Selection

Use ternary operator (Section 5.3); no `if/else` keywords.

```idl
// Simple condition
is_positive(x) = 0; 1 ? (x > 0)

// Nested conditions
safe_divide(a, b) = 0; (a / b) ? (b != 0)
```

### 6.2 Iteration via Functional Generators

Instead of imperative loops, use **generator expressions** to create flows:

**Syntax**:
```
[variable = range : expression]
```

**Examples**:
```idl
// Sine table
sine_table(size) = [i = 0..size : sin(i / size * 2 * pi)]

// Arithmetic sequence
intervals = [i = 0..8 : i * 2]

// Harmonic series
harmonics(fundamental) = [h = 1..16 : fundamental * h]
```

**Why Not Imperative Loops?** Generators are declarative, composable, and map naturally to flows (Idyl's core data structure). Imperative loops would introduce unnecessary complexity.

---

## 7. Flows & Sequences

A **flow** is a special function that organizes data into named, indexed sequences.

### 7.1 Flow Definition

**Syntax**:
```
flow flow_name(params) = {
    member1: [values_or_generator],
    member2: [values_or_generator],
    ...
}
```

**For single-member flows**:
```
flow flow_name(params) = [val1, val2, val3, ...]
```

### 7.2 Flow Semantics

- **Automatic wrapping**: Flows wrap around when indexed beyond their length
- **Synchronized members**: All members have the same logical length; shorter members repeat
- **Repetition syntax**: `[...] |N||` repeats a section N times; `||` restarts counter

**Examples**:
```idl
// Single member, no parenthesis needed when no arguments
flow notes = [60, 62, 64]

// Multi-member
flow drum_pattern = {
    kick: [ ! _ _ _ ]
    snare: [ _ _ ! _ ]
    hat: [ ! ! ! ! ]
}

// With repetition
flow verse = {
    bass: [ 50  |4|| 53 ]
    melody: [ 60 62 64 65 :|2 || 61 62 66 68 |3|| ]
}

// With restart
flow complex = {
    rhythm: [ ! |2|| || _ ! |2|| ]
}

// Programmatic generation
flow lookup_table(size, func) = 
    [i = 0..(size-1) : func(i / size)]
```

### 7.3 Flow Access Methods

#### Trigger-Based Indexing
```idl
metro = metronome(500ms)
current_note = drum_pattern.kick[metro]
```

#### Integer Indexing
```idl
step = counter(metro)
current_note = drum_pattern.kick[step]
```

#### Float Indexing (Smooth Sweep)
```idl
phase = phasor(1hz)
current_note = drum_pattern.kick[phase]
```

### 7.4 Flow Generation Features

#### Bitwise Patterns

Convert bitwise operations to rhythmic flows:

Using the primitive `bit(val, index)` returns the binary state of a bit inside a value. 

```idl

a = int(7)
b = int(3)

create_pattern(a, b) = [i = 0..7 : _; ! ? bit(a&b, i)]
```

#### Flow Transformation Functions

```idl
repeat(flow, N=2)      // Repeat N times
reverse(flow)          // Reverse elements
concat(flow1, flow2)   // Concatenate
scramble(flow)         // Randomly shuffle
```

---

## 8. Modules & Integration

### 8.1 Module Philosophy

Modules expose **functions only**—no OOP-style objects or method chains. This enforces functional composition and prevents deeply nested access patterns.

### 8.2 Module Import (Global Scope Only)

**Idyl Libraries** (`.idyl` files) and **External Modules** (compiled binaries, C libraries, etc.) must be loaded at **global scope**, before any process blocks or function definitions that use them:

```idl
lib("scales.idyl")
lib("synth_utils.idyl")

cs = module("csound")
osc = module("osc")
midi = module("midi")

// Now function definitions and process blocks can use these imports
```

**Important**: Import and library statements **must never appear inside process blocks**. They can only be called at global scope.

### 8.3 Module Usage (Functional Style)

Module functions are called functionally using the `::` namespace operator:

```idl
// Module functions called with :: operator
scheduler = module("scheduler")
config = scheduler::load_config("timeline.json")
timeline = scheduler::create_timeline(config)
result = scheduler::execute(timeline, 0s, 60s)

// OSC example
osc = module("osc")
sender = osc::sender("192.168.1.100", 8080)
osc::send(sender, "/note", 60)
```

---

## 9. Process Blocks

### 9.1 Main Entry Point

The `process` block is the **only executable entry point**. Code outside process blocks is purely declarative — definitions, flow declarations, and module imports only. Bare expression calls at global scope are a parse error.

**Syntax**:
```idl
process: {
    // Executable code here
    // Assignments and bare expression calls
}
```

**Behavior**:
- Runs when the file is loaded
- All expressions execute in parallel (no sequential order dependency)
- Parallel scope: all assignments are simultaneous
- Both assignments (`x = expr`) and bare expression calls (`f(x)`) are valid — bare calls execute for their side effects

**Examples**:
```idl
// Simple metronome
process: {
    metro = metronome(500ms)
}

// Control signal generators
process: {
    lfo1 = lfo(0.5hz, dt=10ms)
    lfo2 = lfo(1.2hz, dt=10ms)
    master_volume = amplitude_envelope(2s)
    control_out = (lfo1 + lfo2) * master_volume * 0.5
}

// Module composition (imports must be at global scope)
scheduler = module("scheduler")

process: {
    config = scheduler::load_config("control_map.json")
    timeline = scheduler::create_timeline(config)
    play_result = scheduler::execute(timeline, 0s, 30s)
}
```

### 9.2 Named Process Blocks (Sequencing)

Define sections with optional duration:

```idl
process intro, duration=8s: {
    // Intro section
}

process verse, duration=16s: {
    // Verse section
}

process main: {
    // Optional main orchestration
}
```

---

## 10. Execution Model

### 10.1 Compilation & Instantiation

**Steps**:
1. File is parsed
2. All function definitions are compiled into models (not executed)
3. Process blocks are identified
4. Main process block executes on file load
5. Named process blocks are available for later composition

### 10.2 Temporal Function Instantiation

When a temporal function is called, it is **instantiated once** with separate state:

```idl
process: {
    // Two independent LFOs, each with its own phase state
    lfo_slow = lfo(0.2hz)   // Instance 1: phase = 0
    lfo_fast = lfo(1.5hz)   // Instance 2: phase = 0
    modulated_param = lfo_slow + lfo_fast
}
```

### 10.3 Parallel Execution

All expressions in a process block execute in **parallel**:

```idl
process: {
    a = expensive_function1()
    b = expensive_function2()
    c = expensive_function3()
    // All three run simultaneously; order doesn't matter
}
```

### 10.4 Dynamic Instances (Synchronous)

Multiple simultaneous instances of temporal functions:

```idl
process parameter_control: {
    flow param_sequence = {
        value: [0.1, 0.3, 0.6, 0.9],
        duration: [500ms, 500ms, 500ms, 1000ms]
    }
    
    // Create control instances for all parameters simultaneously
    control_targets = activate_parameters(param_sequence.value, param_sequence.duration)
}
```

---

## 11. Built-in Functions

### 11.1 Math Functions

```idl
sin(x), cos(x), tan(x)
floor(x), ceil(x), round(x)
abs(x), pow(x, exp), sqrt(x)
fmod(x, m), mod(x, m)
min(a, b), max(a, b)
int(x)           // Convert to integer for bitwise ops
float(x)         // Convert to float
```

### 11.2 Time & Rhythm

```idl
metronome(dt)        // Trigger at fixed interval
beats(clock)         // Trigger stream from clock
clock(bpm)           // Create clock at BPM
tempo(bpm)           // Set global tempo
phasor(freq)         // Ramp 0→1 at frequency
counter(trigger)     // Increment on triggers
```

### 11.3 Random

```idl
rnd()                // Random [0, 1)
rnd(a, b)            // Random [a, b)
rnd(a, b, step)      // Quantized random
rint(a, b)           // Random integer
```

### 11.4 Utility

```idl
quantize(signal, div)   // Quantize to grid
wrap(value, range)      // Wrap value within range
len(flow)               // Length of flow
bit(val, index)         // Returns state of a bit from a value or function (0 or 1)
```

### 11.5 Flow Functions

```idl
repeat(flow, N=2)    // Repeat N times
reverse(flow)        // Reverse order
concat(flow1, flow2) // Concatenate flows
scramble(flow)       // Randomly permute
```

---

## 12. Event Handling & Lifecycle

### 12.1 Catch Blocks (Basic)

Functions can signal completion or important events:

**Syntax**:
```
variable catch end : { ... }
```

**Examples**:
```idl
scheduler = module("scheduler")

process control_sequence: {
    timeline = scheduler::create_timeline()
    sequence = scheduler::schedule(timeline, 0s, 30s)
    
    sequence catch end : {
        print("Sequence finished")
    }
}
```

**Event Types** (Core, Alpha):
- `catch end` — Function reached natural end/expiration

---

## Part II: Yet to Confirm Ideas

The following features are **conceptually designed** but **not included in alpha**. They require further design refinement, community feedback, and testing before implementation.

---

## A. Extended Event Handling & Lifecycle Management

### A.1 Additional Catch Events

Beyond `catch end`, future extensions could include:

- **`catch !`** — Function fired a trigger (capture specific events)
- **`catch change`** — Function's output changed from previous value
- **`catch exception`** — Function encountered a runtime error
- **`catch timeout`** — Function exceeded time threshold without firing

**Conceptual Examples** (not yet implemented):
```idl
// Catch trigger events
counter catch ! : { log("Counter fired!") }

// Catch value changes
signal catch change : { print("Signal changed to:", signal) }

// Catch errors
risky catch exception : { fallback_behavior() }
```

### A.2 Lifecycle Primitives

Beyond events, the language could expose lifecycle primitives:

- **`start`** — Initialize/resume a function
- **`pause`** — Suspend computation (preserve state)
- **`stop`** — Terminate and clean up
- **`reset`** — Reinitialize to startup state

### A.3 Advanced Instance Pool Management

A dedicated abstraction for managing many simultaneous instances:

```idl
// Conceptual: not in alpha
pool = instance_pool(max_instances=32)
new_instance = pool.allocate(create_control, params[i])
pool.release(instance_id)
```

---

## B. Higher-Order Functions & Closures

### B.1 Functions Returning Functions

Functions can create and return other functions with captured parameters:

**Conceptual Syntax** (needs refinement):
```idl
// Function factory (not yet finalized)
make_control_generator(base_param) = 
    gen(freq, dt=10ms) = sin(phase * 2 * pi) * base_param
    |> {
        init: { phase = 0 }
        phase = fmod(phase + 1/freq, 1)
    }

// Usage
gen_slow = make_control_generator(1.0)
gen_fast = make_control_generator(2.0)
```

### B.2 Closures

Functions capture variables from enclosing scope:

**Conceptual Example**:
```idl
make_normalizer(min_val, max_val) = 
    normalize(value) = 
        (value - min_val) / (max_val - min_val)

norm = make_normalizer(0, 1000)
normalized = norm(sensor_input)
```

**Status**: This requires explicit syntax for "flying functions" (functions created at runtime) to avoid confusion with regular definitions. Design pending.

---

## C. Trait System

### C.1 Operator Overloading via Traits

Allow flows and custom types to implement operators:

**Conceptual Syntax** (pending finalization):
```idl
trait melody_operations {
    op+(a : flow, b : flow) = [i = 0..(len(a)-1) : max(a[i]; b[i])]
    op*(sequence : flow, factor : number) = [i = 0..(len(sequence)-1) : sequence[i] * factor]
}

flow seq1 implements sequence_operations = [0.1, 0.3, 0.5]
flow seq2 implements sequence_operations = [0.2, 0.2, 0.3]

combined = seq1 + seq2        // Element-wise max
scaled = seq1 * 1.5           // Scale
```

### C.2 Shared Functions via Traits

```idl
trait sequenceable {
    length() : number
    at(idx : number) : any
}

flow drum implements sequenceable = [!, _, !, _]
flow notes implements sequenceable = [60, 62, 64, 65]

// Both can use trait functions
step = counter(beat)
d = drum.at(step % drum.length())
n = notes.at(step % notes.length())
```

### C.3 Polymorphic Operations

```idl
trait time_aware {
    duration() : time
    scale_tempo(factor : number) : self
}

flow pattern implements time_aware = 
    rhythm: [500ms, 500ms, 1000ms]

// Scale tempo polymorphically
faster = pattern.scale_tempo(1.5)
```

---

## D. Advanced Flow Interpolation

Smooth transitions between flow values when accessed with float indices:

**Conceptual Examples**:
```idl
// Linear interpolation
interp_linear(a, b, t) = a + (b - a) * t

// Cubic interpolation
interp_cubic(a, b, t) = 
    a + (3 * (b - a) * t * t) - (2 * (b - a) * t * t * t)

// Apply to flow access
sweeping(phase) = 
    interp_linear(flow[floor(phase)], flow[ceil(phase)], fmod(phase, 1))
```

---

## E. Advanced Error Handling

### E.1 Error Propagation Strategy

For a real-time system, errors must be handled gracefully:

**Proposed Approach** (not yet implemented):
- **Non-critical errors** (e.g., module not loaded): Log warning, continue execution with fallback
- **Critical errors** (e.g., invalid flow index): Abort performance if necessary; emit event
- **Recovery mechanisms**: Allow catch blocks to intercept and handle specific errors

**Conceptual Syntax**:
```idl
risky_call catch exception : { 
    print("Error:", exception)
    use_fallback_behavior()
}
```

### E.2 Type-Safe Module Loading

```idl
cs = module("csound") catch exception : {
    print("Failed to load Csound module")
    mock_synth_engine = fallback
}
```

---

## F. Functional Generator Enhancements

The current for-loop syntax is adequate but could be extended:

### F.1 Multi-Variable Generators

```idl
// Cartesian product (conceptual)
pairs = [x = [1, 2], y = [a, b] : (x, y)]
// Result: [(1, a), (1, b), (2, a), (2, b)]

// Conditional generation (conceptual)
filter_range = [i = 0..100 : 0; i ? (i % 2 == 0)]
```

### F.2 Recursive Generators

```idl
// Define recursive sequences (conceptual)
fibonacci = [n = 0..20 : fib(n)]

fib(n) = 0; 1; fib(n-1) + fib(n-2) ? (n < 2)
```

---

## G. Advanced Polyphony & Instance Management

### G.1 Dynamic Voice Pools

A more sophisticated abstraction for managing bounded voice allocation:

```idl
// Conceptual: not in alpha
pool = voice_pool(max_instances=16)

allocate_voice = pool.new(synthesize, [pitch, duration])
release_all = pool.clear()
active_count = pool.count()
```

### G.2 Instance Lifecycle Hooks

Beyond simple instantiation, predict instance behavior:

```idl
// Conceptual: future extension
instance catch lifetime_change : {
    new_voices_count = count_active_instances()
    update_cpu_load(new_voices_count)
}
```

---

## H. Module System Enhancements

### H.1 Scoped Module Loading

Load modules into isolated namespaces to avoid conflicts:

```idl
// Conceptual: future extension
local cs = import_local("csound")  // Scoped to current process
global = module("common_lib")      // Global (default)
```

### H.2 Module Composition

Combine multiple modules into a unified interface:

```idl
// Conceptual
combined = compose_modules(cs, osc, midi)
// Access: combined.cs.function(), combined.osc.function(), etc.
```

---

## I. Advanced Type System

While currently implicit, a future explicit type system could:

- **Annotate function signatures** for clarity:
  ```idl
  add(a: number, b: number): number = a + b
  ```

- **Enable compile-time optimization** via type inference

- **Provide IDE support** for autocomplete and refactoring

**Status**: Out of scope for alpha; implicit type detection is sufficient for initial implementation.

---

## J. Macro System

A future metaprogramming layer:

```idl
// Conceptual: not in alpha
macro make_synth(name, freq_var) {
    $name = sine_osc($freq_var, dt=10ms)
}

make_synth(osc1, 440hz)
make_synth(osc2, 880hz)
// Expands to explicit definitions
```

---

## Summary of Core Specification (Alpha)

The **core specification (Part I)** defines a complete, implementable language suitable for:

1. **Live coding** with expression-based evaluation
2. **Real-time scheduling** with explicit time semantics
3. **Modular integration** with external systems (Csound, OSC, etc.)
4. **Functional composition** without hidden state or asynchrony
5. **Pattern-based programming** via flows and generators

**Strengths**:
- Clear temporal semantics (clock-driven vs. trigger-driven)
- Explicit state management via `emit`
- Polymorphic functions (work on values and flows)
- Modular architecture without OOP complexity
- Parallel execution within process blocks

**Limitations** (by design for alpha):
- No explicit higher-order functions (planned for future)
- No trait system or operator overloading (planned)
- Limited event handling (only `catch end` in alpha)
- No advanced error recovery
- No type annotations

**Target Implementation**:
- Tokenizer/Lexer: Parse time literals, operators, flow syntax
- Parser: Build AST for functions, processes, flows
- Type Inference: Determine types at compile time (implicit)
- Interpreter/VM: Execute processes; manage temporal functions and state
- Runtime: Schedule updates, handle triggers, manage instances

This specification is ready for **pre-alpha parser and engine development**.
