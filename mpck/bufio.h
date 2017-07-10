#ifndef _BUFIO_H
#define _BUFIO_H

#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#else
typedef char TCHAR;
#define _topen open
#define _tfopen fopen
#define _tstat stat
#define _stat stat
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define MIN(a, b) (a < b ? a : b)

/* The maximum size of the buffer used by the buffered I/O. 
   This is currently based on the typical size of an MP3 file, 
   but should probably be based on memory size. */
#define BUFFER_LIMIT (6 * 1024 * 1024) // 6 MiB

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
int cisdirectory(const TCHAR *filename);
CFILE *cfopen(const TCHAR *filename, TCHAR *mode);

// use cfeof instead of feof because feof is a macro which messes things up.
struct _io_funcs {
	size_t (*cfread)(char *outbuf, size_t size, CFILE *c);
	int (*cfseek)(CFILE *c, long offset, int whence);
	size_t (*cftell)(CFILE *c);
	int (*cfeof)(CFILE *c);
	int (*cfclose)(CFILE *c);
};

#define cfread(outbuf, size, c) (c->io_funcs->cfread(outbuf, size, c))
#define cfseek(c, offset, whence) (c->io_funcs->cfseek(c, offset, whence))
#define cftell(c) (c->io_funcs->cftell(c))
#define cfeof(c) (c->io_funcs->cfeof(c))
#define cfclose(c) (c->io_funcs->cfclose(c))

#endif
