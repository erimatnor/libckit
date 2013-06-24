AC_INIT([libckit], [0.1], [erik.nordstrom@gmail.com])
AC_CONFIG_MACRO_DIR([m4])
# Use the silent-rules feature when possible.
m4_ifndef([AM_SILENT_RULES], [m4_define([AM_SILENT_RULES],[])])
AM_SILENT_RULES([yes])
# support for cross compiling
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CPP
AM_PROG_AR
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_CONFIG_HEADERS([config.h])

CFLAGS=
CXXFLAGS=
EXTRA_DEFINES=

dnl Option to set debugging                                                                          
AC_ARG_ENABLE([debug],
AS_HELP_STRING([--enable-debug],[Turn on debugging]),
[case "${enableval}" in
  yes) debug=yes ;;
  no)  debug=no ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],[debug=no])

AM_CONDITIONAL([DEBUG], [test x$debug = xyes])

if test x$debug = xyes
then
     EXTRA_DEFINES="${EXTRA_DEFINES} -DENABLE_DEBUG"
     CFLAGS="$CFLAGS -g -O0 -fno-inline"
     CXXFLAGS="$CXXFLAGS -g -O0 -fno-inline"
else
     CFLAGS="$CFLAGS -O2"
     CXXFLAGS="$CXXFLAGS -O2"
fi

AC_CONFIG_FILES([
 Makefile
 src/Makefile
])
AC_OUTPUT
