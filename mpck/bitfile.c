#include "bitfile.h"
#include <stdlib.h>

void bitfile_reset(BITFILE * b) {
	b->currentchar = '\0';
	b->bitoffset = 0;
}

BITFILE * bitfile_new(CFILE * fp) {
	BITFILE * b;
	b = malloc(sizeof(BITFILE));
	b->fp = fp;
	bitfile_reset(b);
	return b;
}

void bitfile_destroy(BITFILE * b) {
	free(b);
}

int bitfile_readbits(BITFILE * b, size_t length) {
	int result;
	int left;
	char ch;
	unsigned int mask;
	int startbit;

	result = 0;
	do {
		if (b->bitoffset == 0) {
			cfread(& (b->currentchar), 1, b->fp);
		}
		mask = (1 << length) - 1;
		left = BYTEBITS - b->bitoffset - length;
		if (left > 0) {
			// 'left' bits remain in this byte
			ch = b->currentchar >> left;
			b->bitoffset = (b->bitoffset + length) % BYTEBITS;
			length = 0;
		} else {
			// abs(left) bits are in the next byte
			left = abs(left);
			ch = b->currentchar << left;
			b->bitoffset = (b->bitoffset + length - left) % BYTEBITS;
			length = left;
		}
		result |= ch & mask;
	} while (length);
	return result;
}

