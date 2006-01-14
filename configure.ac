#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([checkmate], [0.11], [sjoerd-mpck@linuxonly.nl])
AM_INIT_AUTOMAKE([checkmate], [0.11])
AM_CONFIG_HEADER(config.h)

AC_CONFIG_SRCDIR([mpck/main.c])
# AC_CONFIG_HEADER(config.h)

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADER([getopt.h], , CPPFLAGS="$CPPFLAGS -I../libgnugetopt-1.2")
AC_CHECK_HEADERS([unistd.h errno.h getopt.h malloc.h memory.h])
AC_HEADER_DIRENT

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([vsnprintf])

AC_CONFIG_LIBOBJ_DIR([libgnugetopt-1.2])
AC_CHECK_FUNC([getopt_long], [GETOPT_LONG_LIB=], [GETOPT_LONG_LIB=../libgnugetopt-1.2/libgnugetopt.a] )
AC_SUBST(GETOPT_LONG_LIB)
AM_CONDITIONAL(COMPILE_GETOPT_LIB, test x$GETOPT_LONG_LIB != x)

AC_PROG_RANLIB

AC_CONFIG_FILES([Makefile mpck/Makefile libgnugetopt-1.2/Makefile bitrate/Makefile])
AC_OUTPUT
