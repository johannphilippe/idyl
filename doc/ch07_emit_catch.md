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

A **catch block** reacts to an emitted value becoming truthy. It is a standalone statement inside a process block that names the instance and signal using `::` — consistent with how `::` reads emitted values elsewhere.

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

- The catch block fires **once** — the first time the named signal becomes truthy.
- After firing, the catch is deactivated (it does not re-fire on subsequent truthy ticks).
- The handler executes in the same scope as the surrounding process block.
- Multiple catch blocks can watch different signals from the same instance.

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
