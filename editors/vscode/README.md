# VSCode extension for Idyl

Syntax highlighting and live-coding integration for `.idyl` files.

## Features

- Syntax highlighting (tmLanguage grammar)
- Markdown code-block injection (idyl fences get highlighted)
- Live-coding commands — send constructs to a running `idyl --listen` instance over OSC

## Live-coding keys

Requires a running `idyl yourfile.idyl --listen 9000` instance.

| Key | Action |
|-----|--------|
| `Ctrl+Enter` | Hot-reload the construct at the cursor (`/idyl/eval`) |
| `Ctrl+Alt+S` | Start the process block at the cursor |
| `Ctrl+Alt+Q` | Stop the process block at the cursor |

All three commands are also available via the Command Palette (`Ctrl+Shift+P`) under the `Idyl:` category.

The evaluated range is briefly highlighted. Block detection is brace-based — the cursor can be anywhere inside the construct.

## Configuration

In `settings.json`:

```json
"idyl.oscHost": "127.0.0.1",
"idyl.oscPort": 9000
```

## Install

**From source (local):**

```bash
python3 build_vsix.py --install
```

Or manually:

```bash
python3 build_vsix.py
code --install-extension idyl-lang-0.2.0.vsix
```

**Via plugin manager** (point at the extension directory):

```
idyl/editors/vscode/idyl-lang/
```
