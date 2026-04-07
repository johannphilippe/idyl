# ⟡ Idƴl

A language for moments, intervals, and motion.

Idƴl is a functional, time-aware language designed for scheduling, composition, and reactive behavior. Time is not an afterthought bolted onto a general-purpose runtime — it is the material the language is made of. Functions describe shape, temporal functions describe change, and process blocks set them in motion.

The engine underneath is a high-accuracy system-clock scheduler that drives temporal functions at precise intervals. Idƴl describes time. The scheduler executes it.

> Somewhere between a programming language and a score,  
> Idƴl is a quiet place to think about signals and durations,  
> about things that rise and decay, and the spaces between them.

---

## Philosophy

- **Stateless by default.** Definitions are immutable. State exists only where you ask for it, inside temporal lambda blocks, and every mutation is explicit.
- **Time is first-class.** Time literals (`100ms`, `2s`, `440hz`, `120bpm`, `4b`) are primitive values, not strings or numbers pretending. Temporal functions carry their own clock.
- **Functional at the core.** No `if/else`, no `while`, no assignment mutation outside lambda blocks. Control flow is ternary selection. Iteration is generator expressions. Composition replaces inheritance.
- **The show must go on.** Warnings degrade gracefully — a missing value produces a fallback, not a crash. Designed for live contexts where silence is worse than imperfection.

---

## Features

| | |
|---|---|
| **Temporal functions** | Clock-driven (`dt=10ms`), trigger-driven (`spike!`), or hybrid. State via `init` + `\|>` lambda blocks. |
| **Flows** | Ordered sequences with named members, generator expressions, automatic wrapping. The data structure of the language. |
| **Emit system** | Side-channel output from temporal functions. Read emitted values with the `::` accessor. |
| **Catch blocks** | React to events emitted by temporal instances — `timer catch finished: { ... }` |
| **Clock hierarchy** | Create clocks, bind children to parents, change tempo with automatic proportional propagation. |
| **First-class functions** | Functions are values. Store them, pass them, select between them with ternary. |
| **Modules** | Built-in OSC support. External modules via `module()`. Libraries via `lib()` with namespace support. |
| **Process blocks** | The only executable entry points. Named, with optional duration. Start and stop them via OSC in `--listen` mode. |
| **7-pass semantic analysis** | Symbol collection, name resolution, arity checking, type inference, temporal validation, flow validation, scope validation — all before a single line runs. |

---

## Quick start

```bash
# Build (requires CMake ≥ 3.20, C++20, Bison ≥ 3.8, Flex)
mkdir build && cd build
cmake ..
make -j$(nproc)

# Run a file
./idyl path/to/file.idl

# Run a specific process block
./idyl file.idl --process clock_demo

# Listen mode — control process blocks via OSC
./idyl file.idl --listen 9000
```

---

## Language overview

### Constants and time literals

```idyl
silence = 0
concert_pitch = 440hz       // frequency → period in ms
short_delay = 100ms
quarter_note = 120bpm        // BPM → period (500ms)
beat = 4.0b                  // beats, scaled by current tempo
```

All time values normalize to milliseconds internally. Arithmetic between them works naturally: `2s + 100ms` is `2100ms`.

### Functions

Everything is a function. Constants are zero-parameter functions. Definitions are immutable.

```idyl
square(x) = x * x

blend(a, b, mix) = a * (1 - mix) + b * mix

midi_to_freq(note) = 440 * (2.0 ^ ((note - 69) / 12.0))
```

No `return` keyword — the body *is* the result.

### Conditional expressions

No `if/else`. Selection uses the ternary operator with semicolons separating options:

```idyl
// false_val; true_val ? condition
safe_divide(a, b) = 0; (a / b) ? (b != 0)

// Multi-way selection (wraps around)
note(degree) = 261; 293; 329; 349; 391 ? (degree % 5)
```

### Flows

Ordered sequences — the data structure of Idƴl. Single or multi-member, with generator expressions.

```idyl
flow notes = [60 62 64 67 69]

flow drum_pattern = {
    kick:  [! _ _ _]
    snare: [_ _ ! _]
    hat:   [! ! ! !]
}

chromatic(root) = [semitone = 0..11 : root * (2.0 ^ (semitone / 12.0))]
```

### Temporal functions

Functions that carry state across time. The `|>` operator introduces a lambda block with optional `init` and update code. The `dt` parameter sets the tick interval.

```idyl
// LFO: outputs a sine wave, updates every 10ms
lfo(freq, amplitude, dt=10ms) = modulation |> {
    init: { phase = 0  modulation = 0 }
    modulation = amplitude * sin(phase * 2 * pi)
    phase = fmod(phase + 1 / (freq * (dt / 1000)), 1)
}

// Counter: increments on trigger, no clock
trigger_counter(spike!) = count |> {
    init: { count = 0 }
    count = count + 1
}
```

### Emit and the `::` accessor

Temporal functions can emit named values as a side channel. The `::` operator reads them from an instance.

