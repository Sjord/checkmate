/*
 *   This file is part of Checkmate, a program to check MP3 files for rrors
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
 *   print.c - functions that print things, like stats and errors 
 * 
 */

#include <stdio.h>
#include "mpck.h"
#include "errordescs.h"
#include <stdarg.h>
#include <errno.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

extern int quiet;
extern int verbose;
extern int maxname;

void error(const char * format, ...) {
	va_list ap;  
	char buf[1024];
	int len;
	
	va_start(ap, format);
	len = vsnprintf(buf, sizeof(buf), format, ap);
	va_end(ap);
	
	if (len < 0) exit(EINVAL);

	fprintf(stderr, "%s:%s\n", PROGNAME, buf);
}
	
void fileerror(const file_info * f, const char * format, ...) {
	va_list ap;  
	char buf[1024];
	int len;
	
	va_start(ap, format);
	len = vsnprintf(buf, sizeof(buf), format, ap);
	va_end(ap);
	
	if (len < 0) exit(EINVAL);

	error("%s:%s", f->filename, buf);
}
	
void offseterror(const file_info * f, const char * format, ...) {
	va_list ap;  
	char buf[1024];
	int len;
	
	va_start(ap, format);
	len = vsnprintf(buf, sizeof(buf), format, ap);
	va_end(ap);
	
	if (len < 0) exit(EINVAL);

	error("%s:%d:%s", f->filename, cftell(f->fp), buf);
}
	
char *
strversion(version)
	int version;
{
	char * result;
	if (version==MPEG_VER_10) result="MPEG v1.0";
	else if (version==MPEG_VER_20) result="MPEG v2.0";
	else if (version==MPEG_VER_25) result="MPEG v2.5";
	return result;
}

static char *
strbool(value)
	int value;
{
	if (value==0) return FALSE_STRING;
	else return TRUE_STRING;
}

static char *
strfilenameinfo(int finfo) {
	if (finfo==FILENAME_TOOLONG) return FILENAME_TOOLONG_STR;
	if (finfo==FILENAME_STRANGECHARS) return FILENAME_STRANGECHARS_STR;
	return FILENAME_OK_STR;
}


void
print_frame_info(fi, file)
	const frame_info *fi;
	const file_info  *file;
{
	printf("FRAME %d\n", file->frames);
	printf("    %-30s%s\n", "version", strversion(fi->version));
	printf("    %-30s%d\n", "layer", fi->layer);
	printf("    %-30s%d bps\n", "bitrate", fi->bitrate);
	printf("    %-30s%d Hz\n", "samplerate", fi->samplerate);
	printf("    %-30s%d b\n", "frame_length", fi->length);
	printf("    %-30s%d b (0x%x)\n", "offset", fi->offset, fi->offset);
	printf("    %-30s%d ms\n", "time", fi->time);
	printf("    %-30s%u\n", "crc16", fi->crc16);
/*	printf("    %-30s%d\n", "padding", fi->padding);
	printf("    %-30s%d\n", "stereo", fi->stereo);
	printf("    %-30s%d\n", "copyright", fi->copyright);
	printf("    %-30s%d\n", "original", fi->original);
	printf("    %-30s%d\n", "samples", fi->samples);
	*/
}

/* print something to let the user know we are scanning */
void
print_scanning(file)
	const file_info	* file;
{
	printf("SUMMARY: %s\n", file->filename);
}

void print_file_errors(file)
	const file_info * file;
{
	int first=TRUE;
	int x;

	if (!file->errors) {
		printf("    %-30s%s\n", "errors", "none");
		return;
	}

	for (x=0; x<errordescs_siz; x++) {
		if (file->errors&errordescs[x].errflag) {
			if (first) {
				printf("    %-30s%s\n", "errors", 
						errordescs[x].longdesc);
				first=FALSE;
			} else {
				printf("                                  %s\n", 
						errordescs[x].longdesc);
			}
		}
	}
}

static void print_file_result(file)
	const file_info * file;
{
	printf("    %-30s%s\n", "result", ((file->errors) ? BADFILE : GOODFILE));
}


static void print_bitrate(file)
	const file_info * file;
{
	if (file->vbr) {
		printf("    %-30s%d bps (VBR)\n", "average bitrate", file->bitrate);
	} else {
		printf("    %-30s%d bps\n", "bitrate", file->bitrate);
	}
}

