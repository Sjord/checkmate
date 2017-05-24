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
 *   checkframe.c - checks an MP3 frame
 * 
 */

#include "mpck.h"
#include "file.h"
#include "frame.h"
#include "print.h"
#include "id3.h"
#include "ape.h"
#include "crc.h"
#include "options.h"
#include "matrices.h"
#include "layer2.h"

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

/* samplerate in Hz of frame fi with headervalue h */
#define samplerate(h, fi)  samplerate_matrix[h][fi->version]

/* the duration of this frame in ms */
#define frametime(fi)      (1000*fi->samples/fi->samplerate)

/* the number of samples in frame fi */
#define framesamples(fi)   samples_matrix[fi->layer][fi->version]

/* the layer for headervalue h */
#define layer(h) 	   (4-(h))

/* framelength(fi) calculates the length (in bytes) of a frame */
static int framelength(file, fi)
	const file_info  * file;
	const frame_info * fi;
{
	/* if this is the last frame, it has a different length */
	if ((file->lastframe_offset == fi->offset) && (file->lastframe_offset)) {
		return file->lastframe_length;
	}

	/* else, just compute the frame length */
	if (fi->layer == LAYER_1) {
		return 4*fi->padding+4*((12*fi->bitrate)/fi->samplerate);
	} else {
		return fi->padding+fi->samples*fi->bitrate/(8*fi->samplerate);
	}
}

static void
alienbytes(file, num)
	file_info * file;
	int num;
{
	if (file->frames == 0)
		file->alien_before += num;
	else
		file->alien_after += num;
}

/* checks for consistency */
static int
checkconsistency(file, frame)
	const file_info * file;
	const frame_info * frame;
{
	int verbose = options_get_verbose();

	if ((file->version == 0) && (file->layer == 0)) {
		/* this is the first frame, nothing to check */
		return TRUE;
	}

	if (file->version != frame->version) {
		if (verbose) offseterror(file, " version differs from previous frame");
		return FALSE;
	}
	if (file->layer != frame->layer) {
		if (verbose) offseterror(file, " layer differs from previous frame");
		return FALSE;
	}
	return TRUE;
}

/* sets consistency data to current frame */
static void
setconsistent(file, frame)
	file_info * file;
	const frame_info * frame;
{
	file->version = frame->version;
	file->layer = frame->layer;
}

/* returns bitrate in bps */
static int bitrate(headervalue, fi)
	int headervalue;	/* value of the bitrate as in the header */
	frame_info * fi;	/* this function uses version and layer */
{
	return 1000*bitrate_matrix[3*fi->version+fi->layer-1][headervalue];
}

static int 
mpegver(headervalue) 
	int headervalue;
{
	if (headervalue == 3) return MPEG_VER_10;
	if (headervalue == 2) return MPEG_VER_20;
	if (headervalue == 0) return MPEG_VER_25;
	return MPEG_VER_INVALID;
}

static void
parseframe(file, fr, buf)
	const file_info  * file;
	frame_info * fr;
	char * buf;
{
	int res;
	
	fr->version	=mpegver((buf[1]&24)>>3);	/* bits 12,13	*/
	fr->layer   	=layer((buf[1]&6)>>1);		/* bits 14,15	*/
	fr->crc16   	=!(buf[1]&1);			/* bit  16	*/
	fr->bitrate   	=bitrate((buf[2]&240)>>4, fr);	/* bits 17-20	*/
	fr->samplerate 	=samplerate((buf[2]&12)>>2, fr);/* bits 21,22	*/
	fr->padding    	=(buf[2]&2)  >>1;		/* bit  23	*/
	fr->private    	=(buf[2]&1);			/* bit  24	*/
	fr->stereo     	=(buf[3]&192)>>6;		/* bits 25,26	*/
	fr->jointstereo	=(buf[3]&48) >>4;		/* bits 27,28	*/
	fr->copyright  	=(buf[3]&8)  >>3;		/* bit  29	*/
	fr->original	=(buf[3]&4)  >>2;		/* bit  30	*/
	fr->emphasis   	=(buf[3]&3);			/* bits 31,32	*/
	
	fr->offset	=cftell(file->fp)-4;

	if (fr->crc16) {
		res=cfread(buf, 2, file->fp);
		if (!res) offseterror(file, " CRC read error");
		fr->crc16=(buf[0]&0xff)<<8;
		fr->crc16 += (buf[1]&0xff);
	}

	if (fr->samplerate > 0) {
		fr->samples	=framesamples(fr);
		fr->length	=framelength(file, fr);
		fr->time	=frametime(fr);
	}
}

