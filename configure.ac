AC_PREREQ([2.69])
AC_INIT([phosphor-logging], [1.0], [https://github.com/openbmc/phosphor-logging/issues])
y
AM_INIT_AUTOMAKE([subdir-objects -Wall -Werror foreign dist-xz])
AM_PROG_AR()
AM_SILENT_RULES([yes])

# Checks for programs
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_14([noext])
AX_APPEND_COMPILE_FLAGS([-fpic -Wall -Werror], [CXXFLAGS])
AC_PROG_INSTALL #Checks/sets the install variable to be used
AC_PROG_MAKE_SET

# Ensure required python, will fail configure if not found
AM_PATH_PYTHON([2.7], [AC_SUBST([PYTHON], [echo "$PYTHON"])])
AC_SUBST([PYTHONDIR], ${pythondir})

# Surpress the --with-libtool-sysroot error
LT_INIT

PKG_CHECK_MODULES([SYSTEMD], [libsystemd >= 221])

AC_CHECK_HEADER(systemd/sd-journal.h, ,[AC_MSG_ERROR([Could not find \
systemd/sd-journal.h...systemd developement package required])])

# Check/set gtest specific functions.
AX_PTHREAD([GTEST_CPPFLAGS="-DGTEST_HAS_PTHREAD=1"],[GTEST_CPPFLAGS="-DGTEST_HAS_PTHREAD=0"])
AC_SUBST(GTEST_CPPFLAGS)

# Test cases require SDK so only build if we're told to (and SDK is available)
AC_ARG_ENABLE([oe-sdk],
    AS_HELP_STRING([--enable-oe-sdk], [Link testcases absolutely against OE SDK so they can be ran within it.])
)
AC_ARG_VAR(OECORE_TARGET_SYSROOT,
    [Path to the OE SDK SYSROOT])
AS_IF([test "x$enable_oe_sdk" == "xyes"],
    AS_IF([test "x$OECORE_TARGET_SYSROOT" == "x"],
          AC_MSG_ERROR([OECORE_TARGET_SYSROOT must be set with --enable-oe-sdk])
    )
    AC_MSG_NOTICE([Enabling OE-SDK at $OECORE_TARGET_SYSROOT])
    [
        testcase_flags="-Wl,-rpath,\${OECORE_TARGET_SYSROOT}/lib"
        testcase_flags="${testcase_flags} -Wl,-rpath,\${OECORE_TARGET_SYSROOT}/usr/lib"
        testcase_flags="${testcase_flags} -Wl,-dynamic-linker,`find \${OECORE_TARGET_SYSROOT}/lib/ld-*.so | sort -r -n | head -n1`"
    ]
    AC_SUBST([OESDK_TESTCASE_FLAGS], [$testcase_flags])
)

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile test/Makefile tools/Makefile tools/setup.py])
AC_OUTPUT