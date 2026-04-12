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
| `process`, `init`, `catch`, `emit`, `start`, `stop` | **Keyword** (control) |
| `flow`, `lib`, `module` | **Keyword** (declaration) |
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
