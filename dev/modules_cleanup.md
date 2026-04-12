# Module cleanup on exit

## Goal

When idyl is killed (Ctrl-C, `SIGTERM`, or natural end-of-program), every loaded module should have a chance to release its resources — close sockets, stop Csound performance threads, flush pending buffers, etc. — before the process exits.

This must happen whether idyl exits cleanly (all subscriptions expired) or abruptly (signal received).

---

## Current state

`src/idyl.cpp` already installs `SIGINT` and `SIGTERM` handlers that set `g_running = false`, which breaks the main keep-alive loop. After the loop, `scheduler.stop()` is called. No module cleanup is triggered.

The `module::registry` destructor closes `dlopen` handles but calls no module-level teardown.

Modules currently available:

| Module | What needs cleanup |
|--------|--------------------|
| `osc_module` | UDP sockets, dt-driven send subscriptions (already closeable via `osc_close` / `osc_stop` from idyl code, but not called automatically) |
| `csound_module` | `cs_instance` objects each hold a Csound instance + performance thread; `cs_instance::stop()` already exists and joins the thread |

---

## Design

### 1 — Add `cleanup()` to `base_module`

In `src/include/module.hpp`, add a virtual `cleanup()` method to `base_module`:

```cpp
struct base_module {
    // ... existing interface ...

    // Called once, after the scheduler has stopped and before the process exits.
    // Override to release sockets, threads, external resources, etc.
    // Must be non-throwing.
    virtual void cleanup() noexcept {}
};
```

The method is optional (`noexcept`, default no-op) — modules that have nothing to tear down don't need to override it.

---

### 2 — Add `cleanup_all()` to `registry`

In `src/include/module.hpp`, add to `class registry`:

```cpp
// Call cleanup() on every registered module, in reverse registration order.
// Called once, after scheduler.stop(), before process exit.
void cleanup_all() noexcept {
    for (auto it = modules_.rbegin(); it != modules_.rend(); ++it) {
        try { (*it)->cleanup(); } catch (...) {}
    }
}
```

Reverse order mirrors typical LIFO teardown (last-loaded module cleaned up first).

---

### 3 — Call `cleanup_all()` in `src/idyl.cpp`

After `scheduler.stop()`, before `return 0`:

```cpp
scheduler.stop();
module_registry.cleanup_all();   // ← add this line
idyl::debug("Evaluation completed.");
return 0;
```

This single call covers all exit paths through the normal flow. It runs regardless of whether idyl exited because all subscriptions expired or because a signal was caught.

---

### 4 — Implement `cleanup()` in the OSC module

In `src/modules/osc_module.hpp`, override `cleanup()`:

```cpp
void cleanup() noexcept override {
    // Stop all dt-driven scheduled sends
    do_osc_stop();
    // Close every open UDP socket
    std::lock_guard<std::mutex> lk(mutex_);
    senders_.clear();   // destructors close the sockets
}
```

`senders_` is the `std::unordered_map<intptr_t, udp_sender>` (or equivalent) that holds open connections. Clearing it triggers each sender's destructor.

---

### 5 — Implement `cleanup()` in the Csound module

In `src/modules/csound_module.hpp`, override `cleanup()`:

```cpp
void cleanup() noexcept override {
    // stop() joins the performance thread and calls csoundDestroy.
    // Take a snapshot of keys to avoid iterator invalidation.
    std::vector<intptr_t> keys;
    {
        std::lock_guard<std::mutex> lk(map_mutex_);
        for (auto& [k, _] : instances_) keys.push_back(k);
    }
    for (auto k : keys) do_cs_close_by_key(k);
}
```

`cs_instance::stop()` already exists and is correct — `cleanup()` just drives it for all live instances.

---

## Signal safety note

The signal handlers only write to `std::atomic<bool> g_running`. They do not call cleanup directly. Cleanup happens on the main thread, in the normal code path, after the keep-alive loop exits. This is the correct approach — signal handlers must be async-signal-safe, and module teardown (mutex locks, system calls) is not.

No change to the signal handlers is needed.

---

## External modules (dlopen)

External modules loaded via `module("path/to/lib.so")` use the C ABI (`idyl_module_api.h`). To expose a cleanup hook to external modules, `idyl_module_api.h` should gain an optional symbol:

```c
// Optional: if the module exports this symbol, idyl calls it at shutdown.
typedef void (*idyl_module_cleanup_fn)(void);
```

In `registry::cleanup_all()` (or a separate `cleanup_external()` step), look up `idyl_module_cleanup` via `dlsym` for each open handle and call it if present:

```cpp
for (auto* h : dl_handles_) {
    auto fn = reinterpret_cast<idyl_module_cleanup_fn>(
        dlsym(h, "idyl_module_cleanup"));
    if (fn) { try { fn(); } catch (...) {} }
}
```

This is opt-in for external module authors — the absence of the symbol is not an error.

---

## Implementation steps

1. **`src/include/module.hpp`**
   - Add `virtual void cleanup() noexcept {}` to `base_module`
   - Add `void cleanup_all() noexcept` to `registry` (iterates `modules_` in reverse)
   - Add dlopen cleanup loop in `cleanup_all()` or as a separate step after the module loop

2. **`src/idyl.cpp`**
   - Add `module_registry.cleanup_all();` after `scheduler.stop()`

3. **`src/modules/osc_module.hpp`**
   - Override `cleanup()`: call `do_osc_stop()`, then clear `senders_`

4. **`src/modules/csound_module.hpp`**
   - Override `cleanup()`: iterate all live `cs_instance` handles, call `stop()` on each

5. **`src/include/idyl_module_api.h`** *(optional, for external modules)*
   - Document `idyl_module_cleanup` as an optional export symbol

---

## What does NOT change

- Signal handlers (`SIGINT`, `SIGTERM`) — already correct, no modification needed
- `scheduler.stop()` call order — cleanup runs after the scheduler is stopped, so no new scheduler callbacks can fire during teardown
- `registry` destructor — still closes `dlopen` handles after `cleanup_all()` has run (correct order: cleanup first, unload second)
