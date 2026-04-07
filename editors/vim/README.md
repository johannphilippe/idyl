# Idyl — Vim syntax highlighting

## Quick install

### Manual (any Vim / Neovim)

```bash
# From the repo root:
mkdir -p ~/.vim/syntax ~/.vim/ftdetect
cp editors/vim/syntax/idyl.vim   ~/.vim/syntax/
cp editors/vim/ftdetect/idyl.vim ~/.vim/ftdetect/
```

For **Neovim**, replace `~/.vim` with `~/.config/nvim`.

### Using a plugin manager

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

## What it highlights

| Element | Highlight group |
|---------|----------------|
| `process`, `init`, `catch`, `emit`, `end`, `flow`, `lib`, `module`, `dt`, `dur`, `as`, `parent` | **Keyword** |
| `sin`, `cos`, `clock`, `tempo`, `osc_send`, … | **Function** (builtin) |
| `pi`, `tau` | **Constant** |
| `120bpm`, `500ms`, `1s`, `440hz`, `4b` | **Special** (time) |
| Numbers | **Number** / **Float** |
| `"strings"` and `\"escapes\"` | **String** / **SpecialChar** |
| `|>`, `::`, `..`, operators | **Operator** |
| `!` (trigger literal) | **WarningMsg** |
| `_` (rest literal) | **SpecialChar** |
| `// line` and `/* block */` comments | **Comment** |
