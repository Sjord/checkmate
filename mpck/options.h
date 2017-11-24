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
 *   options.h - handles command line options
 * 
 * I hope you like preprocessor macro's: if this file is included from
 * options.c, __OPTIONS_C__ will be defined and the real functions and
 * variables will be defined. In any other case, only the prototypes will
 * be defined.
 *
 */

#undef OPTION
#ifdef __OPTIONS_C__
/* This macro takes a variable name and its type. It defines the variable and
 * the functions options_set_variable and options_get_variable.
 */
#define OPTION(NAME, TYPE) 		\
	static TYPE NAME = (TYPE) 0;	\
	void				\
	options_set_ ## NAME(TYPE value)\
	{				\
		NAME = value;		\
	}				\
	TYPE				\
	options_get_ ## NAME(void) {	\
		return NAME;		\
	}
#else
/* This macro makes prototypes for options_set_variable and 
 * options_get_variable.
 */
#define OPTION(NAME, TYPE) 			\
	void options_set_ ## NAME(TYPE value);	\
	TYPE options_get_ ## NAME(void);
#endif

#define INT_OPTION(NAME) OPTION(NAME, int)
#define STR_OPTION(NAME) OPTION(NAME, char *)

/* Actual options */
INT_OPTION(recursive);
INT_OPTION(verbose);
STR_OPTION(extension);
INT_OPTION(maxname);
INT_OPTION(quiet);
INT_OPTION(badonly);
INT_OPTION(namecheck);
INT_OPTION(xmloutput);
