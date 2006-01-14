/*
 *   This file is part of wimpck, a program to check MP3 files for errors
 *   
 *   Copyright (C)  2004  Sjoerd Langkemper
 *   
 *   mpck is free software; you can redistribute it and/or modify
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
 *   wimpck.c - main message loop
 * 
 */


#include <windows.h>
#include "wimpck.h"
#include "resource.h"
#include <commctrl.h>
#include "rebar.h"

HWND hWndMain=NULL;	//Main window handle
HINSTANCE hInst;
HMENU headMenu;
HACCEL hAccel;
 
WNDCLASSEX * InitWndClassEx(WNDCLASSEX * wcx) {

	ZeroMemory(wcx, sizeof(WNDCLASSEX));

	wcx->cbSize			= sizeof(WNDCLASSEX);
	wcx->style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcx->lpfnWndProc	= (WNDPROC)MainWndProc;
	wcx->cbClsExtra		= 0;
	wcx->cbWndExtra		= 0;
	wcx->hInstance		= hInst;
	wcx->hIcon			= NULL;
	wcx->hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcx->hbrBackground	= (HBRUSH)(COLOR_WINDOW);
	wcx->lpszMenuName	= NULL;
	wcx->lpszClassName	= WIMPCK_CLASSNAME;
	wcx->hIconSm		= LoadIcon(hInst, MAKEINTRESOURCE(IDI_WIMPCK));

	return wcx;
}

HWND CreateWimpckWindow() {
	headMenu=LoadMenu(hInst, MAKEINTRESOURCE(IDR_HEADMENU));
	return CreateWindowEx(0,
			WIMPCK_CLASSNAME,
			WIMPCK_TITLE,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,CW_USEDEFAULT,
			CW_USEDEFAULT,CW_USEDEFAULT,
			HWND_DESKTOP,
			headMenu,
			hInst,
			NULL);

}

