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
 *   mp3checker.h
 * 
 */

#define WINVER 0x0501
#define _WIN32_WINNT 0x0501		/* fix API version, specifically so that the rebar control works*/

#include "listview.h"
#include "mpck.h"
#include "file.h"

/* structs */

typedef struct _DirInfo {
	int cFiles;
	int cGood;
	int cBad;
	int cIsMP3;
} DirInfo;

/* FileInfo
 * when a file is scanned, file_info * info is pointing to even
 * more info. Otherwise, it is NULL. Filesize is here as well as
 * in /info/ because mpck (the back-end) needs to know it
 * beforehand.
 */
typedef struct _FileInfo FileInfo;
struct _FileInfo {
	int filetype;
	char filename[MAX_PATH];
	char dirname[MAX_PATH];
	union {
		file_info * info;
		DirInfo	  * dirinfo;
	};
	UINT64 filesize;
};



/* definitions */
#define MP3CHECKER_CLASSNAME		"MP3Checker"
#define MP3CHECKER_TITLE			"Checkmate MP3 Checker"	/* window name */

/* icons */
enum{
 IDI_DIR=101,
 IDI_DIRBAD,
 IDI_DIRGOOD,
 IDI_DIRUNKNOWN,
 IDI_FILE,
 IDI_FILEBAD,
 IDI_FILEGOOD,
 IDI_FILEUNKNOWN,
 IDI_LASTICON};

#define IDI_FIRSTICON	IDI_DIR
#define C_ICONS			(IDI_LASTICON-IDI_FIRSTICON)

#define CBN_CHAR		WM_USER+1

 /* filetypes */
 /* for directories, the following holds:
		FILE_DIRECTORY	is set
		FILE_SCANNED	is set only if the directory was
						scanned
		FILE_GOOD		is set only if all MP3 files in this
						directory are good
		FILE_BAD		is set when at least one MP3 file is
						bad
		FILE_ISMP3		is set when at least one MP3 file
						exists in this directory
  */
#define FILE_NORMAL		(1<<0)		// 1
#define FILE_DIRECTORY	(1<<1)		// 2
#define FILE_GOOD		(1<<2)		// 4
#define FILE_BAD		(1<<3)		// 8
#define FILE_ISMP3		(1<<4)		// 16
#define FILE_SCANNED    (1<<5)		// 32

/* prototypes */
LRESULT CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
int ErrorBox(int iError, UINT uType);

