#include "bufio.h"

typedef struct {
	CFILE * fp;
	char currentchar;
	size_t bitoffset;
} BITFILE;

#define BYTEBITS 8
