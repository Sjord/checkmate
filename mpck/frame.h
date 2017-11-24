/*
 *   This frame is part of Checkmate, a program to check MP3 frames for errors
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
 *   frame.h - keeps frame stats
 * 
 */

#ifndef __FRAME_H__
#define __FRAME_H__

/* structure which holds information for all frames */
typedef struct _frame_info frame_info;

struct _frame_info {
	int version;		/* MPEG version				*/
	int layer;		/* layer				*/
	int bitrate;		/* bitrate in bits per second		*/
	int samplerate;		/* samplerate in Hertz			*/
	int padding;		/* padding (bool)			*/
	int private;		/* private (bool)			*/
	int stereo;		/* stereo (4 modes)			*/
	int jointstereo;	/* joint stereo				*/
	int copyright;		/* copyright (bool)			*/
	int original;		/* original recording			*/
	int emphasis;		/* emphasis				*/
	int length;		/* length in bytes			*/
	int time;		/* time in miliseconds			*/
	int samples;		/* number of samples			*/
	int crc16;		/* crc16 (0==not available, or crc16 value) */
	int offset;		/* offset of this frame			*/
};

/* prototypes */
frame_info *frame_create(void);
void frame_destroy(frame_info *frame);
void frame_clear(frame_info *frame);
void frame_print(const frame_info *fi);

#endif
