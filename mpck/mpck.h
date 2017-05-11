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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* some definitions for other platforms */
#ifdef _WIN32
#define HAVE_STDLIB_H
#define HAVE_MALLOC_H
#define HAVE_MEMORY_H
#define HAVE__VSNPRINTF
#define HAVE_STRING_H
#define HAVE_ERRNO_H
#define HAVE_SYS_STAT_H
#define HAVE_WINDOWS_H
#define HAVE_IO_H
#define HAVE_FCNTL_H
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
#define VERSION "0.20"
#endif /* PACKAGE_VERSION */
#endif /* VERSION */

#define PROGNAME	"mpck"
#define HOMEPAGE	"https://github.com/Sjord/checkmate"
#define AUTHOR		"Sjoerd Langkemper <sjoerd-mpck@linuxonly.nl>"

/* constants */

#define FALSE_STRING "no"
#define TRUE_STRING  "yes"

#define TRUE    1
#define FALSE   0

#define EXTENSION_MARK '.'	/* the dot in foobar.mp3 */
#define DEF_EXTENSION "mp3"		/* default extention */

#define GOODFILE "Ok"	/* the text to print if a file is fine */
#define BADFILE  "Bad"	/* the text to print if a file is corrupt */
#define BADTYPE  "No MP3"	/* the text to print if the filetype is wrong */

/* FIXME make enums */

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
/*                                 4					*/
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

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

#endif /* _MPCK_H */
