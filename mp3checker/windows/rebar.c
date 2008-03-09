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
 *   rebar.c - handles the rebar and the location combo control
 * 
 */

#include <windows.h>
#include <commctrl.h>
#include "mp3checker.h"
#include "resource.h"

static HWND hWndRebar;
static HWND hWndEdit;	/* deprecated? */
static HWND hWndButton;
static HWND hWndStatic;
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
	return DefComboWndProc(hwnd, msg, wParam, lParam);

}


static BOOL Combo_AddDrives(HWND hWndCombo) {
	DWORD drives;
	int nPos=0;
	TCHAR strDrive[4];

	drives=GetLogicalDrives();
	lstrcpy(strDrive, TEXT("A:\\"));

	while (drives) {
        if (drives & 1) {
			*strDrive='A'+nPos;
			SendMessage(hWndCombo, CB_ADDSTRING, 0, strDrive);
        }
        drives >>= 1;
        nPos++;
    }
	return TRUE;

}

HWND Rebar_ComboControl(HWND hWndOwner) {
	HWND hWndEdit;
	HFONT font;

	hWndCombo = CreateWindow(TEXT("COMBOBOX"),   // predefined class 
                                    NULL,        // no window title 
                                    WS_BORDER | WS_CHILD | WS_VISIBLE | CBS_DROPDOWN ,
                                    0, 0, 100, 200,  // set size in WM_SIZE message 
                                    hWndOwner,        // parent window 
                                    ID_COMBO,   // combo control ID 
                                    hInst, 
                                    NULL);       // pointer not needed 

	/* set the same font as the rebar */
	font=SendMessage(hWndRebar, WM_GETFONT, 0, 0);
	SendMessage(hWndCombo, WM_SETFONT, font, 1);

	hWndEdit=GetWindow(hWndCombo, GW_CHILD);

	DefComboWndProc=GetWindowLong(hWndEdit, GWL_WNDPROC);
	SetWindowLong(hWndEdit, GWL_WNDPROC, OwnComboWndProc);

	Combo_AddDrives(hWndCombo);

	return hWndCombo;
}

HWND Rebar_Button(HWND hWndOwner) {
	hWndButton = CreateWindow( 
		TEXT("BUTTON"),// predefined class 
		TEXT("OK"),    // button text 
		WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // styles 
		10,         // starting x position 
		10,         // starting y position 
		100,        // button width 
		100,        // button height 
		hWndOwner,  // parent window 
		1234    ,
		hInst, 
		NULL);      // pointer not needed 
	return hWndButton;
}

static BOOL Rebar_SetText(TCHAR * text) {
	return SendMessage(hWndCombo, WM_SETTEXT, 0, (LPARAM)text);
}

BOOL Rebar_UpdateText() {
	/* FIXME: overflow risc */
	static TCHAR curdir[255];
	GetCurrentDirectory(255, curdir);
	Rebar_SetText(curdir);
	UpdateWindow(hWndRebar);
	return TRUE;
}

/* moves the contents of the static control to the right position */
BOOL Rebar_ResizeContents() {
	RECT crc, rrc;
	int width, height, x;

	GetWindowRect(hWndCombo, &crc);
	GetWindowRect(hWndRebar, &rrc);
	height=crc.bottom-crc.top;
	width=crc.right-crc.left;
	x=(rrc.right-rrc.left)-width-5;

//	MoveWindow(hWndButton, width-60, 0, 55, height, TRUE);
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
   TCHAR locationtext[20];
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

int Rebar_ButtonID() {
	return GetWindowLong(hWndCombo, GWL_ID);
	// return GetLastError();
}

/* FIXME: static is Evil */
TCHAR * Rebar_GetText() {
	static TCHAR buf[255];
	GetWindowText(hWndCombo, buf, 254);
	return buf;
}

void Rebar_Clear() {
	SetWindowText(hWndCombo, TEXT(""));
}

BOOL Rebar_Destroy() {
	DestroyWindow(hWndRebar);
	return TRUE;
}

int Rebar_Select() {
	int iItem;
	TCHAR * strPath;
	int iLen;
	int iRes;
	
	iItem=SendMessage(hWndCombo, CB_GETCURSEL, 0, 0);
	iLen=SendMessage(hWndCombo, CB_GETLBTEXTLEN, iItem, 0);
	
	strPath=HeapAlloc(GetProcessHeap(), 0, sizeof(strPath[0]) * (iLen+1));
	if (strPath==NULL) return FALSE;

	SendMessage(hWndCombo, CB_GETLBTEXT, iItem, strPath);
	iRes=SetCurrentDirectory(strPath);
	HeapFree(GetProcessHeap(), 0, strPath);
	if (iRes) {
		LV_Refresh();
		Rebar_UpdateText();
	}
	return iRes;
}