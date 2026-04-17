#!/usr/bin/env bash
# ════════════════════════════════════════════════════════════════════════════════
#  Idyl — Temporal test runner
#
#  Runs tests/temporal/ — programs that use the scheduler (metro, lfo, clocks…).
#  These programs run forever; the script kills them after a timeout.
#  Pass criteria: semantic analysis succeeds, the program produces output,
#  and it does not crash before the timeout.
#
#  Tests with external dependencies (Csound, OSC receiver) are skipped by default.
#
#  Usage:
#    ./tests/run_temporal.sh               # run all temporal tests (skip ext deps)
#    ./tests/run_temporal.sh --all         # include Csound and OSC tests
#    ./tests/run_temporal.sh -t 5          # custom timeout per test (default: 3s)
#    ./tests/run_temporal.sh -v            # verbose: show output for every test
#    ./tests/run_temporal.sh -f trig       # filter: only tests matching pattern
#
#  Exit code:
#    0  — all tests passed
#    1  — one or more tests failed
# ════════════════════════════════════════════════════════════════════════════════
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
IDYL="$PROJECT_ROOT/build/idyl"
SUITE_DIR="$SCRIPT_DIR/temporal"

TIMEOUT=3       # seconds — just enough to see output and confirm no crash
VERBOSE=0
FILTER=""
INCLUDE_EXTERNAL=0

if [[ -t 1 ]]; then
    C_RESET="\033[0m"; C_PASS="\033[32m"; C_FAIL="\033[31m"
    C_WARN="\033[33m"; C_INFO="\033[36m"; C_BOLD="\033[1m"; C_DIM="\033[2m"
else
    C_RESET=""; C_PASS=""; C_FAIL=""; C_WARN=""; C_INFO=""; C_BOLD=""; C_DIM=""
fi

while [[ $# -gt 0 ]]; do
    case "$1" in
        -v|--verbose)  VERBOSE=1; shift ;;
        -t|--timeout)  TIMEOUT="$2"; shift 2 ;;
        -f|--filter)   FILTER="$2"; shift 2 ;;
        --all)         INCLUDE_EXTERNAL=1; shift ;;
        -h|--help) sed -n '2,15p' "$0" | sed 's/^#  \?//'; exit 0 ;;
        *) echo "Unknown option: $1"; exit 1 ;;
    esac
done

if [[ ! -x "$IDYL" ]]; then
    echo -e "${C_FAIL}ERROR:${C_RESET} idyl binary not found at $IDYL — run: cmake --build build"
    exit 1
fi

# Tests that require external processes to be running first.
# Skipped unless --all is passed.
EXTERNAL_DEPS=(
    "csound_module"
    "csound_module_clock"
    "csound_osc_schedule"
    "csound_osc_test"
    "eval_08_osc"
    "hot_reload"      # requires OSC to send new source code during run
)

needs_external() {
    local name="$1"
    for dep in "${EXTERNAL_DEPS[@]}"; do
        [[ "$name" == "$dep" ]] && return 0
    done
    return 1
}

mapfile -t ALL_FILES < <(find "$SUITE_DIR" -name '*.idyl' | sort)

if [[ -n "$FILTER" ]]; then
    FILTERED=()
    for f in "${ALL_FILES[@]}"; do
        [[ "$(basename "$f")" == *"$FILTER"* ]] && FILTERED+=("$f")
    done
    ALL_FILES=("${FILTERED[@]}")
fi

