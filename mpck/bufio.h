#ifndef _BUFIO_H
#define _BUFIO_H

#include <stdio.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define MIN(a, b) (a < b ? a : b)

#define BUFFER_LIMIT 10000000 /* the maximum size of the buffer used by the buffered I/O */

typedef struct _io_funcs io_funcs;

typedef struct {
	FILE * fp;
	char * buffer;
	char * bufpnt;
	char * bufend;
	size_t buflen;
	size_t filesize;
	io_funcs * io_funcs;
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

struct _io_funcs {
	size_t (*fread)(char *outbuf, size_t size, CFILE *c);
	int (*fseek)(CFILE *c, long offset, int whence);
	size_t (*ftell)(CFILE *c);
	int (*feof)(CFILE *c);
	int (*fclose)(CFILE *c);
};

#define cfread(outbuf, size, c) c->io_funcs->fread(outbuf, size, c)
#define cfseek(c, offset, whence) c->io_funcs->fseek(c, offset, whence)
#define cftell(c) c->io_funcs->ftell(c)
#define cfeof(c) c->io_funcs->feof(c)
#define cfclose(c) c->io_funcs->fclose(c)

#endif
