/*
 *   This file is part of Checkmate, a program to check MP3 files for errors
 *   
 *   Copyright (C)  2026  Sjoerd Langkemper
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
 *   xing.c - reads the Xing/LAME header from the first MP3 frame
 * 
 */

#include "mpck.h"
#include "file.h"
#include "frame.h"
#include "xing.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif

/* offset of MusicLength within the LAME extension of the Xing header */
#define LAME_MUSICLENGTH_OFFSET	28

static int
readbe32(CFILE * fp, unsigned long * value)
{
	unsigned char buf[4];

	if (cfread((char *)buf, 4, fp) != 4) return FALSE;
	*value = ((unsigned long)buf[0] << 24)
	       | ((unsigned long)buf[1] << 16)
	       | ((unsigned long)buf[2] << 8)
	       |  (unsigned long)buf[3];
	return TRUE;
}

static int
isxingsignature(CFILE * fp, int offset)
{
	char buf[4];

	if (cfseek(fp, offset, SEEK_SET) != 0) return FALSE;
	if (cfread(buf, 4, fp) != 4) return FALSE;
	return (memcmp(buf, "Xing", 4) == 0) || (memcmp(buf, "Info", 4) == 0);
}

/* Reads the Xing/LAME header from frame, if it has one. */
int
xing_read(file_info * file, const frame_info * frame)
{
	int offset;
	int framestart;
	int frameend;
	int savedpos;
	char lame[4];
	unsigned long flags;
	unsigned long value;
	unsigned long musiclength;

	if (frame->layer != LAYER_3) return FALSE;

	framestart = frame->offset;
	frameend = frame->offset + frame->length;

	/* The Xing header follows the MPEG header and side information. */
	if (frame->version == MPEG_VER_10) {
		offset = framestart + 4 + (frame->stereo == MONO ? 17 : 32);
	} else {
		offset = framestart + 4 + (frame->stereo == MONO ? 9 : 17);
	}

	savedpos = cftell(file->fp);

	if (offset + 4 > frameend) {
		cfseek(file->fp, savedpos, SEEK_SET);
		return FALSE;
	}

	/* Some encoders put the header after the CRC, others ignore the CRC
	 * when calculating the offset.  Try both. */
	if (!isxingsignature(file->fp, offset)) {
		if (offset + 2 + 4 > frameend ||
				!isxingsignature(file->fp, offset + 2)) {
			cfseek(file->fp, savedpos, SEEK_SET);
			return FALSE;
		}
		offset += 2;
	}

	if (cfseek(file->fp, offset + 4, SEEK_SET) != 0 ||
			!readbe32(file->fp, &flags)) {
		cfseek(file->fp, savedpos, SEEK_SET);
		return FALSE;
	}

	/* Read the optional Xing fields. */
	offset += 8;
	if (flags & XING_FLAG_FRAMES) {
		if (offset + 4 <= frameend &&
				cfseek(file->fp, offset, SEEK_SET) == 0 &&
				readbe32(file->fp, &value)) {
			file->xingframes = value;
			file->xingflags |= XING_FLAG_FRAMES;
		}
		offset += 4;
	}
	if (flags & XING_FLAG_BYTES) {
		if (offset + 4 <= frameend &&
				cfseek(file->fp, offset, SEEK_SET) == 0 &&
				readbe32(file->fp, &value)) {
			file->xingbytes = value;
			file->xingflags |= XING_FLAG_BYTES;
		}
		offset += 4;
	}
	if (flags & XING_FLAG_TOC)       offset += 100;
	if (flags & XING_FLAG_VBR_SCALE) offset += 4;

	if (offset + LAME_MUSICLENGTH_OFFSET + 4 <= frameend &&
			cfseek(file->fp, offset, SEEK_SET) == 0 &&
			cfread(lame, 4, file->fp) == 4 &&
			memcmp(lame, "LAME", 4) == 0 &&
			cfseek(file->fp, offset + LAME_MUSICLENGTH_OFFSET, SEEK_SET) == 0 &&
			readbe32(file->fp, &musiclength)) {
		file->musiclength = musiclength;
		file->xingflags |= XING_FLAG_MUSICLENGTH;
	}

	cfseek(file->fp, savedpos, SEEK_SET);
	return TRUE;
}
