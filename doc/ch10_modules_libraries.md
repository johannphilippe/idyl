# 10 — Modules & libraries

[← Back to index](idyl_documentation.md) · [Previous: Process blocks](ch09_process_blocks.md)

---

Idƴl supports importing external code through **libraries** and connecting to external systems through **modules**. The built-in OSC module is included by default.

---

## Libraries — `lib()`

Import another `.idl` file to reuse its definitions:

```idyl
lib("utils.idl")
```

All top-level bindings from `utils.idl` become available in the current scope.

### Namespaced imports

To avoid name collisions, import into a namespace:

```idyl
lib("utils.idl", as="u")

// access with dot notation
process: {
    result = u.my_function(42)
}
```

### Deduplication

If the same library is imported multiple times (directly or transitively), it is only loaded once:

```idyl
lib("core.idl")
lib("helpers.idl")    // if helpers.idl also imports core.idl, it's not loaded again
```

---

## Modules — `module()`

Modules connect Idƴl to external systems. They are loaded by name:

```idyl
module("midi")
```

Modules provide functions that become available after loading. The available modules depend on the build configuration.

---

## Built-in OSC module

OSC (Open Sound Control) support is built in and enabled via the `IDYL_MODULE_OSC` CMake option (ON by default).

### OSC output

```idyl
process: {
    osc = osc_out("127.0.0.1", 9000)

    freq(dt=10ms) = v |> {
        init: { v = 440 }
        v = v + sin(now() * 0.001) * 100
    }

    osc_send(osc, "/synth/freq", freq)
}
```

### OSC functions

| Function | Description |
|----------|-------------|
| `osc_out(host, port)` | Create an OSC output handle |
| `osc_send(handle, address, value)` | Send a value to the address |
| `osc_send(handle, address, v1, v2, ...)` | Send multiple values |
| `osc_close(handle)` | Close the connection |
| `osc_in(port)` | Create an OSC input listener |

### Sending multiple values

```idyl
osc_send(osc, "/note", pitch, velocity, duration)
```

### OSC input

```idyl
process: {
    input = osc_in(8000)
    // input receives messages on port 8000
}
```

---

## Building with/without OSC

OSC is controlled at build time:

```bash
# Enable (default)
cmake .. -DIDYL_MODULE_OSC=ON

# Disable
cmake .. -DIDYL_MODULE_OSC=OFF
```

When OSC is disabled, calling `osc_out`, `osc_send`, etc. will produce a runtime error.

---

## Example: complete OSC workflow

```idyl
process synth_control: {
    osc = osc_out("127.0.0.1", 57120)

    // LFO modulating frequency
    lfo(dt=10ms) = v |> {
        init: { v = 0 }
        v = sin(now() * 0.002) * 200 + 440
    }

    // envelope
    env(dt=50ms) = e |> {
        init: { e = 0 }
        e = (e < 1) ? e + 0.05 : 1
    }

    osc_send(osc, "/freq", lfo)
    osc_send(osc, "/amp", env)
}
```

---

[Next: Architecture →](ch11_architecture.md)