```idyl
step_counter(dt=100ms) = count |> {
    init: { count = 0  emit step = 0 }
    emit step = 3
    count = count + step
}

process: {
    c = step_counter()
    print("count:", c, "step:", c::step)
}
```

### Catch blocks

React to events emitted by temporal instances:

```idyl
countdown(dt=100ms) = remaining |> {
    init: { remaining = 10  emit finished = _ }
    remaining = remaining - 1
    emit finished = _; ! ? (remaining == 0)
}

process: {
    timer = countdown()
    print("remaining:", timer)
    timer catch finished: {
        print("done.")
    }
}
```

### Clock hierarchy

A main clock is created at startup (120 BPM by default). Child clocks maintain a ratio to their parent — when the parent tempo changes, children scale proportionally.

```idyl
process: {
    c1 = clock(60bpm)
    c2 = clock(90bpm)

    // Change main tempo → children follow
    tempo(240bpm)
    print("c1:", bpm(c1), "c2:", bpm(c2))
    // c1 = 120 (ratio 0.5), c2 = 180 (ratio 0.75)

    // Hierarchical: c3 is child of c1
    c3 = clock(30bpm, parent=c1)

    // Free-running: unaffected by parent changes
    c_free = clock(100bpm, parent=0)
}
```

### Process blocks

The only code that actually runs. Everything outside a process block is a definition waiting to be called.

```idyl
// Named, with optional duration
process oscillator_demo, dur=1s: {
    osc = lfo(5hz, 1.0, dt=10ms)
    modulated = 440hz * (1 + osc * 0.1)
}

process flow_ops: {
    scale = chromatic(440)
    print("chromatic scale:", scale)
}
```

### OSC

Built-in OSC module for sending and receiving messages over UDP:

```idyl
process: {
    handle = osc_out("127.0.0.1", 9000)
    osc_send(handle, "/synth/freq", 440)
    osc_send(handle, "/synth/gate", 1)
    osc_close(handle)
}
```

### Libraries

Import `.idl` files with optional namespacing:

```idyl
lib("scales.idl")
lib("utils.idl", as="util")

process: {
    scale = util::major_scale(440)
}
```

---

## CLI

```
Usage: idyl [file.idl] [options]

Options:
  --trace              Enable parser/lexer debug tracing
  --process <name>     Run only the named process block
  -p <name>            Short form of --process
  --listen [port]      Listen mode: receive OSC commands on port (default: 9000)
  -l [port]            Short form of --listen
```

### Listen mode

`--listen` loads the file, stores all process blocks without executing them, and waits for OSC messages to start and stop them:

| OSC address | Argument | Effect |
|---|---|---|
| `/idyl/process/start` | process name (string) | Start the named process |
| `/idyl/process/stop` | process name (string) | Stop the named process |
| `/idyl/process/list` | — | Print available process names |

Combine with `--process` to pre-start a specific block:

```bash
idyl file.idl --listen 9000 -p counting
```

---

## Architecture

```
Source (.idl)
     │
     ▼
 Lexer (Flex) → Parser (Bison)
     │
     ▼
 AST (ast.hpp)
     │
     ▼
 Semantic Analysis (7 passes)
     │
     ▼
 Tree-walking Evaluator
     │
     ├── Temporal instances → Scheduler (system clock)
     ├── Module calls → OSC / external modules
     └── Process blocks → reactive execution
```

**Lexer/Parser**: Flex + Bison generate a typed AST with location tracking (line/column on every node).

**Semantic analysis**: Seven passes — symbol collection & import resolution, name resolution, arity & argument validation, soft type inference, temporal validation, flow validation, scope & structure validation. All diagnostics are reported before execution begins.

**Evaluator**: Tree-walking interpreter. Pure functions evaluate inline. Temporal functions are instantiated with isolated state and subscribed to the scheduler. Process blocks create reactive chains — a temporal binding followed by reactions that re-execute on every tick.

**Scheduler**: High-accuracy system-clock scheduler with drift-free rescheduling. Each subscription is a periodic callback. The scheduler runs its own thread; the main thread sleeps until all subscriptions complete (or indefinitely in `--listen` mode).

---

## Building

Requires:
- CMake ≥ 3.20
- C++20 compiler (GCC 11+, Clang 14+)
- Bison ≥ 3.8
- Flex

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install    # installs to /usr/local/bin
```

Optional CMake flags:
- `-DIDYL_MODULE_OSC=OFF` — disable the built-in OSC module

---

## Status

Pre-alpha. The core language, temporal engine, scheduler, module system, and semantic analysis are functional. The language is still young — syntax and semantics may shift.

What exists:
- Full parser (Bison/Flex) with location tracking
- 7-pass semantic analyzer
- Tree-walking evaluator with temporal function instantiation
- System-clock scheduler with drift-free ticking
- Built-in OSC module (send/receive)
- Clock hierarchy with proportional tempo propagation
- Library imports with deduplication and namespacing
- `--process` filter and `--listen` OSC control mode
- VS Code syntax highlighting extension

---

*Idƴl is a small language with a small ambition: to make time visible, and to let the things that change, change clearly.*
