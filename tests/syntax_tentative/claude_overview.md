# Idyl Syntax Analysis & Review

## Executive Summary

The Idyl language shows strong potential as a temporal DSL for audio programming, with a generally coherent design philosophy around functional, time-aware primitives. However, several inconsistencies, design tensions, and missing features require attention before full language definition.

---

## 1. Syntax Inconsistencies

### 1.1 Function Definition Syntax Ambiguity
**Issue**: Mixed notation for function definitions and temporal lambdas creates cognitive load.

```idl
// Basic function
f(a, b) = a - b

// Temporal function with lambda
sine_oscillator(frequency, dt=10ms) = sin(phase * 2 * pi)
~> {
    [init_block]
    update_block
}
```

**Problem**: The `~>` lambda operator appears after the main statement, but it's unclear if:
- This is a required continuation or optional enhancement?
- Can you have `~>` without an initial expression?
- What's the scoping relationship between the initial expression and the lambda body?

**Recommendation**: Consider a more explicit syntax like:
```idl
sine_oscillator(frequency, dt=10ms) = lambda(dt=10ms) {
    init: [phase = 0]
    compute: sin(phase * 2 * pi)
    update: { phase = fmod(phase + incr, 1) }
    emit: [phase!]
}
```

**My Answer**: The lambda operator is not necessary. It is a way to update a function with internal functions (such as phase in the example). The function does still need to have at least one statement. It is still not clear to me if I will introduce a concept of parallel statement (Faust style), separated with a comma for example.  

### 1.2 Emission Operator (`!`) Overloading
**Issue**: The `!` character has dual meanings within the same context:

```idl
// Trigger primitive
metronome(dt=50ms) = !

// Variable emission
phase! = fmod(phase + incr, 1)
```

**Problem**: 
- Is `!` a value type (like the trigger impulse) or an operator (like assignment)?
- How do you distinguish `= !` (assign trigger) from `= thing!` (emit thing)?
- Does `!` have different semantics in init vs. update blocks?

**Recommendation**: Use distinct operators:
- `~` for trigger impulse emissions
- `!>` for variable capture/emission to parent scope

**My Answer**: You are right, it is ambiguous. Maybe for now we should separate these statements. 
We may do something like : 
- `!` for trigger (I like how it feels)
- `emit phase = // something`  for emission (feel free to propose something else more expressive if you have suggestions)

### 1.3 Lambda Capture Syntax Inconsistency
**Issue**: Two different syntaxes for capturing lambda outputs:

```idl
// Method 1: Inline via tilde-pipe (unclear syntax)
other_function = receiver ~> (address, value) {
    // address, value magically come from receiver?
}

// Method 2: Explicit parameter list
modulate(dt = 50ms) = sine_oscillator(10hz) ~> [phase] {
    amp_mod = fmod(phase * 4, 1)  
    < inp * amp_mod
}
```

**Problem**: 
- First syntax doesn't show where `address` and `value` originate
- Second syntax uses `[phase]` but unclear if this is destructuring or module interface

**Recommendation**: Make both explicit:
```idl
// Capture from named outputs
other_function = receiver ~> [address, value] {
    // Use address and value
}
```

**My Answer**: You are right. It is a mix of the old syntax and the new. The correct one is the one from your recommandation (with `[]`). 

### 1.4 Return Operator (`<`) Ambiguity
**Issue**: The `< value` syntax for returning from lambdas is non-standard and conflicts with comparison operators.

```idl
< inp * amp_mod  // What does this return? The modified input?
```

**Problem**:
- Readers might confuse `<` (less-than) with return
- It's unclear if `<` replaces the whole return value or modifies in-place
- No precedent in common languages for this operator

**Recommendation**: Use explicit `return` or `emit`:
```idl
emit: inp * amp_mod
// OR
return inp * amp_mod
```

**My Answer**: I tried this, and you're right, I'm not convinced. The only problem with `return` is that it's long. Any idea ? If we choose emit, then it must be consistant with emission in the previous context : `emit phase = // something`

### 1.5 Inconsistent Initialization Block Syntax
**Issue**: Initialization blocks use inconsistent bracket types:

```idl
// Square brackets
~> {
    [phase = 0]
    phase = fmod(phase + incr, 1)
}

// But elsewhere used implicitly
weird_oscillator(freq, random_amount, dt=10hz) = sine_oscillator(freq){
    // Is this init or direct override?
    phase += rnd(0, random_amount)
}
```

