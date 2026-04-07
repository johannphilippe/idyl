#!/usr/bin/env bash
# ════════════════════════════════════════════════════════════════════════════════
#  Idyl — Test Runner
#
#  Usage:
#    ./tests/tests.sh                  # run all tests
#    ./tests/tests.sh 01_pre-alpha     # run a specific suite
#    ./tests/tests.sh -v               # verbose (show stdout for all tests)
#    ./tests/tests.sh -t 10            # custom timeout (seconds, default: 5)
#    ./tests/tests.sh -f clock         # filter: only tests matching pattern
#
#  Exit code:
#    0  — all tests passed
#    1  — one or more tests failed
# ════════════════════════════════════════════════════════════════════════════════
set -euo pipefail

# ── Resolve paths ───────────────────────────────────────────────────────────────
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
IDYL="$PROJECT_ROOT/build/idyl"

# ── Defaults ────────────────────────────────────────────────────────────────────
TIMEOUT=5          # seconds per test (temporal tests run until killed)
VERBOSE=0
FILTER=""
SUITE=""           # empty = all suites

# ── Colors ──────────────────────────────────────────────────────────────────────
if [[ -t 1 ]]; then
    C_RESET="\033[0m"
    C_PASS="\033[32m"     # green
    C_FAIL="\033[31m"     # red
    C_WARN="\033[33m"     # yellow
    C_INFO="\033[36m"     # cyan
    C_BOLD="\033[1m"
    C_DIM="\033[2m"
else
    C_RESET="" C_PASS="" C_FAIL="" C_WARN="" C_INFO="" C_BOLD="" C_DIM=""
fi

# ── Parse arguments ─────────────────────────────────────────────────────────────
while [[ $# -gt 0 ]]; do
    case "$1" in
        -v|--verbose)   VERBOSE=1; shift ;;
        -t|--timeout)   TIMEOUT="$2"; shift 2 ;;
        -f|--filter)    FILTER="$2"; shift 2 ;;
        -h|--help)
            sed -n '2,14p' "$0" | sed 's/^#  \?//'
            exit 0 ;;
        *)              SUITE="$1"; shift ;;
    esac
done

# ── Pre-flight checks ──────────────────────────────────────────────────────────
if [[ ! -x "$IDYL" ]]; then
    echo -e "${C_FAIL}ERROR:${C_RESET} idyl binary not found at $IDYL"
    echo "       Run:  cmake --build build"
    exit 1
fi

# ── Collect test files ──────────────────────────────────────────────────────────
declare -a TEST_FILES=()

if [[ -n "$SUITE" ]]; then
    SUITE_DIR="$SCRIPT_DIR/$SUITE"
    if [[ ! -d "$SUITE_DIR" ]]; then
        echo -e "${C_FAIL}ERROR:${C_RESET} Suite directory not found: $SUITE_DIR"
        exit 1
    fi
    mapfile -t TEST_FILES < <(find "$SUITE_DIR" -name '*.idl' | sort)
else
    mapfile -t TEST_FILES < <(find "$SCRIPT_DIR" -name '*.idl' | sort)
fi

# Apply filter
if [[ -n "$FILTER" ]]; then
    FILTERED=()
    for f in "${TEST_FILES[@]}"; do
        if [[ "$(basename "$f")" == *"$FILTER"* ]]; then
            FILTERED+=("$f")
        fi
    done
    TEST_FILES=("${FILTERED[@]}")
fi

