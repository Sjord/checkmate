#include "bitfile.h"
#include <stdlib.h>

BITFILE * bitfile_new(const char * buffer) {
	BITFILE * b;
	b = malloc(sizeof(BITFILE));
	b->buffer = buffer;
	b->bitoffset = 0;
	return b;
}

void bitfile_destroy(BITFILE * b) {
	free(b);
}

int bitfile_readbits(BITFILE * b, size_t length) {
	int i;
	size_t byte;
	size_t bit;
	int result = 0;
	int val;

	for (i = 0; i < length; i++) {
		result <<= 1;
		byte = b->bitoffset / 8;
		bit = 7 - b->bitoffset % 8;
		val = (b->buffer[byte] & (1 << bit)) >> bit;
		result |= val;
		b->bitoffset += 1;
	}

	return result;
}
