#!/bin/sh
#
# testlib.sh - a tiny test framework for the mpck test suite.
#
# Source this file from a test script, set MPCK to the program under test,
# call the expect_* functions and finish with test_finish.
#
# The framework deliberately only uses POSIX shell tools, so that it runs
# everywhere mpck is built.

tests_run=0
tests_failed=0
mpck_output=
mpck_status=0

# Run the program under test, capturing stdout and stderr together.
mpck_run() {
	mpck_output=$("$MPCK" "$@" 2>&1)
	mpck_status=$?
}

_test_pass() {
	tests_run=$((tests_run + 1))
	printf 'PASS: %s\n' "$1"
}

_test_fail() {
	tests_run=$((tests_run + 1))
	tests_failed=$((tests_failed + 1))
	printf 'FAIL: %s\n' "$1"
	if [ -n "${2:-}" ]; then
		printf '      %s\n' "$2"
	fi
	if [ -n "$mpck_output" ]; then
		printf '%s\n' "$mpck_output" | sed 's/^/      | /'
	fi
}

# expect_result NAME FILE RESULT [EXITSTATUS]
#
# Check that "mpck -q FILE" prints exactly "mpck:FILE: RESULT", where RESULT
# is "Ok", "Bad" or "No MP3", and exits with EXITSTATUS (default 0; bad MP3
# files exit with 1).
expect_result() {
	_name=$1
	_file=$2
	_want=$3
	_want_status=${4:-0}
	mpck_run -q "$_file"
	_expected="mpck:$_file: $_want"
	if [ "$mpck_status" -eq "$_want_status" ] && [ "$mpck_output" = "$_expected" ]; then
		_test_pass "$_name"
	else
		_test_fail "$_name" "expected exit $_want_status and '$_expected'; got exit $mpck_status and '$mpck_output'"
	fi
}

# expect_contains NAME NEEDLE [MPCK ARGS...]
#
# Check that the combined output of mpck contains NEEDLE.
expect_contains() {
	_name=$1
	_needle=$2
	shift 2
	mpck_run "$@"
	if printf '%s\n' "$mpck_output" | grep -qF "$_needle"; then
		_test_pass "$_name"
	else
		_test_fail "$_name" "output does not contain '$_needle'"
	fi
}

# expect_not_contains NAME NEEDLE [MPCK ARGS...]
expect_not_contains() {
	_name=$1
	_needle=$2
	shift 2
	mpck_run "$@"
	if printf '%s\n' "$mpck_output" | grep -qF "$_needle"; then
		_test_fail "$_name" "output unexpectedly contains '$_needle'"
	else
		_test_pass "$_name"
	fi
}

# expect_status NAME STATUS [MPCK ARGS...]
expect_status() {
	_name=$1
	_want_status=$2
	shift 2
	mpck_run "$@"
	if [ "$mpck_status" -eq "$_want_status" ]; then
		_test_pass "$_name"
	else
		_test_fail "$_name" "expected exit $_want_status, got $mpck_status"
	fi
}

# Print a summary and return non-zero if any test failed.
test_finish() {
	printf '\n%s: %d tests, %d failed\n' "${0##*/}" "$tests_run" "$tests_failed"
	if [ "$tests_failed" -ne 0 ]; then
		return 1
	fi
	return 0
}
