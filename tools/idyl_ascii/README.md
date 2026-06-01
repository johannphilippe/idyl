# idyl_ascii

A live-coding ASCII art visualiser for [Idyl](../../README.md).  
Listens on OSC/UDP, receives Idyl source snippets, and renders tokens  
as large ASCII-art characters in your terminal with its own autonomous rhythm.

```
┌─────────────────────────────────────────────┐
│                                             │
│   ##### ## ##  ##                           │
│   #     #  #  # #                           │
│   ###   #  # #   #                          │
│   #     #  # #####                          │
│   #     ## ## #   #                         │
│                                             │
│                           [ kw: every ]     │
└─────────────────────────────────────────────┘
```

## Folder layout

```
tools/idyl_ascii/
├── CMakeLists.txt
└── src/
    ├── main.cpp           # OSC receiver + animation loop
    ├── renderer.hpp/.cpp  # ANSI terminal renderer + frame diffing
    ├── ascii_font.hpp/.cpp# 5×7 bitmap font (printable ASCII)
    └── token_stream.hpp/.cpp # lightweight Idyl tokeniser
```

## Build

Assumes this folder lives at `<idyl-repo>/tools/idyl_ascii/`.  
Idyl's `src/utilities/` directory (containing `udp.hpp`, `osc.hpp`)  
is picked up automatically.

```bash
cd tools/idyl_ascii
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
./build/idyl_ascii
```

If your Idyl headers are elsewhere:

```bash
cmake -B build -DIDYL_INCLUDE_DIR=/path/to/idyl/src/utilities
```

## Usage

```
./idyl_ascii [options]

  --port   N       UDP port to listen on          (default: 9001)
  --min-ms N       minimum token dwell time in ms (default: 350)
  --max-ms N       maximum token dwell time in ms (default: 1600)
  --hscale N       horizontal pixel magnification (default: 2)
  --vscale N       vertical   pixel magnification (default: 1)
  --scene  NAME    lock to one scene; disables automatic scene switching
  --list-scenes    print the available scene names and exit
  --debug          log received OSC packets to stderr
```

Run in a large terminal window — the bigger the better.  
`tmux`, `kitty` splits, or a second monitor work well.

## Scenes

The visualiser autonomously switches between **scenes** — combinations of a
display mode and a glitch effect — every 20–50 s, with occasional spontaneous
flips. Pass `--scene <name>` to lock one and stop the auto-switching.

| Scene       | Display      | Glitch     | Notes                          |
|-------------|--------------|------------|--------------------------------|
| `calm`      | centred      | clean      | the plain big-token default    |
| `scanlines` | centred      | scanlines  | CRT alternating dim rows       |
| `noise`     | centred      | noise      | low-level animated grain       |
| `drift`     | centred      | drift      | CRT horizontal sync wobble     |
| `corrupt`   | centred      | corrupt    | garbled pixels in alarm colours|
| `static`    | centred      | static     | bright shimmering sparks       |
| `corner`    | quadrant     | clean      | token jumps to screen corners  |
| `wander`    | drifting     | noise      | random offset + grain          |
| `mirror`    | h-flipped    | clean      | rare — mirrored text           |
| `typewrite` | typewriter   | clean      | reveals characters one by one  |
| `chaos`     | drifting     | corrupt    | offset + corruption            |
| `blizzard`  | centred      | static     | densest spark storm            |

Glitch effects in the `noise`, `static`, `corrupt` and `drift` families are
**dynamic**: they re-render at ~15 fps while a token is held, so the grain
"rains" asynchronously from the (slower) token rhythm.

## OSC protocol

| Address               | Args        | Effect                                   |
|-----------------------|-------------|------------------------------------------|
| `/idyl/eval`          | `s` source  | Push new Idyl source; starts animating   |
| `/idyl/code`          | `s` source  | Alias for `/idyl/eval`                    |
| `/idyl/process/stop`  | —           | Blank the screen                          |
| `/idyl/clear`         | —           | Alias for `/idyl/process/stop`            |
| `/idyl/tempo`         | `f` bpm     | Lock token dwell to one beat; 0 = unlock  |

The Idyl Vim/Neovim plugin sends `/idyl/eval` to both port 9000 (the live
engine) and port 9001 (this visualiser) automatically — see
`editors/vim/`.

## Sending from Idyl

A minimal Idyl snippet to push code on every evaluation:

```idyl
-- send the current snippet to the visualiser
let vis_port = 9001

let push_code = fn src ->
  osc.send "127.0.0.1" vis_port "/idyl/code" src

-- example: call push_code with the source text whenever you eval
push_code "let freq = every 0.5 [440, 550, 660, 880]"
```

Or use any OSC sender (oscsend, the bundled Python sender, etc.):

```bash
# oscsend (from liblo-tools)
oscsend localhost 9001 /idyl/eval s "process p: { m = metro(1b) }"
oscsend localhost 9001 /idyl/process/stop
oscsend localhost 9001 /idyl/tempo f 120.0

# bundled python sender (same one the vim plugin uses)
echo -n 'process p: { m = metro(1b) }' \
  | python3 ../../editors/vim/python/idyl_send.py 127.0.0.1 9001 /idyl/eval
```

## Animation design

The animator runs on its **own internal clock**, deliberately decoupled  
from the musical tempo. It cycles through the interesting tokens extracted  
from your code and spends a randomly varying time on each (bounded by  
`--min-ms` / `--max-ms`). Every token is shown bright, then briefly faded  
before the next one appears.

Token categories get distinct colours and fill characters:

| Kind        | Colour  | Fill |
|-------------|---------|------|
| keyword     | orange  | `#`  |
| identifier  | cyan    | `@`  |
| number      | lime    | `%`  |
| symbol      | grey    | `+`  |

A small label in the bottom-left corner shows the raw token text and  
category at all times, so the performer can read what's on screen.

## Dependencies

- C++20 compiler (GCC ≥ 10, Clang ≥ 12)
- POSIX sockets (Linux / macOS)
- `udp_socket.hpp` and `osc.hpp` from the Idyl repo
- No other external dependencies

## Extending

- **Different fill textures**: edit `kind_fill()` in `renderer.hpp`
- **More token types**: extend `token_kind` and `tokenise()` in `token_stream.hpp`
- **Transition effects**: add a wipe/scroll in `frame_buffer::flush_diff()`
- **notcurses backend**: replace `frame_buffer` with a notcurses surface for  
  true half-block / braille / sixel rendering at higher resolution
