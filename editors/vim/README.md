# Idyl — Vim plugin

Provides syntax highlighting and live-coding integration for `.idyl` files.

---

## Quick install

### Manual (any Vim / Neovim)

```bash
# From the repo root:
mkdir -p ~/.vim/syntax ~/.vim/ftdetect ~/.vim/ftplugin ~/.vim/python
cp editors/vim/syntax/idyl.vim    ~/.vim/syntax/
cp editors/vim/ftdetect/idyl.vim  ~/.vim/ftdetect/
cp editors/vim/ftplugin/idyl.vim  ~/.vim/ftplugin/
cp editors/vim/python/idyl_send.py ~/.vim/python/
```

For **Neovim**, replace `~/.vim` with `~/.config/nvim`.

### Using a plugin manager (recommended)

Point the manager at the `editors/vim` directory and it will pick up all
subdirectories automatically.

<details>
<summary>vim-plug</summary>

```vim
Plug '/path/to/idyl/editors/vim'
```
</details>

<details>
<summary>lazy.nvim</summary>

```lua
{ dir = "/path/to/idyl/editors/vim" }
```
</details>

<details>
<summary>packer.nvim</summary>

```lua
use { '/path/to/idyl/editors/vim' }
```
</details>

With a plugin manager the `python/` directory is automatically on the correct
relative path — no extra steps needed.

---

## Syntax highlighting

| Element | Highlight group |
|---------|----------------|
| `process`, `init`, `catch`, `emit`, `start`, `stop` | **Keyword** (control) |
| `flow`, `import`, `module` | **Keyword** (declaration) |
| `dt=`, `dur=`, `parent=` | **Type** (named parameters) |
| `sin`, `cos`, `clock`, `tempo`, `metro`, `as_bpm`, `osc_send`, … | **Function** (builtin) |
| `pi`, `tau` | **Constant** |
| `120bpm`, `500ms`, `1s`, `440hz`, `4b`, `0.5b` | **Special** (time literals) |
| Numbers | **Number** / **Float** |
| `"strings"` and `\"escapes\"` | **String** / **SpecialChar** |
| `\|>` lambda, `::` namespace/emit, `..` range | **Operator** |
| `'(` delay operator, `@(` deferred block | **Operator** |
| `!` (trigger literal), `name!` (trigger parameter) | **WarningMsg** |
| `_` (rest literal) | **Comment** |
| Process block name after `process` | **Title** |
| Flow name after `flow` | **Type** |
| Member access `.name` | **Identifier** |
| `// line` and `/* block */` comments | **Comment** |

---

## Live-coding integration

`plugin/idyl_live.vim` adds key mappings that send the top-level construct
under the cursor to a running idyl instance via OSC.

### Requirements

- **Python 3** (standard library only — no extra packages)
- idyl running with `--listen` (e.g. `idyl song.idyl --listen 9000`)

### Key mappings

| Key | Mode | Action |
|-----|------|--------|
| `t` | Normal | Evaluate the construct at the cursor |
| `Ctrl+e` | Insert | Evaluate without leaving insert mode |

Both mappings are **buffer-local** — they only apply in `.idyl` files and do
not affect other filetypes.

> **Note**: `t` shadows Vim's built-in `t` motion (jump before character).
> You can still use `;` and `,` to repeat the last `f`/`t` search, or remap
> the plugin key — see Configuration below.

### What gets evaluated

The plugin detects the **top-level construct** enclosing the cursor and sends
its full text. It correctly handles all three construct types:

```idyl
" ── Function (single-line) ────────────────────────────────────────────────
square(x) = x * x

" ── Temporal function (multi-line, |> block) ──────────────────────────────
metro(dt=500ms) = !
|> {
    init: { phase = 0 }
    phase = phase + 1
}

" ── Flow ──────────────────────────────────────────────────────────────────
flow scale = [60, 62, 64, 67, 69]

" ── Process block ─────────────────────────────────────────────────────────
process bass: {
    m = metro(dt=250ms)
    print(m)
}
```

Placing the cursor **anywhere** inside a construct (including on indented
lines, the closing `}`, or a blank line just before) and pressing `t` sends
the entire construct.

### Typical live-coding session

```bash
# Terminal 1: start idyl in listen mode
idyl song.idyl --listen 9000

# Terminal 2 (or split): open the source file in Vim/Neovim
nvim song.idyl
```

Edit a function or process, press `t` — the change takes effect on the next
tick of any instance that calls the redefined function, or the running process
is diffed and updated in place.

### Configuration

```vim
" In vimrc / init.vim (before the plugin loads):
let g:idyl_osc_host = '127.0.0.1'   " target host (default)
let g:idyl_osc_port = 9000           " target port (default)
```

To remap the keys without losing the shadowed built-ins:

```vim
" Use <leader>e instead of t
autocmd FileType idyl nnoremap <buffer> <silent> <leader>e
      \ :<C-u>call <SID>EvalAtCursor()<CR>
```

Or in the plugin itself, change the key mapping lines at the bottom of
`ftplugin/idyl.vim`.
