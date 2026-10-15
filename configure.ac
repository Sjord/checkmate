#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT([checkmate], [0.21], [sjoerd-mpck@linuxonly.nl])
AM_INIT_AUTOMAKE([foreign])
AM_CONFIG_HEADER(config.h)

AC_CONFIG_SRCDIR([mpck/main.c])
# AC_CONFIG_HEADER(config.h)

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET

# Programs used by the test suite.  The tests are only hooked into
# "make check" when the encoders and sox are available.
AC_PATH_PROG([LAME], [lame], [])
AC_PATH_PROG([TWOLAME], [twolame], [])
AC_PATH_PROG([SOX], [sox], [])
AM_CONDITIONAL([HAVE_TEST_TOOLS],
	[test -n "$LAME" && test -n "$TWOLAME" && test -n "$SOX"])
if test -z "$LAME" || test -z "$TWOLAME" || test -z "$SOX"; then
	AC_MSG_WARN([lame, twolame and sox are required to run "make check"])
fi

# python3-mutagen is used to generate the APE tag fixtures.  Without it the
# rest of the test suite still runs, but the APE tag tests are skipped.
AC_PATH_PROG([PYTHON], [python3], [])
if test -n "$PYTHON" && $PYTHON -c "import mutagen" >/dev/null 2>&1; then
	HAVE_MUTAGEN=yes
else
	HAVE_MUTAGEN=no
	AC_MSG_WARN([python3-mutagen is needed to test the APE tag handling])
fi

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADER([getopt.h], , CPPFLAGS="$CPPFLAGS -I../libgnugetopt-1.2")
AC_CHECK_HEADERS([unistd.h errno.h getopt.h malloc.h memory.h sys/mman.h fcntl.h stdint.h])
AC_HEADER_DIRENT

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([vsnprintf])

AC_CONFIG_LIBOBJ_DIR([libgnugetopt-1.2])
AC_CHECK_FUNC([getopt_long], [GETOPT_LONG_LIB=], [GETOPT_LONG_LIB=../libgnugetopt-1.2/libgnugetopt.a] )
AC_SUBST(GETOPT_LONG_LIB)
AM_CONDITIONAL(COMPILE_GETOPT_LIB, test x$GETOPT_LONG_LIB != x)

AC_PROG_RANLIB

AC_CONFIG_FILES([Makefile mpck/Makefile libgnugetopt-1.2/Makefile])
AC_OUTPUT
