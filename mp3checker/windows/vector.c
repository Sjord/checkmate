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
 *   vector.c - vector object (infinite storage of void pointers)
 * 
 * This vector object works with an array, which starts with a size of 10
 * elements. When the array is full, a new array with twice the original
 * size is made and the contents of the old array are copied into the new
 * array.
 *
 */


#define VECTOR_NELEM	10	// elements in a new vector

struct _Vector {
	void ** elements;	/* array of void pointers which point to the elements	*/
	int nElem;			/* current number of elements							*/
	int maxElem;		/* current size of elements array						*/
};

#include "vector.h"

/* allocates memory for an array of maxElem elements */
static BOOL Vector_CreateArray(Vector * v, int maxElem) {
	v->maxElem=maxElem;
	v->elements=HeapAlloc(GetProcessHeap(), 0, sizeof(void *)*v->maxElem);
	return TRUE;
}

/* dubbles the size of the current array */
static BOOL Vector_Expand(Vector * v) {
	void * oldElems=v->elements;
	Vector_CreateArray(v, v->maxElem*2);
	CopyMemory(v->elements, oldElems, v->nElem*sizeof(void *));
	HeapFree(GetProcessHeap(), 0, oldElems);
	return TRUE;
}

/* creates a vector */
Vector * Vector_Create() {
	Vector * v;
	v=HeapAlloc(GetProcessHeap(), 0, sizeof(Vector));
	v->nElem=0;
	Vector_CreateArray(v, VECTOR_NELEM);
	return v;
}

/* destroys a vector */
void Vector_Destroy(Vector * v) {
	HeapFree(GetProcessHeap(), 0, v->elements);
	HeapFree(GetProcessHeap(), 0, v);
}

/* adds an element to the end of the vector */
BOOL Vector_Add(Vector * v, void * data) {
	if (v->nElem==v->maxElem) Vector_Expand(v);
	v->elements[v->nElem++]=data;
	return TRUE;
}

/* changes the contents of item /elem/ to /data/   */
BOOL Vector_Update(Vector * v, int elem, void * data) {
	if (elem>v->nElem) return FALSE;
	v->elements[elem]=data;
	return TRUE;
}

/* returns an element, or NULL if elem is over the size of the array */
void * Vector_Get(Vector * v, int elem) {
	if (elem>=v->nElem) return NULL;
	return v->elements[elem];
}

/* clears the vector, but does not destroy it */
void Vector_Purge(Vector * v) {
	v->nElem=0;
}

void Vector_Sort(Vector * v, int (__cdecl *compare )(const void *, const void *)) {
	if (compare!=NULL) qsort(v->elements, v->nElem, sizeof(void *), compare);
}

int Vector_Count(Vector * v) {
	return v->nElem;
}