/* checks the validity of struct frame_info fi */
int
checkvalidity(file, frame)
	const file_info * file;
	const frame_info * frame;
{
	int verbose = options_get_verbose();

	/* These values are not valid */
	if (frame->version == MPEG_VER_INVALID) {
		if (verbose) offseterror(file, " unknown version");
		return FALSE;
	}
	if (frame->layer == 4) {
		if (verbose) offseterror(file, " unknown layer (1-3 allowed)");
		return FALSE;
	}
	if (frame->bitrate < 8000) {
		// FIXME this ignores the freeform bitrate
		if (verbose) offseterror(file, " unknown bitrate");
		return FALSE;
	}
	if (frame->samplerate < 8000) {
		if (verbose) offseterror(file, " unknown samplerate");
		return FALSE;
	}
	return TRUE;
}

/*
 * FIXME: return the number of bytes which could not be interpreted.
 */
int
findframe(file, frame)
	file_info 	* file;
	frame_info 	* frame;
{
	int res;
	char buf[8];
	char * ptr;
	
	do {
		ptr = buf;
		res = cfread(ptr, 1, file->fp);
		if (res <= 0) {
			if (cfeof(file->fp)) {
				return FALSE;
			} else {
				offseterror(file, "read error");
			}
			continue;
		}

		if ((*ptr & 0xff) == 0xff) {
			res = cfread(++ptr, 1, file->fp);
			if (res < 1) continue;
			if ((*ptr & 0xe0) == 0xe0) { /* possible MP3 frame header */
				res = cfread(++ptr, 2, file->fp);
				if (res < 2) continue;
				parseframe(file, frame, buf);
				return TRUE;
			}
		} else if (*ptr == 'T') {	/* TAG -> ID3v1 tag */
			res = cfread(++ptr, 2, file->fp);
			if (res < 2) continue;
			if (*ptr++ == 'A' && *ptr++ == 'G') {
				skip_id3v1_tag(file);
			} else {
				alienbytes(file, 3);
			}
		} else if (*ptr == 'I') {	/* ID3 -> ID3v2 tag */
			res = cfread(++ptr, 2, file->fp);
			if (res < 2) continue;
			if (*ptr++ == 'D' && *ptr++ == '3') {
				skip_id3v2_tag(file);
			} else {
				alienbytes(file, 3);
			}
		} else if (*ptr == 'A') {   /* APETAGEX -> APE tag */
			res = cfread(++ptr, 7, file->fp);
			if (res < 7) continue;
			if (*ptr++ == 'P' && *ptr++ == 'E' &&
					*ptr++ == 'T' && *ptr++ == 'A' &&
					*ptr++ == 'G' && *ptr++ == 'E' &&
					*ptr++ == 'X') {
				skip_ape_tag(file);
			} else {
				alienbytes(file, 8);
			}
		} else {
			alienbytes(file, 1);
		}
	} while (!cfeof(file->fp));
	return FALSE;
}

static int crcdatalength(file, frame)
	const file_info * file;
	frame_info * frame;
{
	if (frame->layer == 2) {
		return crcdatalength2(file, frame);
	}
	if (frame->version == MPEG_VER_10) {
		if (frame->layer == 3) {
			return (frame->stereo == MONO ? 17 * 8 : 32 * 8);
		}
		if (frame->layer == 1) {
			return (frame->stereo == MONO ? 128 : 256);
		}
	} else {
		return (frame->stereo == MONO ?  9 * 8 : 17 * 8);
	}
	return 0; // never reached, but supress warning
}

static int checkcrc16(file, frame)
	const file_info * file;
	frame_info * frame;
{
	// This buffer needs to hold the header and all other CRC bytes.
	// header is 2 bytes, crcdatalength2 returns max. 39 bytes.
	char buf[41];
	int res;
	int nbits;
	int nbytes;

	nbits = crcdatalength(file, frame);
	nbytes = nbits >> 3;
	if (nbits & 7) nbytes += 1;

	// read header
	cfseek(file->fp, frame->offset+2, SEEK_SET);
	res=cfread(buf, 2, file->fp);
	if (!res) {
		return FALSE;
	}

	// read data
	cfseek(file->fp, frame->offset+6, SEEK_SET);
	if (nbytes > frame->length - 6) {
		nbytes = MAX(0, frame->length - 6);
		nbits = nbytes * 8;
	}
	res = cfread(buf+2, nbytes, file->fp);
	if (res<nbytes) {
		return FALSE;
	}

	cfseek(file->fp, frame->offset + frame->length, SEEK_SET);

	return frame->crc16 == crc16bits((unsigned char *)buf, nbits + 16);
}

int
checkframe(file, frame)
	file_info  * file;
	frame_info * frame;
{
	if (!checkvalidity(file, frame)) {
		file->errors |= ERR_INVALID;
		return FALSE;
	}
	
	if (frame->crc16) {
		if (!checkcrc16(file, frame)) {
			file->errors |= ERR_FRAMECRC;
			return FALSE;
		}
	}

	if (!checkconsistency(file, frame)) {
		file->errors |= ERR_INCONSISTENT;
		setconsistent(file, frame);
		return FALSE;
	}
	return TRUE;
}
