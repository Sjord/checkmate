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

static void skipframe(file, frame)
	file_info * file;
	frame_info * frame;
{
	int newpos = MIN(frame->offset + frame->length, file->length);
	cfseek(file->fp, newpos, SEEK_SET);
}

static size_t
lastframelength(file, frame)
	file_info 	* file;
	frame_info 	* frame;
{
	char buf[5];
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
		}
		if (*ptr == 'I') {
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

static int findlastframe(file)
	file_info * file;
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

	cfseek(file->fp, oldpos, SEEK_SET);
	return TRUE;
}

static int openfile(filename, file) 
	char 		* filename;
	file_info	* file;
{
	file->fp = cfopen(filename, "rb");
	if (file->fp == NULL) {
		error("%s: %s", filename, strerror(errno));
		return FALSE;
	}
	file->filename = filename;
	file->length = cfilesize(file->fp);
	return TRUE;
}

int checkfile(filename, file)
	char		* filename;
	file_info 	* file;
{
	frame_info * frame;
	
	file_clear(file);
	if (!openfile(filename, file)) {
		return FALSE;
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
	return TRUE;
}
