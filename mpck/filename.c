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
#include "file.h"
#include "options.h"
#include <string.h>

#ifdef HAVE_TCHAR_H
#include <tchar.h>
#endif

/*
 * @return true if filename is ending in .extension
 * extension is something like "mp3", not starting with a dot
 * filename only matches if it ends in ".mp3" (thus with a dot)
 */
int extension_match(filename)
	const TCHAR * filename;
{
	TCHAR * lastdot;
	TCHAR * extension = options_get_extension();

	if (extension == NULL) {
		/* extension was not set */
		return TRUE;
	}
	
	lastdot=_tcsrchr(filename, EXTENSION_MARK); /* search for last dot */
	if (lastdot == NULL) return FALSE;

	lastdot++;	/* remove the dot */
#ifdef _WIN32
	return _tcsicmp(lastdot, extension) == 0;
#else
	return strcmp(lastdot, extension) == 0;
#endif
}

static int
goodchar(ch)
	TCHAR ch;
{
	if ((ch>='a') && (ch<='z')) return TRUE;
	if ((ch>='A') && (ch<='Z')) return TRUE;
	if ((ch>='0') && (ch<='9')) return TRUE;
	if (ch == ' ') return TRUE;
	if (ch == '.') return TRUE;
	if (ch == '-') return TRUE;
	if (ch == '_') return TRUE;
	if (ch == DIRSEP) return TRUE;
	return FALSE;
}

static int check_filename_length(filename)
	const TCHAR * filename;
{	
	int fnlen; 	/* filename length */
	int maxname = options_get_maxname();
	if (maxname == 0) return TRUE;

	fnlen=_tcslen(filename);
	
	if (fnlen>maxname) {
		return FALSE;
	} else {
		return TRUE;
	}
}

static int check_filename_chars(filename)
	const TCHAR * filename;
{
	int fnlen, i;
	TCHAR ch;

	fnlen=_tcslen(filename);

	for (i=0; i<fnlen; i++) {
		ch=filename[i];
		if (!goodchar(ch)) {
			return FALSE;
		}
	}
	return TRUE;
}

void
check_filename(file)
	file_info * 	file;
{
	TCHAR * filename;

	/* strip directory name */
	filename= _tcsrchr(file->filename, DIRSEP);
	if (filename == NULL) {
		filename=file->filename;
	} else {
		/* strip slash */
		filename++;
	}

	if (!check_filename_length(filename)) {
		file->errors |= ERR_LONGFNAME;
	}
	if ((options_get_namecheck()) && (!check_filename_chars(filename))) {
		file->errors |= ERR_BADFNAME;
	}
	
	if (_tcscmp(filename, PROGNAME) == 0) printf("    I'm fine, thank you!\n");
}	


