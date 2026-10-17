#!/bin/sh
#
# gentestdata.sh - generate the MP2/MP3 files used by the mpck test suite.
#
# Everything the test suite needs is created at test time below $TESTDATA
# (by default the "data" directory next to this script) and is not committed
# to the repository.  The generated directory can safely be cached between
# CI runs; pass --force to regenerate everything from scratch.
#
# The MPEG audio encoders do not accept every combination of bitrate,
# samplerate and number of channels.  Combinations that an encoder rejects
# are skipped and reported in the summary.
#
# Exit status: 0 on success, 77 if a required program is missing.

set -u

testdir=$(CDPATH='' cd -- "$(dirname -- "$0")" && pwd)
TESTDATA=${TESTDATA:-$testdir/data}
GOOD=$TESTDATA/good
BAD=$TESTDATA/bad
SOURCES=$TESTDATA/sources

# All bitrates and samplerates that mpck knows about, each both with and
# without CRC protection.
SAMPLERATES="8000 12000 11025 16000 22050 24000 32000 44100 48000"
BITRATES="32 48 56 64 80 96 112 128 160 192 224 256"
CHANNELS="1 2"
PROTECTIONS="0 1"

force=0
verbose=0
for arg in "$@"; do
	case $arg in
		--force|-f) force=1 ;;
		--verbose|-v) verbose=1 ;;
		*) echo "gentestdata.sh: unknown option: $arg" >&2; exit 2 ;;
	esac
done

for tool in lame twolame sox; do
	if ! command -v "$tool" >/dev/null 2>&1; then
		echo "gentestdata.sh: required program '$tool' not found" >&2
		exit 77
	fi
done

mkdir -p "$GOOD" "$BAD" "$SOURCES"

# One PCM source per samplerate, so that every encoded file is about two
# seconds long regardless of the samplerate we claim it has.
for rate in $SAMPLERATES; do
	raw=$SOURCES/source_${rate}.raw
	if [ "$force" = 1 ] || [ ! -s "$raw" ]; then
		sox -n -r "$rate" -c 2 -b 16 -e signed-integer -t raw "$raw" synth 2 sine 440
	fi
	if [ ! -s "$raw" ]; then
		echo "gentestdata.sh: could not create source audio $raw" >&2
		exit 1
	fi
done

generated=0
skipped=0

# encode ENCODER OUT RATE BITRATE CHANNELS PROTECT [EXTRA...]
#
# Returns non-zero when the encoder rejects the combination.  Encoder output
# is hidden unless --verbose was given.
encode() {
	enc=$1
	out=$2
	rate=$3
	bit=$4
	ch=$5
	prot=$6
	shift 6
	raw=$SOURCES/source_${rate}.raw

	if [ "$force" != 1 ] && [ -s "$out" ]; then
		generated=$((generated + 1))
		return 0
	fi

	rm -f "$out"

	# Build the argument list for the encoder.  "$@" already holds any
	# extra arguments; the options are appended to it, and the input and
	# output files are added when the encoder is called.  This avoids
	# unquoted word splitting of an option string.
	case $enc in
	twolame)
		set -- "$@" -r -N 2 -s "$rate" -b "$bit"
		[ "$ch" = 1 ] && set -- "$@" -a
		[ "$prot" = 1 ] && set -- "$@" -p
		if [ "$verbose" = 1 ]; then
			twolame "$@" "$raw" "$out"
		else
			twolame "$@" "$raw" "$out" >/dev/null 2>&1
		fi
		;;
	lame)
		set -- "$@" -S -r -s "$rate" -b "$bit"
		[ "$ch" = 1 ] && set -- "$@" -m m
		[ "$prot" = 1 ] && set -- "$@" -p
		if [ "$verbose" = 1 ]; then
			lame "$@" "$raw" "$out"
		else
			lame "$@" "$raw" "$out" >/dev/null 2>&1
		fi
		;;
	*)
		echo "gentestdata.sh: unknown encoder '$enc'" >&2
		return 1
		;;
	esac

	if [ ! -s "$out" ]; then
		rm -f "$out"
		return 1
	fi
	generated=$((generated + 1))
	return 0
}

# Valid files: the full matrix, for both encoders.
for rate in $SAMPLERATES; do
	for bit in $BITRATES; do
		for ch in $CHANNELS; do
			for prot in $PROTECTIONS; do
				if [ "$prot" = 1 ]; then suffix="_crc"; else suffix=""; fi
				encode lame "$GOOD/mp3_${rate}_${bit}_c${ch}${suffix}.mp3" \
					"$rate" "$bit" "$ch" "$prot" || skipped=$((skipped + 1))
				encode twolame "$GOOD/mp2_${rate}_${bit}_c${ch}${suffix}.mp2" \
					"$rate" "$bit" "$ch" "$prot" || skipped=$((skipped + 1))
			done
		done
	done
done

# An MP3 carrying ID3v1 and ID3v2 tags, to exercise metadata detection.
encode lame "$GOOD/mp3_44100_128_c2_id3.mp3" 44100 128 2 0 \
	--add-id3v2 --tt "Test title" --ta "Test artist" || skipped=$((skipped + 1))

# A VBR file uses the "Xing" signature instead of the CBR "Info" signature.
encode lame "$GOOD/mp3_44100_vbr.mp3" 44100 128 2 0 -V 4 \
	|| skipped=$((skipped + 1))

# APE tag variants, written with mutagen by addapetags.py.  These are
# regenerated every run because they are cheap and depend on a script that
# may change.
if command -v python3 >/dev/null 2>&1 && python3 -c 'import mutagen' >/dev/null 2>&1; then
	ape_base=$GOOD/mp3_44100_128_c2.mp3
	python3 "$testdir/addapetags.py" --apev1 "$ape_base" \
		"$GOOD/mp3_44100_128_c2_apev1.mp3"
	python3 "$testdir/addapetags.py" --apev2 "$ape_base" \
		"$GOOD/mp3_44100_128_c2_apev2.mp3"
	python3 "$testdir/addapetags.py" "$ape_base" \
		"$GOOD/mp3_44100_128_c2_ape.mp3"
	generated=$((generated + 3))
else
	echo "gentestdata.sh: python3-mutagen not found; skipping APE tag fixtures" >&2
fi

# Known broken files, derived from the valid ones above.
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
			echo "gentestdata.sh: missing reference file $ref" >&2
			exit 1
		fi
	done

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

	# Corrupt MusicLength in the LAME header of the first frame.  The LAME
	# tag starts at offset 156 and MusicLength is 28 bytes into it.
	cp "$ref_mp3" "$BAD/musiclength.mp3"
	printf '\000\000\200\370' | dd of="$BAD/musiclength.mp3" \
		bs=1 seek=184 conv=notrunc 2>/dev/null
}

make_bad_files

echo "gentestdata.sh: generated $generated files in $GOOD ($skipped combinations skipped)"
exit 0
