#include <stdlib.h>

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

#define nchannels(fi) 	(fi->stereo == 3 ? 1 : 2) // FIXME constant for single channel
#define BYTEBITS 8

static int readbits(file, length) 
	const file_info * file;
	int length;
{
	static char currentchar;
	int startbyte;
	int result;
	int bitoffset;
	int left;
	char ch;
	unsigned int mask;
	static int startbit = 0;

	result = 0;
	do {
		startbyte = startbit / BYTEBITS;
		bitoffset = startbit % BYTEBITS;
		if (bitoffset == 0) {
			cfread(&currentchar, 1, file->fp);
		}
		mask = (1 << length) - 1;
		left = BYTEBITS - bitoffset - length;
		if (left > 0) {
			// 'left' bits remain in this byte
			ch = currentchar >> left;
			startbit += length;
			length = 0;
		} else {
			// abs(left) bits are in the next byte
			left = abs(left);
			ch = currentchar << left;
			startbit += length - left;
			length = left;
		}
		result |= ch & mask;
	} while (length);
	return result;
}


static int crcdatalength2(file, frame)
	const file_info * file;
	frame_info * frame;
{
	// struct mad_header *header = &frame->header;
	// struct mad_bitptr start;
	unsigned int index, sblimit, nbal, nchannels, bound, gr, channel, s, sb;
	unsigned char const *offsets;
	unsigned char allocation[2][32], scfsi[2][32], scalefactor[2][32][3];
	int bitrate_per_channel;
	int bits = 0;
	int i;

	nchannels = nchannels(frame);

	if (frame->version == MPEG_VER_25) {
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
	if (frame->stereo == 1) { // FIXME look up constant for joint stereo
		bound = 4 + 4 * frame->jointstereo;
	}
	if (bound > sblimit) bound = sblimit;

	for (sb = 0; sb < sblimit; sb++) { // my for loop
		nbal = bitalloc_table[offsets[sb]].nbal;

		if (sb < bound) {
			for (i = 0; i < nchannels; i++) {
				bits += nbal;
				if (readbits(file, nbal)) bits += 2;
			}
		} else {
			bits += nbal;
			if (readbits(file, nbal)) bits += 2 * nchannels;
		}
	}
	return bits;
}

/*
	for (sb = 0; sb < bound; ++sb) {
		nbal = bitalloc_table[offsets[sb]].nbal;

		for (channel = 0; channel < nchannels; ++channel)
			// allocation[channel][sb] = mad_bit_read(&stream->ptr, nbal);
			printf("Reading %d bits\n", nbal);
	}
  for (sb = bound; sb < sblimit; ++sb) {
    nbal = bitalloc_table[offsets[sb]].nbal;

    allocation[0][sb] =
    allocation[1][sb] = mad_bit_read(&stream->ptr, nbal);
  }

  /* decode scalefactor selection info */
/*
  for (sb = 0; sb < sblimit; ++sb) {
    for (channel = 0; channel < nchannels; ++channel) {
      if (allocation[channel][sb])
	scfsi[channel][sb] = mad_bit_read(&stream->ptr, 2);
    }
  }

  /* check CRC word */
/*
  if (header->flags & MAD_FLAG_PROTECTION) {
    header->crc_check =
      mad_bit_crc(start, mad_bit_length(&start, &stream->ptr),
		  header->crc_check);

    if (header->crc_check != header->crc_target &&
	!(frame->options & MAD_OPTION_IGNORECRC)) {
      stream->error = MAD_ERROR_BADCRC;
      return -1;
    }
  }
*/
	

