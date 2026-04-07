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
    emit finished = _; ! ? (remaining == 0)
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

A **catch block** reacts to an emitted value becoming truthy. It is placed inside a process block, attached to a temporal instance.

**Syntax**:

```
instance catch emitted_name: {
    // handler statements
}
```

### Example: countdown with completion handler

```idyl
countdown(dt=100ms) = remaining |> {
    init: { remaining = 10  emit finished = _ }
    remaining = remaining - 1
    emit finished = _; ! ? (remaining == 0)
}

process catch_demo, dur=3s: {
    timer = countdown()
    print("remaining:", timer)

    timer catch finished: {
        print("Countdown complete!")
    }
}
```

On each tick, `remaining` is printed. When `finished` emits a trigger (`!`), the catch handler fires and prints `"Countdown complete!"`.

### Catch semantics

- The catch block fires **once** — the first time the emitted value becomes truthy.
- After firing, the catch is deactivated (it does not re-fire on subsequent truthy ticks).
- The handler statements execute in the same scope as the process block.
- Multiple catch blocks can be attached to the same instance, watching different emitted names.

### Multiple catches

```idyl
tracker(dt=50ms) = pos |> {
    init: { pos = 0  emit halfway = _  emit done = _ }
    pos = pos + 1
    emit halfway = _; ! ? (pos == 50)
    emit done = _; ! ? (pos == 100)
}

process: {
    t = tracker()

    t catch halfway: {
        print("halfway there")
    }

    t catch done: {
        print("finished")
    }
}
```

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
