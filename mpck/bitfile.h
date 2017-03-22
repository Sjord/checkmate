#include "bufio.h"

typedef struct {
	const char * buffer;
	size_t bitoffset;
} BITFILE;

#define BYTEBITS 8

BITFILE *bitfile_new(const char * buffer);
void bitfile_destroy(BITFILE *b);
int bitfile_readbits(BITFILE *b, size_t length);
