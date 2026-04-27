# ⟡ Idƴl

A programming language for time description.

Idƴl is a functional, time-aware language designed for scheduling, composition, and reactive behavior. Time is not an afterthought bolted onto a general-purpose runtime — it is the material the language is made of. Functions describe shape, temporal functions describe change, and process blocks set them in motion.

The engine underneath is a high-accuracy system-clock scheduler that drives temporal functions at precise intervals. Idƴl describes time. The scheduler executes it.

---

## Author note 

I spent a long time thinking about Idyl as a functional temporal language. 
It took me a while to design it. 
Though, I would probably never have found time to implement it myself completely. 
So this project is a combined work of me and the AI assistants I could find helpful. 
The workflow was simple : I design, the AI implements most features, I check, I fix when needed. 
The use of AI reduced the time to find grammar inconsistencies by a large amount. 
Same for engine implementation.

## Philosophy

- **Stateless by default.** Definitions are immutable. State exists only where you ask for it, inside temporal lambda blocks, and every mutation is explicit.
- **Functional at the core.** No `if/else`, no `while`, no assignment mutation outside lambda blocks. Control flow is ternary selection. Iteration is generator expressions. Composition replaces inheritance.

## Features

| | |
|---|---|
| **Temporal functions** | Clock-driven (`dt=10ms`), trigger-driven (`spike!`), or hybrid. State via `init` + `\|>` lambda blocks. Self-terminate with `stop` inside the update body; the bound variable freezes at the final output and `catch v::end` fires. |
| **Delay operator** | `'(expr)` — one-sample delay. `'(expr, N)` — N-sample delay. Circular buffer, per-expression, independent across call sites. |
| **Deferred blocks** | `@(500ms): { ... }` — schedules a block to run once after a delay. Time expression can be any value. |
| **Flows** | Ordered sequences with named members, generator expressions, parametric flows, live temporal elements per-slot, dynamic rebuilding when arguments change. |
| **Emit system** | Side-channel output from temporal functions. Read emitted values with the `::` accessor. |
| **Catch blocks** | React to events emitted by temporal instances — `catch timer::finished: { ... }`. Built-in `::end` signal fires automatically when an instance self-terminates via `stop`. |
| **`on` blocks** | Trigger-gated reaction blocks — `on m: { ... }` fires only when `m` is a live trigger. Also gates flow members: `melody on rhythm: [60, 63, 65]` advances only on rhythm's trigger ticks. |
| **Clock hierarchy** | Create clocks, bind children to parents, change tempo with automatic proportional propagation. Clock handles are callable: `c(2b)` returns 2 beats at that clock's BPM. Query BPM with `tempo(handle)`. |
| **First-class functions** | Functions are values. Store them, pass them, select between them with ternary. |
| **Local functions & closures** | Define helper functions inside process blocks, init blocks, or update blocks. Local functions have block-limited visibility. Closures capture the enclosing temporal instance by reference — they always see the instance's current params and state at call time. |
| **Process control** | `start name` / `stop name` (or just `stop`) — start and stop named process blocks from within a running process. |
| **Modules** | Built-in OSC and Csound support. Native temporal module functions (`osc_recv`, `cs_chnget`) integrate with the reactive system. External modules via `module()`. Libraries via `import()` with namespace support. |
| **Process blocks** | The only executable entry points. Named, with optional duration. Start and stop them via OSC in `--listen` mode or from within other processes. |
| **7-pass semantic analysis** | Symbol collection, name resolution, arity checking, type inference, temporal validation, flow validation, scope validation — all before a single line runs. |

---

## Quick start

```bash
# Build (requires CMake ≥ 3.20, C++20, Bison ≥ 3.8, Flex)
mkdir build && cd build
cmake ..
make -j$(nproc)

# Run a file
./idyl path/to/file.idyl

# Run a specific process block
./idyl file.idyl --process clock_demo

# Listen mode — control process blocks via OSC
./idyl file.idyl --listen 9000
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

midi_to_freq(note) = 440 * pow(2.0, (note - 69) / 12.0)
```

No `return` keyword — the body *is* the result.

### Conditional expressions

No `if/else`. Selection uses the ternary operator — condition first, options after `?`:

```idyl
// condition ? false_val ; true_val
safe_divide(a, b) = (b != 0) ? 0; (a / b)

// Single-option: evaluates only when condition is truthy, otherwise _
(m) ? osc_send(handle, "/gate", 1)

// Multi-way selection by integer index
note(degree) = (degree % 5) ? 261; 293; 329; 349; 391
```

### Flows

Ordered sequences — the data structure of Idƴl. Single or multi-member, with generator expressions. Flow slots can hold **live temporal expressions** that update on every tick, and parametric flows can be called with temporal arguments for dynamic rebuilding.

