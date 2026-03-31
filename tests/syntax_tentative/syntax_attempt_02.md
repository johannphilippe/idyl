# Idyl Language Specification - Attempt 02

**Status**: Specification Document (Updated from Attempt 01 with author feedback)

**Revision Notes**: 
- Updated lambda operator from `~~` to `|>` for temporal semantics
- Removed explicit `update` block—only optional `init`, rest is implicit update
- Added memory operators section (`@`, `'`)
- Restructured flow syntax for cleaner pattern definition
- Clarified module and process block semantics
- Added multiple examples for complex voice management

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
9. [Module System](#9-module-system)
10. [Process Blocks](#10-process-blocks)
11. [Execution Model](#11-execution-model)

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
- Functions can be imported from modules or libraries, either into a namespace or globally (optional behavior determined at import time)
- Function parameters naturally shadow outer scope (standard lexical scoping)

**Examples**:
```idl
// First definition
volume = 10

// Later definition - this one wins
volume = 5  // Warning: overloading 'volume'

// Import into namespace
cs = import("csound")
// Functions accessed as: cs.function_name()

// Import into global namespace (optional)
lib("my_scale_library.idl")
// Scale functions now available globally

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

**Operator Note**: `|>` (pipe-arrow) represents the temporal flow, suggesting both the flow of computation and the movement through time. This operator is used to attach lambda blocks to temporal functions.

**Examples**:
```idl
// Simple oscillator
sine_osc(freq, dt=10ms) = sin(phase * 2 * pi)
|> {
    init: { phase = 0 }
    phase = fmod(phase + 1/freq, 1)
}

// Integrator with state
integrator(input, dt=10ms) = accumulated
|> {
    init: { accumulated = 0 }
    accumulated = accumulated + input * (dt / 1000)
}

// Counter
counter(trigger, dt=10ms) = count
|> {
    init: { count = 0 }
    count = count + (trigger ? 1 : 0)
}
```

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

// Complex initialization
delay_line(input, delay_time, dt=10ms) = delayed_sample
|> {
    init: { 
        buffer_size = int(delay_time / dt)
        buffer = [for i in 0..buffer_size => 0]
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
- `emit` can appear in init block to set initial persistent state, or in the update section to persist computed values
- Unlike traditional mutable state, emit makes persistence explicit and intentional

**Examples**:
```idl
// Emitting state across updates
running_sum(input, dt=10ms) = total
|> {
    init: { emit total = 0 }
    total = total + input  // Implicitly wrapped in emit for persistence
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

### 3.4 Memory Operators (`@` and `'`)

Inspired by Faust, memory operators provide compact delay syntax:

**Syntax**:
```
@(expression)        // Delay by one sample (previous value)
@(expression, N)     // Delay by N samples
'(expression)        // Shorthand for @(expression, 1)
```

**Behavior**:
- `@(expr)` automatically creates and manages a one-sample delay buffer
- `@(expr, N)` creates an N-sample delay
- `'(expr)` is shorthand for one-sample delay
- Useful for differentiators, feedback loops, and history tracking

**Examples**:
```idl
// One-sample delay (previous value)
previous_input(signal) = '(signal)

// N-sample delay
delayed_signal(signal, delay_samples) = @(signal, delay_samples)

// Differentiator using memory operator (cleaner than explicit state)
simple_diff(signal, dt=10ms) = (signal - '(signal)) / (dt / 1000)

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
- **Type distinction**: The engine automatically determines the type of the function's output based on context

**Examples**:
```idl
// Time values
short_duration = 100ms
long_duration = 2.5s
audio_rate = 48000hz
musical_timing = 4.0b

// Triggers
beat_trigger = beats(main_clock)
metro = metronome(100ms)  // Returns trigger

// Numbers
amplitude = 0.5
frequency = 440.0
random_value = rnd(0, 1)

// Integer conversion for bitwise operations
bit_pattern = int(255)
masked = bit_pattern & int(15)  // Bitwise AND

// Functions that return different types
tempo_function = 120  // Number
note_pattern = [60, 62, 64]  // Flow
clock_beats = beats(my_clock)  // Trigger
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

// Frequency (note: 440hz = period of ~2.27ms)
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

// Get a specific time in the fast clock (0.5 beats = 125ms at 240 BPM)
t1 = fast_clock(0.5b)

// Get beat triggers from the fast clock
fast_beats = beats(fast_clock)

// Default main clock
main_beats = beats(main_clock)
```

### 5.3 Time-Based Update (`dt`) & Inheritance

All temporal functions accept an optional `dt` (delta time) parameter specifying how often their update block executes. When a temporal function calls another temporal function, `dt` inheritance follows **caller-to-callee semantics**: the update rate is passed from the calling function to the called function.

**Default Behavior**: 
- If `dt` is not explicitly specified in the called function, it inherits from the **caller** (the function instantiating it)
- If the caller also has no explicit `dt`, it defaults to `10ms`

**Examples**:
```idl
// Explicit dt (update every 5ms)
fast_filter(input, dt=5ms) = output
~⊙ {
    init: { state = 0 }
    state = state + input * 0.1
    output = state
}

// Inherited dt from caller
slow_oscillator(freq, dt=50ms) = sin(phase * 2 * pi)
~⊙ {
    init: { phase = 0 }
    phase = fmod(phase + 1/freq, 1)
}

// When calling slow_oscillator without explicit dt
// it inherits the 50ms from its own definition
parent(input) = slow_oscillator(input)  // slow_oscillator keeps its 50ms

// Explicit override
parent_override(input) = slow_oscillator(input, dt=10ms)  // Override to 10ms
```

---

## 6. Operators & Expressions

### 6.1 Arithmetic Operators

Standard arithmetic operations: `+`, `-`, `*`, `/`, `%` (integer modulo)

**Examples**:
```idl
sum = 3 + 4          // 7
difference = 10 - 3  // 7
product = 2 * 5      // 10
quotient = 10 / 2    // 5
remainder = 17 % 5   // 2

// With time units
duration = 1s + 500ms  // 1500ms
frequency_ratio = 440hz * 2  // 880hz
```

### 6.2 Comparison Operators

Standard comparisons: `<`, `>`, `<=`, `>=`, `==`, `!=`

**Return Value**: `1` for true, `0` for false (no boolean type)

**Examples**:
```idl
is_loud = amplitude > 0.5      // 1 if true, 0 if false
is_in_range = (x > 0) * (x < 1)  // Combine conditions (1 if both true)
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
- If the index is out of range, returns `0` or silence

**Examples**:
```idl
// Two-way selection (traditional ternary)
envelope_type(is_fast) = 
    linear_env, exponential_env ? is_fast

// Multi-case selection
play_note(scale_degree) = 
    note_c, note_d, note_e, note_f, note_g ? scale_degree

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
- Triggers can be combined: `trig1 + trig2` sums triggers in a single sample cycle

**Examples**:
```idl
// Define a trigger
pulse = !

// Metronome produces triggers
metro = metronome(100ms)  // Trigger every 100ms

// Count triggers
kick_count = counter(metro)  // Increments on each metro trigger

// Combine triggers
combined = metro + beats(clock)  // Fires on either trigger
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
left_shift = a << 1  // 24
right_shift = a >> 1  // 6
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

**Repetition & Structure Syntax**: 
- `[val1, val2] |N|` - Repeat the enclosed section N times
- `[section1] |N| | [section2] |M|` - The standalone `|` indicates restart point; section2 starts fresh counting

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
flow complex_pattern() = 
    drums: [!] |2| | [., !] |2|,
    bass: [50, 52] |4|
}

// Flow with programmatic generation
flow sine_lookup(size) = 
    [for i in 0..(size-1) => sin(i / size * 2 * pi)]

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
step = counter(clock_beat)  // Increments on each beat
note_sequence = basic_pattern.note(step)
```

#### Float-Based Access  
```idl
// Flow smoothly traversed with phasor
phase = phasor(1hz)  // Ramps 0 to 1
note_sequence = basic_pattern.note(phase)  // Wraps to flow length
```

**Complex Example**:
```idl
flow timbre_flow() = {
    tone: [bright, mid, dark] |2|,
    filter: [100hz, 500hz, 2000hz, 5000hz]
}

// Trigger on beats
tone_by_beat = timbre_flow.tone(beats(main_clock))

// Integer indexing with counter
tone_manual = timbre_flow.tone(int(counter(reset_trigger)))

// Float indexing with smooth sweep
tone_slide = timbre_flow.tone(phasor(0.5hz))
```

### 7.3 Multi-Member Flow Examples

Flows become powerful when combining multiple synchronized sequences:

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

// Generative polyrhythm
flow polyrhythm(base_freq) = {
    rhythm_a: [!] |3|,
    rhythm_b: [!] |4|,
    frequency: [base_freq, base_freq * 1.5, base_freq * 2] |2|
}

// Synchronized note and velocity pairs for expressive performance
flow expressive_sequence() = {
    notes: [60, 62, 65, 67, 69],
    velocities: [80, 95, 100, 90, 75]
}
```

---

## 8. Control Flow

### 8.1 Conditional via Ternary

Idyl uses the numeric ternary approach rather than traditional if/else (see Section 6.3):

**Pattern**:
```
false_value, true_value ? condition
// condition = 0 returns false_value
// condition != 0 returns true_value
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
// Scale note selection (0-indexed)
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

Instead of traditional for-loops, use flow generators:

**Syntax**:
```
[for variable in range => expression]
```

**Examples**:
```idl
// Generate sine table
sinetable(size) = [for i in 0..size => sin(i / size * 2 * pi)]

// Generate scale offsets
intervals = [for i in 0..8 => i * 2]  // [0, 2, 4, 6, 8, 10, 12, 14]

// Generate frequency ratios
harmonics(fundamental) = 
    [for harmonic in 1..16 => fundamental * harmonic]
```

---

## 9. Module System

### 9.1 Module Design Philosophy

Modules expose **functions only**—no OOP-style methods or stateful objects. This keeps the language functional and prevents deeply nested method chains. Modules can return:
- **Functions**: Callable units for composition
- **Values**: Constants, configurations, or computed results
- **Wrapped Engines**: Opaque handles (e.g., Csound instances) accessed via functional APIs

The emphasis is on simplicity and composability, avoiding object-oriented complexity.

### 9.2 Module Import

**Syntax**:
```
module_name = import("module_path")
```

**Behavior**:
- Imports a compiled module with a defined API
- Module functions are accessed via dot notation: `module_name.function_name()`
- Errors during import are reported to console; the runtime continues (no interruption)

**Examples**:
```idl
// Import Csound module
cs = import("csound")

// Import OSC module
osc = import("osc")

// Import custom Idyl library (within idyl namespace)
custom = import("my_lib.idl")
```

### 9.3 Module Function Calls

Module functions are called using standard function call syntax on the module handle.

**Syntax**:
```
module_name.function_name(arguments)
```

**Examples**:
```idl
// Csound workflow
cs = import("csound")
orchestra = cs.compile("synth.orc")
instrument = orchestra.get("my_instr")

// Invoke instrument (start at time 0s, run for 5s)
voice = instrument(0s, 5s)

// OSC workflow
osc_module = import("osc")
sender = osc_module.sender("192.168.1.47", 8080)
sender.send("/synth/freq", current_frequency)
```

### 9.4 Library Imports (Idyl Code)

Custom Idyl libraries can be imported to share functions and definitions:

**Syntax**:
```
import("library_path.idl")           // Load into namespace or global (depends on implementation)
lib("library_path.idl")              // Explicit library load
```

**Examples**:
```idl
// Import library
synth_lib = import("synths/common.idl")
synth_lib.sine_synth(440hz)

// Load via lib()
lib("presets/scales.idl")
// Scale and note functions now available
```

---

## 10. Process Blocks

### 10.1 Main Process Block

The `process` block is the **primary entry point** for code execution. Only code within a process block runs when a file is loaded.

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
- Expressions are evaluated and their side effects propagate (e.g., scheduling, OSC sends, module calls)
- Parallel execution of expressions (all active simultaneously)

**Examples**:
```idl
// Simple metronome trigger
process: {
    metro = metronome(500ms)
    // metro triggers are sent to connected engines
}

// Multiple oscillators mixed
process: {
    osc1 = sine_osc(440hz, dt=5ms)
    osc2 = sine_osc(880hz, dt=5ms)
    env = amplitude_envelope(2s)
    
    final_signal = (osc1 + osc2) * env * 0.5
    // final_signal is available for downstream processing
}

// With imported modules
process: {
    cs = import("csound")
    instr = cs.compile("synth.orc").get("my_instr")
    
    voice = instr(0s, 5s)
    // voice runs for 5 seconds
}
```

### 10.2 Multiple Process Blocks (Composition/Sequencing)

For composition, where you want to define different sections that play sequentially, use named process blocks with optional duration:

**Syntax**:
```idl
process intro, duration=8s: {
    // Intro section code
}

process verse, duration=16s: {
    // Verse section code
}

process chorus, duration=32s: {
    // Chorus section code
}

process main: {
    // Main orchestration
    // Sequence the named processes
}
```

**Examples**:
```idl
// Define separate sections with optional durations
process verse, duration=16s: {
    metro = metronome(500ms)
    bass = sine_osc(50hz) * metro
    melody = flow_playback(melody_flow, metro) 
}

process chorus, duration=16s: {
    metro = metronome(250ms)  // Faster tempo
    stab = square_wave(220hz) * metro
    pad = sine_osc(110hz, dt=50ms)
}

// Main orchestration (optional)
process main: {
    // Plays verse, then chorus
    // Can add logic to chain them
}
```

### 10.3 Process Lifecycle (Advanced)

For advanced control, processes can include optional initialization hooks:

**Syntax**:
```idl
process my_process: {
    init: { 
        // Runs once at startup
    }
    
    // Main continuous processing
}
```

**Examples**:
```idl
process synth_engine: {
    init: {
        cs = import("csound")
        orchestra = cs.compile("synth.orc")
        active_voices = 0
    }
    
    // Main execution
    metro = metronome(100ms)
    active_voices = active_voices + metro  // Increment on each trigger
}
```

---

## 11. Execution Model

### 11.1 File Loading & Compilation

**Execution Steps**:
1. File is parsed and all function definitions are compiled into models
2. Process blocks are identified
3. Main process block (if present) is executed
4. Named process blocks are available for composition

**Implications**:
- Functions are **eagerly compiled** but **lazily instantiated** (only when called/used)
- Top-level function definitions create callable models
- Only process blocks trigger actual runtime execution

**Examples**:
```idl
// These are just compiled, not executed:
sine_osc(freq) = sin(phase * 2 * pi)
|> { init: { phase = 0 } phase = fmod(phase + 1/freq, 1) }

// This is executed when file loads:
process: {
    output = sine_osc(440hz)  // Now instantiated and runs
}
```

### 11.2 Temporal Function Instantiation

When a temporal function is called within a process, it is **instantiated immediately** with its own state.

**Behavior**:
- `init` block runs once at instantiation
- Update code starts running at the next cycle
- Each call creates a **new instance** with separate state

**Examples**:
```idl
// Two independent oscillators with separate phase state
process: {
    osc1 = sine_osc(440hz, dt=10ms)   // Instance 1: phase starts at 0
    osc2 = sine_osc(880hz, dt=10ms)   // Instance 2: phase starts at 0
    
    combined = osc1 + osc2
}
```

### 11.3 Parallel Execution

Expressions in a process block execute in **parallel** (not sequentially).

**Implication**: Assignment order does not matter; all are computed simultaneously.

**Examples**:
```idl
process: {
    // These all run in parallel:
    osc1 = sine_osc(440hz)
    osc2 = sine_osc(880hz)
    env = amplitude_envelope(2s)
    output = (osc1 + osc2) * env
    
    // Order doesn't matter - they compute together
}
```

### 11.4 State Persistence & Voice Management

**Voice State**:
- Each temporal function instance maintains its own state
- State persists for the lifetime of the instance

**Voice Lifecycle**:
- Voices are created when functions are instantiated
- Voices run until explicitly terminated or their duration expires
- Use `catch` clause to handle voice termination

### 11.5 Polyphonic Voice Management

Managing multiple simultaneous voices requires careful state tracking. Here are patterns for common scenarios:

#### Pattern 1: Simple Polyphonic Synth (Fixed Voices)

```idl
process: {
    cs = import("csound")
    synth = cs.compile("synth.orc").get("piano")
    
    // Allocate N voices, each with independent state
    voice1 = synth(0s, 5s)
    voice2 = synth(0s, 5s)
    voice3 = synth(0s, 5s)
    
    // Mix all voices
    output = voice1 + voice2 + voice3
}
```

#### Pattern 2: Dynamic Voice Allocation (Trigger-Driven)

```idl
// Note: This is pseudo-code showing the conceptual approach
// Actual implementation may require runtime voice pool management

process voice_pool: {
    cs = import("csound")
    synth = cs.compile("synth.orc").get("note")
    
    metro = metronome(250ms)
    note_flow = note_sequence(metro)
    
    // For each trigger, create and manage a voice instance
    // This requires runtime voice pool capable of dynamic allocation
    
    voice = synth(0s, 1s)  // Each voice duration = 1 second
    output = voice
}
```

#### Pattern 3: Streaming Voice Updates (Time-Based)

```idl
process sequential_voices: {
    cs = import("csound")
    synth = cs.compile("synth.orc").get("note")
    
    metro = metronome(250ms)
    note_flow = note_sequence(metro)
    
    // Create new voice on each trigger, each lasting 1 second
    voice_time = counter(metro) * 250ms  // Time offset per voice
    current_note = note_flow(metro)
    
    // Start a new voice at each time interval
    voice = synth(voice_time, voice_time + 1000ms)
    output = voice
}
```

#### Pattern 4: Overlapping Voices with Envelope Shaping

```idl
process polyphonic_shaping: {
    cs = import("csound")
    synth = cs.compile("synth.orc").get("string")
    
    metro = metronome(100ms)
    note_flow = note_sequence(metro)
    
    // Each voice gets its own envelope
    voice1 = synth(0s, 2s) * amplitude_envelope(1000ms, attack=100ms, release=200ms)
    voice2 = synth(500ms, 2500ms) * amplitude_envelope(1000ms, attack=50ms, release=150ms)
    voice3 = synth(1000ms, 3000ms) * amplitude_envelope(1000ms, attack=200ms, release=300ms)
    
    // Blend with crossfades
    output = voice1 * 0.8 + voice2 * 0.6 + voice3 * 0.4
}
```

#### Considerations for Complex Voice Management

- **Statefulness**: Each voice instance holds its own `emit`-marked state
- **Complexity**: As voice count grows, managing interactions becomes intricate
- **Future Extensions**: Consider a dedicated **voice pool** abstraction for dynamic allocation
- **Performance**: Monitor polyphonic load; excessive voice counts may stress the system

---

## 12. Built-in Functions

### 12.1 Mathematical Functions

Standard math library (implemented via interpreter fast-math):

```idl
sin(x)              // Sine
cos(x)              // Cosine
tan(x)              // Tangent
floor(x)            // Round down to integer
ceil(x)             // Round up to integer
round(x)            // Round to nearest integer
round(x, step)      // Round to nearest step
abs(x)              // Absolute value
pow(x, exponent)    // Power
sqrt(x)             // Square root
fmod(x, modulo)     // Floating-point modulo
mod(x, modulo)      // Integer modulo
x % modulo          // Integer modulo operator
min(a, b)           // Minimum
max(a, b)           // Maximum
```

### 12.2 Time & Rhythm Functions

```idl
metronome(dt)           // Trigger at regular intervals
beats(clock)            // Get triggers from clock
clock(bpm)              // Create named clock
tempo(bpm)              // Set global tempo
phasor(freq, dt)        // Ramp 0-1 at specified frequency
counter(trigger, dt)    // Increment counter by triggers
```

### 12.3 Random Functions

```idl
rnd()                   // Random 0-1
rnd(a, b)               // Random between a and b
rnd(a, b, step)         // Random between a and b, quantized to step
rint(a, b)              // Random integer (shorthand for rnd(a, b, 1))
```

### 12.4 Signal Processing

```idl
quantize(signal, subdivision)  // Quantize to musical grid
wrap(value, range)             // Wrap value within range
int(value)                      // Convert to integer type
float(value)                    // Ensure float representation
```

### 12.5 Flow Operations

```idl
concat(flow1, flow2)         // Concatenate two flows
repeat(count, flow)          // Repeat flow N times
```

---

## Appendix: Trait System (Conceptual)

**Proposed Future Feature**: A trait system (inspired by Rust) could allow flows and functions to share behavior, implement common interfaces, and overload operators without introducing full OOP.

**Example Concept**:
```idl
trait Sequenceable {
    index(idx)
    length()
}

flow melody implements Sequenceable = { ... }
```

This remains speculative and should be revisited as the language matures and patterns emerge.

---

## Summary

This specification defines a **functional, time-aware DSL** for audio scheduling and composition. Key design principles:

1. **Functional Foundation**: Everything is a function; state is explicit
2. **Time as First-Class**: Multiple time representations (ms, s, hz, beats)
3. **Temporal Functions**: Lambdas with optional init, implicit update (`|>` operator)
4. **Memory Operators**: Compact delay syntax (`@`, `'`) from Faust
5. **Process Blocks**: Explicit execution context with optional composition
6. **Flows**: Special indexed functions for patterns and sequences
7. **Module System**: Function-based integration (no OOP)
8. **Parallel Execution**: Expressions run concurrently within a block
9. **Voice Management**: Multiple temporal instances with separate state

The language prioritizes **expressiveness**, **clarity**, and **audio-specific semantics** while maintaining functional purity where feasible.
