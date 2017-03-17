/*
 *   This file is part of Checkmate MP3 checker, a program to check MP3 files for errors
 *   
 *   Copyright (C)  2017  Sjoerd Langkemper
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
 *   rebar.c - handles the rebar and the location combo control
 * 
 */

#include "mp3checker.h"
#include <windows.h>
#include <commctrl.h>
#include "resource.h"

static HWND hWndRebar;
static HWND hWndCombo;

static WNDPROC DefComboWndProc;

extern HINSTANCE hInst;
extern HWND hWndMain;

LRESULT CALLBACK OwnComboWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {
	if (msg==WM_CHAR) {
		switch (wParam) {
			case VK_RETURN:
			case VK_ESCAPE:
				SendMessage(hWndMain, WM_COMMAND, MAKEWPARAM(ID_COMBO, CBN_CHAR), wParam);
				return TRUE;
		}
	}
	return CallWindowProc(DefComboWndProc, hwnd, msg, wParam, lParam);
}

static BOOL Combo_AddDrives(HWND hWndCombo) {
	DWORD drives;
	int nPos=0;
	char strDrive[] = "A:\\";

	drives=GetLogicalDrives();

	while (drives) {
        if (drives & 1) {
			*strDrive='A'+nPos;
			SendMessage(hWndCombo, CB_ADDSTRING, 0, (LPARAM)strDrive);
        }
        drives >>= 1;
        nPos++;
    }
	return TRUE;
}

HWND Rebar_ComboControl(HWND hWndOwner) {
	HWND hWndEdit;
	HFONT font;

	hWndCombo = CreateWindow("COMBOBOX",      // predefined class 
                                    NULL,        // no window title 
                                    WS_BORDER | WS_CHILD | WS_VISIBLE | CBS_DROPDOWN ,
                                    0, 0, 100, 200,  // set size in WM_SIZE message 
                                    hWndOwner,        // parent window 
                                    (HMENU)ID_COMBO,   // combo control ID 
                                    hInst, 
                                    NULL);       // pointer not needed 

	/* set the same font as the rebar */
	font=(HFONT)SendMessage(hWndRebar, WM_GETFONT, 0, 0);
	SendMessage(hWndCombo, WM_SETFONT, (WPARAM)font, MAKELPARAM(TRUE, 0));

	hWndEdit=GetWindow(hWndCombo, GW_CHILD);

	DefComboWndProc=(WNDPROC)GetWindowLongPtr(hWndEdit, GWLP_WNDPROC);
	SetWindowLongPtr(hWndEdit, GWLP_WNDPROC, (LONG_PTR) OwnComboWndProc);

	Combo_AddDrives(hWndCombo);

	return hWndCombo;
}

static BOOL Rebar_SetText(char * text) {
	return SendMessage(hWndCombo, WM_SETTEXT, 0, (LPARAM)text);
}

BOOL Rebar_UpdateText() {
	static char curdir[MAX_PATH];
	GetCurrentDirectory(sizeof(curdir), curdir);
	Rebar_SetText(curdir);
	UpdateWindow(hWndRebar);
	return TRUE;
}

BOOL Rebar_ResizeContents() {
	RECT crc, rrc;
	int width, height, x;

	GetWindowRect(hWndCombo, &crc);
	GetWindowRect(hWndRebar, &rrc);
	height=crc.bottom-crc.top;
	width=crc.right-crc.left;
	x=(rrc.right-rrc.left)-width-5;

	MoveWindow(hWndCombo, x, 0, width, height, TRUE);
	return TRUE;
}

BOOL Rebar_Resize(int width, int height) {
	MoveWindow(hWndRebar,
		0,				/* x		*/
		0,				/* y		*/
		width,			/* width	*/
		0,				/* height	*/
		TRUE);			/* repaint  */
	return Rebar_ResizeContents();
}

int Rebar_Height() {
	RECT          rc;
	GetWindowRect(hWndRebar, &rc);
	return rc.bottom-rc.top;
}

/*
 * creates and initializes the Rebar control and returns the hWnd
 */
HWND Rebar_Create(HWND hWndOwner)
{
   REBARINFO     rbi;
   REBARBANDINFO rbBand;
   RECT          rc;
   HWND			 hWndContents;
   INITCOMMONCONTROLSEX icex;
   char locationtext[20];
   LoadString(hInst, IDS_LOCATION, locationtext, 19);
   
   icex.dwSize=sizeof(INITCOMMONCONTROLSEX);
   icex.dwICC =ICC_COOL_CLASSES | ICC_BAR_CLASSES;
   InitCommonControlsEx(&icex);

   hWndRebar=CreateWindowEx(WS_EX_TOOLWINDOW,
                           REBARCLASSNAME,
                           NULL,
                           WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|
                           WS_CLIPCHILDREN|RBS_VARHEIGHT|RBS_BANDBORDERS|WS_BORDER|
                           CCS_NODIVIDER,
                           0,0,0,0,
                           hWndOwner,
                           NULL,
                           hInst,
                           NULL);
   if(!hWndRebar)
      return NULL;

   rbi.cbSize = sizeof(REBARINFO);
   rbi.fMask  = 0;
   rbi.himl   = (HIMAGELIST)NULL;
   if(!SendMessage(hWndRebar, RB_SETBARINFO, 0, (LPARAM)&rbi))
      return NULL;
   
   rbBand.cbSize = sizeof(REBARBANDINFO);
   rbBand.fMask  = RBBIM_TEXT | 
                   RBBIM_STYLE | RBBIM_CHILD  | RBBIM_CHILDSIZE | 
                   RBBIM_SIZE;
   rbBand.fStyle = RBBS_CHILDEDGE;

   hWndContents=Rebar_ComboControl(hWndRebar);
   GetWindowRect(hWndContents, &rc);
   
   rbBand.lpText     = locationtext;
   rbBand.hwndChild  = hWndContents;
   rbBand.cxMinChild = 100;
   rbBand.cyMinChild = rc.bottom - rc.top;
   rbBand.cx         = 200;
   SendMessage(hWndRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);

   Rebar_UpdateText();
   return (hWndRebar);
}

/* FIXME: static is Evil */
char * Rebar_GetText() {
	static char buf[255];
	GetWindowText(hWndCombo, buf, 254);
	return buf;
}

void Rebar_Clear() {
	SetWindowText(hWndCombo, "");
}

void Rebar_Destroy() {
	DestroyWindow(hWndRebar);
}

int Rebar_Select() {
	int iItem;
	char * strPath;
	int iLen;
	int iRes;
	
	iItem=SendMessage(hWndCombo, CB_GETCURSEL, 0, 0);
	iLen=SendMessage(hWndCombo, CB_GETLBTEXTLEN, iItem, 0);
	
	strPath=HeapAlloc(GetProcessHeap(), 0, iLen+1);
	if (strPath==NULL) return FALSE;

	SendMessage(hWndCombo, CB_GETLBTEXT, iItem, (LPARAM)strPath);
	iRes=SetCurrentDirectory(strPath);
	HeapFree(GetProcessHeap(), 0, strPath);
	if (iRes) {
		LV_Refresh();
		Rebar_UpdateText();
	}
	return iRes;
}