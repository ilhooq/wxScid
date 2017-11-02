SUBDIRS = scid

ACLOCAL_AMFLAGS = -I m4

bin_PROGRAMS=wxscid

wxscid_SOURCES= \
	App.cpp \
	MainFrame.cpp \
	events.cpp \
	widgets/ChessBoard.cpp \
	widgets/GamesListCtrl.cpp \
	widgets/GameTxtCtrl.cpp

wxscid_CXXFLAGS  = $(AM_CXXFLAGS) @WX_CXXFLAGS@
wxscid_LDADD   = $(AM_LIBS) @WX_LIBS@ scid/libscid.la
wxscid_LDFLAGS = $(AM_LDFLAGS)

MAINTAINERCLEANFILES = \
	Makefile.in