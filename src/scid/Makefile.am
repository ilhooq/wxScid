ACLOCAL_AMFLAGS = -I m4

#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib
lib_LTLIBRARIES = libscid.la

# Sources for libscid
libscid_la_SOURCES = \
	scid.cpp \
	scidbase.cpp \
	bytebuf.cpp \
	dbasepool.cpp \
	codec_scid4.cpp \
	game.cpp \
	index.cpp \
	matsig.cpp \
	mfile.cpp \
	misc.cpp \
	position.cpp \
	sortcache.cpp \
	stored.cpp \
	textbuf.cpp

# Linker options
libscid_la_LIBADD  = ${PTHREAD_LIBS}
libscid_la_LDFLAGS = \
	$(AM_LDFLAGS) \
	-no-undefined \
	-release ${SCID_VERSION}
# -export-symbols-regex ^scid_

# Compiler options.
libscid_la_CPPFLAGS = $(AM_CXXFLAGS) -std=c++14

MAINTAINERCLEANFILES = \
	Makefile.in