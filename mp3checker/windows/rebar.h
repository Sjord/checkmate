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
 *   rebar.h
 * 
 */

/* prototypes */

HWND Rebar_Create(HWND hWndParent);
int Rebar_Height(void);
BOOL Rebar_UpdateText(void);
char * Rebar_GetText(void);
BOOL Rebar_Resize(int width, int height);
BOOL Rebar_Destroy(void);
int Rebar_Select();
void Rebar_Clear();