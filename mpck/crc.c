/*
 *   This file is part of mpck, a program to check MP3 files for errors
 *   
 *   Copyright (C)  2006  Sjoerd Langkemper
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
 *   crc.c - functions that compute the crc16 of some data 
 * 
 */

#include "crc.h"

/* from lame 3.93.1 */
static int
crc_update(int value, int crc)
{
    int i;
    value <<= 8;
    for (i = 0; i < 8; i++) {
	value <<= 1;
	crc <<= 1;

	if (((crc ^ value) & 0x10000))
	    crc ^= CRC16_POLYNOMIAL;
    }
    return crc;
}

int
crc16(data, length)
	unsigned char * data;
	int length;
{
	int i;
	int crc=0xffff;

	for (i=0; i<length; i++) 
		crc=crc_update(data[i], crc);

	return crc & 0xffff;
}

static int
crc_update_bits(int value, int crc, int nbits)
{
    int i;
    value <<= 8;
    for (i = 0; i < nbits; i++) {
	value <<= 1;
	crc <<= 1;

	if (((crc ^ value) & 0x10000))
	    crc ^= CRC16_POLYNOMIAL;
    }
    return crc;
}


int crc16bits(data, nbits)
	unsigned char * data;
	int nbits;
{
	int i;
	int crc=0xffff;
	int nbytes = nbits >> 3;
	nbits = nbits & 7;

	for (i=0; i<nbytes; i++) 
		crc=crc_update_bits(data[i], crc, 8);

	// i++;
	crc=crc_update_bits(data[i], crc, nbits);
	return crc & 0xffff;
}
