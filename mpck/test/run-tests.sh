#!/bin/sh
#
# run-tests.sh - test suite for mpck.
#
# The fixtures (valid and known-broken MP2/MP3 files) are produced by
# gentestdata.sh.  This script only runs mpck against them and checks the
# reported result and error for each file.
#
# Usage: run-tests.sh [--rebuild]
#
#   --rebuild   regenerate the cached test files first
#
# Environment:
#   MPCK        path to the mpck binary (defaults to ../mpck)
#   TESTDATA    directory holding the generated files
#               (default: <script directory>/data)
#
# Exit status: 0 when everything passed, 1 on failure, 77 when a required
# program is not available.

set -u

testdir=$(CDPATH='' cd -- "$(dirname "$0")" && pwd)
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
MPCK=$(CDPATH='' cd -- "$(dirname "$MPCK")" && pwd)/$(basename "$MPCK")

for tool in lame twolame sox; do
	if ! command -v "$tool" >/dev/null 2>&1; then
		echo "SKIP: ${0##*/}: required program '$tool' not found"
		exit 77
	fi
done

. "$testdir/testlib.sh"

# Make sure the fixtures exist.  gentestdata.sh repairs partial caches, so
# this is cheap when the data directory is already populated.
if [ "$rebuild" = 1 ]; then
	TESTDATA=$TESTDATA "$testdir/gentestdata.sh" --force || exit $?
elif [ ! -s "$TESTDATA/good/mp3_44100_128_c2.mp3" ] || \
     [ ! -s "$TESTDATA/bad/crc_error.mp3" ] || \
     [ ! -s "$TESTDATA/bad/musiclength.mp3" ] || \
     [ ! -s "$TESTDATA/bad/xingframes.mp3" ] || \
     [ ! -s "$TESTDATA/bad/xingbytes.mp3" ] || \
     [ ! -s "$TESTDATA/bad/lamecrc.mp3" ] || \
     [ ! -s "$TESTDATA/bad/musiccrc.mp3" ] || \
     [ ! -s "$TESTDATA/good/mp3_44100_vbr.mp3" ] || \
     [ ! -s "$TESTDATA/good/mp3_44100_128_c2_lame_old.mp3" ]; then
	TESTDATA=$TESTDATA "$testdir/gentestdata.sh" || exit $?
fi

GOOD=$TESTDATA/good
BAD=$TESTDATA/bad

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
expect_result "Xing MusicLength mismatch" "$BAD/musiclength.mp3" "Bad" 1
expect_result "Xing frame count mismatch" "$BAD/xingframes.mp3" "Bad" 1
expect_result "Xing byte count mismatch" "$BAD/xingbytes.mp3" "Bad" 1
expect_result "LAME tag CRC mismatch" "$BAD/lamecrc.mp3" "Bad" 1
expect_result "music data CRC mismatch" "$BAD/musiccrc.mp3" "Bad" 1

# The error descriptions should be present in the full output.
expect_contains "empty error description" "empty file" "$BAD/empty.mp3"
expect_contains "invalid header description" "invalid header values" "$BAD/invalid_header.mp3"
expect_contains "unidentified bytes description" "unidentified bytes" "$BAD/unidentified_bytes.mp3"
expect_contains "inconsistent frames description" "inconsistent frame headers" "$BAD/inconsistent.mp3"
expect_contains "crc error description" "CRC error" "$BAD/crc_error.mp3"
expect_contains "MusicLength error description" "inconsistent Xing header values" "$BAD/musiclength.mp3"
expect_contains "Xing frame count error description" "inconsistent Xing header values" "$BAD/xingframes.mp3"
expect_contains "Xing byte count error description" "inconsistent Xing header values" "$BAD/xingbytes.mp3"
expect_contains "LAME tag CRC error description" "LAME tag CRC error" "$BAD/lamecrc.mp3"
expect_contains "music data CRC error description" "music data CRC error" "$BAD/musiccrc.mp3"

# --- Metadata tags ---------------------------------------------------------
expect_contains "id3 tags detected" "<id3v2>" -x "$GOOD/mp3_44100_128_c2_id3.mp3"

apev1=$GOOD/mp3_44100_128_c2_apev1.mp3
apev2=$GOOD/mp3_44100_128_c2_apev2.mp3
apeboth=$GOOD/mp3_44100_128_c2_ape.mp3
if [ -s "$apev1" ]; then
	expect_contains "apev1 tag detected" "<apev1>" -x "$apev1"
	expect_not_contains "apev1 has no apev2 tag" "<apev2>" -x "$apev1"
	expect_contains "apev2 tag detected" "<apev2>" -x "$apev2"
	expect_not_contains "apev2 has no apev1 tag" "<apev1>" -x "$apev2"
	expect_contains "both ape tags detected (v1)" "<apev1>" -x "$apeboth"
	expect_contains "both ape tags detected (v2)" "<apev2>" -x "$apeboth"
else
	echo "SKIP: APE tag tests (mutagen not available)"
fi

# --- Options ---------------------------------------------------------------
expect_status "version" 0 --version
expect_status "help" 0 --help
expect_status "no filename" 22
expect_status "missing file" 2 "$BAD/does-not-exist.mp3"

expect_contains "verbose shows version" "MPEG v1.0" -v "$GOOD/mp3_44100_128_c2.mp3"
expect_contains "verbose shows MusicLength" "MusicLength" -v "$GOOD/mp3_44100_128_c2.mp3"
expect_contains "verbose shows Xing tag" "Xing tag" -v "$GOOD/mp3_44100_128_c2.mp3"
expect_contains "verbose shows Xing frames" "        frames" -v "$GOOD/mp3_44100_128_c2.mp3"
expect_contains "verbose shows Xing bytes" "        bytes" -v "$GOOD/mp3_44100_128_c2.mp3"
if [ -s "$GOOD/mp3_44100_vbr.mp3" ]; then
	expect_contains "verbose shows VBR MusicLength" "MusicLength" -v "$GOOD/mp3_44100_vbr.mp3"
fi
expect_contains "verbose shows ID3" "ID3V2" -v "$GOOD/mp3_44100_128_c2_id3.mp3"
expect_contains "xml reports ok" "<ok />" -x "$GOOD/mp3_44100_128_c2.mp3"
expect_contains "xml reports error" "<code>INV</code>" -x "$BAD/invalid_header.mp3"

# badonly only reports the bad file.
expect_contains "badonly bad file" "Bad" -B "$GOOD/mp3_44100_128_c2.mp3" "$BAD/invalid_header.mp3"
expect_not_contains "badonly hides good file" "$GOOD/mp3_44100_128_c2.mp3: Ok" -B "$GOOD/mp3_44100_128_c2.mp3" "$BAD/invalid_header.mp3"

test_finish
