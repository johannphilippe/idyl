# 9 — Process blocks

[← Back to index](idyl_documentation.md) · [Previous: Clock & tempo](ch08_clock_tempo.md)

---

Process blocks are the **entry points** of an Idƴl program. They are top-level blocks that contain temporal functions, variable bindings, and orchestration logic. The evaluator collects all process blocks and runs them.

---

## Basic process block

```idyl
freq(dt=100ms) = v |> {
    init: { v = 440 }
    v = v + 1
}

process: {
    osc = osc_out("127.0.0.1", 9000)
    f = freq()
    osc_send(osc, "/synth/freq", f)
}
```

Function definitions must live at **global scope** — process blocks can only contain bindings, bare calls, and control statements. This is the simplest program structure.

---

## Named process blocks

Giving a process block a name allows selective execution:

```idyl
process drums: {
    // drum pattern logic
}

process bass: {
    // bass line logic
}

process lead: {
    // lead melody logic
}
```

Run only one:

```bash
idyl song.idyl --process drums
# or
idyl song.idyl -p drums
```

When `--process` is not specified, **all** process blocks run.

---

## Duration

A process block can have a finite duration:

```idyl
process intro, dur=8b: {
    // runs for 8 beats, then stops automatically
}

process loop: {
    // runs indefinitely
}
```

The `dur=` parameter accepts any time expression — literal or computed:

```idyl
process short, dur=2000ms: { ... }
process timed, dur=4b:     { ... }
```

When the elapsed time exceeds the duration, the evaluator automatically unsubscribes the process block from the scheduler.

---

## Listen mode

The `--listen` flag starts the program without running any process blocks. Instead, it waits for **OSC commands** to start and stop them on demand.

```bash
idyl song.idyl --listen          # default port 9000
idyl song.idyl --listen 9090     # custom port
idyl song.idyl -l                # short form
idyl song.idyl -l 9090
```

### OSC commands

| OSC address | Arguments | Effect |
|-------------|-----------|--------|
| `/idyl/process/start` | `name` (string) | Start the named process block |
| `/idyl/process/stop` | `name` (string) | Stop the named process block |
| `/idyl/process/list` | *(none)* | Print all stored process block names |

### Example workflow

Terminal 1:

```bash
idyl song.idyl --listen 9000
# Output:
# idyl: listening on port 9000
# Stored process blocks: drums, bass, lead
```

Terminal 2 (using `oscsend` or similar):

```bash
oscsend localhost 9000 /idyl/process/start s "drums"
oscsend localhost 9000 /idyl/process/start s "bass"
oscsend localhost 9000 /idyl/process/stop  s "drums"
oscsend localhost 9000 /idyl/process/list
```

### Combining `--listen` and `--process`

You can pre-start specific process blocks while in listen mode:

```bash
idyl song.idyl --listen --process drums
```

This starts `drums` immediately and waits for OSC commands to start/stop others.

---

## Process block semantics

1. **All process blocks are collected** before any evaluation starts
2. **Temporal functions** inside a process block are instantiated and subscribed to the scheduler
3. **Non-temporal statements** (e.g., `osc_out(...)`) execute once at process start
4. **Duration** is checked each tick — when elapsed time exceeds `dur=`, the block stops
5. In listen mode, blocks are **stored but not started** until an OSC command requests them

Process blocks accept both assignments (`x = expr`) and **bare expression calls** (function calls without assignment). Bare calls run for their side effects:

```idyl
process: {
    out = osc_out("127.0.0.1", 9000)
    osc_send(out, "/gate", 1)          // bare call — valid
    print("gate sent")                 // bare call — valid
}
```

> **Note**: Bare expression calls are **only valid inside process blocks and lambda blocks**. Global scope is declaration-only — attempting a bare call at the top level of a file is a parse error.

---

## Multiple process blocks

A program can have any number of process blocks. They all share the global scope but run independently:

```idyl
show(dt=1000ms) = t |> {
    init: { t = 0 }
    t = t + 1
}

freq_osc(dt=10ms) = v |> {
    init: { v = 440 }
    v = v + sin(now() * 0.001) * 100
}

process clock_display: {
    t = show()
    print("tick:", t)
}

process audio: {
    osc = osc_out("127.0.0.1", 9000)
    f = freq_osc()
    osc_send(osc, "/freq", f)
}
```

---

## `start` and `stop` — process control from within a process

A running process block can start or stop another named process block using the `start` and `stop` keywords.

### `start name`

Starts the named process block. The process must have been stored (either loaded in `--listen` mode, or defined as a named block in the same file).

```idyl
countdown(dt=500ms) = t |> {
    init: { t = 0  emit end = _ }
    t = t + 1
    emit end = _; ! ? (t >= 4)
}

process launcher: {
    timer = countdown()
    timer catch end: {
        start synth     // start "synth" when timer ends
    }
}

process synth: {
    osc = lfo(440hz, 0.8, dt=10ms)
    print("synth freq:", osc)
}
```

### `stop name`

Stops the named process block, unsubscribing all its temporal instances from the scheduler.

```idyl
guard(dt=1000ms) = t |> {
    init: { t = 0  emit timeout = _ }
    t = t + 1
    emit timeout = _; ! ? (t >= 10)
}

process watchdog: {
    g = guard()
    g catch timeout: {
        stop audio_loop     // stop "audio_loop" after 10 seconds
    }
}
```

### `stop` (no name)

Used without a name, `stop` stops **all** currently running process blocks:

```idyl
ticking(dt=100ms) = n |> {
    init: { n = 0  emit done = _ }
    n = n + 1
    emit done = _; ! ? (n >= 5)
}

process oneshot: {
    counter = ticking()
    counter catch done: {
        print("done, stopping everything")
        stop        // stops all running processes
    }
}
```

### Notes

- `start` and `stop` take effect immediately — the next scheduler tick of the target process will either fire (start) or not fire (stop).
- Stopping a process that is not running is a no-op.
- Starting a process that is already running starts a second independent instance.
- These keywords are only valid inside process block bodies.

---

## `on` blocks — trigger reactions

An `on` block fires its body every time a trigger expression is **live** (`!`). On rest ticks, the body is skipped entirely.

```idyl
process: {
    m = metro(dt=100ms)
    on m: {
        print("fired")
    }
}
```

This is equivalent to checking `m` manually in a reaction, but reads as a clear intent: *"when m fires, do this"*.

### Single-statement form

The braces are optional for a single statement:

```idyl
process: {
    m = metro(dt=500ms)
    on m: print("tick")
}
```

### With rhythm gates

`on` blocks compose naturally with flow gates. Use a flow's trigger member as the guard:

```idyl
import("stdlib")

flow pattern = {
    rhythm  : [!, _, _, !, !, _]
    melody on rhythm : [60, 63, 65]
}

process: {
    m = metro(dt=200ms)
    p = pattern[m]
    on p.rhythm: {
        print("note:", p.melody)
    }
}
```

### Placement and redistribution

An `on` block is a **reaction** — it is placed in the scheduler segment that drives its trigger expression. If the expression references multiple temporal sources, the block is placed in all of them (the same redistribution logic that governs other reactions applies).

### Difference from `catch`

| | `on expr: { }` | `expr catch event: { }` |
|---|---|---|
| Fires when | expression is a live trigger | instance emits a named event |
| Guard | value type (trigger/rest) | event name from `emit` |
| Repeats | every trigger tick | **once** — deactivated after first fire |

---

[Next: Modules & libraries →](ch10_modules_libraries.md)
