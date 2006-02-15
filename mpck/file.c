/*
 *   This file is part of Checkmate, a program to check MP3 files for errors
 *   
 *   Copyright (C)  2006  Sjoerd Langkemper
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
 *   file.c - keeps file stats
 * 
 */

#include "mpck.h"
#include "file.h"
#include "mp3errors.h"
#include "options.h"

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

char *
file_strversion(file)
	file_info * file;
{
	char * result;
	int version = file->version;
	if (version == MPEG_VER_10) result="MPEG v1.0";
	else if (version == MPEG_VER_20) result="MPEG v2.0";
	else if (version == MPEG_VER_25) result="MPEG v2.5";
	return result;
}

static void file_print_result(file)
	const file_info * file;
{
	printf("    %-30s%s\n", "result", ((file->errors) ? BADFILE : GOODFILE));
}

static void file_print_bitrate(file)
	const file_info * file;
{
	if (file->vbr) {
		printf("    %-30s%d bps (VBR)\n", "average bitrate", file->bitrate);
	} else {
		printf("    %-30s%d bps\n", "bitrate", file->bitrate);
	}
}

static void file_print_ubytes(file)
	const file_info * file;
{
	printf("    %-30s%d b (%d%%)\n", "unidentified", file->alien_total,
			100*file->alien_total/file->length);
	if (options_get_verbose()) {
		if (file->alien_total>0) {
			printf("        %-26s%d b\n", "before 1st frame", file->alien_before);
			printf("        %-26s%d b\n", "between frames", file->alien_between);
			printf("        %-26s%d b\n", "after last frame", file->alien_after);
		}
	}
}

static void
file_print_basic(file)
	const file_info * file;
{
	printf("    %-30s%s\n", "version", 	file_strversion(file));
	printf("    %-30s%d\n", "layer",	file->layer);
	file_print_bitrate(file);
	printf("    %-30s%d Hz\n", "samplerate", file->samplerate);
	printf("    %-30s%d\n", "frames", 	file->frames);
	printf("    %-30s%d:%02d.%03d\n", "time", file->time/60, file->time%60, file->msec);
	file_print_ubytes(file);
}

static void file_print_errors(file)
	const file_info * file;
{
	int first=TRUE;
	int x;

	if (!file->errors) {
		printf("    %-30s%s\n", "errors", "none");
		return;
	}

	for (x=0; x < errordescs_siz; x++) {
		if (file->errors & errordescs[x].errflag) {
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
	
static char *
strbool(value)
	int value;
{
	if (value == 0) return FALSE_STRING;
	else return TRUE_STRING;
}

static void
file_print_lastframe(file)
	const file_info * file;
{
	printf("    %-30s\n", "last frame");
	if (file->lastframe_offset == 0) {
		printf("        %s\n", "not found");
	} else {
		printf("        %-26s%d b (0x%x)\n", "offset", file->lastframe_offset, file->lastframe_offset);
		printf("        %-26s%d\n", "length", file->lastframe_length);
	}
}

static void
file_print_verbose(file)
	const file_info * file;
{
	printf("    %-30s%s\n", "stereo", strbool(file->stereo));
	printf("    %-30s%d KiB\n", "size", file->length/1024);
	printf("    %-30s%s\n", "ID3V1", strbool(file->id3&ID3V1));
	printf("    %-30s%s\n", "ID3V2", strbool(file->id3&ID3V2));
	file_print_lastframe(file);
}

static void
file_print_terse(file)
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

file_info *
file_create() 
{
	file_info * file;
	
	file = (file_info *) malloc(sizeof(file_info));
	if (file == NULL) return NULL;
	file_clear(file);
	return file;
}

void file_destroy(file)
	file_info * file;
{
	free(file);
}

int
file_update(file, frame)
	file_info       * file;
	const frame_info  * frame;
{
        file->frames++;
        file->lengthcount += frame->length;
        if (file->bitrate != frame->bitrate) {
                if ((!file->vbr) && (file->bitrate) && (frame->bitrate)) {
                        file->vbr=TRUE;
                }
        }
        file->bitratecount += frame->bitrate;
	/* FIXME set only once */
        file->bitrate = frame->bitrate;
        file->version = frame->version;
        file->layer = frame->layer;
        file->samplerate = frame->samplerate;
        file->stereo = frame->stereo;
        file->samples = frame->samples;

        file->alien_between += file->alien_after;
        file->alien_after = 0;
	return TRUE;
}

void
file_final(file) 
	file_info * file;
{
	int samplesleft;
	
	/* calculate time and avg bitrate */
	if (file->vbr) {
		file->bitrate=8*file->lengthcount/file->frames*file->samplerate/file->samples;
	}
	if (file->samplerate) {
		
		file->time=(file->frames*file->samples)/file->samplerate;
		samplesleft=(file->frames*file->samples)%file->samplerate;
		file->msec=(1000*samplesleft)/file->samplerate;
	}
	
	file->alien_total=file->alien_before+file->alien_between+file->alien_after;

	/* look for errors */
	if (file->length == 0) {
		file->errors |= ERR_EMPTY;
	} else if (100-(100*file->alien_total/file->length)<MIN_VALID) {
		file->errors |= ERR_LFRAMES;
	}

	if (file->frames<MIN_FRAMES) file->errors |= ERR_LFRAMES;
	if (file->alien_between) file->errors |= ERR_UBYTES;

	file->ismp3file=(!((file->errors&ERR_EMPTY)||(file->errors&ERR_LFRAMES))); 

}

void
file_clear(file) 
	file_info * file;
{
	memset(file, 0, sizeof(file_info));
}

void
file_print(file)
	const file_info * file;
{
	int verbose = options_get_verbose();
	int quiet = options_get_quiet();
	int badonly = options_get_badonly();
	
	if (quiet) {
		if (!badonly || file->errors) file_print_terse(file);
	} else {
		if (badonly) {
			if (file->errors) {
				print_scanning(file->filename);
			} else {
				return;
			}
		}

		if (file->length == 0) {
			printf("    empty file\n\n");
			return;
		}
		if (!(file->ismp3file)) {
			printf("    no MP3 file\n\n");
			if (!verbose) return;
		}
		file_print_basic(file);
		if (verbose) file_print_verbose(file);
		file_print_errors(file);
		file_print_result(file);
		printf("\n");
	}
}

