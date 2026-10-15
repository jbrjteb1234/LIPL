#!/usr/bin/env bash
# Runs every tests/cases/*.lipl file through the LIPL binary and compares its
# stdout, stderr and exit code with the matching .expected file.
#
#   tests/run_tests.sh [path/to/program]            run the suite (default: src/program)
#   tests/run_tests.sh --update [path/to/program]   rewrite every .expected file from current output
#
# The .expected files are the spec: read the diff of anything --update changes.
set -u

update=false
if [ "${1:-}" = "--update" ]; then
    update=true
    shift
fi

tests_dir="$(cd "$(dirname "$0")" && pwd)"
program="${1:-$tests_dir/../src/program}"
program="$(cd "$(dirname "$program")" && pwd)/$(basename "$program")"

if [ ! -x "$program" ] && [ ! -x "$program.exe" ]; then
    echo "no program at $program - build it first (cd src && make)" >&2
    exit 2
fi

stderr_file="$(mktemp)"
trap 'rm -f "$stderr_file"' EXIT

passed=0
failed=0
for source in "$tests_dir"/cases/*.lipl; do
    name="$(basename "$source" .lipl)"
    expected_file="$tests_dir/cases/$name.expected"

    # run from inside cases/ so error messages say "name.lipl:line:col"
    stdout="$(cd "$tests_dir/cases" && "$program" "$name.lipl" 2>"$stderr_file")"
    code=$?
    actual="$(printf '%s\n--- stderr ---\n%s\n--- exit: %d ---' "$stdout" "$(cat "$stderr_file")" "$code" | tr -d '\r')"

    if $update; then
        printf '%s\n' "$actual" > "$expected_file"
        continue
    fi

    if [ ! -f "$expected_file" ]; then
        echo "MISSING  $name (no $name.expected - run with --update to create it)"
        failed=$((failed + 1))
    elif [ "$actual" = "$(tr -d '\r' < "$expected_file")" ]; then
        passed=$((passed + 1))
    else
        echo "FAIL     $name"
        diff <(tr -d '\r' < "$expected_file") <(printf '%s\n' "$actual") | sed 's/^/         /'
        failed=$((failed + 1))
    fi
done

if $update; then
    echo "rewrote the .expected files in $tests_dir/cases"
    exit 0
fi

echo "$passed passed, $failed failed"
[ "$failed" -eq 0 ]
