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
 *   ape.c - functions that do something with ape tags 
 * 
 */

#include "mpck.h"
#include "file.h"

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

extern int verbose;

/* Skips an id3v2 tag. The first three characters have already been read. */
int
skip_ape_tag(file)
	file_info * file;
{
	char buf[25];
	int res;
	
	uint32_t version;	/* version of APE tag 	*/
	uint32_t flags;		/* flags			 */
	uint32_t items;	    /* number of items in tag	*/
	uint32_t size;		/* size of tag without header (but including footer) */

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
