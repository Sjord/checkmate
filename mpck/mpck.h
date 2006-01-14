/*
 *   This file is part of mpck, a program to check MP3 files for errors
 *   
 *   Copyright (C)  2003  Sjoerd Langkemper
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
 *   mpck.h - some defs 
 * 
 */

#ifndef _MPCK_H
#define _MPCK_H

/* include files */

#include "bufio.h"
#include <stdio.h>	// for FILE in the file_info struct

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

/* some definitions for other platforms */
#ifdef _WIN32
#define HAVE_WINDOWS_H
#define HAVE_STDLIB_H
#define HAVE_MALLOC_H
#define HAVE_MEMORY_H
#define HAVE__VSNPRINTF
#define HAVE_STRING_H
#define DIRSEP '\\' /* directory seperator */
#else /* _WIN32 */
#define DIRSEP '/'
#endif /* _WIN32 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

/* package information */
#ifndef VERSION
#ifdef PACKAGE_VERSION 
#define VERSION PACKAGE_VERSION
#else
#define VERSION "0.11"
#endif /* PACKAGE_VERSION */
#endif /* VERSION */

#define PROGNAME	"mpck"
#define HOMEPAGE	"http://mpck.linuxonly.nl/"
#define AUTHOR		"Sjoerd Langkemper <sjoerd-mpck@linuxonly.nl>"

/* constants */

#define FALSE_STRING "no"
#define TRUE_STRING  "yes"

#define TRUE    1
#define FALSE   0

#define EXTENTION_MARK '.'	/* the dot in foobar.mp3 */
#define DEF_EXTENTION "mp3"		/* default extention */

#define GOODFILE "Ok"	/* the text to print if a file is fine */
#define BADFILE  "Bad"	/* the text to print if a file is corrupt */
#define BADTYPE  "No MP3"	/* the text to print if the filetype is wrong */

#define MPEG_VER_10 0	/* header value for MPEG 1.0 */
#define MPEG_VER_20 1	/* header value for MPEG 2.0 */
#define MPEG_VER_25 2	/* header value for MPEG 2.5 */
#define MPEG_VER_INVALID 3	/* header value for an invalid MPEG version */

#define LAYER_1 1
#define LAYER_2 2
#define LAYER_3 3

#define ID3V1 1
#define ID3V2 2

#define STEREO 0
#define JOINT 1
#define DUAL 2
#define MONO 3

#define FILENAME_OK 0
#define FILENAME_TOOLONG 1
#define FILENAME_STRANGECHARS 2

#define FILENAME_OK_STR "Ok"
#define FILENAME_TOOLONG_STR "too long"
#define FILENAME_STRANGECHARS_STR "strange characters"

#define ERR_NONE		0
#define ERR_UBYTES		1<<0	/* unidentified bytes between frames */
#define ERR_INCONSISTENT	1<<1	/* non-consistent frame headers */
#define ERR_INVALID		1<<2 	/* invalid frame header values */
#define ERR_FRAMECRC		1<<3
//                                 4
#define ERR_NOFRAMES		1<<5	/* no frames found */
#define ERR_LFRAMES		1<<6	/* not enough frames found */
#define ERR_EMPTY		1<<7	/* empty file */
#define ERR_LONGFNAME		1<<8	/* too long filename */
#define ERR_BADFNAME		1<<9	/* filename containing bad chars */


#define END_OFFSET 16384 /* the offset from the end to search for the last MP3 header */

/* FIXME */
#define MIN_FRAMES 10  /* a mp3 file has at least this many frames */
#define MIN_VALID  50 /* a mp3 file has at least this percentage valid data */

/* maximum filename length */
#ifndef MAXFNAME
#ifdef _MAX_FNAME
#define MAXFNAME _MAX_FNAME
#else
#define MAXFNAME 255
#endif /* _MAX_FNAME */
#endif /* MAXFNAME */

/* maximum extention length */
#ifndef MAXEXT
#ifdef _MAX_EXT
#define MAXEXT _MAX_EXT
#else
#define MAXEXT 32 
#endif /* _MAX_EXT */
#endif /* MAXEXT */

