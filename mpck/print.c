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

#ifdef HAVE_TCHAR_H
#include <tchar.h>
#endif

void
print_version(void)
{
	_tprintf(TEXT("%s %s\n"), PROGNAME, VERSION);
	_tprintf(TEXT("Copyright (C) 2006 Sjoerd Langkemper\n"));
	_tprintf(TEXT("This program comes with ABSOLUTELY NO WARRANTY.\n"));
	_tprintf(TEXT("You may redistribute copies of this program\n"));
	_tprintf(TEXT("under the terms of the GNU General Public License.\n"));
	_tprintf(TEXT("For more information about these matters, see the file named COPYING.\n"));
}

void
print_usage(void)
{
	_tprintf(TEXT("%s %s - checks a MP3 file for errors\n\n"), PROGNAME, VERSION);
	_tprintf(TEXT("Usage: %s [OPTION]... [FILE]...\n\n"), PROGNAME);
	_tprintf(TEXT("Verbosity:\n"));
	_tprintf(TEXT("   -v, --verbose        print some extra info\n"));
	_tprintf(TEXT("   -q, --quiet          print only %s or %s per file\n"), GOODFILE, BADFILE);
	_tprintf(TEXT("   -B, --badonly        only report bad files\n"));
	_tprintf(TEXT("Other options:\n"));
	_tprintf(TEXT("   -R, --recursive      check directories recursively\n"));
	_tprintf(TEXT("   -e, --extention=EXT  only check files ending on .EXT\n"));
	_tprintf(TEXT("   -r                   short for -R -e mp3\n"));
	_tprintf(TEXT("   -m, --maxname=NUM    report bad for filenames which exceed NUM characters\n"));
	_tprintf(TEXT("   -n, --namecheck      check for strange characters in the filename\n"));
	_tprintf(TEXT("   -x, --xmloutput      output results in XML\n"));
	_tprintf(TEXT("   -h, --help           print this help, then exit\n"));
	_tprintf(TEXT("   -V, --version        print version information\n"));
}

void 
error(const TCHAR * format, ...) {
	va_list ap;  
	TCHAR buf[1024];
	int len;
	
	va_start(ap, format);
	len = _vsntprintf(buf, sizeof(buf), format, ap);
	va_end(ap);
	
	if (len < 0) exit(EINVAL);

	_ftprintf(stderr, TEXT("%s: %s\n"), PROGNAME, buf);
}

void
fileerror(const TCHAR * filename, const TCHAR * error) {
	_ftprintf(stderr, TEXT("%s:%s: %s\n"), PROGNAME, filename, error);
}
	
void 
offseterror(const file_info * f, const TCHAR * format, ...) {
	va_list ap;  
	TCHAR buf[1024];
	int len;
	
	va_start(ap, format);
	len = _vsntprintf(buf, sizeof(buf), format, ap);
	va_end(ap);
	
	if (len < 0) exit(EINVAL);

	_ftprintf(stderr, TEXT("%s:%s:%zu: %s\n"), PROGNAME, f->filename, cftell(f->fp), buf);
}

/* print something to let the user know we are scanning */
void
print_scanning(filename)
	const TCHAR * filename;
{
	_tprintf(TEXT("SUMMARY: %s\n"), filename);
}

