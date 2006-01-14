/*
 *   This file is part of Checkmate MP3 checker, a program to check MP3 files for errors
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
 *   fileinfo.c - handles the fileinfo "object"
 * 
 * Currently, the FI_Create function creates 26 vectors, in which the files
 * are stored. Which vector is used depends on the filename. The number of
 * the vector is first_letter%26. This could be somewhat improved for speed.
 *
 * Furthermore, an handle to an object/struct should be returned, because
 * static variables are Bad.
 *
 * FIXME: improve for speed
 * FIXME: not thread safe
 * 
 *
 */

#include "mp3checker.h"
#include "vector.h"

#define CVECTORS 26

/* FIXME this is not the right way to do this: */
/* e.g. not thread-safe */
static Vector * fileinfo[CVECTORS];

/* inits the fileinfo */
/* FIXME return something useful */
BOOL FI_Create() {
	int i;
	for (i=0; i<CVECTORS; i++) {
		fileinfo[i]=Vector_Create();
	}
	return TRUE;
}

/* clears all vectors in fileinfo */
void FI_Destroy() {
	int i;
	for (i=0; i<CVECTORS; i++) {
		Vector_Destroy(fileinfo[i]);
	}
}

void FI_AddFile(FileInfo * fi) {
	Vector * v;
	v=fileinfo[fi->filename[0]%CVECTORS];
	Vector_Add(v, fi);
}

/* returns info about the file with this filename and in this directory */
FileInfo * FI_GetFile(char * filename, char * dirname) {
	Vector * v;
	FileInfo * fi;
	int i=0;

	v=fileinfo[filename[0]%CVECTORS];
	while (fi=(FileInfo *)Vector_Get(v, i++)) {
		if ((stricmp(filename, fi->filename)==0)
			&&(stricmp(dirname, fi->dirname)==0)) {
			return fi;
		}
	}
	return NULL;
}

/* free all fi->info and fi */
BOOL FI_FreeAll() {
	int i, index=0;
	Vector * v;
	FileInfo * fi;

	for (i=0; i<CVECTORS; i++) {
		v=fileinfo[i];
		while (NULL!=(fi=Vector_Get(v, index++))) {
			if (fi->info) HeapFree(GetProcessHeap(), 0, fi->info);
			HeapFree(GetProcessHeap(), 0, fi);
		}
	}
	return TRUE;
}

/* returns filetype identifier for the file in FileData */
int FI_GetFileType(WIN32_FIND_DATA * FileData) {
	if (FileData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		return FILE_DIRECTORY;
	} else {
		return FILE_NORMAL;
	}
}	