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
 *   frame.c - keeps frame stats
 * 
 */

#include "mpck.h"
#include "frame.h"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

frame_info *
frame_create() 
{
	frame_info * frame;
	
	frame = (frame_info *) malloc(sizeof(frame_info));
	if (frame == NULL) return NULL;
	frame_clear(frame);
	return frame;
}

void frame_destroy(frame)
	frame_info * frame;
{
	free(frame);
}

void
frame_clear(frame) 
	frame_info * frame;
{
	memset(frame, 0, sizeof(frame_info));
}

void
frame_print(fi)
        const frame_info *fi;
{
        printf("FRAME");
        printf("    %-30s%d\n", "version", (fi->version));
        printf("    %-30s%d\n", "layer", fi->layer);
        printf("    %-30s%d bps\n", "bitrate", fi->bitrate);
        printf("    %-30s%d Hz\n", "samplerate", fi->samplerate);
        printf("    %-30s%d b\n", "frame_length", fi->length);
        printf("    %-30s%d b (0x%x)\n", "offset", fi->offset, fi->offset);
        printf("    %-30s%d ms\n", "time", fi->time);
        printf("    %-30s%u\n", "crc16", fi->crc16);
	printf("    %-30s%d\n", "padding", fi->padding);
        printf("    %-30s%d\n", "stereo", fi->stereo);
        printf("    %-30s%d\n", "joint stereo", fi->jointstereo);
        printf("    %-30s%d\n", "copyright", fi->copyright);
        printf("    %-30s%d\n", "emphasis", fi->emphasis);
        printf("    %-30s%d\n", "original", fi->original);
        printf("    %-30s%d\n", "samples", fi->samples);
}
