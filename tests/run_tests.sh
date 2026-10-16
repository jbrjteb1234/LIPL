#!/usr/bin/env bash
# Runs every test program through the LIPL binary and compares its stdout,
# stderr and exit code with the matching .expected file.
#
#   tests/parser/*.lipl        run with --ast: checks the syntax tree and syntax errors
#   tests/interpreter/*.lipl   run normally: checks program output and runtime errors
#                              (name.stdin, if present, is fed to the program's input)
#   examples/*.lipl            run normally: the showcase programs, whose .expected
#                              and .stdin files live in tests/examples
#   plus a few checks of the command line itself (--help, --version, bad options)
#
#   tests/run_tests.sh [path/to/lipl]            run the suite (default: src/lipl)
#   tests/run_tests.sh --update [path/to/lipl]   rewrite every .expected file from current output
#
# The .expected files are the spec: read the diff of anything --update changes.
set -u

update=false
if [ "${1:-}" = "--update" ]; then
    update=true
    shift
fi

tests_dir="$(cd "$(dirname "$0")" && pwd)"
examples_dir="$(cd "$tests_dir/../examples" && pwd)"
program="${1:-$tests_dir/../src/lipl}"
program="$(cd "$(dirname "$program")" && pwd)/$(basename "$program")"

if [ ! -x "$program" ] && [ ! -x "$program.exe" ]; then
    echo "no LIPL binary at $program - build it first (cd src && make)" >&2
    exit 2
fi

stderr_file="$(mktemp)"
trap 'rm -f "$stderr_file"' EXIT

passed=0
failed=0

# run_suite <name> <directory of .lipl programs> <directory of .expected and .stdin files> [flags...]
run_suite() {
    local suite="$1" source_dir="$2" expected_dir="$3"
    shift 3

    local source
    for source in "$source_dir"/*.lipl; do
        [ -e "$source" ] || continue
        local name expected_file input stdout code actual
        name="$(basename "$source" .lipl)"
        expected_file="$expected_dir/$name.expected"
        input="$expected_dir/$name.stdin"
        [ -f "$input" ] || input=/dev/null

        # run from inside the program's directory so error messages say "name.lipl:line:col"
        # and programs can open files next to them
        stdout="$(cd "$source_dir" && "$program" "$@" "$name.lipl" < "$input" 2>"$stderr_file")"
        code=$?
        actual="$(printf '%s\n--- stderr ---\n%s\n--- exit: %d ---' "$stdout" "$(cat "$stderr_file")" "$code" | tr -d '\r')"

        if $update; then
            printf '%s\n' "$actual" > "$expected_file"
            continue
        fi

        if [ ! -f "$expected_file" ]; then
            echo "MISSING  $suite/$name (no $name.expected - run with --update to create it)"
            failed=$((failed + 1))
        elif [ "$actual" = "$(tr -d '\r' < "$expected_file")" ]; then
            passed=$((passed + 1))
        else
            echo "FAIL     $suite/$name"
            diff <(tr -d '\r' < "$expected_file") <(printf '%s\n' "$actual") | sed 's/^/         /'
            failed=$((failed + 1))
        fi
    done
}

run_suite parser        "$tests_dir/parser"         "$tests_dir/parser"         --ast
run_suite interpreter   "$tests_dir/interpreter"    "$tests_dir/interpreter"
run_suite examples      "$examples_dir"             "$tests_dir/examples"

# the command line itself: test name, expected exit code, expected first line of stdout, arguments
check_cli() {
    local name="$1" expected_code="$2" expected_line="$3"
    shift 3
    local stdout code
    stdout="$("$program" "$@" 2>/dev/null)"
    code=$?
    stdout="$(printf '%s\n' "$stdout" | head -n 1 | tr -d '\r')"
    if [ "$code" = "$expected_code" ] && [ "$stdout" = "$expected_line" ]; then
        passed=$((passed + 1))
    else
        echo "FAIL     cli/$name"
        echo "         expected exit $expected_code and \"$expected_line\", got exit $code and \"$stdout\""
        failed=$((failed + 1))
    fi
}

if $update; then
    echo "rewrote the .expected files in tests/parser, tests/interpreter and tests/examples"
    exit 0
fi

check_cli version           0 "lipl 1.0.0" --version
check_cli help              0 "usage: lipl [--tokens] [--ast] <source-file>" --help
check_cli unknown_option    2 "" --bogus
check_cli no_source_file    2 ""

echo "$passed passed, $failed failed"
[ "$failed" -eq 0 ]
