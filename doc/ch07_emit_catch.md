# 7 — Emit & catch

[← Back to index](idyl_documentation.md) · [Previous: Control flow](ch06_control_flow.md)

---

The **emit system** lets temporal functions output named values alongside their main return value. **Catch blocks** let process blocks react to those emitted events.

---

## Emit

Use `emit name = expression` inside a lambda block to publish a named side-channel value. Emitted values persist across ticks and are readable from outside.

```idyl
step_counter(dt=100ms) = count |> {
    init: { count = 0  emit step = 0 }
    emit step = 3
    count = count + step
}
```

Here, `step_counter` has:
- A main output: `count` (what the caller sees when using the instance as a value)
- An emitted value: `step` (accessible via the `::` operator)

### Emit in init

Emitted variables can be initialized in the `init` block to set a default value before the first tick:

```idyl
init: { emit finished = _ }   // starts as rest (no event)
```

### Emit as event signal

A common pattern is emitting a trigger when a condition is met:

```idyl
countdown(dt=100ms) = remaining |> {
    init: { remaining = 10  emit finished = _ }
    remaining = remaining - 1
    emit finished = (remaining == 0) ? _; !
}
```

`finished` is `_` (rest) on most ticks, and `!` (trigger) on the tick where `remaining` hits zero.

---

## The `::` accessor

The `::` operator reads an emitted value from a temporal instance.

**Syntax**: `instance::emitted_name`

```idyl
process: {
    c = step_counter()
    print("count:", c, "step:", c::step)
    doubled = c::step * 2
    print("doubled step:", doubled)
}
```

`c` is the main output of the instance. `c::step` reads the emitted `step` value.

The `::` operator is also used for module namespace access (`osc::send`), but the evaluator distinguishes these by context.

---

## Catch blocks

A **catch block** is a **one-shot** reaction: it fires its handler exactly once — the first time an emitted signal becomes truthy — and then deactivates. It is a standalone statement inside a process block that names the instance and signal using `::`, consistent with how `::` reads emitted values elsewhere.

