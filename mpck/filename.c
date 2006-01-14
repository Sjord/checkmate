/*
 *   This file is part of Checkmate, a program to check MP3 files for errors
 *   
 *   Copyright (C)  2005  Sjoerd Langkemper
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
 *   filename.c - functions which have to do with filenames 
 * 
 */

#include "mpck.h"
#include <string.h>

/*
 * @return true if filename is ending in .extention
 * extention is something like "mp3", not starting with a dot
 * filename only matches if it ends in ".mp3" (thus with a dot)
 */
int extention_match(filename, extention)
	const char * filename;
	const char * extention;
{
	char * lastdot;
	
	lastdot=strrchr(filename, EXTENTION_MARK); /* search for last dot */
	if (lastdot==NULL) return FALSE;

	lastdot++;	/* remove the dot */
	if (strcmp(lastdot, extention)==0) {
		return TRUE;
	} else {
		return FALSE;
	}
}

static int
goodchar(ch)
	int ch;
{
	if ((ch>='a')&&(ch<='z')) return TRUE;
	if ((ch>='A')&&(ch<='Z')) return TRUE;
	if ((ch>='0')&&(ch<='9')) return TRUE;
	if (ch==' ') return TRUE;
	if (ch=='.') return TRUE;
	if (ch=='-') return TRUE;
	if (ch=='_') return TRUE;
	// FIXME use DIRSEP
	if (ch=='/') return TRUE;
	if (ch=='\\') return TRUE;
	return FALSE;
}

static int check_filename_length(filename, maxname)
	const char * filename;
{	
	int fnlen; 	/* filename length */

	fnlen=strlen(filename);
	
	if (fnlen>maxname) {
		return FALSE;
	} else {
		return TRUE;
	}
}

static int check_filename_chars(filename)
	const char * filename;
{
	int fnlen, ch, i;

	fnlen=strlen(filename);

	for (i=0; i<fnlen; i++) {
		ch=filename[i];
		if (!goodchar(ch)) {
			return FALSE;
		}
	}
	return TRUE;
}

void
check_filename(file, maxname, namecheck)
	file_info * 	file;
	int		maxname;
	int		namecheck;
{
	char * filename;

	/* strip directory name */
	filename=strrchr(file->filename, DIRSEP);
	if (filename==NULL) {
		filename=file->filename;
	} else {
		/* strip slash */
		filename++;
	}

	if (!check_filename_length(filename, maxname)) {
		file->errors|=ERR_LONGFNAME;
	}
	if ((namecheck)&&(!check_filename_chars(filename))) {
		file->errors|=ERR_BADFNAME;
	}
	
	if (strcmp(filename, PROGNAME)==0) printf("    I'm fine, thank you!\n");
}	