**Problem**: The semantics of `[]` are unclear. Is it:
- A list/array syntax (inconsistent with the flow `[]` usage)?
- A special "initialization context" marker?
- Something different in different contexts?

**Recommendation**: Use explicit keywords:
```idl
~> {
    init: { phase = 0 }
    update: { phase = fmod(phase + incr, 1) }
}
```

**My Answer**: These are two different things. First one is a lambda. The other one is an appendice to the original lambda. I tried to avoid `init:` `update:` to keep the syntax as compact as I could. But maybe it's not expressive enough. Do you have any suggestion to make this more consistent & compact at the same time ? 

---

## 2. Missing Features & Gaps

### 2.1 No Error Handling or Failure Cases
**Issue**: No mechanism for runtime errors or edge cases.

- What happens if a clock dies?
- What happens if a module import fails?
- How to handle division by zero or NaN?
- No try/catch or error propagation mechanism

**Recommendation**: Add error handling:
```idl
voice = instr(0s, 5s) catch error => handle_death(voice)
// OR
voice = instr(0s, 5s) | error => handle_error
```

**My Answer**: 
- A clock won't die, unless explicitely killed 
- If module import fails, the interpreter will handle it by ignoring everything related and displaying error message (since it is designed for realtime use, we don't want any exception that could break the runtime)
- Divisions by 0 and NaN will be handled on the interpreter side too (maybe with warnings, and refusing to process the problematic nodes)
- I don't know how a try/catch mechanism could be implemented in an elegant way (not C++/Java style, more like something painless). Feel free to suggest things.  

### 2.2 No Scoping Rules Defined
**Issue**: Global namespace pollution is a real concern.

- How do namespace collisions between modules happen?
- Can you shadow variables?
- What's the lifetime of variables?
- How do function parameters shadow outer scope?

**Recommendation**: Explicit documentation of scoping rules, possibly:
```idl
scope private {
    internal_var = 10
}
external_func = some_value  // This is exported
```

**My Answer**: 
- Namespace collision is when two functions share same name ? Same for libraries. If module or library is not stored in a namespace, then the last loaded function will win (with warning of overload). 
- I don't think there are "variables", only functions. That's how I see it. I think we can shadow functions, but it will be more of an override. 
- Lifetime is difficult. There are some suggestions in basic.idl. 
- "How do function parameters shadow outer scope?" : I don't know what this means, can you elaborate ? 

### 2.3 No Type System or Type Annotations
**Issue**: "Everything is floating-point" can lead to subtle bugs.

- How do you differentiate between a frequency (Hz) and a time duration (ms)?
- What's the return type of `beats(clock)`?
- How do you know if a value is a trigger, a number, or a flow?

**Recommendation**: Optional type annotations:
```idl
sine_oscillator(frequency: Hz, dt: Time = 10ms): Signal
    = sin(phase * 2 * pi)
~> { ... }
```

**My Answer**: 
- There are a few primitive types : time, trigger, numbers (float64), functions, flows (specific type of functions). Time will always be internally converted to milliseconds : so no problem multiplying hz with milliseconds for example. Do you see a problem with this ?  
- beats(clock) returns a metronome, performing a trigger on each beat of a specific clock 
- "How do you know if a value is a trigger, a number, or a flow?" : I don't know yet, any suggestion ? 

### 2.4 No Module Lifecycle Management
**Issue**: Creating/destroying instances is unclear.

- How do you stop a running voice?
- What's the lifecycle of a running oscillator?
- How do you garbage collect finished instances?
- No explicit "kill" or resource cleanup mechanism

**Recommendation**: Explicit lifecycle operators:
```idl
voice = instr(0s, 5s)
voice.kill()  // Explicit termination
// OR
voice = instr(0s, duration) until(condition)
```

**My Answer**: 
- Modules will provide a set of functions, allowing to call specific functions of the API (Csound API for example), so I'll probably stop a voice by calling `kill` defined in Csound module. 
- Running oscillator in idyl runs forever, until it is asked to terminate. There might be something like `idl(5s, {voice.stop} )`
- Garbage collector will be an important question, I have no idea yet 
- The kill mechanism is described only as a set of ideas. Nothing really decided yet. Any suggestion, based on my propositions in basic.idl ? 

### 2.5 No State Persistence Between Calls
**Issue**: How do stateful processes maintain state across function calls?

- Can a function remember previous values?
- How do you build filters or integrators?
- Is there a `@` operator for past values (like in Faust)?

**Recommendation**: Add state/memory operator (inspired by Faust):
```idl
integrator(input, dt=10ms) = output
~> {
    [prev = 0]
    output = prev + input * dt
    prev! = output  // Save state for next iteration
}
// OR shorthand
output = input + @output  // Previous value of output
```

**My Answer**: 
- An @ operator would be great for delays, memory (filters, integrators, delays etc)

### 2.6 No Control Flow (Loops, Conditionals at Definition Level)
**Issue**: How do you express repetitive patterns without flows?

- No `if/else` for conditional function definitions
- No loops for building sequences
- Flow syntax is verbose for simple patterns

**Recommendation**:
```idl
// Conditional definition
tempo_func(bpm) = if(bpm > 120) then fast_engine else slow_engine

// List comprehension for flows
flow beats = [for i in 0..8 => trigger every i*beat]
```

**My Answer**: 
- if control flow is defined with the ternary operator, which operates like Faust select2, except it can take any number of inputs (selectN). Do you see a missing feature in this ? Or something wrong ? 
- There is already something like a for loop in the flows constructors, do you see it ? 

### 2.7 No Time-Domain Synchronization Primitives
**Issue**: No explicit way to synchronize across different time bases.

- How do two functions with different `dt` values stay in sync?
- What about jitter or clock drift?
- How to express phase-locking?

**Recommendation**:
```idl
// Explicit sync points
osc1(freq1, dt=10ms) sync_with osc2(freq2, dt=20ms) at phase=0
```

**My Answer**: 
- Functions with different dt won't probably stay in sync. I don't see what it would offer here ? Can you elaborate ? Give a problem example ? 
- Jitter or clock drift : can you elaborate ? 
- Same for phase locking ? 

---

## 3. Questions from Comments & Answers

### Q1: "what to do when voice dies after 5s: how to know that it is 'dead'? How to treat this?"

**Answer**: 
This requires a **voice lifecycle callback system**. Recommendation:

```idl
voice = instr(0s, 5s) 
voice.on_end => {
    // Handle voice termination
    print("Voice ended")
}

// OR implicit via try/catch
voice = instr(0s, 5s) catch end => cleanup()
```

Also consider returning a `Voice` object with state queries:
```idl
playing = voice.is_active()  // returns 0 (false) or 1 (true)
duration_left = voice.remaining_time()
```

**My Answer**: 
- I like the catch solution. Looks elegant and expressive enough. It also partially answers some previous questions. 

### Q2: "Syntax Problem: how to use receiver (the function already has its statement)?"

**Answer**:
The issue is that `receiver` is an object/handle with methods, not a pure function. The syntax should reflect this:

```idl
// Better: treat as event stream
receiver.on_message => (address, value) {
    another_function(address, value)
}

// OR make receiver return a signal
receiver_signal = receiver()
another_function = receiver_signal ~> (address, value) {
    // process
}
```

The core issue is **you can't layer logic onto external event sources** with the current syntax. Need explicit event binding.

**My Answer**: Then we need to build this event logic. Something that could interoperate nicely with the current syntax. 
Can you suggest something ? 

### Q3: "how to determine if flow is run through triggers, integers or float?"

**Answer**:
This requires **runtime type information** in the engine. Recommendations:

```idl
// Option 1: Type hints in flow definition
flow basic_flow() : Flow<Trigger, Note, Duration> = {
    rhythm: Trigger = | ! . . ! |
    note: Note = | 50 51 52 55 |
    duration: Duration = | 100ms 50ms |
}

// Option 2: Automatic type inference with runtime checks
run_flow = basic_flow(phasor(1hz))
// Engine inspects: phasor returns [0..1] float
// Applies fractional indexing automatically

// Option 3: Explicit index method selection
run_flow = basic_flow.trigger_index(metronome(1s))
run_flow = basic_flow.int_index(counter())
run_flow = basic_flow.float_index(phasor(1hz))
```

I'd recommend **Option 3** (explicit) combined with **Option 2** (inference) for common cases.

**My Answer**: Agreed for option 3. Just reformat it this way : 
- run_flow = basic_flow( metronome(1s) ) // trigger is a specific primitive first class type, no need to specify (engine knows) 
- run_flow = basic_flow(int(counter())) // int function returns a specific object to engine, with integer part of number + type specification 
- run_flow = basic_flow(phasor(1hz)) // no need to specify, it is a number without integer type specification 
What do you think about this ? 

---

## 4. Language Goals Alignment & Improvements

### Goal: "Most Expressive (Less Characters, More Meaning)"

**Current Status**: ✅ Good, but:
- ❌ Lambda syntax is verbose compared to intent
- ❌ Initialization blocks need clearer syntax
- ✅ Time literals (10ms, 5.1hz) are excellent

**Improvements**:
```idl
// Current (verbose)
sine_oscillator(frequency, dt=10ms) = sin(phase * 2 * pi)
~> {
    [phase = 0]
    phase = fmod(phase + incr, 1)
}

// Proposed (more expressive)
sine_osc(freq, dt=10ms) = sin(phase * 2π) ~> {
    phase: 0 => fmod(phase + 1/freq, 1)
}
```

**Recommendation**: Allow `π` (pi), `τ` (tau), single-letter lambdas, and shorthand notation.

**My Answer**: I agree with your suggestion. Though, I try to avoid anything that looks like most mainstream languages (no JS like arrow). Any other suggestion (to replace =>) ? Something elegant 
About pi and tau, these are not easy to write, so hard for live coding. Any other suggestion ? Or an idea to write it easily ? 


### Goal: "Functional"

**Current Status**: ✅ Good foundation
- ✅ Pure function definitions
- ✅ No mutable state syntax (explicit with `!`)
- ❌ Implicit state in lambdas (init block) breaks functional purity

**Improvements**:
Make side effects explicit. Use a proper **monadic** or **applicative** approach rather than implicit state initialization.

**My Answer**: I could use a detailed explaination for this. 

### Goal: "Designed for Audio (Livecoding, Music Composition, Scheduling)"

**Current Status**: ✅ Very good
- ✅ Excellent time primitives
- ✅ Beat-based timing
- ✅ Module system for engines (Csound, OSC)
- ❌ No live-coding reload semantics (hot swapping)
- ❌ No hierarchical grouping/composing of voices

**Improvements**:
```idl
// Live-coding friendly: automatic reload on change
live_synth = watch("synth.idl")

// Group voices for collective control
voice_group = group {
    v1 = instr(0s, 5s)
    v2 = instr(0.5s, 5s)
}
voice_group.kill_all()
```

**My Answer**: Ok, I understand this. I allow you to propose something in this way. 
Try to keep it as functional as possible, and as far as possible from OOP. 

### Goal: "DSL for Accurate Timing"

**Current Status**: ✅ Excellent
- ✅ Multiple time bases (ms, Hz, beats)
- ✅ dt-based update semantics (edge computing)
- ✅ Direct module support for real-time engines

**Minor Improvements**:
- Document drift/jitter guarantees
- Explicit time quantization support

```idl
// Quantize to 16th notes
quantized_trig = quantize(trig, 16)
```


**My Answer**: I will document drift & jitter guarantees once the engine is built (we're not there yet). 
About quantization you're right, a primitive function could be great ! 
---

## 5. Process Blocks (Faust-like) Consideration

### Should Idyl Have Process Blocks?

**Short Answer**: **YES, but with modifications**

### Why Process Blocks Would Help

Faust's `process` block elegantly separates:
- **Signal processor definition** (DSP graph building)
- **Main execution flow** (what actually runs)

Current Idyl issues this would solve:
1. **Ambiguous entry points**: What runs when you load a file?
2. **Voice lifecycle unclear**: Is everything running in parallel or sequentially?
3. **Global vs. local execution context**: Hard to tell what executes immediately vs. on demand

### Proposed Syntax

```idyl
// Define reusable components
voice_factory(freq, duration, amp) = 
    sine_osc(freq, 10ms) * amp_env(duration)

// Main process block: explicitly defines execution flow
process: {
    // Background metronome
    metro = metronome(100ms)
    
    // Triggered voice generation
    on metro => voice_factory(440hz + rnd(-50hz, 50hz), 500ms, 0.5)
    
    // Persistent signal processing
    output = all_voices | reverb(1s)
}

// Alternative: explicit lifecycle
process: (input_from_module) {
    // init: runs once
    init: { reverb_state = 0 }
    
    // on_trigger: runs per trigger
    on_trigger: (trig) => synth(freq, 500ms)
    
    // on_frame: runs every dt (audio-loop)
    on_frame: (dt) => {
        reverb_state! = decay(reverb_state, dt)
        output = process_frame(input_from_module, reverb_state)
    }
    
    // on_end: cleanup
    on_end: { cleanup_resources() }
}
```

**My Answer**
I like your Main process block suggestion. 
May I suggest something more ? I think it would be great to provide a way to have several of them. Not running at the same time, but describing different flows for composition context (where you want to create different sequences that will play one after the other). Offering the possibility to sequence it automatically (with specified time) or externally (external cue). Can you think of something like this ?

### Benefits

| Aspect | Benefit |
|--------|---------|
| **Clarity** | Explicit entry point and execution model |
| **Safety** | Clear lifecycle prevents resource leaks |
| **Scalability** | Easier to manage many voices as instances |
| **Debugging** | Clear separation makes tracing easier |
| **Composition** | Can nest processes within processes |

### Comparison with Current Approach

**Current** (implicit):
```idl
// When does this run? Always? On demand?
running_oscillator = instr.freq(sine_oscillator(1hz, dt=10))
```

**With Process** (explicit):
```idl
process: {
    running_oscillator = instr.freq(sine_oscillator(1hz, dt=10))
    // ^ Clearly runs in main process context
}
```

---

## 6. Methodology & Design Mistakes

### 6.1 Over-Reliance on Implicit Semantics ❌

**Problem**: Too many implicit assumptions about when code runs.

- Does loading a file execute all top-level definitions?
- Are temporal functions evaluated eagerly or lazily?
- When does a `~>` lambda block start executing?

**Fix**: Explicit execution model in documentation and syntax:
```idl
// Lazy - only computes when called
def my_func = sine_osc(440hz)

// Eager - runs immediately
exec: my_func()

// Scheduled - runs at specific time
at 5s => my_func()
```

**My Answer**: 
- Loading a file evaluates (compiles) all functions, creating the models that will be used for runtime. This is how I see it but I might be wrong on something ? 
- eagerly or lazily ? What do you mean ? Suggestion ? 
- The `~>` lambda block starts when function is instanciated, immediatly after initialization. 


### 6.2 Conflating Time Bases Without Explicit Conversion ⚠️

**Current Approach**:
> "Internally, interpreter will treat time as floating point milliseconds"

**Problem**: 
- Users might assume `1hz` and `1000ms` are compatible (they're not: 1 Hz ≠ 1000 ms)
- No explicit conversion forces mental model switch
- Potential for subtle timing bugs

**Fix**: Type-safe time representation:
```idl
// These have different types:
freq: Hz = 5.1hz
duration: Time = 5.1s
period: Time = 1 / freq  // Type error without explicit conversion

// Explicit conversion when needed:
delay_samples = hz_to_period(freq) * sample_rate
```

**My Answer**: I'd like to keep it automatic. Do you see a solution to improve in this way ? 

### 6.3 Ambiguous Scope for Module Methods ❌

**Current Design**:
```idl
instr = my_orchestra.get("my_instr")
// Is instr now a value, a function, or an object?
// What methods does it have? How are they called?

voice = instr(0s, 5s)
// Is this calling instr as a function or invoking a method?
```

**Fix**: Clear object/method syntax:
```idl
instr = my_orchestra.get("my_instr")
voice = instr.invoke(0s, 5s)  // Explicit method call
// OR
voice = instr::(0s, 5s)  // Special calling convention for instruments
```

**My Answer**: My idea was to avoid the `.` OOP syntax as much as possible. Though, maybe its a mistake. 
You're free to go in the way of your suggestion. 

### 6.4 Flow Syntax Too Complex for Simple Cases ⚠️

**Current**:
```idl
flow basic_flow() = {
    rhythm = | ! . . ! |
    note = | 50 51 52 55 45 63 |
}
```

**Problem**: 
- Pipe delimiters `|...|` are hard to view source
- Repetition syntax `|2|` and `|2||` is confusing
- No obvious way to generate flows programmatically

**Fix**: Simpler syntax for common cases + generator support:
```idl
// Simple: use array + symbols
basic_flow = {
    rhythm: [!, ., ., !],
    note: [50, 51, 52, 55, 45, 63]
}

// With repeats: explicit repetition
basic_flow = {
    rhythm: [!, .] * 2 ++ [!, !]
    // OR
    rhythm: repeat(2, [!, .]) ++ [!, !]
}

// Generating:
sinetable = {
    sin: [(i for 0..256): sin(i/256 * 2π)]
}
```
**My Answer**: 
- Ok for using square brackets for delimiters 
- But I want to keep the |2| for repetition. 
- What could do to replace the |2|| ? It needs to explicit the fact that next repeat resstarts from here 
- What suggestion to generate these programmatically ? 

### 6.5 Operator Overloading Creates Ambiguity ❌

**Problem**: Using `!` for both triggers AND variable emission is confusing.

```idl
metronome = !            // What is this value?
phase! = new_phase       // What does ! do here?
```

**Fix**: Use distinct operators with clear semantics:
```idl
metronome = trigger()    // ~ or specific trigger primitive
phase[emit] = new_phase  // Explicit emission annotation
```

### 6.6 Missing Composability Patterns ⚠️

**Problem**: Hard to compose small functions into larger ones without re-defining.

**Current**: 
- Module system exists but not for Idyl libraries clearly
- No functor/higher-order function support shown
- No explicit composition operators

**Fix**: Add composition primitives:
```idl
// Function composition
compose = (f, g) => x => f(g(x))

// Pipe operator
pipe = (x, f1, f2, f3) => f3(f2(f1(x)))
result = pipe(input, normalize, filter, amp)

// Partial application / currying
adder(x) = (y) => x + y
add_ten = adder(10)
```

**My Answer**: 
You are free to suggest a system for composability. Though, it shouldn't be audio graph composition : this part will be for another software. In Idyl, it needs to be abstracted. 

### 6.7 Temporal Functions Lack Determinism Guarantees ⚠️

**Problem**: Same `dt` value might give different results on loaded system.

- No specification of timing precision guarantees
- No jitter tolerance defined
- Unclear if `dt=10ms` means "exactly 10ms" or "nominally 10ms"

**Fix**: Document and provide timing options:
```idl
// Soft real-time (tolerates drift)
sine_osc(freq, dt=10ms, realtime=soft)

// Hard real-time (fails if deadline missed)  
sine_osc(freq, dt=10ms, realtime=hard)

// Adaptive (adjusts dt to system load)
sine_osc(freq, dt=10ms, realtime=adaptive)
```

**My Answer**: 
Maybe something like this could be great, with default to soft realtime. Default could be changed from CLI. 
Also, realtime would then be optional parameter (only set if want to specify). 

---

## 7. Summary of Recommendations

### High Priority
1. **Clarify lambda/temporal function syntax** - Current `~>` notation is ambiguous
2. **Define scoping rules** - Global namespace management unclear
3. **Add explicit error handling** - No way to handle failures
4. **Document execution model** - When do things run? Sequential or parallel?
5. **Add process blocks** - Much clearer voice/instance lifecycle

### Medium Priority
1. **Type system or hints** - Optional but valuable for safety
2. **Simplify flow syntax** - Current pipe notation is unwieldy
3. **Memory/state operators** - Add `@` past-value operator
4. **Voice group management** - Multiple voice control
5. **Composition operators** - pipe, compose, partial application

### Low Priority (Nice to Have)
1. **Unicode support** - Allow π, τ in code
2. **Live-reload semantics** - Hot-swapping for livecoding
3. **Timing guarantees** - Jitter/drift documentation
4. **Quantization primitives** - Align to musical grid
5. **Debugging support** - Breakpoints, inspection

---

## Conclusion

Idyl has an excellent foundation for a temporal audio DSL. The core ideas around time primitives, functional composition, and module system are sound. The main issues are:

1. **Syntactic inconsistencies** that create confusion (!, ~>, <, [], etc.)
2. **Implicit execution semantics** that need explicit documentation
3. **Missing lifecycle management** for voices/processes
4. **Underdeveloped composition patterns** for code reuse

With the addition of **explicit process blocks** and **clearer temporal function syntax**, Idyl could become a powerful live-coding and composition tool comparable to or exceeding Faust's expressiveness while being specifically tailored for audio scheduling and real-time control.

The Faust-like process block approach **strongly recommended** as it solves multiple design issues simultaneously and provides a clear mental model for execution flow.
