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
 *   checkarguments.c - parses passed filenames and calls checkfile on each
 * 
 */

#include "mpck.h"
#include "total.h"
#include "file.h"
#include "print.h"
#include "checkfile.h"
#include "filename.h"
#include "options.h"
#include "checkarguments.h"

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif

#ifdef HAVE_NDIR_H
#include <ndir.h>
#endif

#ifdef HAVE_SYS_DIR_H
#include <sys/dir.h>
#endif

#ifdef HAVE_SYS_NDIR_H
#include <sys/ndir.h>
#endif

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

static int
checkargument(filename, total, file)
	char * filename;
	total_info * total;
	file_info * file;
{
	int res;
	if (extension_match(filename)) {
		if (!options_get_quiet() && !options_get_badonly() && !options_get_xmloutput()) {
			print_scanning(filename);
		}
		res = checkfile(filename, file);
		file_print(file);
		total_update(total, file);
	} 
	return res;
}

#ifdef _WIN32

/* checks whether FindFileData is a directory.
 * only if _WIN32 is defined
 */
static int
fisdirectory(FindFileData)
	WIN32_FIND_DATA * FindFileData;
{
	return FindFileData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
}

/* wildcard_checkfile 
 * only if _WIN32 is defined
 */
int
wildcard_checkfile(wildcardpath, total)
	char 		* wildcardpath;
	total_info 	* total;
{
	WIN32_FIND_DATA FindFileData;	/* struct to store the found file */
	HANDLE hFind;
	int wcplen;		/* wildcard path length 		*/
	int res;		/* function result 			*/
	char * filepart;	/* part of wcp with the filename 	*/
	char * realfilename;	/* expanded filename 			*/
	file_info * file;	/* info of this file			*/

	wcplen=strlen(wildcardpath);
	realfilename=(char*)malloc(wcplen+MAXPATH);
	if (realfilename == NULL) {
		error(" malloc failed while doing wildcard expansion\n");
		return FALSE;
	}

	strcpy(realfilename, wildcardpath);
	filepart=strrchr(realfilename, DIRSEP);
	if (filepart == NULL) {
		filepart=realfilename; /* no backslash found */
	} else {
		filepart += 1; /* skip the backslash */
	}

	hFind=FindFirstFile(wildcardpath, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		error(" no file found: %s", wildcardpath);
		return FALSE;
	}

	file = file_create();
	do {
		strncpy(filepart, FindFileData.cFileName, MAXPATH);
		if (filepart[0] != '.') {
			if (options_get_recursive() && fisdirectory(&FindFileData)) {
				recursivecheck(realfilename, total);
			} else {
				checkargument(realfilename, total, file);
			}
		}
		res=FindNextFile(hFind, &FindFileData);
		if (res == 0) res=GetLastError();
	} while (res != ERROR_NO_MORE_FILES);
	file_destroy(file);

	FindClose(hFind);
	free(realfilename);
	return TRUE;
}

/* WIN32 checkarguments calls wildcard_checkfile for each argument.
 * only if _WIN32 is defined
 */
int
checkarguments(argv, total)
	char		**argv;
	total_info 	* total;
{
	int res=TRUE;

	total_clear(total);
	do {
		res=res && wildcard_checkfile(*argv, total);
		argv++;
	} while (*argv);

	return res;
}

/* only if _WIN32 is defined 
 */
int
recursivecheck(dirname, total)
	const char *dirname;
	total_info * total;
{
	char * wildcarddir;
	int dirlen;

	dirlen=strlen(dirname);
	wildcarddir=(char *)malloc(dirlen+MAXPATH);
	if (wildcarddir == NULL) {
		error(" malloc failed");
		return FALSE;
	}
	
	strcpy(wildcarddir, dirname);
	strcat(wildcarddir, "\\*");

	wildcard_checkfile(wildcarddir, total);
	free(wildcarddir);
	return TRUE;
}

#else /* !_WIN32 */

/* recursively scans all the files in dirname */
int
recursivecheck(dirname, total)
	const char *dirname;
	total_info * total;
{
	DIR * dir;
	struct dirent * ent;
	char * filename;
	int dirlen;
	file_info * file;

	dirlen=strlen(dirname);
	filename=(char *)malloc(2+dirlen+256);
	if (filename == NULL) {
		error(" malloc failed while doing recursion\n");
		return FALSE;
	}
	strcpy(filename, dirname);
	filename[dirlen]='/';


	dir=opendir(dirname);
	if (dir == NULL) {
		error(" error opening directory %s", dirname);
		return FALSE;
	}

	file = file_create();
	while ((ent=readdir(dir)) != NULL) {
		if (ent->d_name[0] == '.') continue; /* hidden files, . and .. */
		strncpy(filename+dirlen+1, ent->d_name, 255);
		if (cisdirectory(filename)) {
			recursivecheck(filename, total);
		} else {
			checkargument(filename, total, file);
		}
	}
	closedir(dir);
	file_destroy(file);
	free(filename);
	
	return TRUE;
}

/* checkarguments opens each file and calls checkfile */
int
checkarguments(argv, total)
	char		**argv;
	total_info	* total;
{
	int res=TRUE;
	char * filename;
	file_info * file;
	
	file = file_create();
	do {
		filename=*argv;
		if (options_get_recursive() && cisdirectory(filename)) {
			recursivecheck(filename, total);
		} else {
			res &= checkargument(filename, total, file);
		}
		argv++;
	} while (*argv); 
	file_destroy(file);

	return res;
}

#endif /* !_WIN32 */


