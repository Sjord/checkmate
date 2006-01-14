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
 *   listview.c - handles the listview and the file info storage
 * 
 */


#include <windows.h>
#include <commctrl.h>
#include "mp3checker.h"
#include "vector.h"
#include "fileinfo.h"
#include "resource.h"
#include "rebar.h"
#include "sort.h"

extern HINSTANCE hInst;
static HWND hWndListView;	/* the hWnd of the listview						*/
/* FIXME: not thread safe */
static Vector * CurrentVector;		/* holds information on the current directory	*/
int (__cdecl *CurrentSort )(const void *, const void *); /* callback to sort function */

/* initializes the vector which holds information on the files in the current
 * directory.
 */
BOOL LV_InitVector() {
	CurrentVector=Vector_Create();
	CurrentSort=NormalAscendingSort;
	return TRUE;
}

BOOL LV_OnKey(NMLVKEYDOWN * key) {
	char buf[15];
	sprintf(buf, "%d\n", key->wVKey);
	OutputDebugString(buf);
	switch (key->wVKey) {
		case VK_BACK:
			return LV_DirUp();
		case VK_RETURN:
			return LV_ActivateSelected();
		case VK_DELETE:
			return LV_DeleteSelected();
		case VK_F6:
			return LV_ScanSelected();
		case VK_F7:
			return LV_OpenSelected();
	}
	return FALSE;
}

/* destroys vector */
void LV_DestroyVector() {
	FileInfo * fi;
	int index=0;

	/* free all elements */
	while (NULL!=(fi=Vector_Get(CurrentVector, index++))) {
		/* leave entries which have an fi->info alone,
		   since they are also in FileInfo and get free()d over there */
		if (fi->info==NULL) HeapFree(GetProcessHeap(), 0, fi);
	}
	Vector_Destroy(CurrentVector);
}

/* makes an image list with the icons for the files
 */
static BOOL LV_InitImages(HWND hWndListView) {
	HICON hiconItem;     // icon for list-view items 
	HIMAGELIST hLarge;   // image list for icon view 
	HIMAGELIST hSmall;   // image list for other views 
	int i;

	hLarge = ImageList_Create(GetSystemMetrics(SM_CXICON), 
		GetSystemMetrics(SM_CYICON), ILC_MASK | ILC_COLOR8, 1, 1); 
    hSmall = ImageList_Create(GetSystemMetrics(SM_CXSMICON), 
		GetSystemMetrics(SM_CYSMICON), ILC_MASK | ILC_COLOR8, 1, 1); 

	for (i=0; i<C_ICONS; i++) {
		hiconItem=LoadIcon(hInst, MAKEINTRESOURCE(IDI_FIRSTICON+i)); 
		ImageList_AddIcon(hLarge, hiconItem); 
	    ImageList_AddIcon(hSmall, hiconItem); 
		DestroyIcon(hiconItem); 
	}

    ListView_SetImageList(hWndListView, hLarge, LVSIL_NORMAL); 
    ListView_SetImageList(hWndListView, hSmall, LVSIL_SMALL); 
    return TRUE; 
} 


/*
 * adds columns to the ListView control
 * returns true on success, false on failure
 */
static BOOL LV_InitColumns(HWND hWndListView) { 
    LVCOLUMN lvc; 
	int iCol;
	int nElem;
	char buf[256];

	ColInfo colinfo[]={
		/* identifier      | alignment     | width */
		{IDS_FILENAME,		LVCFMT_LEFT,	200},
		{IDS_RESULT,		LVCFMT_LEFT,	 80},
		{IDS_VERSION,		LVCFMT_LEFT,	 80},
		{IDS_LAYER,			LVCFMT_RIGHT,	 40},
		{IDS_BITRATE,		LVCFMT_RIGHT,	 80},
		{IDS_VBR,			LVCFMT_RIGHT,	 40},
		{IDS_SAMPLERATE,	LVCFMT_RIGHT,	 80},
		{IDS_FRAMES,		LVCFMT_RIGHT,	 50},
		{IDS_TIME,			LVCFMT_RIGHT,	 40},
		{IDS_SIZE,			LVCFMT_RIGHT,	 60}
	};
	nElem=sizeof(colinfo)/sizeof(ColInfo);

    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
	  
    for (iCol=0; iCol<nElem; iCol++) 
	{ 
        lvc.iSubItem	= iCol;
		LoadString(hInst, colinfo[iCol].identifier, buf, 255);
        lvc.pszText		= buf;	
        lvc.cx			= colinfo[iCol].width;
        lvc.fmt			= colinfo[iCol].alignment;

        if (ListView_InsertColumn(hWndListView, iCol, &lvc) == -1) 
            return FALSE; 
    } 
    return TRUE; 
}