BOOL Destroy() {
	/* loaded are:
		IDR_HEADMENU		DestroyMenu(headMenu);
		IDI_WIMPCK			shared icon, don't destroy
		IDC_ARROW			shared cursor, don't destroy
		InitWndClassEx		locally allocated, don't destroy
		RegisterClassEx()	automagically unregistered
		WimpckWindow		automagically destroyed
		Listview			LV_Destroy();
		Rebar				Rebar_Destroy();
		hAccel				DestroyAcceleratorTable(hAccel);
	*/
	DestroyMenu(headMenu);
	LV_Destroy();
	Rebar_Destroy();
	DestroyAcceleratorTable(hAccel);
	return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow) {
	MSG msg;
	WNDCLASSEX wcx;

	hInst=hInstance;

	InitWndClassEx(&wcx);
	if (!RegisterClassEx(&wcx)) {
		ErrorBox(IDS_FREGISTERCLASS, MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	hWndMain=CreateWimpckWindow();
	if (!hWndMain) {
		ErrorBox(IDS_FCREATEWINDOW, MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

	LV_Create(hWndMain);				/* create listview */
	Rebar_Create(hWndMain);				/* create rebar control */

	ShowWindow(hWndMain, SW_SHOW);
	
	hAccel=LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR));

	while (GetMessage(&msg,NULL,0,0))
	{
        if (!TranslateAccelerator(hWndMain, hAccel, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

/* message handler for the about dialog */
INT_PTR CALLBACK AboutProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg)
	{
		case WM_DESTROY:
		case WM_COMMAND:
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			EndDialog(hwnd, wParam);
			return TRUE;
			break;
		default:
			return FALSE;
	}
	return FALSE;
}

BOOL DirectoryChange() {
	char * text;
	BOOL retval;
	text=Rebar_GetText();
	retval=SetCurrentDirectory(text);
	if (!retval) {
		ErrorBox(IDS_DIRNOTFOUND, MB_OK | MB_ICONWARNING);
	}
	Rebar_UpdateText();
	LV_Refresh();
	return retval;
}

/* gets called whenever a WM_COMMAND message is received */
LRESULT OnCommand(WPARAM wParam,LPARAM lParam) {

	switch (LOWORD(wParam)) {
		case ID_ACCEL_SCAN:
			/* enter is pressed: handle either scanning or changing directory */
			if (Rebar_HasFocus()) {
				return DirectoryChange();
			}
			return LV_ActivateSelected();
		case ID_FILE_SCAN:
		case ID_POPUP_SCAN:
			/* FIXME: should be scan and not activate */
			LV_ScanSelected();
			break;
		case ID_FILE_DELETE:
		case ID_ACCEL_DELETE:
		case ID_POPUP_DELETE:
			return LV_DeleteSelected();
			break;
		case ID_FILE_RENAME:
		case ID_ACCEL_RENAME:
		case ID_POPUP_RENAME:
			if (LV_GetSelectedCount()==1) {
				return LV_StartRename();
			} else {
				return FALSE;
			}
			break;
		case ID_FILE_PROPERTIES:
		case ID_ACCEL_PROPERTIES:
		case ID_POPUP_PROPERTIES:
			break;
		case ID_FILE_EXIT:
			PostQuitMessage(0);
			break;
		case ID_VIEW_REFRESH:
		case ID_ACCEL_REFRESH:
			return LV_Refresh();
			break;
		case ID_HELP_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUT), hWndMain, AboutProc);
			break;
	}
	return FALSE;
}

MenuItemSetEnabled(HMENU hMenu, UINT uItem, BOOL enabled) {
	MENUITEMINFO info;
	info.cbSize=sizeof(MENUITEMINFO);
	info.fMask=MIIM_STATE;
	if (enabled) info.fState=MFS_ENABLED;
			else info.fState=MFS_DISABLED;
	return SetMenuItemInfo(hMenu, uItem, FALSE, &info);
}

/* FIXME: clean this */
/* gets called just before a menu is opened */
LRESULT OnInitMenu(WPARAM wParam,LPARAM lParam) {
	int selcount;
	
	selcount=LV_GetSelectedCount();
	if (selcount==0) {
		/* no files selected: disable all */
		MenuItemSetEnabled(headMenu, ID_FILE_SCAN,			FALSE);
		MenuItemSetEnabled(headMenu, ID_FILE_DELETE,		FALSE);
		MenuItemSetEnabled(headMenu, ID_FILE_RENAME,		FALSE);
		MenuItemSetEnabled(headMenu, ID_FILE_PROPERTIES,	FALSE);
	} else if (selcount==1) {
		/* one file selected: enable all */
		MenuItemSetEnabled(headMenu, ID_FILE_SCAN,			TRUE);
		MenuItemSetEnabled(headMenu, ID_FILE_DELETE,		TRUE);
		MenuItemSetEnabled(headMenu, ID_FILE_RENAME,		TRUE);
		MenuItemSetEnabled(headMenu, ID_FILE_PROPERTIES,	TRUE);
	} else {
		/* more files selected: disable rename, enable rest */
		MenuItemSetEnabled(headMenu, ID_FILE_RENAME,		TRUE);
		MenuItemSetEnabled(headMenu, ID_FILE_SCAN,			FALSE);
		MenuItemSetEnabled(headMenu, ID_FILE_DELETE,		FALSE);
		MenuItemSetEnabled(headMenu, ID_FILE_PROPERTIES,	FALSE);
	}
	return TRUE;
}

/* gets called when a user is done editing a filename */
LRESULT OnEdit(LVITEM * info) {
	return LV_RenameFile(info->iItem, info->pszText);
}

BOOL OnDoubleClick(int iItem) {
	if (iItem==-1) return FALSE;
	LV_Activate(iItem);
	return TRUE;
}

BOOL PopupMenu(POINT ptLocation) {
	HMENU hMenu, popup;
	RECT rc;
	int res;
	
	/* FIXME: disable some items - see OnInitMenu */

	hMenu=LoadMenu(hInst, MAKEINTRESOURCE(IDR_FILEPOPUP));
	popup=GetSubMenu(hMenu, 0);
	LV_GetRect(&rc);

	res=TrackPopupMenu(popup,
		0,
		rc.left+ptLocation.x,
		rc.top+ptLocation.y,
		0,
		hWndMain,
		0);

	DestroyMenu(hMenu);
	return res;
}

/* gets called whenever a WM_NOTIFY is received */
LRESULT OnNotify(WPARAM wParam,LPARAM lParam) {
	NMHDR * nm=(NMHDR *)lParam;
	switch (nm->code) {
		case LVN_ENDLABELEDIT:
			return OnEdit(&((NMLVDISPINFO *)lParam)->item);
			break;
		case NM_DBLCLK:
			/* user doubleclicked */
			OnDoubleClick(((NMITEMACTIVATE *)lParam)->iItem);
			return TRUE;
			break;
		case NM_RCLICK:
			/* user right-clicked */
			/* FIXME: send WM_CONTEXTMENU ? */
			return PopupMenu(((LPNMITEMACTIVATE)lParam)->ptAction);
		default:
			return FALSE;
	}
}

LRESULT CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {
	POINT pt;
	switch (msg)
	{
		case WM_DESTROY:
			Destroy();
			PostQuitMessage(0);
			break;
		case WM_SIZE:
			LV_Resize(LOWORD(lParam), HIWORD(lParam));
			Rebar_Resize(LOWORD(lParam), HIWORD(lParam));
			return 0;
			break;
		case WM_COMMAND:
			return OnCommand(wParam, lParam);
		case WM_NOTIFY:
			return OnNotify(wParam, lParam);
		case WM_INITMENU:
			return OnInitMenu(wParam, lParam);
		case WM_CONTEXTMENU:
			/* lParam==-1 if the user didn't click */
			/* FIXME: do popup menu here */
			if (lParam==-1) {
				LV_GetPopupMenuPosition(&pt);
				PopupMenu(pt);
			} else {
				pt.x=LOWORD(lParam);
				pt.y=HIWORD(lParam);
				PopupMenu(pt);
			}

		default:
			return DefWindowProc(hwnd,msg,wParam,lParam);
	}
	return 0;
}


int ErrorBox(int iError, UINT uType) {
	char text[1024];
	char caption[256];
	LoadString(hInst, iError, text, 1023);
	LoadString(hInst, iError+1, caption, 255);
//	sprintf(text, "%s\nError #%d.", text, GetLastError());

	return MessageBox(hWndMain, text, caption, uType);
}