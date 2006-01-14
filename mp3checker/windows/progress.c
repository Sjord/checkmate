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
 *   progress.c - handles progress dialog
 * 
 */

#include <windows.h>
#include <commctrl.h>
#include "resource.h"


extern HINSTANCE hInst;
extern HWND hWndMain;

static HWND hWndProgress=NULL;

INT_PTR CALLBACK ProgressProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) { 
        case WM_COMMAND: 
			OutputDebugString("Bla\n");
            switch (LOWORD(wParam)) { 
                case IDCANCEL: 
                    Progress_Destroy();
                    return TRUE; 
            } 
    } 

	return FALSE;
}

BOOL Progress_Create() {
	if (IsWindow(hWndProgress)) return FALSE;
	hWndProgress=CreateDialog(hInst, MAKEINTRESOURCE(IDD_PROGRESS), hWndMain, (DLGPROC)ProgressProc);
	ShowWindow(hWndProgress, SW_SHOW);
	// blub
	//WaitMessage();
	//UpdateWindow(hWndProgress);
	//Sleep(100);
	// blob
	return (hWndProgress!=NULL);
}

BOOL Progress_Destroy() {
	BOOL res;
	res=DestroyWindow(hWndProgress);
	hWndProgress=NULL;
	return res;
}

BOOL Progress_SetText(char * text) {
	SetDlgItemText(hWndProgress, IDC_FILENAME, text);
	// WaitMessage();
	UpdateWindow(hWndProgress);
	return TRUE;
}

BOOL Progress_SetRange(int range) {
	HWND hWndBar;

	hWndBar=GetDlgItem(hWndProgress, IDC_PROGRESSBAR);
	SendMessage(hWndBar, PBM_SETRANGE, 0, MAKELPARAM (0, range));
	return TRUE;
}

BOOL Progress_SetPos(int pos) {
	HWND hWndBar;

	hWndBar=GetDlgItem(hWndProgress, IDC_PROGRESSBAR);
	SendMessage(hWndBar, PBM_SETPOS, pos, 0);
	return TRUE;
}