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
#include "crc.h"
#include "xing.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif

/* offsets within the LAME extension of the Xing header */
#define LAME_TAG_SIZE		36
#define LAME_MUSICLENGTH_OFFSET	28
#define LAME_MUSICCRC_OFFSET	32
#define LAME_TAGCRC_OFFSET	34

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

/* The LAME TagCRC covers the tag frame from its MPEG header up to and
 * including the MusicCRC.  It can be used to distinguish a populated LAME
 * extension from an old one that only has zero fields. */
static int
lame_tagcrc_ok(file_info * file, int framestart, int lameoffset, int frameend)
{
	unsigned char buf[256];
	unsigned int crc = 0;
	unsigned int stored;
	int pos;
	int end;
	int chunk;
	int res;

	end = lameoffset + LAME_TAGCRC_OFFSET;
	if (end + 2 > frameend || end <= framestart) return FALSE;

	pos = framestart;
	while (pos < end) {
		chunk = MIN(end - pos, (int)sizeof(buf));
		if (cfseek(file->fp, pos, SEEK_SET) != 0) return FALSE;
		res = cfread((char *)buf, chunk, file->fp);
		if (res != chunk) return FALSE;
		crc = crc16_music_update(crc, buf, chunk);
		pos += chunk;
	}

	if (cfseek(file->fp, end, SEEK_SET) != 0) return FALSE;
	res = cfread((char *)buf, 2, file->fp);
	if (res != 2) return FALSE;

	stored = ((unsigned int)buf[0] << 8) | (unsigned int)buf[1];
	return crc == stored;
}

static void
parse_lame_tag(file_info * file, const frame_info * frame, int offset, int frameend)
{
	unsigned char lame[LAME_TAG_SIZE];
	unsigned long musiclength;
	unsigned int musiccrc;
	unsigned int tagcrc;
	int res;

	if (offset + LAME_TAG_SIZE > frameend) return;

	if (cfseek(file->fp, offset, SEEK_SET) != 0) return;
	res = cfread((char *)lame, LAME_TAG_SIZE, file->fp);
	if (res != LAME_TAG_SIZE) return;
	if (memcmp(lame, "LAME", 4) != 0) return;

	musiclength = ((unsigned long)lame[LAME_MUSICLENGTH_OFFSET] << 24)
	            | ((unsigned long)lame[LAME_MUSICLENGTH_OFFSET + 1] << 16)
	            | ((unsigned long)lame[LAME_MUSICLENGTH_OFFSET + 2] << 8)
	            |  (unsigned long)lame[LAME_MUSICLENGTH_OFFSET + 3];
	musiccrc = ((unsigned int)lame[LAME_MUSICCRC_OFFSET] << 8)
	         |  (unsigned int)lame[LAME_MUSICCRC_OFFSET + 1];
	tagcrc = ((unsigned int)lame[LAME_TAGCRC_OFFSET] << 8)
	       |  (unsigned int)lame[LAME_TAGCRC_OFFSET + 1];

	/* Old LAME tags have these fields zeroed.  There is no TagCRC to
	 * validate in that case. */
	if (musiclength == 0 && musiccrc == 0 && tagcrc == 0) return;

	/* MusicLength can still be compared to the actual stream even when
	 * the TagCRC does not match. */
	file->musiclength = musiclength;
	file->xingflags |= XING_FLAG_MUSICLENGTH;

	if (!lame_tagcrc_ok(file, frame->offset, offset, frameend)) {
		file->xingflags |= XING_FLAG_LAMECRC;
		return;
	}

	file->musiccrc = musiccrc;
	file->xingflags |= XING_FLAG_MUSICCRC;
}

/* Reads the Xing/LAME header from frame, if it has one. */
int
xing_read(file_info * file, const frame_info * frame)
{
	int offset;
	int framestart;
	int frameend;
	int savedpos;
	unsigned long flags;
	unsigned long value;

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

	parse_lame_tag(file, frame, offset, frameend);

	cfseek(file->fp, savedpos, SEEK_SET);
	return TRUE;
}
