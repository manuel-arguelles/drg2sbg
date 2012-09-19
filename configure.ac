#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([drg2sbg],
        m4_esyscmd([./GIT-VERSION-GEN]),
        [manuel.arguelles@gmail.com])

AC_CONFIG_MACRO_DIR([m4]) 
AC_CONFIG_HEADER(config.h)

AM_INIT_AUTOMAKE([foreign])

# Checks for programs.
AC_PROG_CC

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset strerror])

AC_CONFIG_FILES([Makefile src/Makefile doc/Makefile])

AC_OUTPUT

