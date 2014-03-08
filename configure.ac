#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([smallproject], [0.0.1], [vbkaisetsu@gmail.com])
AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([config.h])

if test -z $CXXFLAGS; then
    CXXFLAGS="-g -O3"
fi
CXXFLAGS="${CXXFLAGS} -std=c++11"

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AM_INIT_AUTOMAKE([-Wall])

AC_CHECK_LIB([gflags], [getenv], [], [AC_MSG_ERROR([Please install google-gflags library])])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 test/Makefile])
AC_OUTPUT
