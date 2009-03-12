AC_INIT(cpptut, 1.0, michael@openismus.com)
AC_PREREQ([2.58])

AC_CONFIG_SRCDIR([src/coll.cpp])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.8 -Wall gnu no-define nostdinc check-news])
AC_ARG_VAR([ACLOCAL_FLAGS], [aclocal flags, e.g. -I <macro dir>])

AC_PROG_CC
AC_PROG_CXX

DK_ARG_ENABLE_WARNINGS([CPP_TUTORIAL_WARNINGS],
                       [-Wall],
                       [-Wall -Wextra -Wno-missing-field-initializers])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
