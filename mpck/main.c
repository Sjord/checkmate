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
 *   main.c - this is where the program starts
 * 
 */

#include "mpck.h"
#include "total.h"
#include "print.h"
#include "checkarguments.h"
#include "options.h"
#include <getopt.h>

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

/* needed for getopt */
char *
_getprogname() {
	return PROGNAME;
}

int
parse_options(argc, argv)
	int argc;
	char *argv[];
{	
	
	int ch;			/* option */
	int option_index;
	int maxname=0;

	struct option long_options[] = {
		{"version",   0, 0, 'V'},
		{"verbose",   0, 0, 'v'},
		{"help",      0, 0, 'h'},
		{"quiet",     0, 0, 'q'},
		{"silent",    0, 0, 'q'},
		{"recursive", 0, 0, 'R'},
		{"extension", 1, 0, 'e'},
		{"maxname",   1, 0, 'm'},
		{"badonly",   0, 0, 'B'},
		{"namecheck", 0, 0, 'n'},
		{"xmloutput", 0, 0, 'x'},
		{NULL, 0, 0, 0}
	};

	while ((ch = getopt_long(argc, argv, "Vv::hqRre:m:Bnx", long_options, &option_index)) >32) {
		switch (ch) {
			case 'R':
				/* recursion */
				options_set_recursive(TRUE);
				break;
			case 'r':
				/* -r == -R -e mp3 */
				options_set_extension(DEF_EXTENSION);
				options_set_recursive(TRUE);
				break;
			case 'V':
				/* version info */
				print_version();
				exit(0);
			case 'v':
				/*  verbose  */
				options_set_verbose(TRUE);
	
				break;
			case 'e':
				/* Only scan files with this extension */
				if (strlen(optarg)>MAXEXT) {
					error(" extension is too long");
					exit(ENAMETOOLONG);
				}
				options_set_extension(optarg);
				break;
			case 'm':
				maxname=atoi(optarg);
				if (maxname<=0) {
					error(" %s is not a valid argument to -m", optarg);
					exit(EINVAL);
				}
				options_set_maxname(maxname);
				break;
			case 'h':
				print_usage();
				exit(0);
			case 'q':
				/* Only print either good or bad for a file */
				options_set_quiet(TRUE);
				break;
			case 'B':
				/* Only report bad files */
				options_set_badonly(TRUE);
				break;
			case 'n':
				/* Check for strange chars in filenames */
				options_set_namecheck(TRUE);
				break;
			case 'x':
				/* Print XML output */
				options_set_xmloutput(TRUE);
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
	int res;
	total_info * total;
	
	opt_count = parse_options(argc, argv);

	/* skip options */
	argv += opt_count;
	argc -= opt_count;

	/* If there's no data after the flags, return an error. */
	if (*argv == NULL) {
		error(" no filename specified");
		print_usage();
		return 1;
	}

	/* call checkfile for each file in the argument list 
	 * and put the result in total.							*/
	total = total_create();
	checkarguments(argv, total);
	total_print(total);
	res = total_allgood(total);
	total_destroy(total);

	if (res) {
		/* nothing wrong */
		exit(0);	
	} else {
		/* at least one file is damaged */
		exit(1);
	}
} /* main */ 
