# ⟡ Idƴl — Documentation

Welcome to the Idƴl language documentation.

Idƴl is a functional, time-aware language for scheduling, composition, and reactive behavior. These pages walk through every feature of the language, from first install to the internals of the engine.

> Time is not an afterthought here. It is the material the language is made of.

---

## Table of contents

### Getting started

1. [Installation & first run](ch01_getting_started.md) — build from source, run a file, CLI options

### Language guide

2. [Language fundamentals](ch02_fundamentals.md) — comments, types, constants, time literals
3. [Functions](ch03_functions.md) — pure functions, parameters, defaults, composition
4. [Time & temporal functions](ch04_temporal.md) — lambda blocks, `dt`, triggers, state, the `|>` operator
5. [Flows](ch05_flows.md) — sequences, generators, multi-member flows, access patterns
6. [Control flow & operators](ch06_control_flow.md) — ternary selection, arithmetic, comparison, bitwise
7. [Emit & catch](ch07_emit_catch.md) — side-channel output, the `::` accessor, event handling
8. [Clock & tempo](ch08_clock_tempo.md) — clock hierarchy, tempo propagation, `bpm` literal
9. [Process blocks](ch09_process_blocks.md) — entry points, naming, duration, listen mode

### Integration

10. [Modules & libraries](ch10_modules_libraries.md) — OSC, `module()`, `lib()`, namespaces

### Internals

11. [Architecture & engine](ch11_architecture.md) — pipeline, semantic analysis, evaluator, scheduler

---

## Building the HTML version

A build script is provided to convert this documentation to a set of linked HTML pages. It requires [Pandoc](https://pandoc.org/) (≥ 2.11).

```bash
cd doc/
./build_html.sh          # generates html/ directory
open html/index.html     # or xdg-open on Linux
```

See [build_html.sh](build_html.sh) for details.

---

## Version

This documentation covers Idƴl **pre-alpha v0.2** (April 2026).