TOTAL=${#ALL_FILES[@]}
if [[ $TOTAL -eq 0 ]]; then
    echo -e "${C_WARN}No temporal tests found${C_RESET} (filter: '$FILTER')"
    exit 0
fi

PASSED=0; FAILED=0; SKIPPED=0
declare -a FAIL_NAMES=(); declare -a FAIL_DETAILS=()

echo ""
echo -e "${C_BOLD}Idyl — Temporal tests${C_RESET}"
echo -e "${C_DIM}Suite:   $SUITE_DIR${C_RESET}"
echo -e "${C_DIM}Timeout: ${TIMEOUT}s per test${C_RESET}"
echo -e "${C_DIM}Tests:   $TOTAL$([ $INCLUDE_EXTERNAL -eq 0 ] && echo " (external-dep tests skipped — use --all to include)")${C_RESET}"
echo ""

IDX=0
for TEST_FILE in "${ALL_FILES[@]}"; do
    IDX=$((IDX + 1))
    REL="$(basename "$TEST_FILE")"
    NAME="$(basename "$TEST_FILE" .idyl)"

    # Skip external-dep tests unless --all
    if [[ $INCLUDE_EXTERNAL -eq 0 ]] && needs_external "$NAME"; then
        SKIPPED=$((SKIPPED + 1))
        printf "    [%2d/%d] %-42s" "$IDX" "$TOTAL" "$REL"
        echo -e " ${C_DIM}skipped (external dep — use --all)${C_RESET}"
        continue
    fi

    TMPOUT=$(mktemp)
    EXIT_CODE=0
    timeout --signal=KILL "${TIMEOUT}s" "$IDYL" "$TEST_FILE" > "$TMPOUT" 2>&1 || EXIT_CODE=$?
    OUTPUT="$(cat "$TMPOUT")"; rm -f "$TMPOUT"

    STATUS=""; DETAIL=""

    # Determine whether semantic analysis passed
    ERRORS=0
    if SUMLINE=$(echo "$OUTPUT" | grep -oP '\d+ error\(s\)' | head -1); then
        ERRORS=$(echo "$SUMLINE" | grep -oP '^\d+' || true)
        [[ -z "$ERRORS" ]] && ERRORS=0
    fi
    HAS_SEMANTIC=0
    echo "$OUTPUT" | grep -qE 'Semantic Analysis|No semantic issues' && HAS_SEMANTIC=1

    # Runtime output: any non-diagnostic line
    RUNTIME_OUTPUT=$(echo "$OUTPUT" | grep -v 'Semantic\|error\|warning\|info\|^$' | wc -l || true)

    if [[ $EXIT_CODE -eq 137 || $EXIT_CODE -eq 124 ]]; then
        # Killed by timeout — expected for non-dur temporal tests
        if [[ $HAS_SEMANTIC -eq 1 && $ERRORS -eq 0 && $RUNTIME_OUTPUT -gt 0 ]]; then
            STATUS="PASS"; DETAIL="ran ${TIMEOUT}s, produced output"
        elif [[ $ERRORS -gt 0 ]]; then
            STATUS="FAIL"; DETAIL="semantic error(s) before timeout"
        elif [[ $RUNTIME_OUTPUT -eq 0 ]]; then
            STATUS="FAIL"; DETAIL="timed out with no output produced"
        else
            STATUS="FAIL"; DETAIL="timed out — no semantic analysis output"
        fi
    elif [[ $EXIT_CODE -eq 0 ]]; then
        # Clean exit — valid for tests with dur= or that complete quickly
        if [[ $ERRORS -gt 0 ]]; then
            STATUS="FAIL"; DETAIL="semantic error(s)"
        elif [[ $RUNTIME_OUTPUT -eq 0 ]]; then
            STATUS="FAIL"; DETAIL="no output produced"
        else
            STATUS="PASS"; DETAIL="exited cleanly"
        fi
    else
        STATUS="FAIL"
        TAIL=$(echo "$OUTPUT" | tail -3)
        DETAIL="crash (exit $EXIT_CODE)"
        [[ -n "$TAIL" ]] && DETAIL="$DETAIL\n$(echo "$TAIL" | sed 's/^/      │ /')"
    fi

    if [[ "$STATUS" == "PASS" ]]; then
        PASSED=$((PASSED + 1)); ICON="${C_PASS}✓${C_RESET}"
    else
        FAILED=$((FAILED + 1)); ICON="${C_FAIL}✗${C_RESET}"
        FAIL_NAMES+=("$REL"); FAIL_DETAILS+=("$DETAIL")
    fi

    printf " %b  [%2d/%d] %-42s" "$ICON" "$IDX" "$TOTAL" "$REL"
    [[ -n "$DETAIL" ]] && printf " ${C_DIM}%s${C_RESET}" "$(echo -e "$DETAIL" | head -1)"
    echo ""
    [[ -n "$DETAIL" ]] && echo -e "$DETAIL" | tail -n +2

    if [[ $VERBOSE -eq 1 && -n "$OUTPUT" ]]; then
        echo -e "${C_DIM}── output ──────────────────────────────────────────${C_RESET}"
        echo "$OUTPUT" | head -20
        echo -e "${C_DIM}────────────────────────────────────────────────────${C_RESET}"
    fi
done

echo ""
echo -e "${C_BOLD}══════════════════════════════════════════════════════${C_RESET}"
echo -n -e "${C_BOLD}  Temporal tests: $TOTAL${C_RESET}"
echo -n -e "  ·  ${C_PASS}passed: $PASSED${C_RESET}"
echo -n -e "  ·  ${C_FAIL}failed: $FAILED${C_RESET}"
[[ $SKIPPED -gt 0 ]] && echo -n -e "  ·  ${C_DIM}skipped: $SKIPPED${C_RESET}"
echo ""
echo ""

if [[ $FAILED -gt 0 ]]; then
    echo -e "${C_FAIL}${C_BOLD}  Failed:${C_RESET}"
    for i in "${!FAIL_NAMES[@]}"; do
        echo -e "    ${C_FAIL}✗${C_RESET} ${FAIL_NAMES[$i]}"
        echo -e "${FAIL_DETAILS[$i]}" | head -4 | sed 's/^/      /'
    done
    echo ""
    echo -e "${C_BOLD}══════════════════════════════════════════════════════${C_RESET}"
    exit 1
fi

echo -e "${C_PASS}${C_BOLD}  All temporal tests passed.${C_RESET}"
echo ""
echo -e "${C_BOLD}══════════════════════════════════════════════════════${C_RESET}"
exit 0
