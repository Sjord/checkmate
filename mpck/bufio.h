#ifndef _BUFIO_H
#define _BUFIO_H

#include <stdio.h>

#define CBUFSIZE 4194304 // 4 MB 

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define MIN(a, b) (a < b ? a : b)

typedef struct {
	FILE * fp;
	char * buffer;
	char * bufpnt;
	size_t buflen;
	size_t filesize;
} CFILE;

/* bufio.c */
size_t cfilesize(CFILE *c);
int cisdirectory(const char *filename);
CFILE *cfopen(const char *filename, char *mode);
size_t cfread(char *outbuf, size_t size, CFILE *c);
int cfsetpos(CFILE *c, size_t pos);
int cfseek(CFILE *c, long offset, int whence);
size_t cftell(CFILE *c);
int cfeof(CFILE *c);
int cfclose(CFILE *c);

#endif