```idyl
flow notes = [60, 62, 64, 67, 69]

flow drum_pattern = {
    kick:  [!, _, _, _]
    snare: [_, _, !, _]
    hat:   [!, !, !, !]
}

chromatic(root) = [semitone = 0..11 : root * pow(2.0, semitone / 12.0)]

// Live temporal elements — each slot is a running oscillator
flow oscs = [sine(1hz, dt=100ms), sine(3hz, dt=100ms)]

// Dynamic parametric flow — rebuilt automatically when argument changes
flow melody(transpose) = [60 + transpose, 62 + transpose, 67 + transpose]
```

### Temporal functions

Functions that carry state across time. The `|>` operator introduces a lambda block with optional `init` and update code. The `dt` parameter sets the tick interval.

```idyl
// LFO: outputs a sine wave, updates every 10ms
lfo(freq, amplitude, dt=10ms) = modulation |> {
    init: { phase = 0 }
    modulation = amplitude * sin(phase * 2 * pi)
    phase = fmod(phase + dt / freq, 1)
}

// Counter: increments on trigger, no clock
trigger_counter(spike!) = count |> {
    init: { count = 0 }
    count = count + 1
}
```

Both the `init` block and the update body of a lambda block accept bare expression calls (function calls without assignment). This is useful for side effects such as logging or calling external module functions on each tick:

```idyl
debug_counter(dt=100ms) = n |> {
    init: {
        n = 0
        print("counter initialized")   // bare call in init
    }
    print("tick:", n)                   // bare call on every tick
    n = n + 1
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
    emit finished = (remaining == 0) ? _; !
}

process: {
    timer = countdown()
    print("remaining:", timer)
    catch timer::finished: {
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
    print("c1:", tempo(c1), "c2:", tempo(c2))
    // c1 = 120 (ratio 0.5), c2 = 180 (ratio 0.75)

    // Hierarchical: c3 is child of c1
    c3 = clock(30bpm, parent=c1)

    // Free-running: unaffected by parent changes
    c_free = clock(100bpm, parent=0)

    // Clock handles are callable — returns beat duration at that clock's BPM
    m = metro(c1(2b))       // fires every 2 beats of c1
}
```

### Delay operator

The prime operator `'` returns the value of an expression from a previous tick — a sample-based circular delay:

```idyl
process: {
    a = counter(dt=100ms)
    b = '(a)        // one sample behind a
    c = '(a, 4)     // four samples behind a
    print(a, b, c)
}
```

### `on` blocks

`on expr: { }` is a trigger-gated reaction — the body runs only when the expression is a live trigger, and is skipped on rest ticks:

```idyl
import("stdlib")

flow rhythm = [!, _, _, !, !, _]

process: {
    m = metro(dt=200ms)
    r = rhythm[m]
    on r: {
        print("beat")
    }
}
```

The same keyword gates flow members. `melody on rhythm: [...]` means melody only advances its cursor when rhythm's value for that tick is `!`:

```idyl
flow pattern = {
    rhythm : [!, _, _, !, !, _]
    melody on rhythm : [60, 63, 65]
}
```

### Deferred execution

`@(time_expr): { }` schedules statements to run once after a delay:

```idyl
process: {
    osc_send(out, "/gate", 1)

    @(500ms): {
        osc_send(out, "/gate", 0)   // release after 500ms
    }
}
```

### Process control

Named processes can be started and stopped from within other processes:

```idyl
process launcher: {
    @(2s): {
        start synth       // start "synth" process after 2 seconds
        stop launcher     // stop this process
    }
}

process synth: {
    osc = lfo(440hz, 0.8, dt=10ms)
    print(osc)
}
```

`stop` without a name stops the current process.

### Process blocks

The only code that actually runs. Everything outside a process block is a definition waiting to be called — **global scope is declaration-only**: no bare function calls, no side effects, only function and constant definitions, flow definitions, and module imports.

Inside a process block, both assignments and bare expression calls (function calls without assignment) are valid:

```idyl
// Named, with optional duration
process oscillator_demo, dur=1s: {
    osc = lfo(5hz, 1.0, dt=10ms)
    modulated = 440hz * (1 + osc * 0.1)
}

process flow_ops: {
    scale = chromatic(440)
    print("chromatic scale:", scale)   // bare call — valid in process blocks
}
```

### OSC

Built-in OSC module for sending and receiving messages over UDP:

```idyl
module("osc")

process: {
    out = osc_out("127.0.0.1", 9000)
    osc_send(out, "/synth/freq", 440)
    osc_send(out, "/synth/gate", 1)
    osc_close(out)
}
```

Receive OSC with `osc_recv` — a native temporal function that integrates with the reactive system:

```idyl
module("osc")

process: {
    rx = osc_in(9000)
    msgs = osc_recv(rx, dt=10ms)
    catch msgs::received: {
        print("incoming:", msgs)
    }
}
```

