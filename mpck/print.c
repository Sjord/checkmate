/*
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
 *   print.c - functions that print things, like stats and errors 
 * 
 */

#include "mpck.h"
#include "file.h"
#include "options.h"
#include <stdio.h>
#include <stdarg.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

void
print_version(void)
{
	printf("%s %s\n", PROGNAME, VERSION);
	printf("Copyright (C) 2006 Sjoerd Langkemper\n");
	printf("This program comes with ABSOLUTELY NO WARRANTY.\n");
	printf("You may redistribute copies of this program\n");
	printf("under the terms of the GNU General Public License.\n");
	printf("For more information about these matters, see the file named COPYING.\n");
}

void
print_usage(void)
{
	printf("%s %s - checks a MP3 file for errors\n\n", PROGNAME, VERSION);
	printf("Usage: %s [OPTION]... [FILE]...\n\n", PROGNAME);
	printf("Verbosity:\n");
	printf("   -v, --verbose        print some extra info\n");
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
error(const char * format, ...) {
	va_list ap;  
	char buf[1024];
	int len;
	
	va_start(ap, format);
	len = vsnprintf(buf, sizeof(buf), format, ap);
	va_end(ap);
	
	if (len < 0) exit(EINVAL);

	fprintf(stderr, "%s:%s\n", PROGNAME, buf);
}

void 
fileerror(const file_info * f, const char * format, ...) {
	va_list ap;  
	char buf[1024];
	int len;
	
	va_start(ap, format);
	len = vsnprintf(buf, sizeof(buf), format, ap);
	va_end(ap);
	
	if (len < 0) exit(EINVAL);

	error("%s:%s", f->filename, buf);
}
	
void 
offseterror(const file_info * f, const char * format, ...) {
	va_list ap;  
	char buf[1024];
	int len;
	
	va_start(ap, format);
	len = vsnprintf(buf, sizeof(buf), format, ap);
	va_end(ap);
	
	if (len < 0) exit(EINVAL);

	error("%s:%d:%s", f->filename, cftell(f->fp), buf);
}

/* print something to let the user know we are scanning */
void
print_scanning(filename)
	const char * filename;
{
	printf("SUMMARY: %s\n", filename);
}

