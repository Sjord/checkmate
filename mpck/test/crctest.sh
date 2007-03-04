#!/bin/sh

mkdir -p crc
cd crc
dd if=/dev/urandom of=soundfile.raw count=1000

for BITRATE in 32 48 56 64 80 96 112 128 160 192 224 256; do
for SAMPLERATE in 8000 12000 11025 16000 22050 24000 32000 44100 48000; do
for CHANNELS in 1 2; do
FILE=b${BITRATE}s${SAMPLERATE}c${CHANNELS}
KHZ=`echo ${SAMPLERATE} | sed 's/^\([0-9]*\)\([0-9][0-9][0-9]\)$/\1.\2/'`
if [ $CHANNELS = 1 ]; then
	TOPT="-a"
	LOPT="-m m"
else
	TOPT=""
	LOPT=""
fi
toolame $TOPT -s $KHZ -b $BITRATE -e soundfile.raw $FILE.mp2
lame -S $LOPT -s $KHZ -b $BITRATE -p soundfile.raw $FILE.mp3
done
done
done

cd ..
