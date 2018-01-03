#
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(quid, 1.6, yorick17@outlook.com)
AM_INIT_AUTOMAKE([1.9])
AC_CONFIG_SRCDIR([src/quid.c])

# Checks for programs.
AC_PROG_CC
AM_PROG_LIBTOOL

# Involve libtool
LT_INIT

# Checks for libraries.
AC_CHECK_LIB(rt)

# Checks for header files.
AC_CHECK_HEADERS([stdio.h time.h sys/time.h ctype.h])

# Checks for library functions.
AC_CHECK_FUNCS([gettimeofday strtol])

AC_PREFIX_DEFAULT(/usr)

AC_OUTPUT(Makefile src/Makefile test/Makefile include/Makefile utils/Makefile)
