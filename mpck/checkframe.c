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
 *   checkframe.c - functions for checking MP3 frames 
 * 
 */

#include <errno.h>
#include "mpck.h"
#include "matrices.h"
#include <stdio.h>

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

/* samplerate in Hz of frame fi with headervalue h */
#define samplerate(h, fi)  samplerate_matrix[h][fi->version]

/* the duration of this frame in ms */
#define frametime(fi)      (1000*fi->samples/fi->samplerate)

/* the number of samples in frame fi */
#define framesamples(fi)   samples_matrix[fi->layer][fi->version]

/* the layer for headervalue h */
#define layer(h) 	   (4-(h))

extern int verbose;

/* framelength(fi) calculates the length (in bytes) of a frame */
static int framelength(file, fi)
	const file_info  * file;
	const frame_info * fi;
{
	/* if this is the last frame, it has a different length */
	if ((file->lastframe_offset==fi->offset)&&(file->lastframe_offset)) {
		return file->lastframe_length;
	}

	/* else, just compute the frame length */
	if (fi->layer==LAYER_1) {
		return 4*fi->padding+(48*fi->bitrate)/fi->samplerate;
	} else {
		return fi->padding+fi->samples*fi->bitrate/(8*fi->samplerate);
	}
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
	if (headervalue==3) return MPEG_VER_10;
	if (headervalue==2) return MPEG_VER_20;
	if (headervalue==0) return MPEG_VER_25;
	return MPEG_VER_INVALID;
}

void
parseframe(file, fi, buf)
	const file_info  * file;
	frame_info * fi;
	char * buf;
{
	int res;
	
	fi->version	=mpegver((buf[1]&24)>>3);	/* bits 12,13	*/
	fi->layer   	=layer((buf[1]&6)>>1);		/* bits 14,15	*/
	fi->crc16   	=!(buf[1]&1);			/* bit  16	*/
	fi->bitrate   	=bitrate((buf[2]&240)>>4, fi);	/* bits 17-20	*/
	fi->samplerate 	=samplerate((buf[2]&12)>>2, fi);/* bits 21,22	*/
	fi->padding    	=(buf[2]&2)  >>1;		/* bit  23	*/
	fi->private    	=(buf[2]&1);			/* bit  24	*/
	fi->stereo     	=(buf[3]&192)>>6;		/* bits 25,26	*/
	fi->jointstereo	=(buf[3]&48) >>4;		/* bits 27,28	*/
	fi->copyright  	=(buf[3]&8)  >>3;		/* bit  29	*/
	fi->original	=(buf[3]&4)  >>2;		/* bit  30	*/
	fi->emphasis   	=(buf[3]&3);			/* bits 31,32	*/
	
	fi->offset	=cftell(file->fp)-4;

	if (fi->crc16) {
		res=cfread(buf, 2, file->fp);
		if (!res) offseterror(file, " CRC read error");
		fi->crc16=(buf[0]&0xff)<<8;
		fi->crc16+=(buf[1]&0xff);
	}
}


void
calcframestats(file, fi) 
	const file_info  * file;
	frame_info * fi;
{
	fi->samples	=framesamples(fi);
	fi->length	=framelength(file, fi);
	fi->time	=frametime(fi);
}

/* checks the validity of struct frame_info fi */
int
checkvalidity(file, fi)
	const file_info * file;
	const frame_info * fi;
{
	/* These values are not valid */
	if (fi->version==MPEG_VER_INVALID) {
		if (verbose) offseterror(file, " unknown version");
		return FALSE;
	}
	if (fi->layer==4) {
		if (verbose) offseterror(file, " unknown layer (1-3 allowed)");
		return FALSE;
	}
	if (fi->bitrate<8000) {
		if (verbose) offseterror(file, " unknown bitrate");
		return FALSE;
	}
	if (fi->samplerate<8000) {
		if (verbose) offseterror(file, " unknown samplerate");
		return FALSE;
	}
	return TRUE;
}

/* checks for consistency */
int
checkconsistency(file, fi)
	const file_info * file;
	const frame_info * fi;
{
	if ((file->version==0)&&(file->layer==0)) {
		/* this is the first frame, nothing to check */
		return TRUE;
	}

	if (file->version!=fi->version) {
		if (verbose) offseterror(file, " version differs from previous frame");
		return FALSE;
	}
	if (file->layer != fi->layer) {
		if (verbose) offseterror(file, " layer differs from previous frame");
		return FALSE;
	}
	return TRUE;
}

/* puts data from frame_info in file_info */
int
parse_file_stats(file, fi)
	file_info * file;
	const frame_info * fi;
{
	file->frames++;
	file->lengthcount+=fi->length;
	if (file->bitrate!=fi->bitrate) {
		if ((!file->vbr)&&(file->bitrate)&&(fi->bitrate)) {
			file->vbr=TRUE;
		}
	}
	file->bitratecount+=fi->bitrate;
	file->bitrate=fi->bitrate;
	file->version=fi->version;
	file->layer=fi->layer;
	file->samplerate=fi->samplerate;
	file->stereo=fi->stereo;
	file->samples=fi->samples;

	file->alien_between+=file->alien_after;
	file->alien_after=0;
	return TRUE;
}

/* move the file pointer past the current frame */
static int
skipframe(file, fi)
	const file_info * file;
	const frame_info * fi;
{
	cfseek(file->fp, fi->offset+fi->length, SEEK_SET);
	return TRUE;
}

static int checkcrc16(file, fi)
	const file_info * file;
	frame_info * fi;
{
	char buf[34];
	int res;
	int len;
	int stdlen;
	
	if (fi->version==MPEG_VER_10) {
		stdlen=(fi->stereo==MONO ? 17 : 32);
	} else {
		stdlen=(fi->stereo==MONO ?  9 : 17);
	}
	
	cfseek(file->fp, fi->offset+2, SEEK_SET);
	res=cfread(buf, 2, file->fp);
	if (!res) {
		return FALSE;
	}

	cfseek(file->fp, fi->offset+6, SEEK_SET);
	len=MIN(stdlen, fi->length-6);
	res=cfread(buf+2, len, file->fp);
	if (res<len) {
		return FALSE;
	}

	cfseek(file->fp, fi->offset+fi->length, SEEK_SET);

	return fi->crc16==crc16(buf, len+2);
}
	
int
checkframe(file, buf)
	file_info	*file;
	char		*buf;
{
	frame_info _fi;
	frame_info *fi=&_fi;
	init_frame_info(fi);

	parseframe(file, fi, buf);

	if (!checkvalidity(file, fi)) {
		file->errors|=ERR_INVALID;
		return FALSE;
	}
	
	calcframestats(file, fi);
	
	if (!checkconsistency(file, fi)) {
		file->errors|=ERR_INCONSISTENT;
		return FALSE;
	}

	if (fi->crc16) {
		if (!checkcrc16(file, fi)) {
			file->errors|=ERR_FRAMECRC;
			return FALSE;
		}
	} else {
		skipframe(file, fi);
	}
	
	if (verbose>1) print_frame_info(fi, file);
	parse_file_stats(file, fi);

	return TRUE;
}