### Csound

The Csound module connects idyl's scheduler to Csound instruments:

```idyl
module("csound")

process: {
    cs = cs_open("piano.csd")
    cs_note(cs, 1, 500ms, 440, 0.8)       // instr 1, 500ms, freq=440, amp=0.8
    cs_chnset(cs, "cutoff", 800)           // write a control channel
    fc = cs_chnget(cs, "feedback", dt=20ms) // poll a control channel (temporal)
}
```

### Local functions and closures

Functions can be defined inside process blocks, init blocks, and update blocks. They are visible only within the enclosing scope and do not pollute the global namespace.

```idyl
process: {
    // Helper visible only in this process
    semitones_to_ratio(s) = pow(2, s / 12)
    midi_to_hz(note) = 440 * semitones_to_ratio(note - 69)

    print(midi_to_hz(60))   // 261.626
}
```

Init-block and update-block functions can be returned as closures. The closure captures the owning temporal instance by reference — at call time it always sees the instance's current state:

```idyl
// Returns a closure that adds n to its argument
make_adder(n, dt=100ms) = fn |>
{
    init: {
        fn(x) = x + n
    }
}

process: {
    add5 = make_adder(5)
    print(add5(10))   // 15
    print(add5(20))   // 25
}

// Closure over mutable state
accumulator(dt=200ms) = fn |>
{
    init: { total = 0 }
    total = total + 1
    fn(x) = x + total   // reads current total each call
}
```

Update-block local functions are tick-local aliases — redefined each tick, not persisted between ticks:

```idyl
tracker(input, dt=10ms) = ratio |>
{
    init: { peak = 0 }
    peak = max(peak, abs(input))
    normalize(v) = v / max(peak, 0.001)   // tick-local alias
    ratio = normalize(input)
}
```

### Libraries

Import `.idyl` files with optional namespacing:

```idyl
import("scales.idyl")
util = import("utils.idyl")

process: {
    scale = util::major_scale(440)
}
```

---

## CLI

```
Usage: idyl [file.idyl] [options]

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
| `/idyl/eval` | source code (string) | Hot-reload: re-evaluate a function, flow, or process block while it is running |

`/idyl/eval` is the live-coding entry point. When the payload is a named process block that is currently running, its body is diffed against the live version — surviving temporal instances keep their state, changed reactions and `dt` values take effect on the very next scheduler tick.

Combine with `--process` to pre-start a specific block:

```bash
idyl file.idyl --listen 9000 -p counting
```

---

Source (.idyl)

```
Source (.idyl)
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
- `-DIDYL_MODULE_CSOUND=ON` — enable the Csound module (requires Csound development headers)

---

## Status

Pre-alpha. The core language, temporal engine, scheduler, module system, and semantic analysis are functional. The language is still young — syntax and semantics may shift.

What exists:
- Full parser (Bison/Flex) with location tracking
- 7-pass semantic analyzer
- Tree-walking evaluator with temporal function instantiation
- System-clock scheduler with drift-free ticking and live `dt` updates
- Sample-based delay operator `'(expr)` / `'(expr, N)` with per-expression circular buffers
- Deferred execution blocks `@(time): { }` — one-shot scheduler callbacks
- `start`/`stop` keywords for process-to-process control
- Dynamic parameter re-evaluation — temporal function parameters can themselves be temporal
- Local functions and closures — process-scope, init-scope, update-scope; by-reference capture over temporal instance state
- Flows with live temporal elements (per-slot running instances) and dynamic parametric flows (auto-rebuilt when arguments change)
- `on` blocks — trigger-gated reaction blocks (`on m: { ... }`) and flow member gates (`melody on rhythm: [...]`)
- Built-in OSC module: send, receive, `osc_recv` native temporal poller
- Built-in Csound module: `cs_open`, `cs_note`, `cs_chnset`, `cs_chnget` (native temporal)
- Native temporal module function system — C++ module functions that tick with state and emit values
- Clock hierarchy with proportional tempo propagation; clock handles callable as beat-duration functions (`c(2b)`); `tempo(handle)` for BPM queries
- Library imports with deduplication and namespacing
- `--process` filter and `--listen` OSC control mode
- Editor support: VS Code extension (tmLanguage grammar), Vim/Neovim syntax highlighting, and a Vim live-coding plugin (`t` eval, `s` start, `q` stop, `<C-e>` insert-mode eval) — install with `editors/vim/install.sh`
- Hot reload: `/idyl/eval` OSC command diffs a running process against the new source — surviving temporal segments keep state, reactions and `dt` changes apply on the next tick
- [Online documentation](https://johannphilippe.github.io/idyl/) (GitHub Pages)

---

*Idƴl is a small language with a small ambition: to make time visible, and to let the things that change, change clearly.*
