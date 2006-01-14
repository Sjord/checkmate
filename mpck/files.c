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
 *   files.c - parses passed filenames and calls check_arg on each
 * 
 */

#include "mpck.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif

static int recursive;

int 
set_recursive(val)
	int val;
{
	return recursive=val;
}
	

#ifdef _WIN32
/* WIN32 parse_args calls wildcard_parse_file for each argument.
 * only if _WIN32 is defined
 */
int
parse_args(argv)
	char		**argv;
{
	int res=TRUE;

	do {
		res=res && wildcard_check_arg(*argv);
		argv++;
	} while (*argv);

	return res;
}

/* wildcard_parse_file 
 * only if _WIN32 is defined
 */
// FIXME recursively scan directories if required
int
wildcard_check_arg(wildcardpath)
	char * wildcardpath;
{
	WIN32_FIND_DATA FindFileData;	/* struct to store the found file */
	HANDLE hFind;
	int wcplen;		/* wildcard path length 		*/
	int res;		/* function result 			*/
	char * filepart;	/* part of wcp with the filename 	*/
	char * realfilename;	/* expanded filename 			*/

	wcplen=strlen(wildcardpath);
	realfilename=(char*)malloc(wcplen+MAXPATH);
	if (realfilename==NULL) {
		error(" malloc failed while doing wildcard expansion\n");
		return FALSE;
	}

	strcpy(realfilename, wildcardpath);
	filepart=strrchr(realfilename, DIRSEP);
	if (filepart==NULL) {
		filepart=realfilename; /* no backslash found */
	} else {
		filepart+=1; /* skip the backslash */
	}

	hFind=FindFirstFile(wildcardpath, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		error(" no file found: %s", wildcardpath);
//		error(" %s: I searched for a while, but I cannot find this file.", wildcardpath);
		return FALSE;
	}

	do {
		if (FindFileData.cFileName[0]!='.') { /* skip ".*" */
			strncpy(filepart, FindFileData.cFileName, MAXPATH);
			check_arg(realfilename);
		}
		res=FindNextFile(hFind, &FindFileData);
		if (res==0) res=GetLastError();
	} while (res!=ERROR_NO_MORE_FILES);

	FindClose(hFind);
	free(realfilename);
	return TRUE;
}

/* only if _WIN32 is defined 
 */
int
recursivecheck(dirname)
	const char *dirname;
{
	char * wildcarddir;
	int dirlen;

	dirlen=strlen(dirname);
	wildcarddir=(char *)malloc(dirlen+MAXPATH);
	if (wildcarddir==NULL) {
		error(" malloc failed");
		return FALSE;
	}
	
	strcpy(wildcarddir, dirname);
	strcat(wildcarddir, "\\*");

	wildcard_check_arg(wildcarddir);
	free(wildcarddir);
	return TRUE;
}

#else /* !_WIN32 */

/* parse_args opens each file and calls check_arg */
int
parse_args(argv)
	char		**argv;
{
	int res=TRUE;
	char * filename;
	
	do {
		filename=*argv;
		if ((recursive)&&(cisdirectory(filename))) {
			recursivecheck(filename);
		} else {
//			error(" %s is a directory; use -R for recursive checking", filename);
			res=res && check_arg(*argv);
		}
		argv++;
	} while (*argv); 

	return res;
}

/* recursively scans all the files in dirname */
int
recursivecheck(dirname)
	const char *dirname;
{
	DIR * dir;
	struct dirent * ent;
	char * filename;
	int dirlen;

	dirlen=strlen(dirname);
	filename=(char *)malloc(2+dirlen+256);
	if (filename==NULL) {
		// error(" malloc failed while doing recursion\n");
		error(" malloc failed while doing infinite recursion\n");
		return FALSE;
	}
	strcpy(filename, dirname);
	filename[dirlen]='/';


	dir=opendir(dirname);
	if (dir==NULL) {
		error(" error opening directory %s", dirname);
		return FALSE;
	}
	while ((ent=readdir(dir))!=NULL) {
		if (ent->d_name[0]=='.') continue; /* hidden files, . and .. */
		strncpy(filename+dirlen+1, ent->d_name, 255);
		if (cisdirectory(filename)) {
			recursivecheck(filename);
		} else {
			check_arg(filename);
		}
	}
	closedir(dir);
	free(filename);
	
	return TRUE;
}

#endif /* !_WIN32 */


