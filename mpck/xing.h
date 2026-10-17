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
 *   xing.h - reads the Xing/LAME header from the first MP3 frame
 * 
 */

#ifndef __XING_H__
#define __XING_H__

#include "file.h"
#include "frame.h"

/* flags in the Xing header */
#define XING_FLAG_FRAMES	0x0001
#define XING_FLAG_BYTES		0x0002
#define XING_FLAG_TOC		0x0004
#define XING_FLAG_VBR_SCALE	0x0008
/* internal flags: fields in the LAME extension */
#define XING_FLAG_MUSICLENGTH	0x0010
#define XING_FLAG_MUSICCRC	0x0020

int xing_read(file_info *file, const frame_info *frame);

#endif