/* maximum path length */
#ifndef MAXPATH
#ifdef _MAX_PATH
#define MAXPATH _MAX_PATH
#else
#define MAXPATH 2*MAXFNAME
#endif /* _MAX_PATH */
#endif /* MAXPATH */

/* function definitions */
#ifndef HAVE_VSNPRINTF
#ifdef HAVE__VSNPRINTF
#define vsnprintf(str, size, format, ap) _vsnprintf(str, size, format, ap)
#endif
#endif

/* these macro's clear the structs (e.g. set everything to 0) */ 
#define init_frame_info(s)  memset(s, 0, sizeof(frame_info))
#define init_file_info(s)   memset(s, 0, sizeof(file_info))
#define init_total_info(s)  memset(s, 0, sizeof(total_info))

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

/* some sys/stat.h do not define these macro's */
#ifndef S_ISDIR
#define S_ISDIR(mode) (((mode) & S_IFDIR)==(S_IFDIR))
#endif /* S_ISDIR */

#ifndef S_ISREG
#define S_ISREG(mode) (((mode) & S_IFREG)==(S_IFREG))
#endif /* S_ISREG */

/* structs */

typedef struct { /* frame info */
	int version;		/* MPEG version				*/
	int layer;		/* layer				*/
	int bitrate;		/* bitrate in bits per second		*/
	int samplerate;		/* samplerate in Hertz			*/
	int padding;		/* padding (bool)			*/
	int private;		/* private (bool)			*/
	int stereo;		/* stereo (4 modes)			*/
	int jointstereo;	/* joint stereo				*/
	int copyright;		/* copyright (bool)			*/
	int original;		/* original recording			*/
	int emphasis;		/* emphasis				*/
	int length;		/* length in bytes			*/
	int time;		/* time in miliseconds			*/
	int samples;		/* number of samples			*/
	int crc16;		/* crc16 (0==not available, or crc16 value) */
	int offset;		/* offset of this frame			*/
} frame_info;

typedef struct { /* file info */
	CFILE * fp;		/* filepointer to this file		*/
	char * filename;	/* filename of this file		*/
	int length;		/* length of this file (in bytes)	*/
	int lengthcount;	/* length of all frames			*/
	int time;		/* length of this file (in seconds)	*/
	int msec;		/* milliseconds left (see time)		*/
	int frames;		/* number of valid frames		*/
	int alien_total;	/* total number of unidentifiable bytes	*/
	int alien_before;	/* junk before all frames		*/
	int alien_between;	/* junk between first and last frame	*/
	int alien_after;	/* junk after all frames		*/
	int id3;		/* bitwise OR of 0, ID3V1, ID3V2.	*/
	int version;		/* MPEG version				*/
	int layer;		/* layer				*/
	int bitrate;		/* bitrate in bps			*/
	int bitratecount;	/* add bitrates to calculate an average	*/
	int stereo;		/* boolean				*/
	int samplerate;		/* samplerate in Hertz			*/
	int samples;		/* samples per frame			*/
	int vbr;		/* variable bit rate (boolean)		*/
	int errors;		/* is TRUE or >1 if errors are found	*/
	int lastframe_offset;	/* offset of last frame			*/
	int lastframe_length;	/* length of last frame			*/
	int ismp3file;		/* TRUE if this file is an mp3 file	*/
} file_info;

typedef struct { /* total info */
	int filecount;		/* number of files processed		*/
	int minbitrate;		/* minimum bitrate			*/
	int maxbitrate;		/* maximum bitrate			*/
	int totalbitrate;	/* count of bitrates, to calculate avg	*/
	int goodcount;		/* number of good files			*/
	int time;		/* total time				*/
} total_info;

typedef struct { /* error info */
	int errflag;
	char shortdesc[10];
	char longdesc[40];
} error_info;

#include "proto.h"	/* prototypes */

#endif /* _MPCK_H */
