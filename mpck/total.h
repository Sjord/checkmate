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
 *   total.h - keeps total stats
 * 
 */

#ifndef __TOTAL_H__
#define __TOTAL_H__

#include "file.h"

/* structure which holds information for all files */
typedef struct _total_info total_info;

struct _total_info {
	int filecount;		/* number of files processed		*/
	int minbitrate;		/* minimum bitrate			*/
	int maxbitrate;		/* maximum bitrate			*/
	int totalbitrate;	/* count of bitrates, to calculate avg	*/
	int goodcount;		/* number of good files			*/
	int time;		/* total time				*/
};

/* prototypes */
total_info *total_create(void);
void total_destroy(total_info *total);
int total_update(total_info *total, const file_info *file);
void total_print(const total_info *total);
int total_allgood(const total_info *total);
void total_clear(total_info *total);

#endif
