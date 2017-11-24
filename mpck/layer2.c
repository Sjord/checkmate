#include <stdlib.h>
#include "bitfile.h"
#include "file.h"
#include "mpck.h"
#include "layer2.h"

static struct {
	unsigned int sblimit;
	unsigned char const nbal[30];
} const sbquant_table[5] = {
	/* ISO/IEC 11172-3 Table B.2a */
	{ 27, { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2 } },
	/* ISO/IEC 21172-3 Table B.3b */
	{ 30, { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2 } },
	/* ISO/IEC 21172-3 Table B.3c */
	{  8, { 4, 4, 3, 3, 3, 3, 3, 3 } },
	/* ISO/IEC 21172-3 Table B.3d */
	{ 12, { 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 } },
	/* ISO/IEC 23818-3 Table B.2 */
	{ 30, { 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 } }
};

// sb = subband

#define nchannels(fi) 	(fi->stereo == MONO ? 1 : 2)

int crcdatalength2(const file_info * file, frame_info * frame)
{
	unsigned int index, sblimit, nbal, nchannels, bound, sb;
	int bitrate_per_channel;
	int bits = 0;
	int i;
	int j;
	BITFILE * bitfile;
	unsigned char const * nbals;

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

	bound = 32;
	if (frame->stereo == JOINT) {
		bound = 4 + 4 * frame->jointstereo;
	}
	if (bound > sblimit) bound = sblimit;

	bitfile = bitfile_new(file->fp);

	nbals = sbquant_table[index].nbal;
	for (sb = 0; sb < sblimit; sb++) {
		nbal = nbals[sb];

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