/*
 * fills CurrentVector with information about this file
 */
static BOOL LV_UpdateFindFile(WIN32_FIND_DATA * FileData) {
	FileInfo * fi;
	char curdir[255];
	GetCurrentDirectory(255, curdir);

	fi=FI_GetFile(FileData->cFileName, curdir);
	if (fi==NULL) {		/* no information is available for this file */
		fi=HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(FileInfo));
		strcpy(fi->filename, FileData->cFileName);
		strcpy(fi->dirname, curdir);
		fi->filesize=(FileData->nFileSizeHigh*(MAXDWORD+1))+FileData->nFileSizeLow;
		fi->filetype=FI_GetFileType(FileData);
	}
	Vector_Add(CurrentVector, fi);
	return TRUE;
}

/* returns the icon identifier for this filetype */
static int LV_GetIcon(int filetype) {
	if (filetype & FILE_DIRECTORY) {
		/* directory */
		if (!(filetype & FILE_SCANNED)) {
			/* dir not yet scanned */
			return IDI_DIR - IDI_FIRSTICON;
		} else {
			if (filetype & FILE_GOOD) {
				/* all good mp3s */
				return IDI_DIRGOOD - IDI_FIRSTICON;
			} else if (filetype & FILE_BAD) {
				/* at least one bad mp3 */
				return IDI_DIRBAD - IDI_FIRSTICON;
			} else {
				/* no MP3 files at all */
				return IDI_DIRUNKNOWN - IDI_FIRSTICON;
			}
		}
	} else {
		/* normal file */
		if (!(filetype & FILE_SCANNED)) {
			/* file not yet scanned */
			return IDI_FILE - IDI_FIRSTICON;
		}
		if (filetype & FILE_ISMP3) {
			if (filetype & FILE_GOOD) {
				/* good mp3 */
				return IDI_FILEGOOD - IDI_FIRSTICON;
			} else {
				/* bad mp3 */
				return IDI_FILEBAD - IDI_FIRSTICON;
			}
		} else {
			/* no mp3 */
			return IDI_FILEUNKNOWN - IDI_FIRSTICON;
		}
	}
}

/* returns the next selected item, starting from start */
static int LV_FindSelected(int start) {
	return ListView_GetNextItem(hWndListView, start, LVNI_SELECTED);
}

/* does the actual renaming of the file indicated by index */
BOOL LV_RenameFile(int index, char * newname) {
	FileInfo * fi;
	BOOL retval;
	
	if (newname==NULL) return FALSE;
	
	fi=Vector_Get(CurrentVector, index);
	if (fi==NULL) return FALSE;

	retval=MoveFile(fi->filename, newname);
	if (retval) {
		/* update vector */
		strcpy(fi->filename, newname);
	} else {
		ErrorBox(IDS_RENAMEFAILED, MB_OK | MB_ICONEXCLAMATION);
	}
	return retval;
}

/* resizes the listview */
BOOL LV_Resize(int width, int height) {
	int y=Rebar_Height();
	return MoveWindow(hWndListView,
		0,				/* x		*/
		y,				/* y		*/
		width,			/* width	*/
		height-y,		/* height	*/
		TRUE);			/* repaint  */

}

/* adds an item and sets the image and filename */
static BOOL LV_AddItem(HWND hWndListView, FileInfo * fi, int item) {
	LVITEM lv;

	lv.mask=LVIF_TEXT | LVIF_IMAGE;
	lv.iItem=item;
	lv.iSubItem=0;
	lv.pszText=fi->filename;
	lv.iImage=LV_GetIcon(fi->filetype);
	return ListView_InsertItem(hWndListView, &lv);
}



static BOOL LV_UpdateDirItem(HWND hWndListView, FileInfo * fi, int item) {
	char buf[20];
	LVITEM lv;
	int iRes;

	lv.iItem=item;
	lv.mask=LVIF_IMAGE;
	lv.iSubItem=0;
	lv.iImage=LV_GetIcon(fi->filetype);
	iRes=ListView_SetItem(hWndListView, &lv);
	if (!iRes) return FALSE;

	lv.mask=LVIF_TEXT;

	/* result */
	lv.iSubItem=1;
	sprintf(buf, "%d/%d", fi->dirinfo->cGood, fi->dirinfo->cIsMP3);
	lv.pszText=buf;
	iRes=ListView_SetItem(hWndListView, &lv);
	if (!iRes) return FALSE;

	return TRUE;
}

