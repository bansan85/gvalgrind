#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])

AC_CONFIG_HEADERS([config.h])

AC_INIT(gvalgrind, 0.0.0.2, devel@2lgc.org, gvalgrind, http://2lgc.org)
AC_CONFIG_SRCDIR([src/main.c])
AM_INIT_AUTOMAKE
AC_CONFIG_MACRO_DIR([m4])

LT_INIT

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

for NEW_FLAG in \
    -fmax-errors=10 \
    -fstack-protector-all \
    -fstrict-overflow \
    -Wall \
    -Wcast-align \
    -Wcast-qual \
    -Wconditionally-supported \
    -Wconversion \
    -Wextra \
    -Wfloat-equal \
    -Wformat=2 \
    -Wformat-nonliteral \
    -Wformat-security \
    -Wformat-extra-args\
    -Wformat-zero-length \
    -Wformat-y2k \
    -Wformat-signedness \
    -Winit-self \
    -Winline \
    -Wlogical-op \
    -Wmissing-format-attribute \
    -Wno-comment \
    -Wno-error=deprecated-declarations \
    -Wno-overloaded-virtual \
    -Wno-unused-parameter \
    -Wold-style-cast \
    -Wpacked \
    -Wpedantic \
    -Wpointer-arith \
    -Wredundant-decls \
    -Wshadow \
    -Wsign-conversion \
    -Wstrict-aliasing=1 \
    -fstrict-aliasing \
    -Wstrict-overflow=5 \
    -Wswitch-default \
    -Wswitch-enum \
    -Wtrampolines \
    -Wundef \
    -Wuninitialized \
    -Wunused-parameter \
    -Wuseless-cast \
    -Wsuggest-attribute=pure \
    -Wsuggest-attribute=const \
    -Wsuggest-attribute=noreturn \
    -Wsuggest-attribute=format \
    -Wmissing-format-attribute \
    -Wsuggest-final-types \
    -Wsuggest-final-methods \
    -Wwrite-strings \
    -Wzero-as-null-pointer-constant \
    -Wctor-dtor-privacy \
    -Wc++11-compat \
    -Weffc++ \
    -Wstrict-null-sentinel \
    -Woverloaded-virtual \
    -Weverything \
    -Winconsistent-missing-override
do
  AC_MSG_CHECKING([if $CC accepts $NEW_FLAG])
  OLD_CFLAGS="$CFLAGS"
  CFLAGS="$CFLAGS $NEW_FLAG -Werror"
#AC_COMPILE_IFELSE utilise CFLAGS pas AM_CFLAGS
  AC_COMPILE_IFELSE([AC_LANG_SOURCE(
    [void foo(void); void foo(void) { }])], [
    AM_CFLAGS="$AM_CFLAGS $NEW_FLAG"
    AC_MSG_RESULT([yes])
  ], [
    AC_MSG_RESULT([no])
  ])
  CFLAGS="$OLD_CFLAGS"
done
AC_SUBST(CFLAGS)


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
