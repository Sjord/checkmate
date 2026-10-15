#!/bin/sh
#
# run-tests.sh - test suite for mpck.
#
# The suite generates a set of valid MP2/MP3 files (see crctest.sh), derives
# known broken files from them and checks that mpck reports the expected
# result and error for each of them.
#
# Usage: run-tests.sh [--rebuild]
#
#   --rebuild   throw away cached test files and generate them again
#
# Environment:
#   MPCK        path to the mpck binary (defaults to ../mpck)
#   TESTDATA    directory to store generated files in
#               (default: <script directory>/data)
#
# Exit status: 0 when everything passed, 1 on failure, 77 when a required
# program is not available.

set -u

testdir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
TESTDATA=${TESTDATA:-$testdir/data}
MPCK=${MPCK:-}

usage() {
	echo "Usage: ${0##*/} [--rebuild]"
	echo
	echo "  --rebuild   regenerate the cached test files"
}

rebuild=0
for arg in "$@"; do
	case $arg in
		--rebuild) rebuild=1 ;;
		--help|-h) usage; exit 0 ;;
		*) echo "${0##*/}: unknown option: $arg" >&2; usage >&2; exit 2 ;;
	esac
done

# Locate the program under test.
if [ -z "$MPCK" ]; then
	for candidate in "$testdir/../mpck" ./mpck; do
		if [ -x "$candidate" ]; then
			MPCK=$candidate
			break
		fi
	done
fi
if [ -z "$MPCK" ] || [ ! -x "$MPCK" ]; then
	echo "${0##*/}: mpck binary not found; build it first or set MPCK" >&2
	exit 1
fi
MPCK=$(CDPATH= cd -- "$(dirname "$MPCK")" && pwd)/$(basename "$MPCK")

# The test files are produced by these programs.
for tool in lame twolame sox; do
	if ! command -v "$tool" >/dev/null 2>&1; then
		echo "SKIP: ${0##*/}: required program '$tool' not found"
		exit 77
	fi
done

. "$testdir/testlib.sh"

if [ "$rebuild" = 1 ]; then
	rm -rf "$TESTDATA"
fi

# Generate the valid files if they are not cached yet.  crctest.sh itself
# detects individually missing files, so a partial cache is repaired.
if [ ! -s "$TESTDATA/good/mp3_44100_128_c2.mp3" ]; then
	TESTDATA=$TESTDATA "$testdir/crctest.sh" || exit $?
fi

GOOD=$TESTDATA/good
BAD=$TESTDATA/bad

# Derive known broken files from the generated good ones.
#
# The reference MP3 is 128 kbps, 44100 Hz, stereo MPEG-1 Layer III, so its
# frames are 417 bytes long.  The first frame header starts at offset 0 and
# the second one at offset 417.
make_bad_files() {
	ref_mp3=$GOOD/mp3_44100_128_c2.mp3
	ref_mp2=$GOOD/mp2_44100_128_c2.mp2
	ref_crc=$GOOD/mp3_44100_128_c2_crc.mp3

	for ref in "$ref_mp3" "$ref_mp2" "$ref_crc"; do
		if [ ! -s "$ref" ]; then
			echo "${0##*/}: missing reference file $ref" >&2
			exit 1
		fi
	done

	mkdir -p "$BAD"

	: > "$BAD/empty.mp3"
	printf 'This is not an MP3 file, just some text.\n' > "$BAD/text.mp3"
	head -c 1000 "$ref_mp3" > "$BAD/truncated.mp3"

	# Replace the first frame header with an invalid but syncing header.
	cp "$ref_mp3" "$BAD/invalid_header.mp3"
	printf '\377\340\000\000' | dd of="$BAD/invalid_header.mp3" \
		bs=1 seek=0 conv=notrunc 2>/dev/null

	# Destroy the second frame header, leaving 417 unidentified bytes.
	cp "$ref_mp3" "$BAD/unidentified_bytes.mp3"
	dd if=/dev/zero of="$BAD/unidentified_bytes.mp3" \
		bs=1 count=4 seek=417 conv=notrunc 2>/dev/null

	# A Layer II file followed by a Layer III file.
	cat "$ref_mp2" "$ref_mp3" > "$BAD/inconsistent.mp3"

	# Corrupt the CRC of a protected frame.
	cp "$ref_crc" "$BAD/crc_error.mp3"
	printf '\000' | dd of="$BAD/crc_error.mp3" bs=1 seek=4 conv=notrunc 2>/dev/null
}

make_bad_files

echo "mpck test suite"
echo "  mpck:     $MPCK"
echo "  testdata: $TESTDATA"
echo

# --- Valid files -----------------------------------------------------------
# Every generated, encoder-produced file must be reported as Ok.
for file in "$GOOD"/*.mp3 "$GOOD"/*.mp2; do
	[ -e "$file" ] || continue
	expect_result "valid $(basename "$file")" "$file" "Ok" 0
done

# A file with ID3v1/ID3v2 tags is still a valid MP3.
expect_result "valid tagged mp3" "$GOOD/mp3_44100_128_c2_id3.mp3" "Ok" 0

# --- Broken files ----------------------------------------------------------
# An empty or non-MP3 file is not reported as bad (there is nothing to check),
# but as "No MP3".
expect_result "empty file" "$BAD/empty.mp3" "No MP3" 0
expect_result "text file" "$BAD/text.mp3" "No MP3" 0
expect_result "truncated file" "$BAD/truncated.mp3" "No MP3" 0

# These files contain identifiable MPEG audio with a defect.
expect_result "invalid frame header" "$BAD/invalid_header.mp3" "Bad" 1
expect_result "unidentified bytes" "$BAD/unidentified_bytes.mp3" "Bad" 1
expect_result "inconsistent frames" "$BAD/inconsistent.mp3" "Bad" 1
expect_result "crc error" "$BAD/crc_error.mp3" "Bad" 1

# The error descriptions should be present in the full output.
expect_contains "empty error description" "empty file" "$BAD/empty.mp3"
expect_contains "invalid header description" "invalid header values" "$BAD/invalid_header.mp3"
expect_contains "unidentified bytes description" "unidentified bytes" "$BAD/unidentified_bytes.mp3"
expect_contains "inconsistent frames description" "inconsistent frame headers" "$BAD/inconsistent.mp3"
expect_contains "crc error description" "CRC error" "$BAD/crc_error.mp3"

# --- Options ---------------------------------------------------------------
expect_status "version" 0 --version
expect_status "help" 0 --help
expect_status "no filename" 22
expect_status "missing file" 2 "$BAD/does-not-exist.mp3"

expect_contains "verbose shows version" "MPEG v1.0" -v "$GOOD/mp3_44100_128_c2.mp3"
expect_contains "verbose shows ID3" "ID3V2" -v "$GOOD/mp3_44100_128_c2_id3.mp3"
expect_contains "xml reports ok" "<ok />" -x "$GOOD/mp3_44100_128_c2.mp3"
expect_contains "xml reports error" "<code>INV</code>" -x "$BAD/invalid_header.mp3"

# badonly only reports the bad file.
expect_contains "badonly bad file" "Bad" -B "$GOOD/mp3_44100_128_c2.mp3" "$BAD/invalid_header.mp3"
expect_not_contains "badonly hides good file" "$GOOD/mp3_44100_128_c2.mp3: Ok" -B "$GOOD/mp3_44100_128_c2.mp3" "$BAD/invalid_header.mp3"

test_finish
