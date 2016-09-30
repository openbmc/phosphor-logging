AC_PREREQ([2.69])
AC_INIT([phosphor-logging], [1.0], [https://github.com/openbmc/phosphor-logging])
AM_INIT_AUTOMAKE([subdir-objects -Wall -Werror foreign dist-xz])
AM_PROG_AR()
AM_SILENT_RULES([yes])

# Checks for programs
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX(14, [noext])
AC_PROG_INSTALL #Checks/sets the install variable to be used
AC_PROG_MAKE_SET

PKG_CHECK_MODULES([SYSTEMD], [libsystemd >= 221])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT