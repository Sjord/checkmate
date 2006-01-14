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
 *   checkfile.c - functions for checking the MP3 file 
 * 
 */

#include <errno.h>
#include "mpck.h"
#include <stdio.h>
#include <signal.h>

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

file_info * curfile;

static int partcheck=0; 	/* partial checking		  -b || -f */
static int first_n_bytes=0;	/* check only the first n bytes 	-b */
static int first_n_frames=0;	/* check only the first n frames 	-f */
int verbose=0;		/* verbosity level  				-v */
int quiet=0;		/* only output if a MP3 is good or not		-q */
int maxname=MAXFNAME;	/* report bad for filenames that exceed maxname
			   characters					-m */
int badonly=0;		/* report only the bad files */
int namecheck=0;	/* check for strange characters in filename s   -n */

int set_first_n_bytes(int value) {
	partcheck=TRUE;
	return first_n_bytes=value;
}

int set_first_n_frames(int value) {
	partcheck=TRUE;
	return first_n_frames=value;
}

int set_verbose(int value) {
	return verbose=value;
}

int set_quiet(int value) {
	return quiet=value;
}

int set_maxname(int value) {
	return maxname=value;
}

int set_badonly(int value) {
	return badonly=value;
}

int set_namecheck(int value) {
	return namecheck=value;
}

/* when the user presses Ctrl-C, print the stats of this file and exit */
void
sigint(signal)
	int signal;
{
	if ((quiet)||(badonly)) exit(EINTR);
//	printf("    A SIGINT came along, the statistics are probably wrong.\n");
	printf("    SIGINT received: statistics are probably wrong.\n");
	final_file_stats(curfile);
	print_file_info(curfile);
	exit(EINTR);
}


/* this function gets called whenever there are bytes found
 * which do not belong to a MPEG frame or an ID3 tag. */
#define alienbytes(f, num) do {\
	if (f->frames==0)\
		f->alien_before+=num;\
	else\
		f->alien_after+=num;\
} while(0)

/*
void
alienbytes(f, num)
	file_info * f;
	int num;
{
	if (f->frames==0)
		f->alien_before+=num;
	else
		f->alien_after+=num;
}
*/

/* after a file is scanned, this function does the final work on the
 * statistics
 */
void
final_file_stats(file)
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
	if (file->length==0) {
		file->errors|=ERR_EMPTY;
	} else if (100-(100*file->alien_total/file->length)<MIN_VALID) {
		file->errors|=ERR_LFRAMES;
	}

	if (file->frames<MIN_FRAMES) file->errors|=ERR_LFRAMES;
	if (file->alien_between) file->errors|=ERR_UBYTES;

	file->ismp3file=(!((file->errors&ERR_EMPTY)||(file->errors&ERR_LFRAMES))); 
}
/**
 * starts at the end of the file and searches for the last frame in this file
 **/
int 
check_end(file)
	file_info	*file;
{
	int res;
	int frame_length=32768;
	int frame_offset=0;
	int old_verbose;
	char buf[5];
	frame_info _fi;
	frame_info * fi=&_fi;

	init_frame_info(fi);

	/* temporarily turn off verbosity, since each byte may give an error */
	old_verbose=verbose;
	verbose=FALSE;
	
	/* go somewhere near the end of the file */
	cfseek(file->fp, -END_OFFSET, SEEK_END);
	
	do {
		res=cfread(buf, 1, file->fp);
		if (res<=0) {
			if (verbose) { 
				if (cfeof(file->fp))
					offseterror(file, " EOF reached");
				else
					error(" read error");
			}
			continue;
		}

		if ((buf[0]&0xff)==0xff) { /* possible MP3 frame header */
			res=cfread(buf+1, 1, file->fp);
			if (res<=0) continue;

			if ((buf[1]&224)==224) {
				res=cfread(buf+2, 2, file->fp);
				if (res<2) continue;

				parseframe(file, fi, buf);
				if (checkvalidity(file, fi)) {
					frame_offset=(int)cftell(file->fp)-4;
					calcframestats(file, fi);
				} else {
					cfseek(file->fp, -2, SEEK_CUR);
				}
			}
		} else if (buf[0]=='T') { /* TAG -> ID3v1 tag */
			res=cfread(buf, 2, file->fp);
			if (res<2) continue;
			if ((buf[0]=='A')&&(buf[1]=='G')) {
				frame_length=cftell(file->fp)-3-frame_offset;
			} else {
				cfseek(file->fp, -1, SEEK_CUR);
			}
		} else if (buf[0]=='I') { /* ID3 -> ID3v2 tag */
			res=cfread(buf, 2, file->fp);
			if (res<2) continue;
			if ((buf[0]=='D')&&(buf[1]=='3')) {
				frame_length=cftell(file->fp)-3-frame_offset;
			} else {
				cfseek(file->fp, -1, SEEK_CUR);
			}
		} 
	} while (!cfeof(file->fp));

	/* the frame end at the above found ID3 tag, at the EOF or if the limit
	 * of the frame is reached (e.g. fi->length)
	 */
	if (frame_offset!=0) {
		file->lastframe_length=MIN(frame_length, MIN(fi->length, file->length-frame_offset));
	}

	file->lastframe_offset=frame_offset;

	/* set the file pointer back to the start of the file */
	cfseek(file->fp, 0, SEEK_SET);

	/* turn verbosity back on */
	verbose=old_verbose;

	/* always return TRUE, for it is not an error if we can not find the
	 * last frame */
	return TRUE;
}