Reach for `catch` when you want to react to a **single milestone**: a threshold reached, a phase completed, or — most importantly — an instance ending (`::end`, see below). For a reaction that should run *every* time a signal is live, use an [`on` block](ch09_process_blocks.md#on-blocks--trigger-reactions) instead (`on(instance::signal): { … }`); `catch` is deliberately not that tool. See [`catch` vs `on`](#catch-vs-on--which-to-use) below.

**Syntax**:

```
catch instance::signal_name: {
    // handler statements
}
```

### Example: countdown with completion handler

```idyl
countdown(dt=100ms) = remaining |> {
    init: { remaining = 10  emit finished = _ }
    remaining = remaining - 1
    emit finished = (remaining == 0) ? _; !
}

process catch_demo, dur=3s: {
    timer = countdown()
    print("remaining:", timer)

    catch timer::finished: {
        print("Countdown complete!")
    }
}
```

On each tick, `remaining` is printed. When `finished` emits a trigger (`!`), the catch handler fires once and prints `"Countdown complete!"`.

### Catch semantics

- The catch block fires **once** — the first tick the named signal is **truthy** — then deactivates permanently. It is a one-shot *latch*, **not** a per-change or per-tick reaction.
- "Truthy" means: a trigger `!`, or any non-zero / non-rest value. A trigger that stays `!` for several ticks, or a number that keeps changing, still fires the catch only on the **first** truthy tick.
- ⚠️ Because the test is truthiness, an emitted **number** that is meaningfully `0` is treated as falsy. `catch f::val` fires on the first *non-zero* value, not on the first change. If you need to react to every value or every change, that is an `on` block, not `catch`.
- The handler executes in the same scope as the surrounding process block.
- Multiple catch blocks can watch different signals from the same instance, and each fires independently (once each).

### Multiple catches

```idyl
tracker(dt=50ms) = pos |> {
    init: { pos = 0  emit halfway = _  emit done = _ }
    pos = pos + 1
    emit halfway = (pos == 50)  ? _; !
    emit done    = (pos == 100) ? _; !
}

process: {
    t = tracker()

    catch t::halfway: {
        print("halfway there")
    }

    catch t::done: {
        print("finished")
    }
}
```

### Anonymous instance catch (experimental)

The instance expression can be an inline call rather than a named binding. The instance is created when the process starts and lives for the lifetime of the process (or until it is removed by hot reload).

```idyl
process: {
    // No binding needed — the counter instance is anonymous
    catch counter_n(4)::done: {
        print("caught after 4 ticks")
        stop
    }
}
```

The signal name resolves against the anonymous instance's `emitted_` map. If no matching emitted name is found, the instance's main return value is used (so `catch metro(dt=500ms)::tick: {}` triggers whenever the metro's trigger output is live).

**Lifetime rule**: the anonymous instance is alive as long as the process block that contains the `catch` is alive — it is cancelled automatically when the process stops or is hot-reloaded away.

---

## The `::end` built-in signal

Every temporal instance automatically emits the built-in `end` signal when it self-terminates via `stop`. No `emit end = ...` declaration is needed.

```idyl
catch instance::end: {
    // handler runs once when the instance fires `stop`
}
```

When the handler fires, the bound variable holds the **frozen** last output value of the stopped instance. Further ticks do not happen — the value remains constant until the process itself ends.

```idyl
counter(limit, dt=200ms) = x
|> {
    init: { x = 0 }
    x = x + 1
    x >= limit ? _ ; stop
}

process: {
    a = counter(5)
    b = counter(10)
    print("a:", a, "b:", b)

    catch a::end: { print("a stopped at", a) }
    catch b::end: { print("b stopped at", b) }
}
```

Both catches fire independently: `a` at 5, `b` at 10. After `a` stops, the `print` line still runs on each tick of `b`, with `a` frozen at 5.

### `::end` semantics

- Fires exactly once, the same tick `stop` executes.
- Fires in the same scheduler callback, after the final output is committed.
- The handler sees the frozen value through the bound variable name.
- Fires even when `stop` executes on the very first tick (no init block, bare `stop`).

---

## `catch` vs `on` — which to use

`catch`, the [`on` block](ch09_process_blocks.md#on-blocks--trigger-reactions), and the `::` accessor overlap, but each has a distinct job. Pick by **how often** you need to react:

| You want to… | Use | Fires |
|---|---|---|
| Read an emitted value | `instance::signal` | — (it's just a value) |
| React **every** time a signal is live | `on(instance::signal): { … }` | every tick the signal is a live trigger `!` |
| React **once** at the first milestone | `catch instance::signal: { … }` | once, on the first truthy tick |
| React when an instance **ends** | `catch instance::end: { … }` | once, the tick it `stop`s |

Rules of thumb:

- **Recurring reaction → `on`.** `on(x::sig)` runs its body on every tick `sig` is a live trigger. This is the right tool for "do this each time it happens."
- **Single milestone → `catch`.** `catch x::sig` latches on the first truthy value and never fires again. Reproducing this with `on` would require a manual "already fired" flag.
- **Instance termination → `catch` only.** `::end` is *not* an emitted value and cannot be read or watched any other way: `x::end` is not even a valid expression (`end` is a keyword), and a stopped instance no longer ticks, so an `on` block would have no tick to run on. `catch x::end` is the **only** way to react to an instance ending.

Two finer points:

- `on` requires the guard to be **trigger-typed** (`!` / `_`); it ignores plain non-zero numbers. `catch` reacts to *any* truthy value (see [Catch semantics](#catch-semantics)). So `on(x::count)` where `count` is a number does nothing, while `catch x::count` fires on the first non-zero count.
- If an emitted signal is **momentary** (a single-tick `!`), `on(x::sig)` and `catch x::sig` both fire just once for that event — but `catch` still states the one-shot intent more clearly and stays correct if the signal later becomes sticky.

---

## Emit vs. output

| | Main output | Emitted values |
|---|---|---|
| Access | Use instance as value: `c` | Use `::` accessor: `c::step` |
| Per function | Exactly one | Any number |
| Declared with | Output variable before `\|>` | `emit name = expr` |
| Re-evaluated | Every tick | Every tick |
| External visibility | Always | Always (via `::`) |

The main output is what makes the instance usable in expressions (`440 * (1 + osc * 0.1)`). Emitted values are for metadata, events, and side-channel signals.

---

[Next: Clock & tempo →](ch08_clock_tempo.md)
