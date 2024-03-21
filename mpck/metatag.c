/*
 *   This file is part of mpck, a program to check MP3 files for errors
 *   
 *   Copyright (C)  2006  Sjoerd Langkemper
 *   
 *   mpck is free software; you can redistribute it and/or modify
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
 *   metatag.c - functions that do something with metadata tags
 * 
 */

#include "mpck.h"
#include "file.h"
#include "metatag.h"
#include "synchsafe.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif

extern int verbose;

/* Skips an id3v1 tag. The first three characters have already been read. */
int
skip_id3v1_tag(file_info * file)
{
	file->id3 |= ID3V1;
	cfseek(file->fp, 125, SEEK_CUR);
	return TRUE;
}

/* Skips an id3v2 tag. The first three characters have already been read. */
int
skip_id3v2_tag(file_info * file)
{
	char buf[8];
	size_t res;

	int version, revision;		/* version of ID3v2 tag 	*/
	int flags;			/* flags			*/
	int size;			/* size of tag without header	*/

	res=cfread(buf, 7, file->fp);	/* read rest of header		*/
	if (res == 0) return FALSE;
	version  = (int)(buf[0]);
	revision = (int)(buf[1]);
	flags    = (int)(buf[2]);
	size     = INT_SS(buf+3);

	file->id3 |= ID3V2;
	file->id3v2_size = size;
	cfseek(file->fp, size, SEEK_CUR);

	return 0;
}

/* Skips an APE tag. The first eight characters have already been read. */
int
skip_ape_tag(file_info * file)
{
	char buf[25];
	size_t res;

	unsigned long version;	/* version of APE tag 		*/
	unsigned long flags;	/* flags			*/
	unsigned long items;	/* number of items in tag	*/
	unsigned long size;	/* size of tag without header (but including footer) */

	res=cfread(buf, 24, file->fp);	/* read rest of header		*/
	if (res == 0) return FALSE;

	memcpy(&version, buf, 4);
	memcpy(&size, buf+4, 4);
	memcpy(&items, buf+8, 4);
	memcpy(&flags, buf+12, 4);

	//only seek ahead if this is version 2,
	//apev1 only has footers, so we already
	//passed all the data to get here
	if (version == 2000) {
		file->ape |= APEV2;
		file->apev2_size = size;
		cfseek(file->fp, size, SEEK_CUR);
	} else {
		file->ape |= APEV1;
		file->apev1_size = size;
	}

	return 0;
}
