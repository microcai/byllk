dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(别样连连看, 1.0)

AC_CONFIG_HEADERS(config.h)

AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE()

AC_PROG_CC_C99


PKG_CHECK_MODULES(GTK2,gtk+-2.0)

PKG_CHECK_MODULES(GTKGL,gtkglext-1.0)


AC_CONFIG_FILES(Makefile src/Makefile)
AC_OUTPUT

