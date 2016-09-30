AC_PREREQ([2.69])
AC_INIT([phosphor-logging], [1.0], [https://github.com/openbmc/phosphor-logging/issues])

AM_INIT_AUTOMAKE([subdir-objects -Wall -Werror foreign dist-xz])
AM_PROG_AR()
AM_SILENT_RULES([yes])

# Checks for programs
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_14([noext])
AX_APPEND_COMPILE_FLAGS([-fpic -Wall -Werror], [CXXFLAGS])
AC_PROG_INSTALL #Checks/sets the install variable to be used
AC_PROG_MAKE_SET

# Surpress the --with-libtool-sysroot error
LT_INIT

PKG_CHECK_MODULES([SYSTEMD], [libsystemd >= 221])

AC_CHECK_HEADER(systemd/sd-journal.h, ,[AC_MSG_ERROR([Could not find \
systemd/sd-journal.h...systemd developement package required])])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT