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
 *   vector.h
 * 
 */

#include <windows.h>

typedef struct _Vector Vector;

Vector * Vector_Create(void);
void Vector_Destroy(Vector * v);
BOOL Vector_Add(Vector * v, void * data);
void * Vector_Get(Vector * v, int elem);
void Vector_Purge(Vector * v);
void Vector_Sort(Vector * v, int (__cdecl *compare )(const void *, const void *));
int Vector_Count(Vector * v);