static void print_ubytes(file)
	const file_info * file;
{
	printf("    %-30s%d b (%d%%)\n", "unidentified", file->alien_total,
			100*file->alien_total/file->length);
	if (verbose) {
		if (file->alien_total>0) {
			printf("        %-26s%d b\n", "before 1st frame", file->alien_before);
			printf("        %-26s%d b\n", "between frames", file->alien_between);
			printf("        %-26s%d b\n", "after last frame", file->alien_after);
		}
	}
}

static void
print_file_basic(file)
	const file_info * file;
{
	printf("    %-30s%s\n", "version", 	strversion(file->version));
	printf("    %-30s%d\n", "layer",	file->layer);
	print_bitrate(file);
	printf("    %-30s%d Hz\n", "samplerate", file->samplerate);
	printf("    %-30s%d\n", "frames", 	file->frames);
	printf("    %-30s%d:%02d.%03d\n", "time", file->time/60, file->time%60, file->msec);
	print_ubytes(file);
}

static void
print_lastframe(file)
	const file_info * file;
{
	printf("    %-30s\n", "last frame");
	if (file->lastframe_offset==0) {
		printf("        %s\n", "not found");
	} else {
		printf("        %-26s%d b (0x%x)\n", "offset", file->lastframe_offset, file->lastframe_offset);
		printf("        %-26s%d\n", "length", file->lastframe_length);
	}
}
	
static void
print_file_verbose(file)
	const file_info * file;
{
	printf("    %-30s%s\n", "stereo", strbool(file->stereo));
	printf("    %-30s%d KiB\n", "size", file->length/1024);
	printf("    %-30s%s\n", "ID3V1", strbool(file->id3&ID3V1));
	printf("    %-30s%s\n", "ID3V2", strbool(file->id3&ID3V2));
	print_lastframe(file);
}

void
print_file_info(file)
	const file_info	* file;
{
	if (file->length==0) {
		printf("    empty file\n\n");
		return;
	}
	if (!(file->ismp3file)) {
		printf("    no MP3 file\n\n");
		if (!verbose) return;
	}
	print_file_basic(file);
	if (verbose) print_file_verbose(file);
	print_file_errors(file);
	print_file_result(file);
	printf("\n");
}

void
print_usage(void)
{
	printf("%s %s - checks a MP3 file for errors\n\n", PROGNAME, VERSION);
	printf("Usage: %s [OPTION]... [FILE]...\n\n", PROGNAME);
	printf("Partial checking of files:\n");
	printf("   (using these options produces inaccurate statistics)\n");
	printf("   -b, --bytes=NUM      check only the first NUM bytes\n");
	printf("   -f, --frames=NUM     check only the first NUM frames\n");
	printf("Verbosity:\n");
	printf("   -v, --verbose        print some extra info. Use twice for mega-verbosity\n");
	printf("                        which prints info for each frame. Use with care\n");
	printf("   -q, --quiet          print only %s or %s per file\n", GOODFILE, BADFILE);
	printf("   -B, --badonly	only report bad files\n");
	printf("Other options:\n");
	printf("   -R, --recursive      check directories recursively\n");  
	printf("   -e, --extention=EXT  only check files ending on .EXT\n");
	printf("   -r			short for -R -e mp3\n");
	printf("   -m, --maxname=NUM 	report bad for filenames which exceed NUM characters\n");
	printf("   -n, --namecheck	check for strange characters in the filename\n");
	printf("   -h, --help           print this help, then exit\n");
	printf("   -V, --version        print version information\n");
}

void
print_version(void)
{
	printf("%s %s\n", PROGNAME, VERSION);
	printf("Copyright (C) 2005 Sjoerd Langkemper\n");
	printf("This program comes with ABSOLUTELY NO WARRANTY.\n");
	printf("You may redistribute copies of this program\n");
	printf("under the terms of the GNU General Public License.\n");
	printf("For more information about these matters, see the file named COPYING.\n");
}

void
print_terse_info(file)
	const file_info *file;
{
	if (!file->ismp3file) {
		printf("%s:%s: %s\n", PROGNAME, file->filename, BADTYPE);
	} else if (file->errors) {
		printf("%s:%s: %s\n", PROGNAME, file->filename, BADFILE);
	} else {
		printf("%s:%s: %s\n", PROGNAME, file->filename, GOODFILE);
	}
}


