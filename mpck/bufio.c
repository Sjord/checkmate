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

#define USE_BUFFERED_IO
#ifdef USE_BUFFERED_IO

#define reminbuf(c) (size_t)(c->bufend - c->bufpnt)

/* some sys/stat.h do not define these macro's */
#ifndef S_ISDIR
#define S_ISDIR(mode) (((mode) & S_IFDIR)==(S_IFDIR))
#endif /* S_ISDIR */

#ifndef S_ISREG
#define S_ISREG(mode) (((mode) & S_IFREG)==(S_IFREG))
#endif /* S_ISREG */

static int buffered;

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
	if (!buffered) return c;

	c->buffer=(char *)(c+1);
	c->bufpnt=c->buffer;
	c->buflen=filesize;
	c->bufend=c->buffer+filesize;
	nread=fread(c->buffer, 1, filesize, c->fp);
	fclose(c->fp);
	c->fp=NULL;
	return c;
}

size_t cfread(char * outbuf, size_t size, CFILE * c) {
	if (buffered) {
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
	} else {
		return fread(outbuf, 1, size, c->fp);
	}
}

int cfsetpos(CFILE * c, size_t pos) {
	if (buffered) {
		if (pos>c->buflen) return -1;
		c->bufpnt=c->buffer+pos;
		return 0;
	} else {
		return fseek(c->fp, pos, SEEK_SET);
	}
}

int cfseek(CFILE * c, long offset, int whence) {
	if (buffered) {
		if (whence == SEEK_SET) return cfsetpos(c, offset);
		if (whence == SEEK_CUR) {
			return cfsetpos(c, cftell(c)+offset);
		}
		if (whence == SEEK_END) {
			return cfsetpos(c, c->buflen+offset);
		}
		return -1;
	} else {
		return fseek(c->fp, offset, whence);
	}
}

size_t cftell(CFILE * c) {
	if (buffered) {
		return c->bufpnt-c->buffer;
	} else {
		return ftell(c->fp);
	}
}

int cfeof(CFILE * c) {
	if (buffered) {
		return reminbuf(c) == 0;
	} else {
		return feof(c->fp);
	}
}

int cfclose(CFILE * c) {
	int res=TRUE;
	if (c->fp != NULL) res=fclose(c->fp);
	free(c);
	return res;
}

#else /* not USE_BUFFERED_IO */

CFILE * cfopen(const char * filename, char * mode) {
	CFILE *c=(CFILE *)malloc(sizeof(CFILE));
	if (c == NULL) return NULL;
	c->fp=fopen(filename, "rb");
	if (c->fp == NULL) {
		free(c);
		return NULL;
	}
	return c;
}

size_t cfread(char * outbuf, size_t size, CFILE * c) {
	return fread(outbuf, 1, size, c->fp);
}

int cfseek(CFILE * c, long offset, int whence) {
	return fseek(c->fp, offset, whence);
}

long cftell(CFILE * c) {
	return ftell(c->fp);
}

int cfeof(CFILE * c) {
	return feof(c->fp);
}

int cfclose(CFILE * c) {
	return fclose(c->fp);
}

#endif /* USE_BUFFERED_IO */
