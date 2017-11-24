/*
 *   This file is part of Checkmate, a program to check MP3 files for errors
 *   
 *   Copyright (C)  2006  Sjoerd Langkemper
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
 *   checkfile.c - checks a MP3 file
 * 
 */

#include "mpck.h"
#include "file.h"
#include "checkframe.h"
#include "print.h"
#include "filename.h"

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

static void skipframe(file_info * file, frame_info * frame)
{
	int newpos = MIN(frame->offset + frame->length, file->length);
	cfseek(file->fp, newpos, SEEK_SET);
}

static size_t
lastframelength(file_info * file, frame_info * frame)
{
	char buf[4 + sizeof(int)];
	char * ptr;
	int res;
	int i = 0;

	frame->length = MIN(frame->length, file->length - frame->offset);
	
	cfseek(file->fp, frame->offset, SEEK_SET);
	while (i < frame->length) {
		ptr = buf;
		res = cfread(ptr, 1, file->fp);
		if (res <= 0) {
			offseterror(file, "read error");
			continue;
		}
		if (*ptr == 'T') {
			res = cfread(++ptr, 2, file->fp);
			if (res < 2) {
				i += res;
			} else if (*ptr++ == 'A' && *ptr++ == 'G') {
				frame->length = i;
				break;
			}
			i += 2;
		} else if (*ptr == 'I') {
			res = cfread(++ptr, 2, file->fp);
			if (res < 2) {
				i += res;
			} else if (*ptr++ == 'D' && *ptr++ == '3') {
				frame->length = i;
				break;
			}
			i += 2;
		}
		i++;
	}
	return frame->length;
}

static int findlastframe(file_info * file)
{
	int oldpos;
	frame_info * frame;
	frame = frame_create();
	
	oldpos = cftell(file->fp);
	cfseek(file->fp, -END_OFFSET, SEEK_END);

	while (findframe(file, frame)) {
		if (checkvalidity(file, frame)) {
			skipframe(file, frame);
		}
	}

	file->lastframe_offset = frame->offset;
	file->lastframe_length = lastframelength(file, frame);
	file->alien_before = 0;

	frame_destroy(frame);
	cfseek(file->fp, oldpos, SEEK_SET);
	return TRUE;
}

static errno_t openfile(char * filename, file_info * file)
{
	errno_t errno_keep;

	file->fp = cfopen(filename, "rb");
	if (file->fp == NULL) {
		errno_keep = errno;
		fileerror(filename, strerror(errno_keep));
		return errno_keep;
	}
	file->filename = filename;
	file->length = cfilesize(file->fp);
	return 0;
}

errno_t checkfile(char * filename, file_info * file)
{
	frame_info * frame;
	errno_t _errno;
	
	file_clear(file);
	_errno = openfile(filename, file);
	if (_errno) {
		return _errno;
	}

	frame = frame_create();
	findlastframe(file);
	while (findframe(file, frame)) {
		if (checkframe(file, frame)) {
			file_update(file, frame);
			skipframe(file, frame);
		}
	}
	frame_destroy(frame);
	
	check_filename(file);

	cfclose(file->fp);

	file_final(file);
	return 0;
}
