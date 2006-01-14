/*
 *   This file is part of Checkmate, a program to check MP3 files for errors
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
 *   matrices.h - data about various MPEG layers/versions 
 * 
 */

/* the bitrates in kbps for different layers and versions */
static int bitrate_matrix[9][16] = {
  /* MPEG 1 */
  {1, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 0},
  {1, 32, 48, 56, 64,  80,  96,  112, 128, 160, 192, 224, 256, 320, 384, 0},
  {1, 32, 40, 48, 56,  64,  80,  96,  112, 128, 160, 192, 224, 256, 320, 0},
  /* MPEG 2 */
  {1, 32, 48, 56, 64,  80,  96,  112, 128, 144, 160, 176, 192, 224, 256, 0},
  {1, 8,  16, 24, 32,  40,  48,  56,  64,  80,  96,  112, 128, 144, 160, 0},
  {1, 8,  16, 24, 32,  40,  48,  56,  64,  80,  96,  112, 128, 144, 160, 0},
  /* MPEG 2.5 */
  {1, 32, 48, 56, 64,  80,  96,  112, 128, 144, 160, 176, 192, 224, 256, 0},
  {1, 8,  16, 24, 32,  40,  48,  56,  64,  80,  96,  112, 128, 144, 160, 0},
  {1, 8,  16, 24, 32,  40,  48,  56,  64,  80,  96,  112, 128, 144, 160, 0},
};

/* samplerates in Hz for different versions and header values */
static int samplerate_matrix[3][3] = {
/* MPEG   1.0    2.0     2.5   */
	{44100,	22050,	11025},
	{48000,	24000,	12000},
	{32000,	16000,	8000},
};

/* samples per frame for different versions and layers */
static int samples_matrix[4][3] = {
/* MPEG   1.0    2.0     2.5   */
	{0,	0,	0},
	{384,	384,	384},
	{1152,	1152,	1152},
	{1152,	576,	576},
};

