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

/*
 * CRC-16/ARC lookup table, as used by LAME for the Xing MusicCRC and
 * TagCRC fields.  This is a different variant from the MPEG audio CRC
 * computed by crc16() below.
 */
static const unsigned short crc16_music_table[256] = {
	0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
	0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
	0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
	0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
	0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
	0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
	0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
	0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
	0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
	0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
	0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
	0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
	0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
	0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
	0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
	0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
	0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
	0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
	0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
	0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
	0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
	0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
	0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
	0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
	0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
	0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
	0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
	0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
	0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
	0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
	0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
	0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

/*
 * Slicing-by-8 tables.  Table 0 is crc16_music_table; table k contains the
 * CRC of a byte followed by k zero bytes.  The tables are built once, at
 * startup, and stay in L1 cache.
 */
static unsigned short crc16_music_slice[8][256];
static unsigned int crc16_table[256];
static int crc16_tables_ready;

static int crc_update(int value, int crc);

void
crc_init(void)
{
	unsigned int i;
	unsigned int k;
	unsigned int crc;

	if (crc16_tables_ready) return;

	for (i = 0; i < 256; i++) {
		crc16_music_slice[0][i] = crc16_music_table[i];
		crc16_table[i] = crc_update(i, 0) & 0xffff;
	}
	for (k = 1; k < 8; k++) {
		for (i = 0; i < 256; i++) {
			crc = crc16_music_slice[k - 1][i];
			crc16_music_slice[k][i] =
				(unsigned short)((crc >> 8) ^
				crc16_music_table[crc & 0xff]);
		}
	}
	crc16_tables_ready = 1;
}

int
crc16_music_update(int crc, const unsigned char *data, int length)
{
	int i = 0;
	unsigned int c;

	crc_init();

	while (i + 7 < length) {
		c = (unsigned int)crc ^
			((unsigned int)data[i] | ((unsigned int)data[i + 1] << 8));
		crc = crc16_music_slice[7][c & 0xff]
		    ^ crc16_music_slice[6][(c >> 8) & 0xff]
		    ^ crc16_music_slice[5][data[i + 2]]
		    ^ crc16_music_slice[4][data[i + 3]]
		    ^ crc16_music_slice[3][data[i + 4]]
		    ^ crc16_music_slice[2][data[i + 5]]
		    ^ crc16_music_slice[1][data[i + 6]]
		    ^ crc16_music_slice[0][data[i + 7]];
		i += 8;
	}
	while (i < length) {
		crc = (crc >> 8) ^
			crc16_music_table[(crc ^ data[i]) & 0xff];
		i++;
	}
	return crc & 0xffff;
}

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
crc16(unsigned char * data, int length)
{
	int i;
	int crc=0xffff;

	crc_init();

	for (i=0; i<length; i++) {
		crc = ((crc << 8) ^
			crc16_table[((crc >> 8) ^ data[i]) & 0xff]) & 0xffff;
	}

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


int crc16bits(unsigned char * data, int nbits)
{
	int i;
	int crc=0xffff;
	int nbytes = nbits >> 3;
	nbits = nbits & 7;

	crc_init();

	for (i=0; i<nbytes; i++) {
		crc = ((crc << 8) ^
			crc16_table[((crc >> 8) ^ data[i]) & 0xff]) & 0xffff;
	}

	if (nbits) {
		crc = crc_update_bits(data[i], crc, nbits);
	}
	return crc & 0xffff;
}
