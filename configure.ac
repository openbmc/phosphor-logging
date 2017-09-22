# Initialization
AC_PREREQ([2.69])
AC_INIT([phosphor-logging], [1.0], [https://github.com/openbmc/phosphor-logging/issues])
AC_LANG([C++])
AM_INIT_AUTOMAKE([subdir-objects -Wall -Werror foreign dist-xz])
AM_SILENT_RULES([yes])

AC_ARG_ENABLE([eloggen],
    AS_HELP_STRING([--disable-eloggen], [Disable elog parser copying]))
AM_CONDITIONAL([WANT_ELOGGEN], [test "x$enable_eloggen" != "xno"])

AC_ARG_ENABLE([phosphor_logging],
    AS_HELP_STRING([--disable-phosphor_logging], [Disable phosphor logging]))
AM_CONDITIONAL([WANT_PHOSPHOR_LOGGING], [test "x$enable_phosphor_logging" != "xno"])
AS_IF([test "x$enable_phosphor_logging" != "xno"], [
    # Checks for typedefs, structures, and compiler characteristics.
    AX_CXX_COMPILE_STDCXX_14([noext])
    AX_APPEND_COMPILE_FLAGS([-fpic -Wall -Werror], [CXXFLAGS])

    # Checks for programs
    AC_PROG_CXX
    AM_PROG_AR
    AC_PROG_INSTALL #Checks/sets the install variable to be used
    AC_PROG_MAKE_SET
    AC_PROG_MKDIR_P
    AC_CHECK_PROG([DIRNAME], dirname, dirname)
    # Python
    AM_PATH_PYTHON([2.7], [AC_SUBST([PYTHON], [echo "$PYTHON"])], [AC_MSG_ERROR(
        [Could not find python-2.7 installed...python-2.7 is required])])

    # Surpress the --with-libtool-sysroot error
    LT_INIT

    # Checks for libraries.
    AX_PKG_CHECK_MODULES([SYSTEMD], [], [libsystemd >= 221], [], [AC_MSG_ERROR(["systemd required and not found."])])

    # sdbusplus is non-private because we have no library to link.
    # If we ever have a library, move this to private.
    AX_PKG_CHECK_MODULES([SDBUSPLUS], [sdbusplus], [], [], [AC_MSG_ERROR(["sdbusplus required and not found."])])
    AX_PKG_CHECK_MODULES([PHOSPHOR_DBUS_INTERFACES], [], [phosphor-dbus-interfaces], [], [AC_MSG_ERROR(["phosphor-dbus-interfaces required and not found."])])

    # Check for sdbus++
    AC_PATH_PROG([SDBUSPLUSPLUS], [sdbus++])
    AS_IF([test "x$SDBUSPLUSPLUS" == "x"],
        AC_MSG_ERROR(["Requires sdbus++"]))

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

    AC_DEFINE(BUSNAME_LOGGING, "xyz.openbmc_project.Logging", [The log manager DBus busname to own.])
    AC_DEFINE(OBJ_LOGGING, "/xyz/openbmc_project/logging", [The log manager DBus object path.])
    AC_DEFINE(OBJ_INTERNAL, "/xyz/openbmc_project/logging/internal/manager", [The private log manager DBus object path.])
    AC_DEFINE(OBJ_ENTRY, "/xyz/openbmc_project/logging/entry", [The log entry DBus object path.])
    AC_DEFINE(INVENTORY_ROOT, "/xyz/openbmc_project/inventory", [The inventory root.])
    AC_DEFINE(CALLOUT_FWD_ASSOCIATION, "callout", [The name of the callout's forward association.])
    AC_DEFINE(CALLOUT_REV_ASSOCIATION, "fault", [The name of the callout's reverse association.])

    AC_ARG_VAR(YAML_DIR_TEST, [The path to the test error yaml files.])
    AS_IF([test "x$YAML_DIR_TEST" == "x"], \
        [YAML_DIR_TEST="${srcdir}/tools/"])
    AC_ARG_VAR(YAML_DIR, [The path to the yaml error files.])
    AS_IF([test "x$YAML_DIR" == "x"], \
        [YAML_DIR="None"])
    AC_ARG_VAR(CALLOUTS_YAML, [YAML filepath containing generated callouts.])
    AS_IF([test "x$CALLOUTS_YAML" == "x"], \
        [CALLOUTS_YAML="callouts-example.yaml"])

    AC_ARG_VAR(ERRLOG_PERSIST_PATH, [Path of directory housing persisted errors.])
    AS_IF([test "x$ERRLOG_PERSIST_PATH" == "x"], \
        [ERRLOG_PERSIST_PATH="/var/lib/phosphor-logging/errors"])
    AC_DEFINE_UNQUOTED([ERRLOG_PERSIST_PATH], ["$ERRLOG_PERSIST_PATH"], \
        [Path of directory housing persisted errors])

    # Compile error metadata handlers if we're asked to do so.
    AC_ARG_ENABLE([metadata-processing],
        AS_HELP_STRING([--enable-metadata-processing], [Compile metadata handlers]),
        [AX_APPEND_COMPILE_FLAGS([-DPROCESS_META], [CXXFLAGS])]
    )

    AC_ARG_VAR(ERROR_CAP, [Max number of error entries allowed for commit])
    AS_IF([test "x$ERROR_CAP" == "x"], [ERROR_CAP=100])
    AC_DEFINE_UNQUOTED([ERROR_CAP], [$ERROR_CAP], [Max number of error entries allowed for commit])
])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile test/Makefile tools/Makefile tools/setup.py])
AC_CONFIG_FILES([phosphor-logging.pc])
AC_OUTPUT