static BOOL LV_UpdateFileItem(HWND hWndListView, FileInfo * fi, int item) {
	char buf[20];
	LVITEM lv;
	int iRes;

	lv.iItem=item;

	lv.mask=LVIF_IMAGE;
	lv.iSubItem=0;
	lv.iImage=LV_GetIcon(fi->filetype);
	iRes=ListView_SetItem(hWndListView, &lv);
	if (!iRes) return FALSE;

	lv.mask=LVIF_TEXT;

	/* result */
	lv.iSubItem=1;

	if (!fi->info->ismp3file) {
		LoadString(hInst, IDS_NOMP3, buf, 19);
	} else if (fi->info->errors) {
		LoadString(hInst, IDS_BROKEN, buf, 19);
	} else {
		LoadString(hInst, IDS_OK, buf, 19);
	}
	lv.pszText=buf;
	iRes=ListView_SetItem(hWndListView, &lv);
	if (!iRes) return FALSE;

	if (fi->info->ismp3file) {
		/* version */
		lv.iSubItem=2;
		lv.pszText=strversion(fi->info->version);
		iRes=ListView_SetItem(hWndListView, &lv);
		if (!iRes) return FALSE;

		/* layer */
		lv.iSubItem=3;
		sprintf(buf, "%d", fi->info->layer);
		lv.pszText=buf;
		iRes=ListView_SetItem(hWndListView, &lv);
		if (!iRes) return FALSE;

		/* bitrate */
		lv.iSubItem=4;
		sprintf(buf, "%d bps", fi->info->bitrate);
		lv.pszText=buf;
		iRes=ListView_SetItem(hWndListView, &lv);
		if (!iRes) return FALSE;

		/* vbr/cbr */
		lv.iSubItem=5;
		if (fi->info->vbr) {
			lv.pszText="VBR";
		} else {
			lv.pszText="CBR";
		}
		iRes=ListView_SetItem(hWndListView, &lv);
		if (!iRes) return FALSE;

		/* samplerate */
		lv.iSubItem=6;
		sprintf(buf, "%d Hz", fi->info->samplerate);
		lv.pszText=buf;
		iRes=ListView_SetItem(hWndListView, &lv);
		if (!iRes) return FALSE;

		/* frames */
		lv.iSubItem=7;
		sprintf(buf, "%d", fi->info->frames);
		lv.pszText=buf;
		iRes=ListView_SetItem(hWndListView, &lv);
		if (!iRes) return FALSE;

		/* time */
		lv.iSubItem=8;
		sprintf(buf, "%d:%02d", fi->info->time/60, fi->info->time%60);
		lv.pszText=buf;
		iRes=ListView_SetItem(hWndListView, &lv);
		if (!iRes) return FALSE;
	}

	/* size */
	lv.iSubItem=9;
	sprintf(buf, "%d KiB", fi->filesize/1024);
	lv.pszText=buf;
	iRes=ListView_SetItem(hWndListView, &lv);
	if (!iRes) return FALSE;
	
	return TRUE;
}

/* updates all subitem of an item in the listview 
 * e.g. doesn't update the filename 
 */
static BOOL LV_UpdateItem(HWND hWndListView, FileInfo * fi, int item) {
	if (fi->filetype & FILE_NORMAL) {
		return LV_UpdateFileItem(hWndListView, fi, item);
	} else {
		return LV_UpdateDirItem(hWndListView, fi, item);
	}
}

/*
 * puts the contents of CurrentDirectory on the listview
 */
static BOOL LV_UpdateView(HWND hWndListView) {
	int i=0;
	int retval;
	FileInfo * fi;

	retval=ListView_DeleteAllItems(hWndListView);

	while (fi=Vector_Get(CurrentVector, i)) {
		/* filename */
		LV_AddItem(hWndListView, fi, i);
		if (fi->info!=NULL) {
			LV_UpdateItem(hWndListView, fi, i);
		}
		i++;
	}

	return TRUE;
}



