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

#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_IO_H
#include <io.h>
#endif

static CFILE * ub_fopen(const char * filename, char * mode, size_t filesize);
static size_t ub_fread(char *outbuf, size_t size, CFILE *c);
static int ub_fseek(CFILE *c, long offset, int whence);
static size_t ub_ftell(CFILE *c);
static int ub_feof(CFILE *c);
static int ub_fclose(CFILE *c);

static CFILE * mm_fopen(const char * filename, char * mode, size_t filesize);
static size_t mm_fread(char *outbuf, size_t size, CFILE *c);
static int mm_fclose(CFILE * c);

static CFILE * wb_fopen(const char * filename, char * mode, size_t filesize);
static size_t wb_fread(char *outbuf, size_t size, CFILE *c);
static int wb_fseek(CFILE *c, long offset, int whence);
static size_t wb_ftell(CFILE *c);
static int wb_feof(CFILE *c);
static int wb_fclose(CFILE *c);

static io_funcs ub_funcs = {
	ub_fread,
	ub_fseek,
	ub_ftell,
	ub_feof,
	ub_fclose
};

static io_funcs mm_funcs = {
	mm_fread,
	wb_fseek,
	wb_ftell,
	wb_feof,
	mm_fclose
};

static io_funcs wb_funcs = {
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
	
	filesize=_cfilesize(filename);
	if (filesize == -1) return NULL;

	if (filesize < BUFFER_LIMIT) {
		c = wb_fopen(filename, mode, filesize);
	} else {
		c = mm_fopen(filename, mode, filesize);
	}
	if (c == NULL)
	{
		// TODO print warning about being slow
		c = ub_fopen(filename, mode, filesize);
	}
	if (c != NULL)
	{
		c->filesize = filesize;
	}

	return c;
}

// whole buffered

static size_t readall(int fd, void * buf, size_t count) {
	char * p = buf;
	size_t nread = 0;
	size_t result;
	do {
		result = read(fd, p, count);
		if (result == 0)
		{
			break;
		}
		nread += result;
		p += result;
	} while (nread < count);
	return nread;
}

static CFILE * wb_fopen(const char * filename, char * mode, size_t filesize) {
	CFILE *c;
	size_t nread; // TODO use this or remove this
	int fd;
	
	// we malloc an extra int because of what cfread does
	c=(CFILE *)malloc(sizeof(CFILE)+filesize+sizeof(int));
	if (c == NULL) {
		return NULL;
	}

	memset(c, 0, sizeof(CFILE));
	fd=open(filename, O_RDONLY
#ifdef _WIN32
 	                | O_BINARY
#endif
		);
	if (fd==-1) {
		free(c);
		return NULL;
	}

	c->io_funcs = & wb_funcs;
	c->buffer=(char *)(c+1);
	c->bufpnt=c->buffer;
	c->buflen=filesize;
	c->bufend=c->buffer+filesize;
	nread=readall(fd, c->buffer, filesize);
	close(fd);
	if (nread != filesize)
	{
		free(c);
		return NULL;
	}
	return c;
}

static size_t wb_fread(char * outbuf, size_t size, CFILE * c) {
	size_t readnow;
	readnow=MIN(reminbuf(c), size); 
	// Most of the time, size is small and we do a fast assignment.
	// This means we read beyond the end of the buffer. Hope it doesn't matter.
	if (readnow <= sizeof(int)) {
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

static int wb_fseek(CFILE * c, long offset, int whence) {
	if (whence == SEEK_SET) return wb_fsetpos(c, offset);
	if (whence == SEEK_CUR) {
		return wb_fsetpos(c, cftell(c)+offset);
	}
	if (whence == SEEK_END) {
		return wb_fsetpos(c, c->buflen+offset);
	}
	return -1;
}

static size_t wb_ftell(CFILE * c) {
	return c->bufpnt-c->buffer;
}

static int wb_feof(CFILE * c) {
	return reminbuf(c) == 0;
}

static int wb_fclose(CFILE * c) {
	free(c);
	return TRUE;
}

// memory mapped

#ifdef _WIN32

static CFILE * mm_fopen(const char * filename, char * mode, size_t filesize) {
	HANDLE file;
	HANDLE fileMapping;
	CFILE *c;

	c = (CFILE *) malloc(sizeof(CFILE));
	if (c == NULL) return NULL;

	file = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file == INVALID_HANDLE_VALUE) {
		free(c);
		return NULL;
	}

	fileMapping = CreateFileMapping(file, NULL, PAGE_READONLY, 0, 0, NULL);
	CloseHandle(file);
	if (fileMapping == INVALID_HANDLE_VALUE) {
		free(c);
		return NULL;
	}

	c->buffer = MapViewOfFile(fileMapping, FILE_MAP_READ, 0, 0, 0);
	CloseHandle(fileMapping);
	if (c->buffer == NULL)
	{
		free(c);
		return NULL;
	}

	c->io_funcs = & mm_funcs;
	c->bufpnt=c->buffer;
	c->buflen=filesize;
	c->bufend=c->buffer+filesize;
	return c;
}

static int mm_fclose(CFILE * c) {
	int res = UnmapViewOfFile(c->buffer);
	free(c);
	if (res)
		return 0;
	else
		return EOF;
}
#else // _WIN32

static CFILE * mm_fopen(const char * filename, char * mode, size_t filesize) {
	CFILE *c;
	FILE * fp;
	int fd;

	c = (CFILE *) malloc(sizeof(CFILE));
	if (c == NULL) return NULL;

	fp = fopen(filename, mode);
	if (fp == NULL)
	{
		free(c);
		return NULL;
	}
	fd = fileno(fp);
		
	c->buffer = mmap(0, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
	fclose(fp);
	if (c->buffer == MAP_FAILED)
	{
		free(c);
		return NULL;
	}

	c->io_funcs = & mm_funcs;
	c->bufpnt=c->buffer;
	c->buflen=filesize;
	c->bufend=c->buffer+filesize;
	return c;
}

static int mm_fclose(CFILE * c) {
	munmap(c->buffer, c->filesize);
	free(c);
	return TRUE;
}

#endif // _WIN32

// Dont use wb_fread because it reads beyond the end of the buffer
static size_t mm_fread(char * outbuf, size_t size, CFILE * c) {
	size_t readnow;
	readnow=MIN(reminbuf(c), size); 
	memcpy(outbuf, c->bufpnt, readnow);
	c->bufpnt += readnow;
	return readnow;
}

// unbuffered
static CFILE * ub_fopen(const char * filename, char * mode, size_t filesize) {
	CFILE *c = (CFILE *) malloc(sizeof(CFILE));
	if (c == NULL) return NULL;
	c->fp = fopen(filename, mode);
	if (c->fp == NULL)
	{
		free(c);
		return NULL;
	}
	c->io_funcs = & ub_funcs;
	return c;
}

static size_t ub_fread(char * outbuf, size_t size, CFILE * c) {
	return fread(outbuf, 1, size, c->fp);
}

static int ub_fseek(CFILE * c, long offset, int whence) {
	return fseek(c->fp, offset, whence);
}

static size_t ub_ftell(CFILE * c) {
	return ftell(c->fp);
}

static int ub_feof(CFILE * c) {
	return feof(c->fp);
}

static int ub_fclose(CFILE * c) {
	int res = fclose(c->fp);
	free(c);
	return res;
}
