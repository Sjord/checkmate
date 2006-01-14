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
 *   main.c - this is where the program starts
 * 
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "mpck.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

/* Flag variables */
static int first_n_bytes=0; 
static int first_n_frames=0; 
static int quiet=0;		/* only output if a MP3 is good or not	-q */
static char extention[MAXEXT];	/* only scan files with this extention	-e */

/* needed for getopt */
char *
_getprogname() {
	return PROGNAME;
}


/* check_arg decides whether the file must be checked, opens it and passes
 * it to checkfile
 */
int
check_arg(filename)
	char * filename;
{
	file_info _file;
	file_info * file=&_file;
	init_file_info(file);

	/* check if the extention of the file matches the extention */
	if (*extention) {
		if (!extention_match(filename, extention)) {
			return FALSE;
		}
	}

	/* open file */
	if ((file->fp = cfopen(filename, "rb")) != NULL) {
		file->filename=filename;
	} else {
		error("%s: %s", filename, strerror(errno));
		return FALSE;
	}

	file->length=cfilesize(file->fp);
	checkfile(file);

	cfclose(file->fp);

	total_update(file);

	return TRUE;

}

int
parse_options(argc, argv)
	int argc;
	char *argv[];
{	
	
	int ch;			/* option */
	int option_index;
	int verbose=0, maxname=0;

	struct option long_options[] = {
		{"version",   0, 0, 'V'},
		{"verbose",   0, 0, 'v'},
		{"bytes",     1, 0, 'b'},
		{"frames",    1, 0, 'f'},
		{"help",      0, 0, 'h'},
		{"quiet",     0, 0, 'q'},
		{"silent",    0, 0, 'q'},
		{"recursive", 0, 0, 'R'},
		{"extention", 1, 0, 'e'},
		{"maxname",   1, 0, 'm'},
		{"badonly",   0, 0, 'B'},
		{"namecheck", 0, 0, 'n'},
		{NULL, 0, 0, 0}
	};

	extention[0]=0;

	while ((ch = getopt_long(argc, argv, "Vv::b:f:hqRre:m:Bn", long_options, &option_index)) >32) {
		switch (ch) {
			case 'R':
				/* recursion */
				set_recursive(TRUE);
				break;
			case 'r':
				/* -r == -R -e mp3 */
				strcpy(extention, DEF_EXTENTION);
				set_recursive(TRUE);
				break;
			case 'V':
				/* version info */
				print_version();
				exit(0);
			case 'v':
				/*  verbose  */
				verbose++;

				if (optarg && *optarg == 'v') {
					error(" switching to mega-super-power-boosting verbosity mode...");
					verbose++;
				}
				set_verbose(verbose);
	
				break;

			case 'b':
				/*  Only scan the first # of bytes, then quit.*/
				first_n_bytes=atoi(optarg);
				if (first_n_bytes<=0) {
					error(" %s is not a valid argument to -b", optarg);
					exit(EINVAL);
				}
				set_first_n_bytes(first_n_bytes);
				break;
			case 'f':
				/* Only scan the first # of frames, then quit.*/
				first_n_frames=atoi(optarg);
				if (first_n_frames<=0) {
					error(" %s is not a valid argument to -f", optarg);
					exit(EINVAL);
				}
				set_first_n_frames(first_n_frames);
				break;
			case 'e':
				/* Only scan files with this extention */
				if (strlen(optarg)>MAXEXT) {
					error(" extention is too long");
					exit(ENAMETOOLONG);
				}
				strcpy(extention, optarg);
				break;
			case 'm':
				maxname=atoi(optarg);
				if (maxname<=0) {
					error(" %s is not a valid argument to -m", optarg);
					exit(EINVAL);
				}
				set_maxname(maxname);
				break;
			case 'h':
				print_usage();
				exit(0);
			case 'q':
				/* Only print either good or bad for a file */
				quiet=TRUE;
				set_quiet(quiet);
				break;
			case 'B':
				/* Only report bad files */
				set_badonly(TRUE);
				break;
			case 'n':
				/* Check for strange chars in filenames */
				set_namecheck(TRUE);
				break;
			case '?':
				error(" ambiguous match or an extraneous parameter");
				exit(EINVAL);
			case ':':
				error(" missing parameter");
				exit(EINVAL);
			default:
				exit(EINVAL);
		}
	}

	/* return the number of parsed words */
	return optind;
}


int
main(argc, argv)
	int argc;
	char *argv[];
{
	int opt_count;

	total_init();

	opt_count=parse_options(argc, argv);
	argv+=opt_count;	/* skip options */

	/* If there's no data after the flags, return an error. */
	if (*argv == NULL) {
		error(" no filenames specified");
//		error(" no filename supplied, your request is denied");
		return 1;
	}

	/* -b and -f screw up the statistics */
	if (!quiet) {
		if (first_n_bytes) error(" only scanning first %d bytes of each file, which probably results in wrong statistics", first_n_bytes);
		if (first_n_frames) error(" only scanning first %d frames of each file, which probably results in wrong statistics", first_n_frames);
	}

	parse_args(argv);

	if (!quiet) total_print();

	if (total_allgood()) {
		/* nothing wrong */
		exit(0);	
	} else {
		/* at least one file is damaged */
		exit(1);
	}
} /* main */ 
