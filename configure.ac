dnl Process this file with autoconf to produce a configure script.

dnl Check for autoconf version > 2.59
AC_PREREQ(2.59)

dnl Define package version
AC_INIT(wxscid, 1.0)

dnl Put resulting configuration defines in this header file.
AC_CONFIG_HEADERS([config.h])

dnl Specify the src directory
AC_CONFIG_SRCDIR([src/App.cpp])

dnl Specify the directory where to copy m4 macros
AC_CONFIG_MACRO_DIR([m4])

dnl foreign tells to Automake that you don't want to follow GNU standard and you don't need 
dnl mandatory documentation files: INSTALL, NEWS, README, AUTHORS, ChangeLog and COPYING
AM_INIT_AUTOMAKE([foreign])

dnl Include maintainer mode targets in Makefile
AM_MAINTAINER_MODE

dnl  Check for compilers
dnl AC_PROG_CC
AC_PROG_CXX

AC_CHECK_LIB(pthread, pthread_create, [PTHREAD_LIBS+=-lpthread])
AC_SUBST(PTHREAD_LIBS)

dnl Check and init WxWidgets
WX_CONFIG_OPTIONS
WX_STANDARD_OPTIONS([debug,unicode,shared,toolkit,wxshared])

WX_CONVERT_STANDARD_OPTIONS_TO_WXCONFIG_FLAGS
WX_CONFIG_CHECK([3.0.0], [wxWin=1],,[base,core,xrc,aui,richtext],[$WXCONFIG_FLAGS])
WX_DETECT_STANDARD_OPTION_VALUES

# Initialize libtool
LT_PREREQ([2.2])
LT_INIT([disable-static])

# SCID code version. See https://sourceforge.net/p/scid/code/
SCID_VERSION=4.6.5
AC_SUBST(SCID_VERSION)

AC_CONFIG_FILES([
	Makefile
	resources/Makefile
	src/Makefile
	src/scid/Makefile
	tests/Makefile
])

AC_OUTPUT

