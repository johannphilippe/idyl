#!/usr/bin/env bash
# ════════════════════════════════════════════════════════════════════════════════
#  Idyl — Static test runner
#
#  Runs tests/static/ — programs with no temporal instances.
#  Every test must exit 0 and produce at least one line of output.
#  Files named expected_fail_*.idyl must produce a non-zero exit code.
#
#  Usage:
#    ./tests/run_static.sh               # run all static tests
#    ./tests/run_static.sh -v            # verbose: show output for every test
#    ./tests/run_static.sh -f flow       # filter: only tests matching pattern
#
#  Exit code:
#    0  — all tests passed
#    1  — one or more tests failed
# ════════════════════════════════════════════════════════════════════════════════
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
IDYL="$PROJECT_ROOT/build/idyl"
SUITE_DIR="$SCRIPT_DIR/static"

VERBOSE=0
FILTER=""

if [[ -t 1 ]]; then
    C_RESET="\033[0m"; C_PASS="\033[32m"; C_FAIL="\033[31m"
    C_WARN="\033[33m"; C_INFO="\033[36m"; C_BOLD="\033[1m"; C_DIM="\033[2m"
else
    C_RESET=""; C_PASS=""; C_FAIL=""; C_WARN=""; C_INFO=""; C_BOLD=""; C_DIM=""
fi

while [[ $# -gt 0 ]]; do
    case "$1" in
        -v|--verbose) VERBOSE=1; shift ;;
        -f|--filter)  FILTER="$2"; shift 2 ;;
        -h|--help) sed -n '2,12p' "$0" | sed 's/^#  \?//'; exit 0 ;;
        *) echo "Unknown option: $1"; exit 1 ;;
    esac
done

if [[ ! -x "$IDYL" ]]; then
    echo -e "${C_FAIL}ERROR:${C_RESET} idyl binary not found at $IDYL — run: cmake --build build"
    exit 1
fi

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
    echo -e "${C_WARN}No static tests found${C_RESET} (filter: '$FILTER')"
    exit 0
fi

PASSED=0; FAILED=0
declare -a FAIL_NAMES=(); declare -a FAIL_DETAILS=()

echo ""
echo -e "${C_BOLD}Idyl — Static tests${C_RESET}"
echo -e "${C_DIM}Suite:   $SUITE_DIR${C_RESET}"
echo -e "${C_DIM}Tests:   $TOTAL${C_RESET}"
echo ""

IDX=0
for TEST_FILE in "${ALL_FILES[@]}"; do
    IDX=$((IDX + 1))
    REL="$(basename "$TEST_FILE")"
    NAME="$(basename "$TEST_FILE" .idyl)"

    # expected_fail_*.idyl files must produce a non-zero exit
    EXPECT_FAIL=0
    [[ "$REL" == expected_fail_* ]] && EXPECT_FAIL=1

    TMPOUT=$(mktemp)
    EXIT_CODE=0
    # Safety net timeout of 5s — static tests should complete in milliseconds
    timeout --signal=KILL 5s "$IDYL" "$TEST_FILE" > "$TMPOUT" 2>&1 || EXIT_CODE=$?
    OUTPUT="$(cat "$TMPOUT")"; rm -f "$TMPOUT"

    STATUS=""; DETAIL=""

    if [[ $EXPECT_FAIL -eq 1 ]]; then
        if [[ $EXIT_CODE -ne 0 && $EXIT_CODE -ne 124 && $EXIT_CODE -ne 137 ]]; then
            STATUS="PASS"; DETAIL="correctly rejected"
        else
            STATUS="FAIL"; DETAIL="expected-fail did not produce an error (exit $EXIT_CODE)"
        fi
    else
        if [[ $EXIT_CODE -eq 124 || $EXIT_CODE -eq 137 ]]; then
            STATUS="FAIL"; DETAIL="timed out — static test must exit on its own"
        elif [[ $EXIT_CODE -ne 0 ]]; then
            STATUS="FAIL"
            TAIL=$(echo "$OUTPUT" | tail -3)
            DETAIL="exit code $EXIT_CODE"
            [[ -n "$TAIL" ]] && DETAIL="$DETAIL\n$(echo "$TAIL" | sed 's/^/      │ /')"
        else
            # Check semantic analysis passed
            if echo "$OUTPUT" | grep -qE '[1-9][0-9]* error'; then
                ERR=$(echo "$OUTPUT" | grep -oP '\d+ error' | head -1)
                STATUS="FAIL"; DETAIL="$ERR(s) in semantic analysis"
            elif ! echo "$OUTPUT" | grep -qE 'Semantic|process'; then
                STATUS="FAIL"; DETAIL="no output produced"
            else
                OUTPUT_LINES=$(echo "$OUTPUT" | grep -v 'Semantic\|error\|warning\|info' | grep -c . || true)
                if [[ $OUTPUT_LINES -eq 0 ]]; then
                    STATUS="FAIL"; DETAIL="no print output (did the process block run?)"
                else
                    STATUS="PASS"
                fi
            fi
        fi
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
echo -e "${C_BOLD}  Static tests: $TOTAL${C_RESET}  ·  ${C_PASS}passed: $PASSED${C_RESET}  ·  ${C_FAIL}failed: $FAILED${C_RESET}"
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

echo -e "${C_PASS}${C_BOLD}  All static tests passed.${C_RESET}"
echo ""
echo -e "${C_BOLD}══════════════════════════════════════════════════════${C_RESET}"
exit 0
