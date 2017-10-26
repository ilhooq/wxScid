ACLOCAL_AMFLAGS = -I m4

EXTRA_PROGRAMS = scidbase

scidbase_SOURCES = scidbase.cpp

scidbase_CXXFLAGS  = $(AM_CXXFLAGS) -std=c++11
scidbase_LDADD   = $(AM_LIBS) @top_srcdir@/src/scid/libscid.la
scidbase_LDFLAGS = $(AM_LDFLAGS)

tests: scidbase

.PHONY: tests

MAINTAINERCLEANFILES = \
	Makefile.in