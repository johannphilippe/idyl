# 8 — Clock & tempo

[← Back to index](idyl_documentation.md) · [Previous: Emit & catch](ch07_emit_catch.md)

---

Idƴl has a built-in clock hierarchy. A main clock is created at startup, child clocks can be created and bound to parents, and tempo changes propagate through the tree.

---

## The main clock

When a program starts, a **main clock** is created at 120 BPM. All beat-relative time literals (`4b`, `2b`) resolve against this clock.

Query the main clock's tempo:

```idyl
process: {
    print("tempo:", tempo())    // 120
}
```

Set it:

```idyl
process: {
    tempo(140bpm)
    print("tempo:", tempo())    // 140
}
```

---

## The `bpm` literal

The `bpm` suffix converts beats-per-minute to a period in milliseconds:

```idyl
period = 120bpm     // 500ms  (60000 / 120)
period = 60bpm      // 1000ms
period = 240bpm     // 250ms
```

This is useful as a `dt` parameter:

```idyl
beat_counter(dt=120bpm) = n |> {
    init: { n = 0 }
    n = n + 1
}
```

---

## Creating child clocks

Use `clock(bpm)` to create a child clock bound to the main clock:

```idyl
process: {
    c1 = clock(60bpm)
    c2 = clock(90bpm)
    print("c1:", tempo(c1), "c2:", tempo(c2))
    // c1: 60  c2: 90
}
```

### Binding to a specific parent

```idyl
c1 = clock(60bpm)
c3 = clock(30bpm, parent=c1)     // c3 is child of c1, not main
```

### Free-running clocks

A clock with `parent=0` is free-running — unaffected by any tempo changes:

```idyl
c_free = clock(100bpm, parent=0)
```

---

## Tempo propagation

When a parent's BPM changes, children **scale proportionally**. The ratio is captured at creation time.

```idyl
process: {
    c1 = clock(60bpm)        // ratio to main = 60/120 = 0.5
    c2 = clock(90bpm)        // ratio to main = 90/120 = 0.75

    tempo(240bpm)             // main → 240

    print("c1:", tempo(c1))  // 240 * 0.5  = 120
    print("c2:", tempo(c2))  // 240 * 0.75 = 180
}
```

### Hierarchical propagation

```idyl
process: {
    c1 = clock(60bpm)                    // child of main, ratio 0.5
    c3 = clock(30bpm, parent=c1)         // child of c1, ratio 30/60 = 0.5

    tempo(240bpm)                         // main → 240
    // c1 → 120 (0.5 × 240)
    // c3 → 60  (0.5 × 120)

    print("c3:", tempo(c3))              // 60
}
```

### Setting a specific clock's tempo

```idyl
tempo(c1, 80bpm)          // set c1 to 80, propagate to c1's children
```

---

## Clock handles as callables

A clock handle is also **callable**. Calling it with a beat count returns the duration of that many beats at the clock's current BPM:

```idyl
process: {
    c1 = clock(60bpm)

    // c1(2b) → duration of 2 beats at 60 BPM = 2000ms
    m = metro(c1(2b))       // fires every 2 beats of c1
    m2 = metro(c1(0.5b))    // fires every half-beat of c1
    m3 = metro(c1(500ms))   // plain ms pass-through
    m4 = metro(c1())        // 1 beat (default)

    print(m, m2)
}
```

This is the primary way to drive temporal functions from a specific clock: the beat literal `2b` is resolved against **that clock's BPM**, not the main clock.

---

## Clock intrinsics

| Function | Description |
|----------|-------------|
| `clock(bpm)` | Create clock bound to main |
| `clock(bpm, parent=h)` | Create clock bound to parent `h` |
| `clock(bpm, parent=0)` | Create free-running clock |
| `tempo()` | Query main clock BPM |
| `tempo(handle)` | Query a specific clock's BPM |
| `tempo(bpm)` | Set main clock BPM (propagates) |
| `tempo(handle, bpm)` | Set specific clock BPM (propagates) |
| `handle(Nb)` | Duration of N beats at that clock's BPM |
| `handle(ms)` | Pass-through: returns the ms value unchanged |
| `handle()` | Duration of 1 beat at that clock's BPM |

All clocks return **handles** — opaque values that identify the clock in the registry.

---

## Design notes

The clock system is designed for musical contexts where tempo is hierarchical. A conductor (main clock) sets the global pulse, and individual instruments (child clocks) may run at different subdivisions. When the conductor changes tempo, everyone follows proportionally.

Free-running clocks (`parent=0`) are for things that should never change speed — metronomes locked to an external source, or clocks driven by hardware.

---

[Next: Process blocks →](ch09_process_blocks.md)
