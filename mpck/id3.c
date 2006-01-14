/*
 *   This file is part of mpck, a program to check MP3 files for errors
 *   
 *   Copyright (C)  2003  Sjoerd Langkemper
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
 *   id3.c - functions that do something with id3 tags 
 * 
 */

#include "mpck.h"
#include <stdio.h>
#include <stdlib.h> // malloc
#include "synchsafe.h"

extern int verbose;

int
read_id3v1_tag(file)
	file_info * file;
{
	char buf[31];
	int res=0;
	CFILE * fp=file->fp;
	
	res+=cfread(buf, 30, fp); 	// title
	res+=cfread(buf, 30, fp); 	// artist
	res+=cfread(buf, 30, fp); 	// album
	res+=cfread(buf, 4, fp);  	// year
	res+=cfread(buf, 30, fp);	// comment (and maybe track #)
	res+=cfread(buf, 1, fp);	// genre
	
	if (res<125) offseterror(file, " error while reading id3v1 tag");

	file->id3=file->id3 | ID3V1;
	
	return 0;
}

/* parses an id3v2 tag. The first three letters "ID3" have already been read. */
int
read_id3v2_tag(file)
	file_info * file;
{
	char buf[8];
	char * pnt;
	int res;
	CFILE * fp=file->fp;
	
	int version, revision;		// version of ID3v2 tag
	int flags;			// flags
	int size;			// size of tag without header

	res=cfread(buf, 7, fp);	// read rest of header
	if (res==0) return FALSE;
	version  = (int)(buf[0]);
	revision = (int)(buf[1]);
	flags    = (int)(buf[2]);
	size     = IVAL_SS(buf, 3);

	if (version==0xff) {
		if (verbose) offseterror(file, "ID3v2 tag has version 255");
		return FALSE;
	}
	if (revision==0xff) {
		if (verbose) offseterror(file, "ID3v2 tag has revision 255");
		return FALSE;
	}
	if (flags==0xff) {
		if (verbose) offseterror(file, "ID3v2 tag has flags 255");
		return FALSE;
	}
	if ((size>65535)||(size<0)) {
		if (verbose) offseterror(file, "ID3v2 size seems incorrect");
		return FALSE;
	}
	// TODO: Check for 0xff in the /size/ bytes

	// FIXME: Read the whole tag
	pnt=malloc(size);
	cfread(pnt, size, fp);
	free(pnt);
	
	file->id3=file->id3 | ID3V2;
	
	return 0;
}
