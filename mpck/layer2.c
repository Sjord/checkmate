#include <stdlib.h>
#include "bitfile.h"
#include "file.h"
#include "mpck.h"

// shamelessly copied from layer12.c, libmad
/* possible quantization per subband table */
static
struct {
  unsigned int sblimit;
  unsigned char const offsets[30];
} const sbquant_table[5] = {
  /* ISO/IEC 11172-3 Table B.2a */
  { 27, { 7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 3, 3,	/* 0 */
	  3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0 } },
  /* ISO/IEC 11172-3 Table B.2b */
  { 30, { 7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 3, 3,	/* 1 */
	  3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0 } },
  /* ISO/IEC 11172-3 Table B.2c */
  {  8, { 5, 5, 2, 2, 2, 2, 2, 2 } },				/* 2 */
  /* ISO/IEC 11172-3 Table B.2d */
  { 12, { 5, 5, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 } },		/* 3 */
  /* ISO/IEC 13818-3 Table B.1 */
  { 30, { 4, 4, 4, 4, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1,	/* 4 */
	  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 } }
};

/* bit allocation table */
static
struct {
  unsigned short nbal;
  unsigned short offset;
} const bitalloc_table[8] = {
  { 2, 0 },  /* 0 */
  { 2, 3 },  /* 1 */
  { 3, 3 },  /* 2 */
  { 3, 1 },  /* 3 */
  { 4, 2 },  /* 4 */
  { 4, 3 },  /* 5 */
  { 4, 4 },  /* 6 */
  { 4, 5 }   /* 7 */
};

// sb = subband

#define nchannels(fi) 	(fi->stereo == MONO ? 1 : 2) // FIXME constant for single channel

int crcdatalength2(file, frame)
	const file_info * file;
	frame_info * frame;
{
	unsigned int index, sblimit, nbal, nchannels, bound, sb;
	unsigned char const *offsets;
	int bitrate_per_channel;
	int bits = 0;
	int i;
	int j;
	BITFILE * bitfile;

	nchannels = nchannels(frame);

	if (frame->version != MPEG_VER_10) {
		index = 4;
	} else {
		bitrate_per_channel = frame->bitrate / nchannels;
		if (bitrate_per_channel <= 48000) {
			index = (frame->samplerate == 32000) ? 3 : 2;
		} else if (bitrate_per_channel <= 80000) {
			index = 0;
		} else {
			index = (frame->samplerate == 48000) ? 0 : 1;
		}
	}

	sblimit = sbquant_table[index].sblimit;
	offsets = sbquant_table[index].offsets;

	bound = 32;
	if (frame->stereo == JOINT) {
		bound = 4 + 4 * frame->jointstereo;
	}
	if (bound > sblimit) bound = sblimit;

	bitfile = bitfile_new(file->fp);

	for (sb = 0; sb < sblimit; sb++) {
		nbal = bitalloc_table[offsets[sb]].nbal;

		if (sb < bound) {
			for (i = 0; i < nchannels; i++) {
				bits += nbal;
				j = bitfile_readbits(bitfile, nbal);
				if (j) {
					bits += 2;
				}
			}
		} else {
			bits += nbal;
			j = bitfile_readbits(bitfile, nbal);
			if (j) {
				bits += 2 * nchannels;
			}
		}
	}
	bitfile_destroy(bitfile);
	return bits;
}

