#!/usr/bin/env bash
#
# build_html.sh — Convert Idƴl documentation from Markdown to HTML
#
# Requirements: pandoc  (https://pandoc.org)
#   sudo apt install pandoc        # Debian/Ubuntu
#   brew install pandoc             # macOS
#
# Usage:
#   cd doc/
#   ./build_html.sh                # outputs to doc/html/
#

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OUT_DIR="${SCRIPT_DIR}/html"
CSS="style.css"
SYNTAX_DEF="${SCRIPT_DIR}/idyl.xml"

# Check for pandoc
if ! command -v pandoc &>/dev/null; then
    echo "Error: pandoc is not installed."
    echo "  sudo apt install pandoc    # Debian/Ubuntu"
    echo "  brew install pandoc        # macOS"
    exit 1
fi

# Create output directory
mkdir -p "${OUT_DIR}"

# Copy CSS
cp "${SCRIPT_DIR}/${CSS}" "${OUT_DIR}/${CSS}"

# Markdown files to convert
FILES=(
    idyl_documentation.md
    ch01_getting_started.md
    ch02_fundamentals.md
    ch03_functions.md
    ch04_temporal.md
    ch05_flows.md
    ch06_control_flow.md
    ch07_emit_catch.md
    ch08_clock_tempo.md
    ch09_process_blocks.md
    ch10_modules_libraries.md
    ch11_architecture.md
)

count=0
for md in "${FILES[@]}"; do
    src="${SCRIPT_DIR}/${md}"
    if [[ ! -f "${src}" ]]; then
        echo "Warning: ${md} not found, skipping."
        continue
    fi

    html_name="${md%.md}.html"

    # Convert .md links to .html links in a temp copy
    tmp=$(mktemp)
    sed 's/\(\[^)]*\)\.md\b/\1.html/g' "${src}" > "${tmp}"

    pandoc "${tmp}" \
        --from=markdown \
        --to=html5 \
        --standalone \
        --css="${CSS}" \
        --syntax-definition="${SYNTAX_DEF}" \
        --metadata title="Idƴl Documentation" \
        --highlight-style=kate \
        -o "${OUT_DIR}/${html_name}"

    rm -f "${tmp}"
    count=$((count + 1))
done

echo "Converted ${count} files → ${OUT_DIR}/"
