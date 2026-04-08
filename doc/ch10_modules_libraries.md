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
