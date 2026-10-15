## Automake

bin_PROGRAMS = mpck
mpck_SOURCES = \
	bufio.c \
	bufio.h \
	checkarguments.c \
	checkarguments.h \
	checkfile.c \
	checkfile.h \
	checkframe.c \
	checkframe.h \
	crc.c \
	crc.h \
	file.c \
	file.h \
	filename.c \
	filename.h \
	frame.c \
	frame.h \
	metatag.c \
	metatag.h \
	main.c \
	matrices.h \
	mp3errors.h \
	mpck.h \
	options.c \
	options.h \
	print.c \
	print.h \
	synchsafe.h \
	total.c \
	total.h \
	bitfile.c \
	bitfile.h \
	layer2.c \
	layer2.h
mpck_LDADD = @GETOPT_LONG_LIB@

# Test suite.  It generates its input files at test time, so the test data
# directory can be cached between runs and is removed again on distclean.
if HAVE_TEST_TOOLS
TESTS = test/run-tests.sh
AM_TESTS_ENVIRONMENT = \
	MPCK='$(abs_top_builddir)/mpck/mpck'; export MPCK; \
	TESTDATA='$(abs_builddir)/test/data'; export TESTDATA;
endif

EXTRA_DIST = \
	test/crctest.sh \
	test/run-tests.sh \
	test/testlib.sh

distclean-local:
	rm -rf test/data
