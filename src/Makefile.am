# SUBDIRS = scid

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS=wxScid

wxScid_SOURCES= \
	App.cpp \
	MainFrame.cpp \
	widgets/ChessBoard.cpp

wxScid_CXXFLAGS  = $(AM_CXXFLAGS) @WX_CXXFLAGS@
# wxScid_LDADD   = $(AM_LIBS) @WX_LIBS@ scid/libscid.la
wxScid_LDADD   = $(AM_LIBS) @WX_LIBS@
wxScid_LDFLAGS = $(AM_LDFLAGS)

MAINTAINERCLEANFILES = \
	Makefile.in