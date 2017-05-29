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
 *   file.h - keeps file stats
 * 
 */

#ifndef __FILE_H__
#define __FILE_H__

#include "frame.h"
#include "bufio.h"

/* structure which holds information for all files */
typedef struct _file_info file_info;

struct _file_info { 
	CFILE * fp;		/* filepointer to this file		*/
	char * filename;	/* filename of this file		*/
	int length;		/* length of this file (in bytes)	*/
	int lengthcount;	/* length of all frames			*/
	int time;		/* length of this file (in seconds)	*/
	int msec;		/* milliseconds left (see time)		*/
	int frames;		/* number of valid frames		*/
	int alien_total;	/* total number of unidentifiable bytes	*/
	int alien_before;	/* junk before all frames		*/
	int alien_between;	/* junk between first and last frame	*/
	int alien_after;	/* junk after all frames		*/
	int id3;		/* bitwise OR of 0, ID3V1, ID3V2.	*/
	size_t id3v2_size; 	/* total number of bytes in ID3v2 tag */
	int ape;		/* bitwise OR of 0, APEV1, APEV2.	*/
	size_t apev1_size;   	/* total number of bytes in APEv1 tag */
	size_t apev2_size;   	/* total number of bytes in APEv2 tag */
	int version;		/* MPEG version				*/
	int layer;		/* layer				*/
	int bitrate;		/* bitrate in bps			*/
	int bitratecount;	/* add bitrates to calculate an average	*/
	int stereo;		/* boolean				*/
	int samplerate;		/* samplerate in Hertz			*/
	int samples;		/* samples per frame			*/
	int vbr;		/* variable bit rate (boolean)		*/
	int errors;		/* is TRUE or >1 if errors are found	*/
	int lastframe_offset;	/* offset of last frame			*/
	int lastframe_length;	/* length of last frame			*/
	int ismp3file;		/* TRUE if this file is an mp3 file	*/
};

/* prototypes */
file_info *file_create(void);
void file_destroy(file_info *file);
int file_update(file_info *file, const frame_info *frame);
void file_final(file_info *file);
void file_clear(file_info *file);
void file_print(const file_info *file);
const TCHAR * file_strversion(const file_info * file);

#endif

