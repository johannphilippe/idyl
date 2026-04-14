#!/usr/bin/env bash
# install.sh — Install the Idyl Vim plugin (syntax + live-coding integration)
#
# Works on Linux and macOS with Vim, Neovim, or both.
#
# Usage:
#   ./install.sh               # auto-detect editors, prompt if both found
#   ./install.sh --vim         # install for Vim only
#   ./install.sh --nvim        # install for Neovim only
#   ./install.sh --both        # install for both without prompting
#   ./install.sh --uninstall   # remove installed files
#
# The following files are installed:
#   syntax/idyl.vim       — syntax highlighting
#   ftdetect/idyl.vim     — filetype detection for *.idyl
#   ftplugin/idyl.vim     — live-coding key mappings (t, Ctrl+e) + eval highlight
#   python/idyl_send.py   — OSC UDP sender (used by idyl_live.vim)

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# ── Terminal colours ──────────────────────────────────────────────────────────
if [[ -t 1 ]]; then
  GRN='\033[0;32m' YEL='\033[1;33m' BLU='\033[0;34m' RED='\033[0;31m' NC='\033[0m'
else
  GRN='' YEL='' BLU='' RED='' NC=''
fi

ok()   { printf "${GRN}✓${NC} %s\n"      "$*"; }
info() { printf "${BLU}→${NC} %s\n"      "$*"; }
warn() { printf "${YEL}!${NC} %s\n"      "$*"; }
die()  { printf "${RED}✗${NC} %s\n" "$*" >&2; exit 1; }

# ── Files that must be installed (path relative to editors/vim/) ──────────────
FILES=(
  "syntax/idyl.vim"
  "ftdetect/idyl.vim"
  "ftplugin/idyl.vim"
  "python/idyl_send.py"
)

# ── Install all files into a Vim-compatible runtime directory ─────────────────
install_to() {
  local dst_root="$1"
  local label="$2"

  info "Installing into $dst_root  ($label)"

  local any=0
  for rel in "${FILES[@]}"; do
    local src="$SCRIPT_DIR/$rel"
    local dst="$dst_root/$rel"
    local dst_dir; dst_dir="$(dirname "$dst")"

    if [[ ! -f "$src" ]]; then
      warn "  source not found: $rel — skipping"
      continue
    fi

    mkdir -p "$dst_dir"
    cp -f "$src" "$dst"
    ok "  $rel"
    any=1
  done

  if [[ $any -eq 1 ]]; then ok "$label install complete."; fi
}

# ── Uninstall all files from a directory ──────────────────────────────────────
uninstall_from() {
  local dst_root="$1"
  local label="$2"

  info "Removing from $dst_root  ($label)"
  local any=0
  for rel in "${FILES[@]}"; do
    local dst="$dst_root/$rel"
    if [[ -f "$dst" ]]; then
      rm -f "$dst"
      ok "  removed $rel"
      any=1
    fi
  done
  if [[ $any -eq 0 ]]; then warn "  nothing to remove in $dst_root"; fi
}

# ── Detect available editor runtime directories ───────────────────────────────
detect_dirs() {
  local -n _vim_ref=$1
  local -n _nvim_ref=$2

  # Vim: ~/.vim on all platforms
  if command -v vim &>/dev/null || [[ -d "$HOME/.vim" ]]; then
    _vim_ref="$HOME/.vim"
  fi

  # Neovim: follows XDG_CONFIG_HOME on all platforms (Linux and macOS)
  local nvim_cfg="${XDG_CONFIG_HOME:-$HOME/.config}/nvim"
  if command -v nvim &>/dev/null || [[ -d "$nvim_cfg" ]]; then
    _nvim_ref="$nvim_cfg"
  fi
}

# ── Interactive editor selection ──────────────────────────────────────────────
choose_editor() {
  local vim_dir="$1"
  local nvim_dir="$2"

  echo ""
  echo "Both Vim and Neovim detected."
  echo "  [1] Vim only       ($vim_dir)"
  echo "  [2] Neovim only    ($nvim_dir)"
  echo "  [3] Both"
  echo ""
  printf "Install for: [1/2/3] "
  read -r choice </dev/tty

  case "$choice" in
    1) echo "vim"  ;;
    2) echo "nvim" ;;
    3) echo "both" ;;
    *) die "Invalid choice." ;;
  esac
}