TOTAL=${#TEST_FILES[@]}
if [[ $TOTAL -eq 0 ]]; then
    echo -e "${C_WARN}No test files found.${C_RESET}"
    exit 0
fi

# ── Counters ────────────────────────────────────────────────────────────────────
PASSED=0
FAILED=0
WARNED=0
TIMED_OUT=0
CRASHED=0

declare -a FAIL_NAMES=()
declare -a FAIL_DETAILS=()

# ── Run tests ───────────────────────────────────────────────────────────────────
echo ""
echo -e "${C_BOLD}Idyl Test Runner${C_RESET}"
echo -e "${C_DIM}Binary:  $IDYL${C_RESET}"
echo -e "${C_DIM}Timeout: ${TIMEOUT}s per test${C_RESET}"
echo -e "${C_DIM}Tests:   $TOTAL${C_RESET}"
echo ""

IDX=0
for TEST_FILE in "${TEST_FILES[@]}"; do
    IDX=$((IDX + 1))
    REL_PATH="${TEST_FILE#"$SCRIPT_DIR/"}"
    TEST_NAME="$(basename "$TEST_FILE" .idl)"

    # ── Run with timeout, capture stdout+stderr and exit code ───────────────
    TMPOUT=$(mktemp)
    trap "rm -f '$TMPOUT'" EXIT

    EXIT_CODE=0
    timeout --signal=KILL "${TIMEOUT}s" "$IDYL" "$TEST_FILE" > "$TMPOUT" 2>&1 || EXIT_CODE=$?
    wait 2>/dev/null  # suppress bash "Killed" messages

    OUTPUT="$(cat "$TMPOUT")"
    rm -f "$TMPOUT"

    # ── Parse idyl output ───────────────────────────────────────────────────
    ERRORS=0
    WARNINGS=0
    INFOS=0
    RUNTIME_WARNS=0
    HAS_SEMANTIC=0

    # Extract error/warning/info counts from "N error(s), N warning(s), N info(s)."
    if SUMMARY_LINE=$(echo "$OUTPUT" | grep -oP '\d+ error\(s\), \d+ warning\(s\), \d+ info\(s\)' | head -1); then
        if [[ -n "$SUMMARY_LINE" ]]; then
            HAS_SEMANTIC=1
            ERRORS=$(echo "$SUMMARY_LINE" | grep -oP '^\d+')
            WARNINGS=$(echo "$SUMMARY_LINE" | grep -oP '\d+(?= warning)')
            INFOS=$(echo "$SUMMARY_LINE" | grep -oP '\d+(?= info)')
        fi
    fi

    # Check for "No semantic issues found."
    if echo "$OUTPUT" | grep -q "No semantic issues found"; then
        HAS_SEMANTIC=1
    fi

    # Count runtime warnings
    RUNTIME_WARNS=$(echo "$OUTPUT" | grep -c "runtime warning:" || true)

    # ── Determine result ────────────────────────────────────────────────────
    STATUS=""
    DETAIL=""

    if [[ $EXIT_CODE -eq 137 || $EXIT_CODE -eq 124 ]]; then
        # Killed by timeout (SIGKILL=137, timeout exit=124)
        # Temporal tests without dur= are expected to be killed — that's OK
        # as long as semantic analysis passed
        if [[ $HAS_SEMANTIC -eq 1 && $ERRORS -eq 0 ]]; then
            STATUS="PASS"
            DETAIL="timed out (expected for temporal test)"
        else
            STATUS="TIMEOUT"
            DETAIL="killed after ${TIMEOUT}s — no clean semantic output"
            TIMED_OUT=$((TIMED_OUT + 1))
        fi
    elif [[ $EXIT_CODE -ne 0 ]]; then
        STATUS="CRASH"
        DETAIL="exit code $EXIT_CODE"
        # Grab last few lines for context
        TAIL=$(echo "$OUTPUT" | tail -5)
        if [[ -n "$TAIL" ]]; then
            DETAIL="$DETAIL\n$(echo "$TAIL" | sed 's/^/         │ /')"
        fi
        CRASHED=$((CRASHED + 1))
    elif [[ $ERRORS -gt 0 ]]; then
        STATUS="FAIL"
        # Extract semantic error lines
        ERR_LINES=$(echo "$OUTPUT" | grep -E "Semantic Error:" | head -5)
        DETAIL="$ERRORS semantic error(s)"
        if [[ -n "$ERR_LINES" ]]; then
            DETAIL="$DETAIL\n$(echo "$ERR_LINES" | sed 's/^/         │ /')"
        fi
    elif [[ $RUNTIME_WARNS -gt 0 ]]; then
        STATUS="WARN"
        UNIQUE_WARNS=$(echo "$OUTPUT" | grep "runtime warning:" | sort -u | head -5)
        DETAIL="$RUNTIME_WARNS runtime warning(s)"
        if [[ -n "$UNIQUE_WARNS" ]]; then
            DETAIL="$DETAIL\n$(echo "$UNIQUE_WARNS" | sed 's/^/         │ /')"
        fi
        WARNED=$((WARNED + 1))
    elif [[ $HAS_SEMANTIC -eq 0 ]]; then
        # No semantic output at all — might be a parse error
        STATUS="FAIL"
        TAIL=$(echo "$OUTPUT" | tail -5)
        DETAIL="no semantic analysis output (parse error?)"
        if [[ -n "$TAIL" ]]; then
            DETAIL="$DETAIL\n$(echo "$TAIL" | sed 's/^/         │ /')"
        fi
    else
        STATUS="PASS"
        if [[ $WARNINGS -gt 0 ]]; then
            DETAIL="$WARNINGS semantic warning(s), $INFOS info(s)"
        fi
    fi

    # ── Print result ────────────────────────────────────────────────────────
    case "$STATUS" in
        PASS)
            PASSED=$((PASSED + 1))
            ICON="${C_PASS}✓${C_RESET}"
            ;;
        WARN)
            PASSED=$((PASSED + 1))  # warnings are soft-pass
            ICON="${C_WARN}⚠${C_RESET}"
            ;;
        FAIL)
            FAILED=$((FAILED + 1))
            FAIL_NAMES+=("$REL_PATH")
            FAIL_DETAILS+=("$DETAIL")
            ICON="${C_FAIL}✗${C_RESET}"
            ;;
        CRASH)
            FAILED=$((FAILED + 1))
            FAIL_NAMES+=("$REL_PATH")
            FAIL_DETAILS+=("$DETAIL")
            ICON="${C_FAIL}💥${C_RESET}"
            ;;
        TIMEOUT)
            FAILED=$((FAILED + 1))
            FAIL_NAMES+=("$REL_PATH")
            FAIL_DETAILS+=("$DETAIL")
            ICON="${C_FAIL}⏱${C_RESET}"
            ;;
    esac

    printf " %b  [%2d/%d] %-40s" "$ICON" "$IDX" "$TOTAL" "$REL_PATH"
    if [[ -n "$DETAIL" ]]; then
        printf " ${C_DIM}%s${C_RESET}" "$(echo -e "$DETAIL" | head -1)"
    fi
    echo ""

    # Print additional detail lines (indented)
    if [[ -n "$DETAIL" ]]; then
        EXTRA=$(echo -e "$DETAIL" | tail -n +2)
        if [[ -n "$EXTRA" ]]; then
            echo -e "$EXTRA"
        fi
    fi

    # Verbose: show full output
    if [[ $VERBOSE -eq 1 && -n "$OUTPUT" ]]; then
        echo -e "${C_DIM}── output ──────────────────────────────────────────${C_RESET}"
        echo "$OUTPUT" | head -30
        LINES=$(echo "$OUTPUT" | wc -l)
        if [[ $LINES -gt 30 ]]; then
            echo -e "${C_DIM}  ... ($((LINES - 30)) more lines)${C_RESET}"
        fi
        echo -e "${C_DIM}────────────────────────────────────────────────────${C_RESET}"
    fi
