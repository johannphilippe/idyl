# 9 — Process blocks

[← Back to index](idyl_documentation.md) · [Previous: Clock & tempo](ch08_clock_tempo.md)

---

Process blocks are the **entry points** of an Idƴl program. They are top-level blocks that contain temporal functions, variable bindings, and orchestration logic. The evaluator collects all process blocks and runs them.

---

## Basic process block

```idyl
process: {
    osc = osc_out("127.0.0.1", 9000)
    freq(dt=100ms) = v |> {
        init: { v = 440 }
        v = v + 1
    }
    osc_send(osc, "/synth/freq", freq)
}
```

A single anonymous process block. This is the simplest program structure.

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
idyl song.idyl --listen          # default port 7771
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
idyl song.idyl --listen 7771
# Output:
# idyl: listening on port 7771
# Stored process blocks: drums, bass, lead
```

Terminal 2 (using `oscsend` or similar):

```bash
oscsend localhost 7771 /idyl/process/start s "drums"
oscsend localhost 7771 /idyl/process/start s "bass"
oscsend localhost 7771 /idyl/process/stop  s "drums"
oscsend localhost 7771 /idyl/process/list
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

---

## Multiple process blocks

A program can have any number of process blocks. They all share the global scope but run independently:

```idyl
process clock_display: {
    show(dt=1000ms) = t |> {
        init: { t = 0 }
        t = t + 1
        print("tick:", t)
    }
}

process audio: {
    osc = osc_out("127.0.0.1", 9000)
    freq(dt=10ms) = v |> {
        init: { v = 440 }
        v = v + sin(now() * 0.001) * 100
    }
    osc_send(osc, "/freq", freq)
}
```

---

## `start` and `stop` — process control from within a process

A running process block can start or stop another named process block using the `start` and `stop` keywords.

### `start name`

Starts the named process block. The process must have been stored (either loaded in `--listen` mode, or defined as a named block in the same file).

```idyl
process launcher: {
    timer(dt=500ms) = t |> {
        init: { t = 0 }
        t = t + 1
    }
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
process watchdog: {
    guard(dt=1000ms) = t |> {
        init: { t = 0 }
        t = t + 1
        emit timeout = _; ! ? (t >= 10)
    }
    guard catch timeout: {
        stop audio_loop     // stop "audio_loop" after 10 seconds
    }
}
```

### `stop` (no name)

Used without a name, `stop` stops the **current** process block:

```idyl
process oneshot: {
    counter(dt=100ms) = n |> {
        init: { n = 0 }
        n = n + 1
        emit done = _; ! ? (n >= 5)
    }
    counter catch done: {
        print("done, stopping self")
        stop        // stops "oneshot"
    }
}
```

### Notes

- `start` and `stop` take effect immediately — the next scheduler tick of the target process will either fire (start) or not fire (stop).
- Stopping a process that is not running is a no-op.
- Starting a process that is already running starts a second independent instance.
- These keywords are only valid inside process block bodies.

---

[Next: Modules & libraries →](ch10_modules_libraries.md)