/* recursively scan directory, except if it is . or .. */
/* FIXME: update filetype of directory */
BOOL LV_ScanDir(int item, int * filetype) {
	FileInfo * fi;
	int iRes;
	int iItem=-1;
	int iFiletype;
	int cBad=0;
	int cGood=0;
	int cIsMP3=0;
	int cTotal=0;

	fi=Vector_Get(CurrentVector, item);
	if (fi==NULL) return FALSE;
	if (fi->filename[0]=='.') return FALSE; /* FIXME: quick fix */
	
	fi->dirinfo=HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(DirInfo));
	FI_AddFile(fi);

	iRes=SetCurrentDirectory(fi->filename);
	if (!iRes) return FALSE;
	Rebar_UpdateText();
	LV_Refresh();

	while (-1!=(iItem=ListView_GetNextItem(hWndListView, iItem, LVNI_ALL))) {
		iRes=LV_Scan(iItem, &iFiletype);
		if (iRes) {
			if (iFiletype & FILE_ISMP3) cIsMP3++;
			if (iFiletype & FILE_GOOD)  cGood++;
			if (iFiletype & FILE_BAD)   cBad++;
			cTotal++;
		}
	}

	fi->filetype|=FILE_SCANNED;
	if (cIsMP3>0) {
		fi->filetype|=FILE_ISMP3;
		if (cGood==cIsMP3) {
			fi->filetype|=FILE_GOOD;
		} else {
			fi->filetype|=FILE_BAD;
		}
	}
	if (filetype!=NULL) *filetype=fi->filetype;

	fi->dirinfo->cBad=cBad;
	fi->dirinfo->cGood=cGood;
	fi->dirinfo->cFiles=cTotal;
	fi->dirinfo->cIsMP3=cIsMP3;

	iRes=SetCurrentDirectory("..");
	LV_Refresh();

	return TRUE;
}

/* mpck-scans the file and stores this info in FI */
BOOL LV_ScanFile(int item, int * filetype) {
	FileInfo * file;

	file=Vector_Get(CurrentVector, item);
	if (file==NULL) return FALSE;

	file->info=HeapAlloc(GetProcessHeap(), 0, sizeof(file_info));
	if (file->info==NULL) return FALSE;
	init_file_info(file->info);

	file->info->fp=cfopen(file->filename, "r");
	if (file->info->fp==NULL) return FALSE;
	file->info->filename=file->filename;
	file->info->length=file->filesize;

	check_end(file->info);
	check_begin(file->info);
	check_filename(file->info);
	final_file_stats(file->info);

	cfclose(file->info->fp);

	/* update filetype */
	file->filetype|=FILE_SCANNED;
	if (file->info->ismp3file) file->filetype|=FILE_ISMP3;
	if (file->info->errors) {
		file->filetype|=FILE_BAD;
	} else {
		file->filetype|=FILE_GOOD;
	}
	if (filetype!=NULL) *filetype=file->filetype;

	FI_AddFile(file);
	return LV_UpdateItem(hWndListView, file, item);
}

BOOL LV_ScanSelected() {
	int index=-1, i=0;
	Vector * selected;
	
	selected=Vector_Create();

	while (-1!=(index=LV_FindSelected(index))) {
		Vector_Add(selected, (void *)index);
	}
	Vector_Add(selected, (void *)-1);

	while (-1!=(index=Vector_Get(selected, i++))) {
		LV_Scan(index, NULL);
	}
	Vector_Destroy(selected);

	return TRUE;
}

/* scan item - could be a file or directory */
BOOL LV_Scan(int item, int * filetype) {
	FileInfo * fi;
	int iRes;
	fi=Vector_Get(CurrentVector, item);
	if (fi==NULL) return FALSE;
	if (fi->filetype & FILE_NORMAL) {
		iRes=LV_ScanFile(item, filetype);
	} else {
		iRes=LV_ScanDir(item, filetype);
	}
	UpdateWindow(hWndListView);
	return iRes;
}

/* does default action on item */
BOOL LV_Activate(int item) {
	FileInfo * fi;
	int iRes;

	fi=Vector_Get(CurrentVector, item);
	if (fi==NULL) return FALSE;
	if (fi->filetype & FILE_DIRECTORY) {
		/* traverse directory */
		iRes=SetCurrentDirectory(fi->filename);
		LV_Refresh();
	} else {
		/* scan file */
		iRes=LV_Scan(item, NULL);
	}
	return iRes;
}

