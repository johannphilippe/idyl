# 10 — Modules & libraries

[← Back to index](idyl_documentation.md) · [Previous: Process blocks](ch09_process_blocks.md)

---

Idƴl supports importing external code through **libraries** and connecting to external systems through **modules**.

---

## Libraries — `lib()`

Import another `.idyl` file to reuse its definitions:

```idyl
lib("utils.idyl")
```

All top-level bindings from `utils.idyl` become available in the current scope.

### Namespaced imports

To avoid name collisions, assign the import to a name:

```idyl
u = lib("utils.idyl")

process: {
    result = u::my_function(42)
}
```

Everything from `utils.idyl` is then accessible via `u::name` notation. Functions,
flows, and constants all work the same way. The bare name `my_function` is **not**
added to the global scope — only `u::my_function` is.

#### Library-internal cross-calls

Functions inside a namespaced library can freely call each other without qualification.
For example, if `stdlib` defines both `sine_shape` and `lfo` (which calls `sine_shape`
internally), this works correctly even when loaded under a namespace:

```idyl
std = lib("stdlib")

process: {
    l = std::lfo(1hz)   // internally, lfo calls sine_shape — no namespace prefix needed
}
```

This works because each namespaced library function carries a **library-local scope**: a
private mapping from every original function name in that library to its qualified version.
This scope is pushed into the environment whenever the function body or temporal tick
executes, then immediately removed. As a result:

- Internal cross-calls (`sine_shape`, `lfo`, etc.) resolve correctly inside the library.
- Bare names never appear in the caller's global scope.
- Two libraries with overlapping internal names can coexist under different namespaces.

#### What namespace isolation means in practice

| What you write | What resolves |
|---|---|
| `lib("stdlib")` | `sine`, `lfo`, ... added to global scope |
| `std = lib("stdlib")` | only `std::sine`, `std::lfo`, ... in global scope |
| `std = lib("stdlib")` then `lfo(...)` | **error** — `lfo` is not in global scope |
| `std = lib("stdlib")` then `std::lfo(...)` | works correctly |

### Deduplication

If the same library is imported multiple times (directly or transitively), it is only loaded once:

```idyl
lib("core.idyl")
lib("helpers.idyl")    // if helpers.idyl also imports core.idyl, it's not loaded again
```

---

## Modules — `module()`

Modules connect Idƴl to external systems. Unlike libraries, modules are compiled
components (built-in or external `.so` files). They must be explicitly imported
before their functions are available.

```idyl
module("osc")
```

After this, all OSC functions are in scope. To avoid polluting the global
namespace, import into a name:

```idyl
osc = module("osc")

process: {
    h = osc::osc_out("127.0.0.1", 9000)
    osc::osc_send(h, "/freq", 440)
    osc::osc_close(h)
}
```

---

## Built-in OSC module

OSC (Open Sound Control) support is compiled in when `IDYL_MODULE_OSC=ON`
(the default). Import it with:

```idyl
module("osc")
```

### OSC output

```idyl
module("osc")

process: {
    out = osc_out("127.0.0.1", 9000)
    osc_send(out, "/synth/freq", 440)
    osc_send(out, "/synth/gate", 1)
    osc_close(out)
}
```

### OSC functions

| Function | Description |
|----------|-------------|
| `osc_out(host, port)` | Open a UDP output endpoint, return handle |
| `osc_in(port)` | Open a UDP input listener, return handle |
| `osc_send(handle, address, value...)` | Send one or more values to an OSC address |
| `osc_close(handle)` | Close the connection |
| `osc_stop()` | Stop all scheduled (dt-driven) sends |

### Receiving OSC — `osc_recv`

`osc_recv` is a **native temporal function** — it ticks on a clock and polls a UDP
input for incoming OSC messages. It integrates naturally with the reactive system:

```idyl
module("osc")

process: {
    rx = osc_in(9000)
    msgs = osc_recv(rx, dt=10ms)   // poll every 10ms
    print("received:", msgs)

    msgs catch received: {
        print("new message arrived!")
    }
}
```

| Parameter | Description |
|-----------|-------------|
| `handle` | A receiver handle from `osc_in()` |
| `dt` | Poll interval (default `10ms`) |

**Output**: the last received OSC message as a flow `[address, arg0, arg1, ...]`, or unchanged if nothing arrived.

