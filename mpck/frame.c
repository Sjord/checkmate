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