/* does default action on all selected items */
BOOL LV_ActivateSelected() {
	int index=-1;

	if (LV_GetSelectedCount()>1) {
		LV_ScanSelected();
	} else {
		LV_Activate(LV_FindSelected(-1));
	}
	return TRUE;
}

BOOL LV_OpenSelected() {
	int index=-1;
	FileInfo * fi;
	char filename[512];

	if (LV_GetSelectedCount()<1) return FALSE;
	
	while (-1!=(index=LV_FindSelected(index))) {
		fi=Vector_Get(CurrentVector, index);
		if (fi==NULL) return FALSE;
		strcpy(filename, "\"");
		strcat(filename, fi->dirname);
		strcat(filename, "\\");
		strcat(filename, fi->filename);
		strcat(filename, "\"");
		ShellExecute(NULL, "OPEN", fi->filename, NULL, NULL, SW_SHOWDEFAULT);
	}
	return TRUE;
}

/* edit name of selected file */
BOOL LV_StartRename() {
	int selected=LV_FindSelected(-1);
	if (selected==-1) return FALSE;
	return (ListView_EditLabel(hWndListView, selected)!=NULL);
}

int LV_GetSelectedCount() {
	return ListView_GetSelectedCount(hWndListView);
}

/*
 * frees all entries in CurrentVector and purges it
 */
/* FIXME: look into this */
static BOOL LV_CleanVector() {
	FileInfo * fi;
	int index=0;

	while (NULL!=(fi=Vector_Get(CurrentVector, index++))) {
		/* don't touch fi if it has fi->info, because then it is
		   in FileInfo and it needs to be kept */
		if (fi->info==NULL) HeapFree(GetProcessHeap(), 0, fi);
	}
	Vector_Purge(CurrentVector);
	return TRUE;
}

/*
 * fills the listview with the contents of the current directory
 */
static BOOL LV_UpdateDirectory(HWND hWndListView) {
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = NULL;
	int iItem=0;

	LV_CleanVector();

	hFind=FindFirstFile("*", &FindFileData);
	if (hFind==INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	do {
		if (strcmp(FindFileData.cFileName, ".")) LV_UpdateFindFile(&FindFileData);
	} while (FindNextFile(hFind, &FindFileData));

	FindClose(hFind);

	Vector_Sort(CurrentVector, CurrentSort);
	LV_UpdateView(hWndListView);
	Rebar_UpdateText();

	return TRUE;
}

/* 
 * refresh view of current directory
 */
BOOL LV_Refresh() {
	return LV_UpdateDirectory(hWndListView);
}

/* delete all selected files */
BOOL LV_DeleteSelected() {
	int index=-1;		/* file index in CurrentVector			*/
	FileInfo * fi;		/* info from CurrentVector				*/
	BOOL retval;		/* value of DeleteFile()				*/
	char * files;		/* buffer with files, seperated by NULL */
	char * curfile;		/* pointer to current/next file			*/
	int cursize=256;	/* current size of buffer				*/
	int sizeleft=cursize; /* number of bytes left in buffer		*/
	int namelen;
	SHFILEOPSTRUCT FileOp;

	if (LV_GetSelectedCount()==0) return FALSE;

	files=HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, cursize);
	curfile=files;

	FileOp.hwnd=hWndListView;
	FileOp.wFunc=FO_DELETE;
	FileOp.pTo=NULL;
	FileOp.fFlags=0;
	FileOp.fAnyOperationsAborted=0;
	FileOp.hNameMappings=NULL;
	FileOp.lpszProgressTitle=NULL;
	FileOp.pFrom=files;
	

	/* read the docs for SHFILEOPSTRUCT.pFrom for an explaination about
	 * the strange data structure in /files/
	 */
	while (-1!=(index=LV_FindSelected(index))) {
		fi=Vector_Get(CurrentVector, index);
		namelen=strlen(fi->filename);
		if (namelen>sizeleft) {	/* buffer is full, enlarge it */
			cursize+=1024;
			sizeleft+=1024;
			files=HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, files, cursize);
		}
		strncpy(curfile, fi->filename, namelen);
		curfile+=namelen+1; /* +1 because of trailing zero */
		sizeleft-=namelen+1;
	}
	/* trailing zero */
	if (sizeleft==0) {
		cursize+=2;
		files=HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, files, cursize);
	}
	retval=SHFileOperation(&FileOp);

	HeapFree(GetProcessHeap(), 0, files);

	LV_Refresh();
	return TRUE;
}

