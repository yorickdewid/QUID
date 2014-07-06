#
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(quid, 1.0, tech@quenza.net)
AM_INIT_AUTOMAKE()
AC_CONFIG_SRCDIR([src/quid.c])

# Checks for programs.
AC_PROG_CC
AM_PROG_LIBTOOL

LT_INIT

# Optional arguments
# AC_ARG_WITH(util, [  --with-util             also compile exmaple generator])

# Checks for libraries.
AC_CHECK_LIB(rt, clock_gettime)

# Checks for header files.
AC_CHECK_HEADERS([stdio.h time.h sys/time.h ctype.h])

# Checks for library functions.
AC_CHECK_FUNCS([clock_gettime gettimeofday strtol])

AC_PREFIX_DEFAULT(/usr)

AC_OUTPUT(Makefile src/Makefile include/Makefile utils/Makefile)
