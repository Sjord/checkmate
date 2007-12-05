#include "bufio.h"

typedef struct {
	CFILE * fp;
	char currentchar;
	size_t bitoffset;
} BITFILE;

#define BYTEBITS 8

void bitfile_reset(BITFILE *b);
BITFILE *bitfile_new(CFILE *fp);
void bitfile_destroy(BITFILE *b);
int bitfile_readbits(BITFILE *b, size_t length);
