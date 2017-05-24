/*
 *   This file is part of Checkmate, a program to check MP3 files for rrors
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
 *   print.h - functions that print things, like stats and errors 
 * 
 */

void print_version(void);
void print_usage(void);
void error(const char *format, ...);
void offseterror(const file_info *f, const char *format, ...);
void print_scanning(const char *filename);
