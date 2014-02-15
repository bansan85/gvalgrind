#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(gvalgrind, 0.0.0.2, devel@2lgc.org, gvalgrind, http://2lgc.org)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/fonctions.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
GTK_REQUIRED_VERSION=3.0.3
GLIB_REQUIRED_VERSION=2.28.3
LIBXML_REQUIRED_VERSION=2.7.0
AC_SUBST(XML_CPPFLAGS)
AC_SUBST(XML_LIBS)
AM_PATH_GTK_3_0($GTK_REQUIRED_VERSION, :, AC_MSG_ERROR(Test for GTK+ failed.))
AM_PATH_GLIB_2_0($GLIB_REQUIRED_VERSION, :, AC_MSG_ERROR(Test for GLIB failed.))
AM_PATH_XML2($LIBXML_REQUIRED_VERSION, :, AC_MSG_ERROR(Test for libXML failed.))
AM_GNU_GETTEXT([external])
ALL_LINGUAS="fr"

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([libintl.h locale.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([strstr strtol])

AC_CONFIG_FILES([Makefile po/Makefile.in
                 files/Makefile
                 src/Makefile])
AC_OUTPUT