# ── Parse arguments ───────────────────────────────────────────────────────────
MODE=""         # vim | nvim | both | auto
UNINSTALL=0

for arg in "$@"; do
  case "$arg" in
    --vim)        MODE=vim  ;;
    --nvim)       MODE=nvim ;;
    --both)       MODE=both ;;
    --uninstall)  UNINSTALL=1 ;;
    -h|--help)
      # Print the comment header: skip shebang (line 1), stop at first non-# line
      awk 'FNR==1{next} /^[^#]/{exit} {sub(/^# ?/,""); print}' "$0"
      exit 0
      ;;
    *) die "Unknown option: $arg  (use --help for usage)" ;;
  esac
done

# ── Main ──────────────────────────────────────────────────────────────────────
echo "Idyl Vim plugin installer"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

VIM_DIR=""
NVIM_DIR=""
detect_dirs VIM_DIR NVIM_DIR

# When a mode is explicitly requested, supply default dirs for editors that
# were not auto-detected (e.g. nvim installed but ~/.config/nvim doesn't exist yet).
if [[ "$MODE" == "vim"  || "$MODE" == "both" ]] && [[ -z "$VIM_DIR"  ]]; then
  VIM_DIR="$HOME/.vim"
fi
if [[ "$MODE" == "nvim" || "$MODE" == "both" ]] && [[ -z "$NVIM_DIR" ]]; then
  NVIM_DIR="${XDG_CONFIG_HOME:-$HOME/.config}/nvim"
fi

# Validate: at least one editor must be found (unless user forced a mode above)
if [[ -z "$VIM_DIR" && -z "$NVIM_DIR" ]]; then
  die "Neither Vim nor Neovim detected. Install vim or nvim, then re-run."
fi

# Resolve which editor(s) to target (auto-detect only when no --flag given)
if [[ -z "$MODE" ]]; then
  if [[ -n "$VIM_DIR" && -n "$NVIM_DIR" ]]; then
    MODE="$(choose_editor "$VIM_DIR" "$NVIM_DIR")"
  elif [[ -n "$VIM_DIR" ]]; then
    MODE=vim
  else
    MODE=nvim
  fi
fi

# ── Install or uninstall ──────────────────────────────────────────────────────
if [[ $UNINSTALL -eq 1 ]]; then
  echo ""
  [[ "$MODE" == "vim"  || "$MODE" == "both" ]] && uninstall_from "$VIM_DIR"  "Vim"
  [[ "$MODE" == "nvim" || "$MODE" == "both" ]] && uninstall_from "$NVIM_DIR" "Neovim"
  echo ""
  ok "Uninstall complete."
  exit 0
fi

echo ""
[[ "$MODE" == "vim"  || "$MODE" == "both" ]] && install_to "$VIM_DIR"  "Vim"
[[ "$MODE" == "nvim" || "$MODE" == "both" ]] && { echo ""; install_to "$NVIM_DIR" "Neovim"; }

# ── Post-install instructions ─────────────────────────────────────────────────
echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
ok "Installation complete."
echo ""
echo "  Open any .idyl file to get syntax highlighting."
echo ""
echo "  Live-coding workflow:"
echo "    1. idyl yourfile.idyl --listen 9000"
echo "    2. Open yourfile.idyl in Vim/Neovim"
echo "    3. Press  t        (normal mode) — evaluate construct at cursor"
echo "               Ctrl+e  (insert mode) — evaluate without leaving insert mode"
echo ""
echo "  Configuration (add to vimrc / init.vim):"
echo "    let g:idyl_osc_host = '127.0.0.1'   \" default"
echo "    let g:idyl_osc_port = 9000           \" default"
echo ""
echo "  Alternative: use a plugin manager pointing at editors/vim/"
echo "    vim-plug:  Plug '/path/to/idyl/editors/vim'"
echo "    lazy.nvim: { dir = '/path/to/idyl/editors/vim' }"