**Emits**: `received` — a trigger that fires whenever a new message arrives. Use with `catch received:` to react only to new messages.

### Sending multiple values

```idyl
osc_send(out, "/note", pitch, velocity, duration)
```

---

## Building with/without OSC

```bash
# Enable (default)
cmake .. -DIDYL_MODULE_OSC=ON

# Disable
cmake .. -DIDYL_MODULE_OSC=OFF
```

---

## Built-in Csound module

The Csound module bridges Idƴl's temporal system with Csound instrument control.
Each `cs_open()` call creates an independent Csound instance running in a dedicated
performance thread. Multiple instances can run simultaneously.

Enable at build time:

```bash
cmake .. -DIDYL_MODULE_CSOUND=ON
```

Import in your program:

```idyl
module("csound")
```

### Functions

| Function | Description |
|----------|-------------|
| `cs_open(path)` | Compile and start a `.csd` or `.orc` file; returns a handle |
| `cs_note(handle, instr, dur_ms, p4, ...)` | Schedule a Csound i-event (fires immediately) |
| `cs_chnset(handle, channel, value)` | Write a named control channel |
| `cs_chnget(handle, channel, dt=50ms)` | Temporal: poll a named control channel |
| `cs_close(handle)` | Stop and destroy the Csound instance |

### `cs_note`

Schedules a Csound instrument event starting at the current moment (p2 = 0):

- `instr` — instrument number (number) or instrument name (string)
- `dur_ms` — duration in milliseconds (number or time literal)
- `p4, p5, ...` — additional p-fields passed verbatim

```idyl
module("csound")

process: {
    cs = cs_open("my_synth.csd")
    cs_note(cs, 1, 500ms, 440, 0.8)     // instr 1, 500ms, freq=440, amp=0.8
    cs_note(cs, "flute", 1s, 523, 0.6)  // named instrument
}
```

### `cs_chnset` and `cs_chnget`

Control channels are Csound's named software bus — a fast thread-safe way to pass
values between the host and running instruments.

```idyl
module("csound")

process: {
    cs = cs_open("synth.csd")

    lfo_val = lfo(0.5hz, 1.0, dt=10ms)
    cs_chnset(cs, "cutoff", 800 + lfo_val * 400)   // modulate cutoff with LFO

    feedback = cs_chnget(cs, "level", dt=20ms)      // read back output level
    print("level:", feedback)

    feedback catch changed: {
        print("level changed to:", feedback)
    }
}
```

`cs_chnget` is a native temporal function. It outputs the current channel value on
every tick and emits `changed` whenever the value differs from the previous tick.

### Full example

```idyl
module("csound")

process: {
    cs = cs_open("piano.csd")

    // Play a chord
    cs_note(cs, 1, 2s, 261, 0.7)    // C4
    cs_note(cs, 1, 2s, 329, 0.6)    // E4
    cs_note(cs, 1, 2s, 392, 0.6)    // G4

    @(2s): {
        cs_close(cs)
    }
}
```

---

## Native temporal module functions

Some module functions are **native temporal** — they behave like temporal functions
defined with `|>`, but their state and update logic is implemented in C++.
They are instantiated, ticked by the scheduler, and can emit values just like
user-defined temporal functions.

Currently:
- `osc_recv(handle, dt=10ms)` — polls a UDP OSC receiver
- `cs_chnget(handle, channel, dt=50ms)` — polls a Csound control channel

You call them the same way as any temporal function:

```idyl
msgs = osc_recv(rx, dt=5ms)
msgs catch received: {
    print("got:", msgs)
}
```

---

## External modules

External modules are shared libraries (`.so` / `.dylib` / `.dll`) placed in
`~/.idyl/modules/`. Load them by name:

```idyl
module("my_module")
```

The runtime looks for `~/.idyl/modules/my_module.so` (or the platform-appropriate
extension).

### Namespaced external module

```idyl
mm = module("my_module")

process: {
    result = mm::some_function(42)
}
```

---

## Example: complete OSC workflow

```idyl
module("osc")

process synth_control: {
    out = osc_out("127.0.0.1", 57120)

    lfo_freq(dt=10ms) = v |> {
        init: { v = 440 }
        v = 440 + sin(now() * 0.002) * 200
    }

    osc_send(out, "/freq", lfo_freq)
}
```

---

[Next: Architecture →](ch11_architecture.md)