/*
 * creates and initializes the ListView control and returns the hWnd
 */
HWND LV_Create(HWND hWndParent) { 
	RECT rcl; 
	INITCOMMONCONTROLSEX icex;

	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC  = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icex); 

	GetClientRect (hWndParent, &rcl); 
	hWndListView = CreateWindow (WC_LISTVIEW, "", 
			WS_CHILD | LVS_REPORT | LVS_EDITLABELS, 
			0, 0, rcl.right - rcl.left, rcl.bottom -
			rcl.top, 
			hWndParent, NULL, hInst, NULL); 
	if (hWndListView == NULL) 
		return NULL; 
	
	LV_InitColumns(hWndListView);			/* set up columns							*/
	LV_InitVector();						/* create the vector for the current dir	*/
	LV_InitImages(hWndListView);
	FI_Create();							/* create the fileinfo						*/
	LV_UpdateDirectory(hWndListView);		/* fill the listview						*/

	ShowWindow(hWndListView, SW_SHOW);		/* show the control							*/

	return hWndListView;
}

/* clean up */
void LV_Destroy() {
	LV_DestroyVector();		/* free() elements and CurrentVector	*/
	FI_FreeAll();			/* free() all elements in FileInfo		*/
	FI_Destroy();			/* FileInfo clean up					*/
}


RECT * LV_GetRect(RECT * rc) {
	GetWindowRect(hWndListView, rc);
	return rc;
}

BOOL LV_GetPopupMenuPosition(POINT * ppt) {
	POINT ptItem;
	RECT rcItem;
	int iItem=LV_FindSelected(-1);
	ListView_GetItemPosition(hWndListView, iItem, &ptItem);
	ListView_GetItemRect(hWndListView, iItem, &rcItem, LVIR_SELECTBOUNDS);
	ppt->x=ptItem.x+(rcItem.right-rcItem.left)/2;
	ppt->y=ptItem.y+(rcItem.bottom-rcItem.top)/2;
	return TRUE;
}

static void LV_SwitchSort(int (__cdecl *callback1 )(const void *, const void *),
						 int (__cdecl *callback2 )(const void *, const void *)) {
	if (CurrentSort==callback1) {
		CurrentSort=callback2;
	} else {
		CurrentSort=callback1;
	}
}

int LV_SortColumn(int column) {
	if (column==0) {		/* filename		*/
		LV_SwitchSort(NormalAscendingSort, NormalDescendingSort);
	} else if (column==1) {	/* result		*/
		LV_SwitchSort(ResultAscendingSort, ResultDescendingSort);
	} else if (column==2) {	/* version		*/
		LV_SwitchSort(VersionAscendingSort, VersionDescendingSort);
	} else if (column==3) {	/* layer		*/
		LV_SwitchSort(LayerAscendingSort, LayerDescendingSort);
	} else if (column==4) {	/* bitrate		*/
		LV_SwitchSort(BitrateAscendingSort, BitrateDescendingSort);
	} else if (column==5) {	/* vbr/cbr	*/
		LV_SwitchSort(VBRAscendingSort, VBRDescendingSort);
	} else if (column==6) {	/* samplerate	*/
		LV_SwitchSort(SamplerateAscendingSort, SamplerateDescendingSort);
	} else if (column==7) {	/* frames		*/
		LV_SwitchSort(FramesAscendingSort, FramesDescendingSort);
	} else if (column==8) {	/* time			*/
		LV_SwitchSort(TimeAscendingSort, TimeDescendingSort);
	} else if (column==9) {	/* size			*/
		LV_SwitchSort(SizeAscendingSort, SizeDescendingSort);
	}
	LV_Refresh();
	return TRUE;
}

/* Go one directory up */
BOOL LV_DirUp() {
	SetCurrentDirectory("..");
	LV_Refresh();
	return TRUE;
}

/* select index */
void LV_Select(int index) {
	ListView_SetItemState(hWndListView, index, LVIS_SELECTED, LVIS_SELECTED);
}

/* Select all mp3 files */
int LV_SelectAll() {
	FileInfo * fi;
	int i=0;
	char * ext;

	while (fi=Vector_Get(CurrentVector, i)) {
		ext=strrchr(fi->filename, '.');
		if (ext!=NULL) {
			if (0==stricmp(ext, ".mp3")) LV_Select(i);
		}
		i++;
	}
	return i;
}
