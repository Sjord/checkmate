/*
 *   This file is part of mpck, a program to check MP3 files for errors
 *   
 *   Copyright (C)  2026  Sjoerd Langkemper
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
 *   crctest.c - tests the CRC implementations
 * 
 */

#include <stdio.h>
#include "../crc.h"

int
main(void)
{
	unsigned char test[] = "123456789";
	int crc;

	crc = crc16_music_update(0, test, 9);
	if (crc != 0xBB3D) {
		printf("crc16_music_update: expected 0xBB3D, got 0x%04X\n", crc);
		return 1;
	}

	crc = crc16(test, 9);
	if (crc != 0xAEE7) {
		printf("crc16: expected 0xAEE7, got 0x%04X\n", crc);
		return 1;
	}

	crc = crc16bits(test, 9 * 8);
	if (crc != 0xAEE7) {
		printf("crc16bits: expected 0xAEE7, got 0x%04X\n", crc);
		return 1;
	}
	return 0;
}
