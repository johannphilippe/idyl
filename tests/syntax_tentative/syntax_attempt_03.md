# Idyl Language Specification - Attempt 03

**Status**: Specification Document (Updated from Attempt 02 with author feedback)

**Revision Notes**: 
- Fixed counter/trigger synchronization issues with `dt=?` notation
- Removed `'(expression)` shorthand (keeping only `@` operators)
- Changed ternary operator to wrap on out-of-range instead of returning 0
- Removed OOP-style method chaining from all examples
- Fixed imports to use only `lib()` for Idyl files
- Added flow generation features (bitwise, interpolators)
- Simplified voice management patterns
- Added higher-order functions section
- Changed for-loop syntax from `=>` to `:`
- Enhanced trait system with complete examples
- Added concluding design notes

---

## Table of Contents

1. [Core Language Model](#1-core-language-model)
2. [Function Definitions](#2-function-definitions)
3. [Temporal Functions & Lambdas](#3-temporal-functions--lambdas)
4. [Primitive Types](#4-primitive-types)
5. [Time System](#5-time-system)
6. [Operators & Expressions](#6-operators--expressions)
7. [Flows](#7-flows)
8. [Control Flow](#8-control-flow)
9. [Higher-Order Functions & Closures](#9-higher-order-functions--closures)
10. [Module System](#10-module-system)
11. [Process Blocks](#11-process-blocks)
12. [Execution Model](#12-execution-model)

---

## 1. Core Language Model

### 1.1 Functional Foundation

Idyl is a **purely functional language** with no traditional variables—only function definitions.

- Everything is a function
- Functions can be composed, passed as arguments, and returned
- State is explicit through emission operators and lambda blocks

**Examples**:
```idl
// Simple constant function
x = 5
y = x + 3

// Function with parameters
add(a, b) = a + b
result = add(2, 3)  // result = 5

// Function calling another function
double(n) = multiply(n, 2)
multiply(a, b) = a * b
```

### 1.2 Namespace & Import

- A file evaluates (compiles) all functions, creating models for runtime use
- If two functions share the same name, the last loaded function wins (with warning)
- **Idyl libraries** (`.idl` files) are loaded via the `lib()` keyword
- **External modules** (compiled binaries or language bindings) are loaded via `import()`
- Function parameters naturally shadow outer scope (standard lexical scoping)

**Examples**:
```idl
// First definition
volume = 10

// Later definition - this one wins
volume = 5  // Warning: overloading 'volume'

// Load Idyl library
lib("my_scales.idl")
// Scale functions now available globally

// Import external module into namespace
cs = import("csound")
// Access via: cs.function_name()

// Parameters shadow outer scope
global_param = 100
test(global_param) = global_param + 10  // local global_param shadows outer
result = test(5)  // result = 15, not 110
```

---

## 2. Function Definitions

### 2.1 Basic Function Definition

Functions are defined using `name(parameters) = expression` syntax.

**Syntax**:
```
function_name(param1, param2, ...) = expression
```

**Examples**:
```idl
// No parameters
silence = 0

// Single parameter
square(x) = x * x

// Multiple parameters
mix(a, b, mix_amount) = a * (1 - mix_amount) + b * mix_amount

// Nested function calls
compose_filters(input, cutoff) = 
    lowpass(highpass(input, 100hz), cutoff)
```

### 2.2 Default Parameters

Functions can have default parameter values, specified with `param=value` syntax.

**Examples**:
```idl
// With default parameters
fade_out(duration, curve=linear) = 
    envelope(duration, 1, 0, curve)

// Calling with defaults
env1 = fade_out(500ms)        // Uses linear curve
env2 = fade_out(500ms, exponential)  // Override default

// Multiple defaults
synthesize(freq, amp=0.5, dt=10ms) = 
    sine_oscillator(freq, dt) * amp
```

---

## 3. Temporal Functions & Lambdas

### 3.1 Temporal Functions

Functions that need internal state and updates are called **temporal functions**. They combine:
1. A main expression (the output value)
2. An optional lambda block (for initialization and state updates)

**Syntax**:
```
function_name(params, dt=time_unit) = output_expression
|> {
    init: { initialization_statements }
    // Remaining code is implicit update executed every dt
    update_statements
}
```

**Time Basis (`dt` Parameter)**:
- For **continuous functions**, specify a time interval: `dt=10ms`
- For **trigger-based functions**, use `dt=?` to indicate the function computes when its input triggers fire
- `dt=?` is a special notation meaning "execute on trigger, not on a fixed time step"

**Operator Note**: `|>` (pipe-arrow) represents the temporal flow, suggesting both the flow of computation and the movement through time.

**Examples**:
```idl
// Simple oscillator (time-based)
sine_osc(freq, dt=10ms) = sin(phase * 2 * pi)
|> {
    init: { phase = 0 }
    phase = fmod(phase + 1/freq, 1)
}

// Integrator with state (time-based)
integrator(input, dt=10ms) = accumulated
|> {
    init: { accumulated = 0 }
    accumulated = accumulated + input * (dt / 1000)
}

// Counter (trigger-based) - executes on trigger edges, not fixed dt
counter(trigger) = count
|> {
    init: { count = 0 }
    count = count + (trigger ? 1 : 0)
}

// Gate/sampler triggered by metronome
sample_on_beat(input, metro) = sampled_value
|> {
    init: { sampled_value = input }
    sampled_value = metro ? input : sampled_value
}
```

**Semantic Note on Trigger Functions**: Trigger-based functions (implicitly `dt=?`) compute synchronously with their input triggers, not at fixed time intervals. This ensures proper synchronization between event-driven logic (like beat counting) and time-driven signals (like oscillators).

### 3.2 Initialization Block (`init`)

The initialization block runs once when the function is first instantiated.

**Behavior**:
- Executed immediately after function instantiation
- Typically sets initial values for state variables
- All variables defined here are available in the update (implicit) block
- Optional—omit if no initialization needed

**Examples**:
```idl
// Simple initialization
low_pass(input, cutoff, dt=10ms) = output
|> {
    init: { 
        state = 0 
        alpha = 0.1
    }
    output = state
    state = state + (input - state) * alpha
}

// Complex initialization with programmatic buffer
delay_line(input, delay_time, dt=10ms) = delayed_sample
|> {
    init: { 
        buffer_size = int(delay_time / dt)
        buffer = [for i in 0..buffer_size : 0]
        write_pos = 0
    }
    delayed_sample = buffer[wrap(write_pos - delay_time/dt, buffer_size)]
    buffer[wrap(write_pos, buffer_size)] = input
    write_pos = write_pos + 1
}
```

### 3.3 State Emission & Memory

Within lambda blocks, variables can be marked for emission using the `emit` keyword to define state that persists across updates.

**Syntax**:
```
emit variable_name = value
```

**Behavior**:
- Variables marked with `emit` persist their value across update cycles
- Without `emit`, variables are temporary (computed fresh each cycle)
- `emit` can appear in init block or in the update section
- Unlike traditional mutable state, emit makes persistence explicit and intentional

**Examples**:
```idl
// Emitting state across updates
running_sum(input, dt=10ms) = total
|> {
    init: { emit total = 0 }
    total = total + input
}

// Differentiator (stores previous value)
differentiate(input, dt=10ms) = derivative
|> {
    init: { emit prev_value = input }
    derivative = (input - prev_value) / (dt / 1000)
    prev_value = input
}

// Multi-state example
envelope_follower(input, dt=10ms) = env_out
|> {
    init: { 
        emit envelope = abs(input)
        attack_coeff = 0.1
        release_coeff = 0.05
    }
    envelope = envelope < abs(input) ? 
        envelope + (abs(input) - envelope) * attack_coeff :
        envelope * (1 - release_coeff)
    env_out = envelope
}
```

### 3.4 Memory Operators (`@`)

Inspired by Faust, the memory operator provides compact delay syntax:

**Syntax**:
```
@(expression)        // Delay by one sample (previous value)
@(expression, N)     // Delay by N samples
```

**Behavior**:
- `@(expr)` automatically creates and manages a one-sample delay buffer
- `@(expr, N)` creates an N-sample delay
- Useful for differentiators, feedback loops, and history tracking

**Examples**:
```idl
// One-sample delay (previous value)
previous_input(signal) = @(signal)

// N-sample delay
delayed_signal(signal, delay_samples) = @(signal, delay_samples)

// Differentiator using memory operator
simple_diff(signal, dt=10ms) = (signal - @(signal)) / (dt / 1000)

// Feedback delay line using @ operator
echo(input, delay_time, feedback=0.7, dt=10ms) = 
    delayed = @(input + delayed * feedback, int(delay_time / dt))
```

---

## 4. Primitive Types

Idyl recognizes the following primitive types:

| Type | Description | Examples |
|------|-------------|----------|
| **Time** | Temporal duration or frequency | `100ms`, `2.5s`, `440hz`, `1.5b` (beats) |
| **Trigger** | Instantaneous impulse/event | `!`, `beats(clock)` |
| **Number** | Floating-point value (64-bit) | `3.14`, `0`, `-1.5` |
| **Integer** | 64-bit or 32-bit integer (for bitwise ops) | Via `int()` conversion |
| **Flow** | Special function organizing indexed sequences | Defined in Flows section |

### 4.1 Type Characteristics

- **Time** is always internally converted to milliseconds for consistent computation
- **Numbers** are 64-bit floating-point by default
- **Integers** are created via explicit `int()` conversion, enabling bitwise operations
- **Triggers** are a first-class primitive (not just boolean values)
- **Flows** are a special category of function with indexed access semantics
- **Type distinction**: The engine automatically determines the type based on context

**Examples**:
```idl
// Time values
short_duration = 100ms
long_duration = 2.5s
audio_rate = 48000hz
musical_timing = 4.0b

// Triggers
beat_trigger = beats(main_clock)
metro = metronome(100ms)

// Numbers
amplitude = 0.5
frequency = 440.0
random_value = rnd(0, 1)

// Integer conversion for bitwise operations
bit_pattern = int(255)
masked = bit_pattern & int(15)

// Functions that return different types
tempo_function = 120
note_pattern = [60, 62, 64]
clock_beats = beats(my_clock)
```

---

## 5. Time System

### 5.1 Time Literals

Idyl supports multiple time representations that are automatically converted to milliseconds:

| Literal | Unit | Conversion |
|---------|------|-----------|
| `Xms` | Milliseconds | Direct (X ms) |
| `Xs` | Seconds | X × 1000 ms |
| `Xhz` | Frequency (Hertz) | 1000 / X ms |
| `Xb` | Beats (relative to tempo) | Scaled by current tempo |

**Conversion Rule**: All time values are internally represented as milliseconds for computation.

**Examples**:
```idl
// Direct time
short_delay = 100ms
long_delay = 2.5s

// Frequency
note_freq = 440hz
sample_rate = 48000hz

// Musical timing (beats relative to current tempo)
half_note = 0.5b
whole_note = 2.0b

// Mixed operations
combined = 100ms + 1s  // Results in 1100ms
```

### 5.2 Tempo & Clocks

**Tempo** is a global reference (default 60 BPM). Beats are always relative to the active clock.

**Syntax**:
```
tempo(bpm)              // Set global tempo
clock(bpm)              // Create new clock at specified BPM
clock_name(beats)       // Get time in beats from specific clock
beats(clock)            // Get trigger stream from clock beats
```

**Examples**:
```idl
// Set global tempo to 120 BPM (2 beats per second)
tempo(120)

// Create a faster clock at 240 BPM
fast_clock = clock(240)

// Get a specific time in the fast clock
t1 = fast_clock(0.5b)

// Get beat triggers from the fast clock
fast_beats = beats(fast_clock)

// Default main clock
main_beats = beats(main_clock)
```

### 5.3 Time-Based Update (`dt`) & Inheritance

All temporal functions accept an optional `dt` parameter specifying how often their update block executes. When a temporal function calls another, `dt` inheritance follows **caller-to-callee semantics**: the update rate is passed from the calling function to the called function.

**Default Behavior**: 
- If `dt` is not explicitly specified, it inherits from the **caller**
- If the root caller has no explicit `dt`, it defaults to `10ms`

**Examples**:
```idl
// Explicit dt
fast_filter(input, dt=5ms) = output
|> {
    init: { state = 0 }
    state = state + input * 0.1
    output = state
}

// Inherited dt from caller
slow_oscillator(freq, dt=50ms) = sin(phase * 2 * pi)
|> {
    init: { phase = 0 }
    phase = fmod(phase + 1/freq, 1)
}

// Inheritance example
parent(input) = slow_oscillator(input)  // Inherits 50ms
```

---

## 6. Operators & Expressions

### 6.1 Arithmetic Operators

Standard arithmetic operations: `+`, `-`, `*`, `/`, `%` (integer modulo)

**Examples**:
```idl
sum = 3 + 4
difference = 10 - 3
product = 2 * 5
quotient = 10 / 2
remainder = 17 % 5

// With time units
duration = 1s + 500ms
frequency_ratio = 440hz * 2
```

### 6.2 Comparison Operators

Standard comparisons: `<`, `>`, `<=`, `>=`, `==`, `!=`

**Return Value**: `1` for true, `0` for false (no boolean type)

**Examples**:
```idl
is_loud = amplitude > 0.5
is_in_range = (x > 0) * (x < 1)
is_equal = (note == 60)
is_different = (state != idle_state)
```

### 6.3 Ternary & Multi-Way Selection

A clean ternary operator with multiple case handling:

**Syntax**:
```
option0, option1, option2, ... ? condition
```

**Behavior**:
- If `condition` evaluates to `0`, returns `option0`
- If `condition` is not an integer, it's rounded down to select the corresponding option
- **If the index is out of range, wraps around** (like modulo) to stay within valid options

**Examples**:
```idl
// Two-way selection
envelope_type(is_fast) = 
    linear_env, exponential_env ? is_fast

// Multi-case selection (wraps on out-of-range)
play_note(scale_degree) = 
    note_c, note_d, note_e, note_f, note_g ? scale_degree
    // If scale_degree = 7, wraps to note_c (index 0)

// Using with conditions
volume_control(velocity) =
    0.1, 0.3, 0.6, 0.9 ? (velocity > 60) + (velocity > 100) + (velocity > 127)

// State machine
audio_state(state) =
    silent_output, 
    attack_env, 
    sustain_env, 
    release_env ? state
```

### 6.4 Emit Operator

The `emit` keyword marks values for persistence within lambda blocks (see Section 3.3).

### 6.5 Trigger Primitive (`!`)

The `!` symbol represents an instantaneous trigger/impulse.

**Behavior**:
- A trigger is a discrete event (not a continuous signal)
- Triggers are first-class primitives for timing and scheduling
- Triggers can be combined: `trig1 + trig2` sums triggers

**Examples**:
```idl
// Define a trigger
pulse = !

// Metronome produces triggers
metro = metronome(100ms)

// Count triggers
kick_count = counter(metro)

// Combine triggers
combined = metro + beats(clock)
```

### 6.6 Bitwise Operators

For bitwise operations on integers (requires `int()` conversion):

**Operators**: `&` (AND), `|` (OR), `^` (XOR), `<<` (left shift), `>>` (right shift), `~` (NOT)

**Examples**:
```idl
a = int(12)  // 0b1100
b = int(10)  // 0b1010

and_result = a & b  // 8 (0b1000)
or_result = a | b   // 14 (0b1110)
xor_result = a ^ b  // 6 (0b0110)
```

---

## 7. Flows

A **Flow** is a special function that organizes data into indexed sequences. Flows are themselves functions, with indexed access as their primary semantic feature.

### 7.1 Flow Definition

**Syntax**:
```
flow flow_name(params) = [
    { member1: values, member2: values, ... }
]
```

Or, a simpler syntax for single-member flows:
```
flow flow_name(params) = [ val1, val2, val3, ... ]
```

**Repetition Syntax**: 
- `[val1, val2] |N|` - Repeat the enclosed section N times
- `[section1] |N| | [section2] |M|` - Standalone `|` indicates restart; section2 starts fresh

**Examples**:
```idl
// Simple single-member flow
flow notes() = [60, 62, 64, 67]

// Multi-member flow
flow basic_pattern() = {
    rhythm: [!, ., ., !],
    note: [60, 62, 64, 67]
}

// Flow with repetition
flow verse() = {
    kick: [!] |4|,
    bass: [50] |4|,
    melody: [60, 62, 64, 65] |2|
}

// Flow with restart operator
flow complex_pattern() = {
    drums: [!] |2| | [., !] |2|,
    bass: [50, 52] |4|
}

// Flow with programmatic generation
flow sine_lookup(size) = 
    [for i in 0..(size-1) : sin(i / size * 2 * pi)]

// Flow with arpeggio
flow arp(scale, offset) = 
    [scale[0] + offset, scale[2] + offset, scale[4] + offset]
```

### 7.2 Flow Access

Flows can be indexed in three ways:

| Method | Index Type | Use Case |
|--------|-----------|----------|
| **Trigger Indexing** | Trigger signal | Sequence driven by metronome |
| **Integer Indexing** | Counter result | Manual step-through |
| **Float Indexing** | Phasor/ramp | Smooth sweep through pattern |

**Syntax & Examples**:

#### Trigger-Based Access
```idl
// Flow triggered by metronome
metro = metronome(500ms)
current_note = basic_pattern.note(metro)
rhythm = basic_pattern.rhythm(metro)
```

#### Integer-Based Access
```idl
// Flow indexed by counter
step = counter(clock_beat)
note_sequence = basic_pattern.note(step)
```

#### Float-Based Access  
```idl
// Flow smoothly traversed with phasor
phase = phasor(1hz)
note_sequence = basic_pattern.note(phase)
```

### 7.3 Multi-Member Flow Examples

Flows combine multiple synchronized sequences:

```idl
// Drum pattern with kick, snare, hi-hat
flow drum_loop() = {
    kick: [! . . . | . . ! . |2|],
    snare: [. . ! . | . . ! . |2|],
    hihat: [! ! ! ! | ! ! ! ! |2|]
}

// Melodic pattern with notes and durations
flow melody_phrase() = {
    pitch: [60, 62, 64, 65, 67],
    duration: [500ms, 500ms, 500ms, 1s, 2s]
}

// Synchronized note and velocity pairs
flow expressive_sequence() = {
    notes: [60, 62, 65, 67, 69],
    velocities: [80, 95, 100, 90, 75]
}
```

### 7.4 Flow Generation Features

#### Bitwise-Generated Flows

Generate flows from bitwise operations to create patterns:

```idl
// Convert bitwise AND result to trigger flow
to_trigger(value) = (value != 0) ? 1 : 0

create_bitwise_pattern(a, b) = 
    [for i in 0..7 : to_trigger(int(a) & int(b))]

// Example: mask creates pattern
pattern = create_bitwise_pattern(255, 127)  // Creates alternating pattern
```

#### Flow Interpolators

Smoothly interpolate between flow values when accessed with float indices:

```idl
// Define interpolation strategies
interp_linear(a, b, t) = a + (b - a) * t

interp_cubic(a, b, t) = 
    a + (3 * (b - a) * t * t) - (2 * (b - a) * t * t * t)

// Use with float-indexed flows
flow melody() = [60, 64, 67, 71]

// Smooth sweep through melody with linear interpolation
sweeping_melody(phase) = 
    interp_linear(
        melody(floor(phase * 3)),
        melody(floor(phase * 3) + 1),
        fmod(phase * 3, 1)
    )
```

#### Algorithmic Flow Generation

```idl
// Fibonacci-based pattern generator
fib_flow(count) = 
    [for i in 0..(count-1) : fib(i + 1)]

// Random walk flow
random_walk(steps, start=60, range=12) = 
    [for i in 0..(steps-1) : start + rnd(-range/2, range/2)]

// Scale harmonizer
harmonize(scale, root_idx, interval) =
    [for i in 0..7 : scale[(root_idx + i * interval) % len(scale)]]
```

---

## 8. Control Flow

### 8.1 Conditional via Ternary

Idyl uses the numeric ternary approach rather than traditional if/else (see Section 6.3):

**Pattern**:
```
false_value, true_value ? condition
```

**Examples**:
```idl
// Simple conditional
is_positive(x) = 0, 1 ? (x > 0)

// Value selection based on condition
choose_amp(is_loud) = 0.2, 0.8 ? is_loud

// Nested conditions
safe_divide(a, b) = 
    0, (a / b) ? (b != 0)
```

### 8.2 Multi-Way Selection

For more than two options, use the multi-way ternary (see Section 6.3):

```idl
// Scale note selection
scale_note(degree, scale_root) = 
    scale_root + 0,      // Case 0
    scale_root + 2,      // Case 1
    scale_root + 4,      // Case 2
    scale_root + 5,      // Case 3
    scale_root + 7,      // Case 4
    scale_root + 9,      // Case 5
    scale_root + 11,     // Case 6
    scale_root + 12 ? (degree % 8)

// State machine
state_output(state) = 
    idle_output,
    attack_output,
    decay_output,
    sustain_output,
    release_output ? state
```

### 8.3 Loops via Flows & Generators

Instead of traditional for-loops, use flow generators with the `:` syntax:

**Syntax**:
```
[for variable in range : expression]
```

**Examples**:
```idl
// Generate sine table
sinetable(size) = [for i in 0..size : sin(i / size * 2 * pi)]

// Generate scale offsets
intervals = [for i in 0..8 : i * 2]

// Generate frequency ratios
harmonics(fundamental) = 
    [for harmonic in 1..16 : fundamental * harmonic]
```

---

## 9. Higher-Order Functions & Closures

Idyl supports functions as first-class values, allowing the creation of functions that return other functions. This enables powerful composition patterns and parameter capture.

### 9.1 Function Factories

Functions can create and return other functions with captured parameters:

**Syntax**:
```
higher_order_func(captured_param1, captured_param2, ...) = 
    func(x, y, ...) = expression_using_captured_and_local
```

**Examples**:
```idl
// Function that creates oscillators with a specific timbre
make_oscillator(harmonic_series) = 
    osc(freq, dt=10ms) = 
        sin(phase * 2 * pi)
    |> {
        init: { phase = 0 }
        phase = fmod(phase + 1/freq, 1)
    }

// Function that creates custom filters
make_filter(cutoff, resonance) = 
    filter(input, dt=10ms) = processed
    |> {
        init: { state = 0 }
        state = state + (input - state) * cutoff * resonance
        processed = state
    }

// Function composition
apply_filters(filters_list) = 
    compose(signal) = processed
    |> {
        init: { }
        // Apply each filter in sequence (simplified)
        processed = signal
    }
```

### 9.2 Closures & Scoped Capture

Higher-order functions capture variables from their enclosing scope, creating closures:

```idl
// Create a custom scaler with captured min/max bounds
make_scaler(min_val, max_val, target_min=0, target_max=1) = 
    scale(value) = 
        target_min + (value - min_val) / (max_val - min_val) * (target_max - target_min)

// Usage
note_scaler = make_scaler(0, 127, 60, 84)  // MIDI to note range
scaled_note = note_scaler(velocity_input)

// Create parameterized envelope
make_envelope(attack_time, decay_time, sustain_level, release_time) = 
    env(trigger, dt=10ms) = env_out
    |> {
        init: { phase = 0, emit level = 0 }
        phase = trigger ? phase + dt/attack_time : max(0, phase - dt/release_time)
        level = phase < 1 ? 
            phase :
            sustain_level + (1 - sustain_level) * (1 - phase)
        env_out = level
    }

// Usage
fast_env = make_envelope(50ms, 100ms, 0.5, 200ms)
slow_env = make_envelope(500ms, 1s, 0.7, 2s)
```

---

## 10. Module System

### 10.1 Module Design Philosophy

Modules expose **functions only**—no OOP-style methods or stateful objects. This keeps the language functional and prevents deeply nested constructs. Modules return:
- **Functions**: Callable units for composition
- **Values**: Constants, configurations, or computed results
- **Wrapped Engines**: Opaque handles accessed via functional APIs

### 10.2 Module Import

**Syntax**:
```
module_name = import("module_path")
```

**Behavior**:
- Imports a compiled module with a defined API
- Module functions are accessed via dot notation: `module_name.function_name()`
- Errors during import are reported to console

**Examples**:
```idl
// Import Csound module
cs = import("csound")

// Import OSC module
osc = import("osc")

// Access module functions functionally
compile_result = cs.compile("synth.orc")
orchestra_get = cs.get(compile_result, "my_instr")
```

### 10.3 Module Function Calls (Functional Style)

Module functions are called functionally without method chaining:

**Examples**:
```idl
// Csound workflow - functional approach
cs = import("csound")

compiled = cs.compile("synth.orc")
instrument = cs.get(compiled, "my_instr")
voice = cs.schedule(instrument, 0s, 5s)

// OSC workflow
osc_module = import("osc")
sender = osc_module.sender("192.168.1.47", 8080)
osc_module.send(sender, "/synth/freq", current_frequency)
```

### 10.4 Idyl Library Imports

Idyl libraries (`.idl` files) are loaded using the `lib()` function:

**Syntax**:
```
lib("library_path.idl")  // Load into global namespace
```

**Examples**:
```idl
// Load common scales library
lib("scales.idl")
// Scale functions now available globally

// Load synthesis utilities
lib("synth_utils.idl")
// Utility functions available for composition
```

---

## 11. Process Blocks

### 11.1 Main Process Block

The `process` block is the **primary entry point** for code execution. Only code within process blocks runs when a file is loaded.

**Syntax**:
```idl
process: {
    // Variable/function definitions that will run
    // Expressions that produce output or side effects
}
```

**Behavior**:
- Executed when the file is loaded
- All definitions within are scoped to the process
- Expressions are evaluated and their side effects propagate
- Parallel execution of expressions (all active simultaneously)

**Examples**:
```idl
// Simple metronome
process: {
    metro = metronome(500ms)
}

// Multiple oscillators
process: {
    osc1 = sine_osc(440hz, dt=5ms)
    osc2 = sine_osc(880hz, dt=5ms)
    env = amplitude_envelope(2s)
    
    final_signal = (osc1 + osc2) * env * 0.5
}

// Functional module calls
process: {
    cs = import("csound")
    
    compiled = cs.compile("synth.orc")
    instrument = cs.get(compiled, "my_instr")
    voice = cs.schedule(instrument, 0s, 5s)
}
```

### 11.2 Multiple Process Blocks (Composition/Sequencing)

For composition, define sections with optional durations:

**Syntax**:
```idl
process intro, duration=8s: {
    // Intro section code
}

process verse, duration=16s: {
    // Verse section code
}
```

**Examples**:
```idl
process verse_section, duration=16s: {
    metro = metronome(500ms)
    bass = sine_osc(50hz) * metro
}

process chorus_section, duration=16s: {
    metro = metronome(250ms)
    stab = square_wave(220hz) * metro
}

process main: {
    // Can orchestrate named sections
}
```

### 11.3 Process Lifecycle (Advanced)

Processes can include optional initialization hooks:

**Syntax**:
```idl
process my_process: {
    init: { 
        // Runs once at startup
    }
    
    // Main continuous processing
}
```

---

## 12. Execution Model

### 12.1 File Loading & Compilation

**Execution Steps**:
1. File is parsed and all function definitions are compiled into models
2. Process blocks are identified
3. Main process block (if present) is executed
4. Named process blocks are available for composition

**Implications**:
- Functions are **eagerly compiled** but **lazily instantiated**
- Top-level function definitions create callable models
- Only process blocks trigger actual runtime execution

### 12.2 Temporal Function Instantiation

When a temporal function is called within a process, it is **instantiated immediately** with its own state.

**Behavior**:
- `init` block runs once at instantiation
- Update code starts running at the next cycle
- Each call creates a **new instance** with separate state

**Examples**:
```idl
// Two independent oscillators with separate phase state
process: {
    osc1 = sine_osc(440hz, dt=10ms)   // Instance 1
    osc2 = sine_osc(880hz, dt=10ms)   // Instance 2
    
    combined = osc1 + osc2
}
```

### 12.3 Parallel Execution

Expressions in a process block execute in **parallel** (not sequentially).

**Implication**: Assignment order does not matter; all are computed simultaneously.

### 12.4 State Persistence & Voice Management

**Voice State**:
- Each temporal function instance maintains its own state
- State persists for the lifetime of the instance

**Key Patterns**:

#### Simple Polyphonic Synth

```idl
process polyphony: {
    cs = import("csound")
    
    compile = cs.compile("synth.orc")
    synth_func = cs.get(compile, "piano")
    
    // Multiple independent voices
    voice1 = cs.schedule(synth_func, 0s, 3s)
    voice2 = cs.schedule(synth_func, 1s, 3s)
    voice3 = cs.schedule(synth_func, 2s, 3s)
}
```

#### Dynamic Voices Using Flows

```idl
process dynamic_voices: {
    metro = beats(main_clock)
    
    // Flow of note durations
    flow note_seq() = {
        pitch: [60, 62, 65, 67],
        duration: [500ms, 500ms, 500ms, 1000ms]
    }
    
    // Get current note from flow
    current_pitch = note_seq.pitch(counter(metro))
    current_duration = note_seq.duration(counter(metro))
    
    // Create voice for each note
    voice = synthesize(current_pitch, current_duration)
}
```

---

## 13. Built-in Functions

### 13.1 Mathematical Functions

```idl
sin(x)              // Sine
cos(x)              // Cosine
tan(x)              // Tangent
floor(x)            // Round down
ceil(x)             // Round up
round(x)            // Round to nearest
abs(x)              // Absolute value
pow(x, exp)         // Power
sqrt(x)             // Square root
fmod(x, m)          // Float modulo
mod(x, m)           // Integer modulo
min(a, b)           // Minimum
max(a, b)           // Maximum
```

### 13.2 Time & Rhythm Functions

```idl
metronome(dt)           // Trigger at intervals
beats(clock)            // Get triggers from clock
clock(bpm)              // Create named clock
tempo(bpm)              // Set global tempo
phasor(freq)            // Ramp 0-1 at frequency
counter(trigger)        // Increment on triggers
```

### 13.3 Random Functions

```idl
rnd()                   // Random 0-1
rnd(a, b)               // Random between a and b
rnd(a, b, step)         // Quantized random
rint(a, b)              // Random integer
```

### 13.4 Signal Processing

```idl
quantize(signal, div)   // Quantize to grid
wrap(value, range)      // Wrap within range
int(value)              // Convert to integer
float(value)            // Convert to float
len(flow)               // Length of flow/array
```

### 13.5 Flow Operations

```idl
concat(flow1, flow2)     // Concatenate flows
repeat(count, flow)      // Repeat flow N times
reverse(flow)            // Reverse flow elements
```

---

## Appendix: Trait System (Future Feature)

**Status**: Conceptual design for future implementation (not included in alpha)

**Overview**: A trait system allows flows and functions to share behavior and overload operators without full OOP complexity.

### Example 1: Operator Overloading

```idl
trait melody_operations {
    // Overload + to combine melodies element-wise
    op+(a : flow, b : flow) = flow:
        melody = [for i in 0..max(len(a), len(b)) : 
            max(a[i % len(a)], b[i % len(b)])]
    
    // Overload * for transpose/scale
    op*(mel : flow, factor : number) = flow:
        melody = [for i in 0..len(mel) : mel[i] * factor]
}

flow melody1 implements melody_operations = [60, 62, 64]
flow melody2 implements melody_operations = [67, 67, 65]

combined = melody1 + melody2  // Combines element-wise
transposed = melody1 * 2      // Transposes up
```

### Example 2: Shared Functions via Traits

```idl
trait sequenceable {
    // Functions all sequenceable types must provide
    fn length() : number
    fn at(idx : number) : any
    fn wrapping_index(idx : number) = at(idx % length())
}

flow drum_pattern implements sequenceable = [!, ., !, .]

flow note_pattern implements sequenceable = [60, 62, 64, 65]

// Both can use sequenceable functions
idx = counter(beat)
drum = drum_pattern.wrapping_index(idx)
note = note_pattern.wrapping_index(idx)
```

### Example 3: Polymorphic Operations

```idl
trait time_aware {
    fn duration() : time
    fn tempo_scale(multiplier : number) : self
}

flow pattern1 implements time_aware = 
    rhythm: [500ms, 500ms, 1000ms]

flow pattern2 implements time_aware = 
    rhythm: [250ms, 250ms, 500ms]

// Apply tempo change to any time_aware flow
faster_pattern1 = pattern1.tempo_scale(1.5)
```

---

## Design Notes & Critical Assessment

### Strengths

1. **Time as First-Class Citizen**: Excellent foundation for temporal/musical DSL. `dt`, triggers, and time literals are well-integrated.

2. **Functional Purity**: Explicit state management via `emit` and lambdas prevents hidden mutable state. Clean and predictable.

3. **Trigger-Based Semantics**: Distinguishing `dt=?` (trigger-driven) from `dt=time` (clock-driven) solves a real synchronization problem elegantly.

4. **Memory Operators**: The `@` operator from Faust is concise and powerful for delay-based feedback patterns.

5. **Flow Abstraction**: A clever way to express patterns and sequences without introducing list/array types at the top level.

6. **Higher-Order Functions**: First-class function creation enables powerful composition and parameter capture patterns.

### Weaknesses & Open Questions

1. **For-Loop Syntax**: The change from `=>` to `:` is marginal improvement. Consider whether generators are the right abstraction or if imperative loops are needed.

2. **Trigger Synchronization**: The `dt=?` notation is clever but unintuitive. Needs clear documentation and perhaps a keyword like `dt=trigger` or `on_trigger` would be clearer.

3. **OOP Temptation**: The module system must remain functional, but examples could still slip into chaining patterns. Vigilance required in documentation.

4. **Error Handling**: Not discussed yet—what happens when a module fails to load? How are runtime errors propagated?

5. **Voice/Instance Management**: While patterns are provided, managing dozens of overlapping voices could become unwieldy. A dedicated voice pool abstraction may be needed.

6. **Type System**: Implicit type detection works but could lead to ambiguities. Consider whether explicit type annotations would help clarity (future enhancement).

### Consistency Issues

**Current Status**: The specification is now more internally consistent:
- Lambda syntax is uniform (`|>`)
- Imports are clear (`.lib()` for Idyl, `import()` for modules)
- Functional module style is enforced throughout
- For-loop syntax is unified (`:` instead of mixed `=>`)
- Ternary out-of-range behavior is consistent (wraps)

**Areas Needing Continued Attention**:
- Keep all examples functional (no method chaining)
- Trigger vs. time-based `dt` distinction must be explicit in all examples
- Flow examples should cover both simple and complex generation patterns
- Trait system examples should evolve as usage patterns emerge

---

## Summary

This specification defines a **functional, time-aware DSL** for scheduling, composition, and real-time processing. Key principles:

1. **Functional Foundation**: Everything is a function; state is explicit
2. **Time as First-Class**: Multiple representations (ms, s, hz, beats)
3. **Temporal Functions**: Lambdas with optional init, implicit update, and trigger-based variants
4. **Memory Operators**: Faust-inspired `@` for delay/feedback
5. **Process Blocks**: Explicit execution context
6. **Flows**: Special indexed functions for patterns
7. **Higher-Order Functions**: Functions creating functions with captured scope
8. **Module System**: Function-based, no OOP
9. **Parallel Execution**: Concurrent expression evaluation
10. **Trait System**: Future feature for code reuse and polymorphism (not in alpha)

The language achieves a good balance between expressiveness and simplicity, prioritizing clarity for composers, musicians, and developers while maintaining functional purity where feasible.
