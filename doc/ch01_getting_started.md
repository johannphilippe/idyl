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
| `IDYL_MODULE_MIDI` | `ON` | Enable the built-in MIDI module (requires RtMidi, fetched automatically) |
| `IDYL_MODULE_SERIAL` | `ON` | Enable the built-in SERIAL module |
| `IDYL_MODULE_CSOUND` | `ON` | Enable the built-in CSOUND module (requires `libcsound64-dev`) |
| `IDYL_AUDIO_CLOCK` | `ON` | Enable the high-resolution timer scheduler (`--audio-clock` flag) |

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
  --trace                        Enable parser/lexer debug tracing
  --process <name>               Run only the named process block
  -p <name>                      Short form of --process
  --listen [port]                Listen mode (default port: 9000)
  -l [port]                      Short form of --listen
  --audio-clock                  Use high-resolution timer scheduler
  -ac                            Short form of --audio-clock
  --audio-buffer-size <frames>   Scan period in frames (default: 32)
  -abs <frames>                  Short form of --audio-buffer-size
  --audio-sample-rate <hz>       Sample rate for period calculation (default: 48000)
  -asr <hz>                      Short form of --audio-sample-rate
```

### `--process`

Run a single named process block and skip all others:

```bash
idyl file.idyl --process clock_demo
idyl file.idyl -p emit_demo
```

### `--listen`

Enter listen mode: load the file, store all process blocks, and wait for OSC commands to start, stop, and hot-reload them.

```bash
idyl file.idyl --listen 9000
```

The port argument is optional (defaults to 9000). Key OSC addresses:

| Address | Argument | Effect |
|---------|----------|--------|
| `/idyl/process/start` | name | Start a process |
| `/idyl/process/stop` | name | Stop a process |
| `/idyl/eval` | source | Hot-reload a definition or running process |

See [Chapter 9 — Process blocks](ch09_process_blocks.md) for the full listen mode protocol and hot-reload behaviour.

### `--audio-clock`

Switch to the high-resolution timer scheduler. Wakes at a fixed period (`buffer_size / sample_rate`) and checks all active temporal subscriptions. Does not open any audio device — safe to use alongside Csound or any other audio engine.

```bash
idyl file.idyl --audio-clock                               # 0.67 ms scan period (default)
idyl file.idyl --audio-clock --audio-buffer-size 128       # 2.67 ms scan period, lower CPU
idyl file.idyl --audio-clock --audio-sample-rate 44100     # 0.73 ms scan period at 44.1 kHz
```

See [Chapter 8 — Clock & tempo](ch08_clock_tempo.md#scheduler-backends) for a full comparison of both modes and a tuning guide.

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
