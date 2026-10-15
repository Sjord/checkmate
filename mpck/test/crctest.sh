#!/bin/sh
#
# crctest.sh - generate valid MP2/MP3 files for the mpck test suite.
#
# The files are created at test time below $TESTDATA (by default the "data"
# directory next to this script) and are not committed to the repository.
# The generated directory can safely be cached between CI runs; pass --force
# to regenerate everything from scratch.
#
# The MPEG audio encoders do not accept every combination of bitrate,
# samplerate and number of channels.  Combinations that an encoder rejects
# are skipped and reported in the summary.
#
# Exit status: 0 on success, 77 if a required program is missing.

set -u

testdir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
TESTDATA=${TESTDATA:-$testdir/data}
GOOD=$TESTDATA/good
RAW=$TESTDATA/source.raw

force=0
verbose=0
for arg in "$@"; do
	case $arg in
		--force|-f) force=1 ;;
		--verbose|-v) verbose=1 ;;
		*) echo "crctest.sh: unknown option: $arg" >&2; exit 2 ;;
	esac
done

for tool in lame twolame sox; do
	if ! command -v "$tool" >/dev/null 2>&1; then
		echo "crctest.sh: required program '$tool' not found" >&2
		exit 77
	fi
done

mkdir -p "$GOOD"

# A few seconds of stereo 16-bit PCM.  The same source is fed to every
# encoder regardless of the samplerate we claim it has, which is fine: we
# only care about the structure of the encoded bitstream, not the audio.
if [ "$force" = 1 ] || [ ! -s "$RAW" ]; then
	sox -n -r 44100 -c 2 -b 16 -e signed-integer -t raw "$RAW" synth 3 sine 440
fi
if [ ! -s "$RAW" ]; then
	echo "crctest.sh: could not create source audio $RAW" >&2
	exit 1
fi

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

	if [ "$force" != 1 ] && [ -s "$out" ]; then
		generated=$((generated + 1))
		return 0
	fi

	rm -f "$out"
	case $enc in
	twolame)
		opts="-r -N 2 -s $rate -b $bit"
		[ "$ch" = 1 ] && opts="$opts -a"
		[ "$prot" = 1 ] && opts="$opts -p"
		if [ "$verbose" = 1 ]; then
			twolame $opts "$@" "$RAW" "$out"
		else
			twolame $opts "$@" "$RAW" "$out" >/dev/null 2>&1
		fi
		;;
	lame)
		opts="-S -r -s $rate -b $bit"
		[ "$ch" = 1 ] && opts="$opts -m m"
		[ "$prot" = 1 ] && opts="$opts -p"
		if [ "$verbose" = 1 ]; then
			lame $opts "$@" "$RAW" "$out"
		else
			lame $opts "$@" "$RAW" "$out" >/dev/null 2>&1
		fi
		;;
	*)
		echo "crctest.sh: unknown encoder '$enc'" >&2
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

# A representative matrix of samplerates, bitrates and channel counts.
# 44100/32000 are MPEG-1, 22050/24000 are MPEG-2 and 11025/8000 are MPEG-2.5.
for rate in 44100 32000 22050 11025 8000; do
	for bit in 64 128 192 320; do
		for ch in 1 2; do
			encode lame "$GOOD/mp3_${rate}_${bit}_c${ch}.mp3" \
				"$rate" "$bit" "$ch" 0 || skipped=$((skipped + 1))
			encode twolame "$GOOD/mp2_${rate}_${bit}_c${ch}.mp2" \
				"$rate" "$bit" "$ch" 0 || skipped=$((skipped + 1))
		done
	done
done

# The same, but with CRC protection enabled.
for rate in 44100 22050; do
	for bit in 128 192; do
		for ch in 1 2; do
			encode lame "$GOOD/mp3_${rate}_${bit}_c${ch}_crc.mp3" \
				"$rate" "$bit" "$ch" 1 || skipped=$((skipped + 1))
			encode twolame "$GOOD/mp2_${rate}_${bit}_c${ch}_crc.mp2" \
				"$rate" "$bit" "$ch" 1 || skipped=$((skipped + 1))
		done
	done
done

# An MP3 carrying ID3v1 and ID3v2 tags, to exercise metadata detection.
encode lame "$GOOD/mp3_44100_128_c2_id3.mp3" 44100 128 2 0 \
	--add-id3v2 --tt "Test title" --ta "Test artist" || skipped=$((skipped + 1))

echo "crctest.sh: generated $generated files in $GOOD ($skipped combinations skipped)"
exit 0
