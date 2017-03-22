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
 *   listview.h
 * 
 */

#include <windows.h>
#include <commctrl.h>

/* definitions */

/* prototypes */
HWND LV_GetHwnd(void);
HWND LV_Create(HWND hwndParent);
BOOL LV_DeleteSelected(void);
int LV_GetSelectedCount(void);
BOOL LV_StartRename(void);
BOOL LV_Refresh(void);
BOOL LV_RenameFile(int index, char * newname);
BOOL LV_Resize(int width, int height);
BOOL LV_ActivateSelected(void);
BOOL LV_Activate(int item);
void LV_Destroy(void);
RECT * LV_GetRect(RECT * rc);
BOOL LV_GetPopupMenuPosition(POINT * ppt);
BOOL LV_Scan(int item, int * filetype);
BOOL LV_ScanSelected();
BOOL LV_DirUp();
int LV_SelectAll();
int LV_SortColumn(int column);
BOOL LV_OnKey(NMLVKEYDOWN * key);
BOOL LV_OpenSelected();

typedef struct {
	int identifier;
	int  alignment;
	int  width;
} ColInfo;

typedef struct {
	char name[255];
	int  filesize;
} ItemInfo;

enum {
	COL_FILENAME,
	COL_RESULT,
	COL_VERSION,
	COL_LAYER,
	COL_BITRATE,
	COL_VBR,
	COL_SAMPLERATE,
	COL_FRAMES,
	COL_TIME,
	COL_SIZE,
};