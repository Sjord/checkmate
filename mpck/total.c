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
 *   total.c - keeps total stats
 * 
 */

#include "mpck.h"
#include "total.h"
#include "options.h"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

total_info *
total_create() 
{
	total_info * total;
	
	total = (total_info *) malloc(sizeof(total_info));
	if (total == NULL) return NULL;
	return total;
}

void total_destroy(total)
	total_info * total;
{
	free(total);
}

/*
 * puts stats from file (bitrate, file count) into the total stats
 */
int
total_update(total, file)
	total_info       * total;
	const file_info  * file;
{
	/* don't do stats for files which aren't MP3s */
	if (!file->ismp3file) return TRUE;

	total->filecount++;

	if (!file->errors) total->goodcount++;
	total->time += file->time;
	total->totalbitrate += file->bitrate;
	if ((!total->minbitrate)||(file->bitrate < total->minbitrate)) {
		total->minbitrate=file->bitrate;
	}
	if (file->bitrate > total->maxbitrate) {
		total->maxbitrate=file->bitrate;
	}
	return TRUE;
}

void
total_print(total)
	const total_info * total;
{
	if (total->filecount<2) return;
	if (options_get_quiet()) return;
	printf("TOTAL:\n");
	printf("    %-30s%d\n", "number of files", total->filecount);
	printf("        %-26s%d\n", GOODFILE, total->goodcount);
	printf("        %-26s%d\n", BADFILE, total->filecount-total->goodcount);
	printf("    %-30s%d:%02d:%02d\n", "time",
		total->time/3600, (total->time/60)%60, total->time%60);
	printf("    %-30s\n", "bitrate");
	printf("        %-26s%d\n", "minimum", total->minbitrate);
	printf("        %-26s%d\n", "average", total->totalbitrate/total->filecount);
	printf("        %-26s%d\n", "maximum", total->maxbitrate);
	printf("\n");
}
	
int
total_allgood(total) 
	const total_info * total;
{
	return total->goodcount == total->filecount;
}

void
total_clear(total) 
	total_info * total;
{
	memset(total, 0, sizeof(total_info));
}
