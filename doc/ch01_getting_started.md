# 1 — Getting started

[← Back to index](idyl_documentation.md)

---

## Requirements

- **CMake** ≥ 3.20
- **C++20** compiler (GCC 11+, Clang 14+)
- **Bison** ≥ 3.8
- **Flex**

On Ubuntu/Debian:

```bash
sudo apt install cmake g++ bison flex
```

On macOS (Homebrew):

```bash
brew install cmake bison flex
```

---

## Building from source

```bash
git clone <repository-url> idyl
cd idyl
mkdir build && cd build
cmake ..
make -j$(nproc)
```

The binary is at `build/idyl`. To install system-wide:

```bash
sudo make install    # installs to /usr/local/bin
```

### CMake options

| Flag | Default | Description |
|------|---------|-------------|
| `IDYL_MODULE_OSC` | `ON` | Enable the built-in OSC module |

Example:

```bash
cmake .. -DIDYL_MODULE_OSC=OFF
```

---

## Running a file

```bash
idyl path/to/file.idyl
```

Idƴl reads the source, runs semantic analysis (7 passes), and evaluates. If process blocks contain temporal functions, the scheduler keeps the program alive until they complete.

Reading from stdin:

```bash
echo 'silence = 0' | idyl -
```

---

## CLI reference

```
Usage: idyl [file.idyl] [options]

Options:
  --trace              Enable parser/lexer debug tracing
  --process <name>     Run only the named process block
  -p <name>            Short form of --process
  --listen [port]      Listen mode (default port: 9000)
  -l [port]            Short form of --listen
```

### `--process`

Run a single named process block and skip all others:

```bash
idyl file.idyl --process clock_demo
idyl file.idyl -p emit_demo
```

### `--listen`

Enter listen mode: load the file, store all process blocks, and wait for OSC commands to start/stop them.

```bash
idyl file.idyl --listen 9000
```

The port argument is optional (defaults to 9000). See [Chapter 9 — Process blocks](ch09_process_blocks.md) for the full listen mode protocol.

---

## Hello, Idƴl

Create a file `hello.idyl`:

```idyl
greeting = "hello, idyl"

process: {
    print(greeting)
}
```

Run it:

```bash
$ idyl hello.idyl
hello, idyl
```

Definitions outside process blocks are inert — they only execute when called from within a `process` block.

---

## A first temporal program

```idyl
counter(dt=200ms) = n |> {
    init: { n = 0 }
    n = n + 1
}

process counting, dur=2s: {
    c = counter()
    print("tick:", c)
}
```

This prints `tick: 0`, `tick: 1`, ... every 200ms for 2 seconds, then exits.

---

[Next: Language fundamentals →](ch02_fundamentals.md)
