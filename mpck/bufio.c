/*
 *   This file is part of Checkmate, a program to check MP3 files for errors
 *   
 *   Copyright (C)  2005  Sjoerd Langkemper
 *   
 *   Checkmate is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *   
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *****************************************************************************
 *
 *   bufio.c - buffered cfopen, cfread, cfclose, cftell and cfseek 
 * 
 */

#include "mpck.h" 
#include "bufio.h"
#include <stdio.h>

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

size_t ub_fread(char *outbuf, size_t size, CFILE *c);
int ub_fseek(CFILE *c, long offset, int whence);
size_t ub_ftell(CFILE *c);
int ub_feof(CFILE *c);
int ub_fclose(CFILE *c);

size_t wb_fread(char *outbuf, size_t size, CFILE *c);
int wb_fseek(CFILE *c, long offset, int whence);
size_t wb_ftell(CFILE *c);
int wb_feof(CFILE *c);
int wb_fclose(CFILE *c);

static io_funcs unbuffered_funcs = {
	ub_fread,
	ub_fseek,
	ub_ftell,
	ub_feof,
	ub_fclose
};

static io_funcs buffered_funcs = {
	wb_fread,
	wb_fseek,
	wb_ftell,
	wb_feof,
	wb_fclose
};

#define reminbuf(c) (size_t)(c->bufend - c->bufpnt)

/* some sys/stat.h do not define these macro's */
#ifndef S_ISDIR
#define S_ISDIR(mode) (((mode) & S_IFDIR)==(S_IFDIR))
#endif /* S_ISDIR */

#ifndef S_ISREG
#define S_ISREG(mode) (((mode) & S_IFREG)==(S_IFREG))
#endif /* S_ISREG */

static size_t _cfilesize(const char * filename) {
	struct stat _ss;
	struct stat * ss=&_ss; 
	int statres;

	statres=stat(filename, ss);
	if (statres != 0) return -1;	

	return ss->st_size;
}

size_t cfilesize(CFILE * c) {
	return c->filesize;
}

int cisdirectory(const char * filename) {
	struct stat _ss;
	struct stat * ss=&_ss; 
	int statres;

	statres=stat(filename, ss);
	if (statres != 0) return -1;	

	return S_ISDIR(ss->st_mode);
}

CFILE * cfopen(const char * filename, char * mode) {
	size_t filesize;
	CFILE *c;
	size_t nread;
	int buffered;
	
	filesize=_cfilesize(filename);
	if (filesize == -1) return NULL;
	// we malloc an extra int because of what cfread does
	c=(CFILE *)malloc(sizeof(CFILE)+filesize+sizeof(int));
	if (c == NULL) {
		buffered=FALSE;
		c=(CFILE *)malloc(sizeof(CFILE));
		if (c == NULL) return NULL;
	} else {
		buffered=TRUE;
	}

	memset(c, 0, sizeof(CFILE));
	c->fp=fopen(filename, mode);
	if (c->fp == NULL) {
		free(c);
		return NULL;
	}
	c->filesize=filesize;
	if (!buffered) {
		c->io_funcs = & unbuffered_funcs;
		return c;
	}

	c->io_funcs = & buffered_funcs;
	c->buffer=(char *)(c+1);
	c->bufpnt=c->buffer;
	c->buflen=filesize;
	c->bufend=c->buffer+filesize;
	nread=fread(c->buffer, 1, filesize, c->fp);
	fclose(c->fp);
	c->fp=NULL;
	return c;
}

/*
size_t cfread(char * outbuf, size_t size, CFILE * c) {
	return c->io_funcs->fread(outbuf, size, c);
}

int cfseek(CFILE * c, long offset, int whence) {
	return c->io_funcs->fseek(c, offset, whence);
}

size_t cftell(CFILE * c) {
	return c->io_funcs->ftell(c);
}

int cfeof(CFILE * c) {
	return c->io_funcs->feof(c);
}

int cfclose(CFILE * c) {
	return c->io_funcs->fclose(c);
}
*/

// whole buffered

size_t wb_fread(char * outbuf, size_t size, CFILE * c) {
	size_t readnow;
	readnow=MIN(reminbuf(c), size); 
	// Most of the time, size is small and we do a fast assignment.
	// This means we read beyond the end of the buffer. Hope it doesn't matter.
	if (size <= sizeof(int)) {
		* (int *)outbuf = * (int *)c->bufpnt;
	} else {
		memcpy(outbuf, c->bufpnt, readnow);
	}
	c->bufpnt += readnow;
	return readnow;
}

static int wb_fsetpos(CFILE * c, size_t pos) {
	if (pos>c->buflen) return -1;
	c->bufpnt=c->buffer+pos;
	return 0;
}

int wb_fseek(CFILE * c, long offset, int whence) {
	if (whence == SEEK_SET) return wb_fsetpos(c, offset);
	if (whence == SEEK_CUR) {
		return wb_fsetpos(c, cftell(c)+offset);
	}
	if (whence == SEEK_END) {
		return wb_fsetpos(c, c->buflen+offset);
	}
	return -1;
}

size_t wb_ftell(CFILE * c) {
	return c->bufpnt-c->buffer;
}

int wb_feof(CFILE * c) {
	return reminbuf(c) == 0;
}

int wb_fclose(CFILE * c) {
	int res=TRUE;
	if (c->fp != NULL) res=fclose(c->fp);
	free(c);
	return res;
}

// unbuffered
size_t ub_fread(char * outbuf, size_t size, CFILE * c) {
	return fread(outbuf, 1, size, c->fp);
}

int ub_fseek(CFILE * c, long offset, int whence) {
	return fseek(c->fp, offset, whence);
}

long ub_ftell(CFILE * c) {
	return ftell(c->fp);
}

int ub_feof(CFILE * c) {
	return feof(c->fp);
}

int ub_fclose(CFILE * c) {
	return fclose(c->fp);
}