int
check_begin(file)
	file_info	*file;
{
	char buf[5];
	int res;


	do {
		/* check if we are too far */
		if (partcheck) {
			if ((first_n_bytes)&&(cftell(file->fp)>first_n_bytes)) break;
			if ((first_n_frames)&&(file->frames>(first_n_frames-1))) break;
		}

		/* check if there are enough frames in the first 100 KiB */
		if ((file->frames<MIN_FRAMES)&&(cftell(file->fp)>102400)) {
			if (verbose) fileerror(file, " Broken out after 102400 bytes");
			file->errors=ERR_LFRAMES;
			break;
		}

		res=cfread(buf, 1, file->fp);
		if (res<=0) {
			if (verbose) { 
				if (cfeof(file->fp))
					offseterror(file, " EOF reached");
				else
					error(" read error");
			}
			continue;
		}
		
		if ((buf[0]&0xff)==0xff) { /* possible MP3 frame header */
			res=cfread(buf+1, 1, file->fp);
			if (res<=0) continue;

			if ((buf[1]&224)==224) {
				res=cfread(buf+2, 2, file->fp);
				if (res<2) continue;

				res=checkframe(file, buf);
				if (!res) {
					/* The frame is damaged */ 
					/* FIXME */
					alienbytes(file, 4);
				}
			} else {
				alienbytes(file, 2);
			}
		} else if (buf[0]=='T') { 	/* TAG -> ID3v1 tag */
			res=cfread(buf, 2, file->fp);
			if (res<2) continue;
			if ((buf[0]=='A')&&(buf[1]=='G')) {
				read_id3v1_tag(file);
			} else {
				alienbytes(file, 3);
			}
		} else if (buf[0]=='I') { 	/* ID3 -> id3v2 tag */
			res=cfread(buf, 2, file->fp);
			if (res<2) continue;
			if ((buf[0]=='D')&&(buf[1]=='3')) {
				read_id3v2_tag(file);
			} else {
				alienbytes(file, 3);
			}
		} else {
			alienbytes(file, 1);
		}
	} while (!cfeof(file->fp));

	return TRUE;
}

/* checks a file */
int checkfile(file)
	file_info	*file;
{
	curfile=file;

	/* let the user know the program is doing something */
	if ((!quiet)&&(!badonly)) print_scanning(file);

	signal(SIGINT, sigint);

	/* unless we are instructed to only touch the beginning of the file,
	 * first scan the last part of the file to determine where the last
	 * frame is.
	 */
	if ((!partcheck)&&(file->length>2*END_OFFSET)) check_end(file);

	check_begin(file);
	
	check_filename(file, maxname, namecheck);

	final_file_stats(file);

	signal(SIGINT, SIG_DFL);

	if (quiet) {
		if ((!badonly)||(file->errors)) print_terse_info(file);
	} else {
		if (badonly) {
			if (file->errors) {
				print_scanning(file);
				print_file_info(file);
			}
		} else {
			print_file_info(file);
		}
	}	

	return TRUE;

}

