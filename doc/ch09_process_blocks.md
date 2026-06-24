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
| `/idyl/process/pause` | `name` (string) | Pause the named process block |
| `/idyl/process/resume` | `name` (string) | Resume a paused process block |
| `/idyl/process/list` | *(none)* | Print all stored process block names |
| `/idyl/eval` | source code (string) | Hot-reload: re-evaluate a definition or running process block |

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

## Hot reload

Sending source code to `/idyl/eval` re-evaluates it against the running program without stopping anything. This is the primary live-coding workflow.

### What can be hot-reloaded

| Payload | Behaviour |
|---------|-----------|
| Function definition | Replaces the definition in `function_defs_`. Existing temporal instances using the old definition continue with the old code until their next instantiation; reactions that call the function pick up the new body immediately. |
| Flow definition | Replaces the flow in `flow_defs_` and evicts its cache. The next access rebuilds from the new definition. |
| Named process block (running) | Diffs the new body against the live process — see below. |
| Named process block (not running) | Updates the stored AST. The next `start` will use the new version. |

### Process diff

When the payload is a named process block that is currently running, `diff_and_apply` compares the new body against the live segments:

- **Surviving segments** (same temporal function, same variable name): the instance is kept alive with all its state intact. The reaction list and `dt` are updated atomically under a mutex — the change is visible to the scheduler on the very next tick. No gap, no drift, no restart.
- **Changed `dt`**: if the new binding has a different `dt` value (e.g. `counter(dt=0.3s)` → `counter(dt=0.2s)`), the scheduler interval is updated in place.
- **Changed temporal function** (e.g. `counter` → `metro`): the old instance is stopped and a new one is started.
- **Removed bindings**: the instance is stopped and unsubscribed.
- **New bindings**: a fresh instance is created and subscribed.
- **Reactions** (`fl = melody(i)[cnt]`, `a = note(...)`, etc.): replaced with the new AST nodes. The redistribution pass runs again so reactions always fire on the correct segment.
- **`@` blocks**: not re-executed on hot reload — existing deferred timers continue running.
- **`catch` blocks**: replaced; the new handler takes effect on the next event.

### Vim live-coding keys

The Vim plugin (installed via `editors/vim/install.sh`) binds these keys for `.idyl` buffers:

| Key | Mode | Action |
|-----|------|--------|
| `t` | normal | Send the construct at the cursor to `/idyl/eval` (hot-reload) |
| `s` | normal | Send the process block's name to `/idyl/process/start` |
| `q` | normal | Send the process block's name to `/idyl/process/stop` |
| `<C-e>` | insert | Hot-reload without leaving insert mode |

The evaluated range is briefly highlighted. Block detection is brace-based — position the cursor anywhere inside (or on the first line of) the construct you want to send.

Configuration (in `vimrc` / `init.vim`):

```vim
let g:idyl_osc_host = '127.0.0.1'   " default
let g:idyl_osc_port = 9000           " default
```

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

## Process control keywords

A running process block can control other named process blocks using `start`, `stop`, `pause`, and `resume`.

### `start name`

Starts the named process block. The process must be stored (defined as a named block in the same file, or loaded in `--listen` mode).

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

Without a name, `stop` stops **all** currently running process blocks:

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

### `pause name` and `resume name`

`pause` suspends a process without destroying its state. The scheduler stops firing its callbacks, but all temporal instances keep their current values. `resume` restores it to full operation from where it left off.

```idyl
process controller: {
    m = metro(dt=1000ms)
    on m: {
        pause music     // freeze "music" every second
    }
}

process music: {
    tempo(120bpm)
    ph = phasor(4b)
    // ... playback logic
}
```

Via OSC in listen mode:

```bash
oscsend localhost 9000 /idyl/process/pause  s "music"
oscsend localhost 9000 /idyl/process/resume s "music"
```

### Summary table

| Keyword | Target | Effect |
|---------|--------|--------|
| `start name` | Named process | Start (or restart) |
| `stop name` | Named process | Stop, discard state |
| `stop` | All running | Stop everything |
| `pause name` | Named process | Suspend, keep state |
| `resume name` | Named process | Resume from suspended state |

**Notes**:
- `start`/`stop`/`pause`/`resume` take effect immediately on the next scheduler tick.
- Stopping a process that is not running is a no-op.
- Starting a process that is already running creates a second independent instance.
- These keywords are only valid inside process block bodies (and catch/reaction handlers).

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

| | `on expr: { }` | `catch instance::event: { }` |
|---|---|---|
| Fires when | expression is a live trigger | emitted signal is truthy (or `::end` on stop) |
| Guard | value type (trigger/rest) | event name from `emit`, or built-in `end` |
| Repeats | every trigger tick | **once** — deactivated after first fire |

Rule of thumb: `on` for recurring reactions, `catch` for a one-shot milestone or reacting to an instance ending. See [`catch` vs `on`](ch07_emit_catch.md#catch-vs-on--which-to-use) for the full guide.

---

[Next: Modules & libraries →](ch10_modules_libraries.md)