done

# ── Summary ─────────────────────────────────────────────────────────────────────
echo ""
echo -e "${C_BOLD}══════════════════════════════════════════════════════${C_RESET}"
echo -e "${C_BOLD}  Results: $TOTAL tests${C_RESET}"
echo ""
echo -e "  ${C_PASS}Passed:${C_RESET}    $PASSED"
if [[ $WARNED -gt 0 ]]; then
    echo -e "  ${C_WARN}Warnings:${C_RESET}  $WARNED  ${C_DIM}(runtime warnings — soft pass)${C_RESET}"
fi
if [[ $FAILED -gt 0 ]]; then
    echo -e "  ${C_FAIL}Failed:${C_RESET}    $FAILED"
    if [[ $CRASHED -gt 0 ]]; then
        echo -e "    ${C_DIM}├─ Crashed:    $CRASHED${C_RESET}"
    fi
    if [[ $TIMED_OUT -gt 0 ]]; then
        echo -e "    ${C_DIM}└─ Timed out:  $TIMED_OUT${C_RESET}"
    fi
fi
echo ""

# ── Failed test details ────────────────────────────────────────────────────────
if [[ $FAILED -gt 0 ]]; then
    echo -e "${C_FAIL}${C_BOLD}  Failed tests:${C_RESET}"
    for i in "${!FAIL_NAMES[@]}"; do
        echo -e "    ${C_FAIL}✗${C_RESET} ${FAIL_NAMES[$i]}"
        EXTRA=$(echo -e "${FAIL_DETAILS[$i]}" | head -6)
        if [[ -n "$EXTRA" ]]; then
            echo -e "$(echo "$EXTRA" | sed 's/^/      /')"
        fi
    done
    echo ""
fi

echo -e "${C_BOLD}══════════════════════════════════════════════════════${C_RESET}"

if [[ $FAILED -gt 0 ]]; then
    exit 1
else
    echo -e "${C_PASS}${C_BOLD}  All tests passed.${C_RESET}"
    echo ""
    exit 0
fi
