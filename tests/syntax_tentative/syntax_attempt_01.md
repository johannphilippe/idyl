# Idyl Language Specification - Attempt 01

**Status**: Specification Document (Generated from analysis review with author feedback)

**Generation Note**: This document synthesizes the analysis from `claude_overview.md` and the author's responses. Where the author provided explicit answers, they have been followed exactly. Where responses were open or interrogative, I have made suggestions (marked with **[INITIATIVE]**).

---

## Table of Contents

1. [Core Language Model](#1-core-language-model)
2. [Function Definitions](#2-function-definitions)
3. [Temporal Functions & Lambdas](#3-temporal-functions--lambdas)
4. [Primitive Types](#4-primitive-types)
5. [Time System](#5-time-system)
6. [Operators & Expressions](#6-operators--expressions)
7. [Control Flow](#7-control-flow)
8. [Flows](#8-flows)
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

### 1.2 Namespace & Shadowing

- A file evaluates (compiles) all functions, creating models for runtime use
- If two functions share the same name, the last loaded function wins (with warning)
- Functions defined in libraries are loaded into a namespace derived from the library name
- Function parameters naturally shadow outer scope (standard lexical scoping)

**Examples**:
```idl
// First definition
volume = 10

// Later definition - this one wins
volume = 5  // Warning: overloading 'volume'

// With libraries
cs = import("csound")
// Functions from csound module are accessed as: cs.function_name()

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
~~ {
    init: { initialization_statements }
    update: { update_statements }
}
```

**[INITIATIVE] Note on Operator**: The analysis suggested `~>` but the lambda operator semantics need clarification. I propose `~~` (double tilde) to differentiate more clearly from comparison operators and to emphasize the "continuation" nature. This avoids confusion with `<` and `>` operators.

**Examples**:
```idl
// Simple oscillator
sine_osc(freq, dt=10ms) = sin(phase * 2 * pi)
~~ {
    init: { phase = 0 }
    update: { phase = fmod(phase + 1/freq, 1) }
}

// With emitted state
integrator(input, dt=10ms) = accumulated
~~ {
    init: { accumulated = 0 }
    update: { 
        accumulated = accumulated + input * (dt / 1000)
    }
}

// Counter with multiple emissions
counter(trigger, dt=10ms) = count
~~ {
    init: { count = 0 }
    update: { count = count + (trigger ? 1 : 0) }
}
```

### 3.2 Initialization Block (`init`)

The initialization block runs once when the function is first instantiated.

**Behavior**:
- Executed immediately after function instantiation
- Typically sets initial values for state variables
- All variables defined here are available in the update block

**Examples**:
```idl
// Simple initialization
low_pass(input, cutoff, dt=10ms) = output
~~ {
    init: { 
        state = 0 
        alpha = 0.1
    }
    update: { 
        output = state
        state = state + (input - state) * alpha
    }
}

// Complex initialization
delay_line(input, delay_time, dt=10ms) = delayed_sample
~~ {
    init: { 
        buffer_size = int(delay_time / dt)
        buffer = [for i in 0..buffer_size => 0]
        write_pos = 0
    }
    update: { 
        delayed_sample = buffer[wrap(write_pos - delay_time/dt, buffer_size)]
        buffer[wrap(write_pos, buffer_size)] = input
        write_pos = write_pos + 1
    }
}
```

### 3.3 Update Block (`update`)

The update block runs at each time step (`dt` interval) when the function is active.

**Behavior**:
- Executes repeatedly at the specified `dt` precision
- Can read and modify state variables (via emission)
- All statements here have access to both init-defined state and function parameters

**Examples**:
```idl
// Simple update
ramp(start, end, duration, dt=10ms) = value
~~ {
    init: { 
        progress = 0
        step = dt / duration
    }
    update: { 
        value = start + (end - start) * progress
        progress = progress + step
    }
}

// Update with conditions
frequency_modulator(base_freq, mod_signal, dt=10ms) = modulated_freq
~~ {
    init: { 
        current_mod = 0
    }
    update: { 
        current_mod = mod_signal * 0.5  // Limit modulation
        modulated_freq = base_freq * (1 + current_mod)
    }
}
```

### 3.4 State Emission & Memory

Within lambda blocks, variables can be marked for emission using the `emit` keyword to define state that persists across updates.

**Syntax**:
```
emit variable_name = value
```

**Behavior**:
- Variables marked with `emit` persist their value across update cycles
- Without `emit`, variables are temporary (computed fresh each cycle)
- Emitted variables maintain state for use in subsequent updates

**Examples**:
```idl
// Emitting state across updates
running_sum(input, dt=10ms) = total
~~ {
    init: { 
        emit total = 0 
    }
    update: { 
        emit total = total + input
    }
}

// Using memory operator for more compact syntax
// [INITIATIVE] Proposed: @ operator for previous value (inspired by Faust)
// This provides a compact way to reference the previous cycle's value
delay_one_sample(input) = previous_input
~~ {
    init: { previous_input = 0 }
    update: { previous_input = input }
}

// Shorthand with @ operator (proposed for future)
// delay_one_sample(input) = @input
// This automatically creates state for the previous value
```

---

## 4. Primitive Types

Idyl recognizes the following primitive types:

| Type | Description | Examples |
|------|-------------|----------|
| **Time** | Temporal duration or frequency | `100ms`, `2.5s`, `440hz`, `1.5b` (beats) |
| **Trigger** | Instantaneous impulse/event | `!`, `beats(clock)` |
| **Number** | Floating-point value (64-bit) | `3.14`, `0`, `-1.5` |
| **Flow** | Sequence/pattern with indexed access | Defined in Flow section |
| **Function** | Callable unit | Any defined function |

### 4.1 Type Characteristics

- **Time** is always internally converted to milliseconds for consistent computation
- **Numbers** are always 64-bit floating-point
- **Triggers** are a first-class primitive (not just boolean values)
- **Flows** are special functions that support both indexed and continuous access
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
// [INITIATIVE] Note: For automatic conversion, no explicit casting needed
// The interpreter handles all conversions transparently
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

### 5.3 Time-Based Update (`dt`)

All temporal functions accept an optional `dt` (delta time) parameter specifying how often their update block executes.

**Default Behavior**: `dt` is inherited from the parent function or defaults to `10ms` if unspecified.

**Examples**:
```idl
// Explicit dt (update every 5ms)
fast_filter(input, dt=5ms) = output
~~ {
    init: { state = 0 }
    update: { state = state + input * 0.1 }
}

// Inherited dt from parent
parent(input) = fast_filter(input)  // fast_filter uses 5ms from parent

// Default dt
slow_processor(input) = process(input, dt=50ms)
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
```

### 6.3 Ternary/Multi-Way Selector

**[INITIATIVE] Proposed Ternary with Multiple Cases**:

Since Idyl uses numeric conditions (not true/false), a multi-way selector makes sense:

**Syntax**:
```
selector(index, option0, option1, option2, ...)
// Returns option_N based on index value
// If index is not an integer, it's rounded down
// If index is out of range, returns 0/silence
```

Alternatively, using the question mark operator as in your basic.idl:

**Syntax**:
```
option0, option1, option2 ? condition
```

**Examples**:
```idl
// Using selector function
choose_note(scale_degree) = 
    notes(scale_degree, [60, 62, 64, 65, 67])

// Using condition-based selection (from basic.idl)
envelope_type(is_fast) = 
    linear_env, exponential_env ? is_fast

// Multi-case with multiple conditions
state_machine(current_state) =
    idle_state, 
    attack_state, 
    decay_state, 
    release_state ? current_state
```

### 6.4 Emission Operator

The `emit` operator marks a value for persistence across function updates.

**Syntax**:
```
emit variable_name = expression
```

**Behavior**:
- Available only within lambda blocks (`~~` { }` blocks)
- The emitted value persists to the next update cycle
- Non-emitted assignments are temporary (local scope only)

**Examples**:
```idl
// State that persists
counter(trig, dt=10ms) = count
~~ {
    init: { emit count = 0 }
    update: { 
        // Temporary: increment_value only exists this cycle
        increment_value = trig ? 1 : 0
        // Persistent: count is saved for next cycle
        emit count = count + increment_value
    }
}

// Differentiator (stores previous value)
differentiate(input, dt=10ms) = derivative
~~ {
    init: { emit prev_value = input }
    update: { 
        derivative = (input - prev_value) / (dt / 1000)
        emit prev_value = input
    }
}
```

### 6.5 Trigger Primitive (`!`)

The `!` symbol represents an instantaneous trigger/impulse.

**Behavior**:
- A trigger is a discrete event (not a continuous signal)
- Triggers are the first-class primitive for timing and scheduling
- Triggers can be combined: `trig1 + trig2` (sum of triggers)

**Examples**:
```idl
// Define a trigger
pulse = !

// Metronome produces triggers
metro = metronome(100ms)  // Trigger every 100ms

// Gate (modulate trigger with condition)
conditional_trigger(condition) = 
    ! if condition else nothing

// Combine triggers
combined = metro + beats(clock)  // Fires on either trigger
```

### 6.6 Return from Lambda Blocks (`>`)

**[INITIATIVE] Proposed: Return operator for lambda blocks**

In a lambda block, you may want to override the main output. Since `return` is verbose and `<` is ambiguous, I propose the `>` operator:

**Syntax**:
```
> expression   // Override and return this value
```

**Behavior**:
- Can appear in lambda blocks after emit statements
- Replaces the main function output with the specified expression
- Useful for post-processing or conditional returns

**Examples**:
```idl
// Simple return
modulated_sine(freq, mod_signal, dt=10ms) = sin(phase * 2 * pi)
~~ {
    init: { phase = 0 }
    update: { 
        phase = fmod(phase + 1/freq, 1)
        > sin(phase * 2 * pi) * (1 + mod_signal)  // Override output
    }
}

// Conditional return
gated_oscillator(freq, gate, dt=10ms) = sin(phase * 2 * pi)
~~ {
    init: { phase = 0 }
    update: { 
        phase = fmod(phase + 1/freq * gate, 1)
        > gate ? (sin(phase * 2 * pi) * gate) : 0
    }
}
```

---

## 7. Control Flow

### 7.1 Conditional via Selector

Idyl uses the numeric selector approach rather than traditional if/else:

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

### 7.2 Multi-Way Selection

For more than two options, use the selector function:

**Syntax**:
```
selector(index, case0, case1, case2, ...)
```

**Examples**:
```idl
// Scale note selection (0-indexed)
scale_note(degree, scale_root) = selector(
    degree % 8,
    scale_root + 0,      // Case 0
    scale_root + 2,      // Case 1
    scale_root + 4,      // Case 2
    scale_root + 5,      // Case 3
    scale_root + 7,      // Case 4
    scale_root + 9,      // Case 5
    scale_root + 11,     // Case 6
    scale_root + 12      // Case 7
)

// State machine
state_output(state) = selector(
    state,
    idle_output,
    attack_output,
    decay_output,
    sustain_output,
    release_output
)
```

### 7.3 Loops via Flows & Generators

**[INITIATIVE] Proposed: Generator expressions for programmatic flow creation**

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

## 8. Flows

A **Flow** is a special function that organizes data into indexed sequences with multiple named members.

### 8.1 Flow Definition

**Syntax**:
```
flow flow_name(params) = {
    member1: values_or_expression,
    member2: values_or_expression,
    ...
}
```

**Array Syntax**: 
- `[val1, val2, val3]` - Direct array
- `[val1, val2] |2| [val3, val4]` - Repetition syntax (repeat first section 2 times)
- `[val1, val2] |2|| [val3, val4]` - **[INITIATIVE]** Restart syntax (section repeats, but restart iteration counter for following sections)

**Examples**:
```idl
// Simple flow
flow basic_pattern() = {
    rhythm: [!, ., ., !],
    note: [60, 62, 64, 67]
}

// Flow with repetition
flow verse() = {
    kick: [!] |4|,        // Kick 4 times
    bass: [50] |4|,       // Low note 4 times
    melody: [60, 62, 64, 65] |2|  // Melody repeats twice
}

// Flow with restart operator (proposed)
flow complex_pattern() = {
    drums: [!] |2|| [., !] |2||,  // Pattern, repeat twice, restart, new pattern, repeat twice
    bass: [50, 52] |4||
}

// Flow with programmatic generation
flow sine_lookup(size) = {
    sine_values: [for i in 0..(size-1) => sin(i / size * 2 * pi)]
}

// Flow with arpeggio
flow arp(scale, offset) = {
    note: [scale[0] + offset, scale[2] + offset, scale[4] + offset]
}
```

### 8.2 Flow Access

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
note_sequence = basic_pattern(metro)

// Access specific member
current_note = note_sequence.note
rhythm = note_sequence.rhythm(metro)
```

#### Integer-Based Access
```idl
// Flow indexed by counter
step = counter(clock_beat)  // Increments on each beat
note_sequence = basic_pattern(step)

// Must explicitly convert if not already integer
notes = basic_pattern(int(counter(clock_beat)))
```

#### Float-Based Access  
```idl
// Flow smoothly traversed with phasor
phase = phasor(1hz)  // Ramps 0 to 1
note_sequence = basic_pattern(phase)  // Wraps to flow length

// No type specification needed for float
sweep = basic_pattern(phasor(2hz))
```

**Complex Example**:
```idl
// Combine multiple indexing methods
timbre_flow = {
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

---

## 9. Module System

### 9.1 Module Import

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

### 9.2 Module Function Calls

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

// Handle voice lifecycle
voice catch end => 
    print("Voice ended")

// OSC workflow
osc_module = import("osc")
sender = osc_module.sender("192.168.1.47", 8080)
sender.send("/synth/freq", current_frequency)

receiver = osc_module.receiver(8081)
```

### 9.3 Library Imports (Idyl Code)

**[INITIATIVE] Proposed: Idyl library loading**

Custom Idyl libraries can be imported to share functions and definitions:

**Syntax**:
```
lib("library_path.idl")           // Load into global namespace (dangerous)
namespace = lib("library_path.idl")  // Load into namespace
```

**Examples**:
```idl
// Load library into namespace
synth_lib = lib("synths/common.idl")
synth_lib.sine_synth(440hz)

// Load into global namespace (be careful!)
lib("presets/scales.idl")
// Now scale and note functions are available globally
```

---

## 10. Process Blocks

### 10.1 Main Process Block

**[INITIATIVE] Comprehensive process block design**

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
- Output is sent to the default output channel
- Parallel execution of expressions (all active simultaneously)

**Examples**:
```idl
// Simple metronome output
process: {
    metro = metronome(500ms)
    output = metro
}

// Multiple oscillators mixed
process: {
    osc1 = sine_osc(440hz, dt=5ms)
    osc2 = sine_osc(880hz, dt=5ms)
    env = amplitude_envelope(2s)
    
    output = (osc1 + osc2) * env * 0.5
}

// With imported modules
process: {
    cs = import("csound")
    instr = cs.compile("synth.orc").get("my_instr")
    
    voice = instr(0s, 5s)
    
    output = voice
}
```

### 10.2 Multiple Process Blocks (Composition/Sequencing)

**[INITIATIVE] Proposed: Named processes with sequencing**

For composition, where you want to define different sections that play sequentially, use named process blocks with scheduling:

**Syntax**:
```idl
process intro : {
    // Intro section code
}

process verse : {
    // Verse section code
}

process chorus : {
    // Chorus section code
}

process sequence : {
    // Define the playback sequence
    play(intro, 0s)
    then(verse, duration_of_intro)
    then(chorus, duration_of_intro + duration_of_verse)
}
```

**Alternatively: Automatic Sequencing**

**[INITIATIVE] Proposed: Define processes with automatic timing**

```idl
process intro="path/to/intro.idl", duration=8s
process verse="path/to/verse.idl", duration=16s
process chorus="path/to/chorus.idl", duration=32s

process main: {
    sequence = [intro, verse, chorus] |3|  // Repeat 3 times
}
```

**Examples**:
```idl
// Define separate sections
process verse: {
    metro = metronome(500ms)
    bass = sine_osc(50hz) * metro
    melody = flow_playback(melody_flow, metro) 
    output = bass + melody
}

process chorus: {
    metro = metronome(250ms)  // Faster tempo
    stab = square_wave(220hz) * metro
    pad = sine_osc(110hz, dt=50ms)
    output = stab + pad * 0.3
}

// Main orchestration
process main: {
    section1 = verse
    section2 = chorus
    
    // Play verse for 8s, then chorus for 8s, then repeat
    output = sequence([verse, chorus], [8s, 8s])
}
```

### 10.3 Process Lifecycle (Advanced)

**[INITIATIVE] Proposed: Lifecycle hooks within processes**

For advanced control, processes can include lifecycle hooks:

**Syntax**:
```idl
process my_process: {
    init: { 
        // Runs once at startup
    }
    
    // Main continuous processing
    // (default behavior)
    
    cleanup: {
        // Runs when process ends
    }
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
    active_voices += metro  // Increment on each trigger
    output = orchestra.play_voice(current_note, active_voices)
    
    cleanup: {
        cs.stop_engine()
    }
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
~~ { init: { phase = 0 } update: { phase = fmod(phase + 1/freq, 1) } }

// This is executed when file loads:
process: {
    output = sine_osc(440hz)  // Now fused instantiated and runs
}
```

### 11.2 Temporal Function Instantiation

When a temporal function is called within a process, it is **instantiated immediately** with its own state.

**Behavior**:
- `init` block runs once at instantiation
- `update` block starts running at the next cycle
- Each call creates a **new instance** with separate state

**Examples**:
```idl
// Two independent oscillators with separate phase state
process: {
    osc1 = sine_osc(440hz, dt=10ms)   // Instance 1: phase starts at 0
    osc2 = sine_osc(880hz, dt=10ms)   // Instance 2: phase starts at 0
    
    output = osc1 + osc2
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

### 11.4 Realtime Mode (Optional)

**[INITIATIVE] Proposed: Optional realtime guarantees**

For applications requiring predictable timing, specify realtime mode:

**Syntax**:
```
process realtime=soft:  { ... }  // Default: tolerates drift
process realtime=hard:  { ... }  // Fails if deadlines missed
process realtime=adaptive: { ... }  // Adjusts dt to system load
```

**Default**: `soft` (tolerant of system load variations)
**CLI Override**: Can be set from command line for all processes

**Examples**:
```idl
// Strict timing requirements
process realtime=hard: {
    clock_output = beats(precise_clock)
}

// Normal timing (default)
process: {
    synth_output = sine_osc(440hz)  // May drift imperceptibly
}

// Adaptive timing
process realtime=adaptive: {
    filter_output = low_pass(input, 100hz)  // Adjusts dt automatically
}
```

### 11.5 State Persistence & Voice Management

**Voice State**:
- Each temporal function instance maintains its own state
- State persists for the lifetime of the instance

**Voice Lifecycle**:
- Voices are created when functions are instantiated
- Voices run until explicitly terminated or their duration expires
- Use `catch` clause to handle voice termination

**Examples**:
```idl
process: {
    instr = import("csound").compile("synth.orc").get(1)
    
    // Voice runs for 5 seconds, then terminates
    voice = instr(0s, 5s) 
    
    // Handle termination
    voice catch end => {
        print("Voice ended")
        next_voice = instr(5s, 10s)
    }
    
    output = voice
}
```

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
int(value)                      // Extract integer type conversion
float(value)                    // Ensure float representation
```

### 12.5 Flow Operations

```idl
concat(flow1, flow2)         // Concatenate two flows
repeat(count, flow)          // Repeat flow N times
selector(index, val0, val1, val2, ...)  // Select by index
```

---

## Summary

This specification defines a **functional, time-aware DSL** for audio scheduling and composition. Key design principles:

1. **Functional Foundation**: Everything is a function; state is explicit
2. **Time as First-Class**: Multiple time representations (ms, s, hz, beats)
3. **Temporal Functions**: Lambdas for stateful processing (`init`/`update` blocks)
4. **Process Blocks**: Explicit execution context
5. **Flows**: Indexed sequences for patterns and data
6. **Module System**: Integration with compiled engines and custom code
7. **Parallel Execution**: Processes run concurrently within a block
8. **Optional Realtime Guarantees**: Soft/hard real-time modes for precision control

The language prioritizes **expressiveness**, **clarity**, and **audio-specific semantics** while maintaining functional purity where feasible.
