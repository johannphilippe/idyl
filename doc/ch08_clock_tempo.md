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

## Scheduler backends

All temporal functions — counters, metros, LFOs, process durations — are ultimately fired by a **scheduler** that tracks when each subscription is due. Idƴl provides two schedulers selectable at runtime.

---

### Default: system-clock scheduler

The default scheduler (`sys_clock`) uses `std::chrono::steady_clock` — the highest-resolution steady clock the OS exposes. It uses a hybrid sleep + busy-wait strategy: it sleeps until ~500 µs before the target time, then busy-waits to the exact deadline.

```bash
idyl file.idyl           # system-clock scheduler (default)
```

**Characteristics**:
- Typical jitter: ~0.5 ms; ~1–2 ms under heavy system load
- No extra resources (no timer device, no audio device)
- Each subscription has its own scheduled event in a priority queue, so CPU scales with the number of active temporal functions

**Best for**: general scripting, OSC control, MIDI, any work where sub-millisecond accuracy is not critical.

---

### Audio clock: high-resolution timer scheduler

The `--audio-clock` flag activates a polling scheduler driven by an OS high-resolution timer. It wakes at a fixed period (`buffer_size / sample_rate` seconds), checks all active subscriptions against the current time, and fires any that are due.

```bash
idyl file.idyl --audio-clock
idyl file.idyl --audio-clock --audio-buffer-size 32 --audio-sample-rate 48000
```

| Flag | Short | Default | Description |
|------|-------|---------|-------------|
| `--audio-clock` | `-ac` | — | Enable the audio-clock scheduler |
| `--audio-buffer-size <n>` | `-abs <n>` | `32` | Frames per tick (lower = more frequent checks) |
| `--audio-sample-rate <hz>` | `-asr <hz>` | `48000` | Sample rate used to derive the tick period |

**Important**: this scheduler does **not** open any audio device. It uses the OS timer API directly, so it never conflicts with Csound, Jack, PipeWire, or any other audio engine running in the same process.

---

### Accuracy: two independent components

The audio-clock scheduler's timing accuracy has two independent components that should be understood separately.

#### 1 — OS timer wakeup jitter

This is the intrinsic precision of the platform timer. It is **not** affected by `--audio-buffer-size`.

| Platform | Mechanism | Typical wakeup jitter |
|----------|-----------|-----------------------|
| Linux | `timerfd_create(CLOCK_MONOTONIC)` | ~0.1 ms |
| macOS | `mach_wait_until` | ~0.01–0.1 ms |
| Windows 10 1803+ | `CreateWaitableTimerEx` (high-res) | ~0.5 ms |
| Windows (older) | `timeBeginPeriod(1)` + waitable timer | ~1 ms |
| Other | `std::this_thread::sleep_until` | ~1–5 ms |

#### 2 — Scan granularity

The scheduler only checks subscriptions once per timer wakeup. A subscription whose deadline falls between two wakeups is held until the next check. The **maximum additional latency** per subscription is exactly one scan period:

```
scan_period = buffer_size / sample_rate
```

| `--audio-buffer-size` | Scan period at 48 kHz | Max subscription latency |
|-----------------------|-----------------------|--------------------------|
| 32 (default) | 0.67 ms | 0.67 ms |
| 64 | 1.33 ms | 1.33 ms |
| 128 | 2.67 ms | 2.67 ms |
| 256 | 5.33 ms | 5.33 ms |
| 512 | 10.67 ms | 10.67 ms |

Because `now_ms()` always reads the actual steady clock at the moment subscriptions are checked, there is **no accumulated drift** — only a one-shot latency bounded by one scan period.

**Total worst-case latency** = OS timer jitter + scan granularity.  
On Linux at the default `buffer_size=32`: ~0.1 + 0.67 ≈ **0.77 ms**.

---

### Choosing between modes

| Situation | Recommended mode |
|-----------|-----------------|
| OSC, MIDI, general scripting | `sys_clock` (default) |
| Csound note scheduling, tight sequencing | `--audio-clock` |
| Sub-millisecond accuracy required | `--audio-clock --audio-buffer-size 32` |
| Lower CPU, accuracy ≥ 3 ms acceptable | `--audio-clock --audio-buffer-size 128` |
| Coarse scheduling, accuracy ≥ 10 ms | `--audio-clock --audio-buffer-size 512` |

Both modes produce **drift-free** scheduling: each subscription's next deadline is computed from its previous deadline (`next = prev + dt`), never from the current time. Over long runs, the timing does not drift.

---

### Tuning for your system

**Tight audio work (Csound, precise sequencing)**:
```bash
idyl file.idyl --audio-clock --audio-buffer-size 32
```
Default settings. Scan period ≈ 0.67 ms. Works reliably on all supported platforms.

**Reduced CPU at the cost of some latency**:
```bash
idyl file.idyl --audio-clock --audio-buffer-size 128
```
Scan period ≈ 2.67 ms. The scheduler wakes up ~4× less often. Good for systems under load or when battery matters.

**Non-standard sample rate**:
```bash
idyl file.idyl --audio-clock --audio-sample-rate 44100 --audio-buffer-size 32
```
Scan period = 32 / 44100 ≈ 0.73 ms. The rate parameter is used only to compute the scan period — no audio device is opened at any sample rate.

**Diagnosing timing issues**: run without `--audio-clock` first to establish a baseline with `sys_clock`. If the two modes differ significantly in tempo or regularity, check whether another process is contending for the CPU.

---

[Next: Process blocks →](ch09_process_blocks.md)
