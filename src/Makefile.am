SUBDIRS = scid

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS=wxscid

wxscid_SOURCES= \
	App.cpp \
	MainFrame.cpp \
	Scid.cpp \
	widgets/ChessBoard.cpp \
	widgets/GamesListCtrl.cpp \
	widgets/GameTxtCtrl.cpp

wxscid_CXXFLAGS  = $(AM_CXXFLAGS) @WX_CXXFLAGS@ -std=c++11
wxscid_LDADD   = $(AM_LIBS) @WX_LIBS@ scid/libscid.la
wxscid_LDFLAGS = $(AM_LDFLAGS)

MAINTAINERCLEANFILES = \
	Makefile.in