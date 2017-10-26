# phosphor-logging
phosphor logging provides mechanism for common event and logging creation based
on information from the journal log.

## To Build
```
To build this package, do the following steps:

    1. ./bootstrap.sh
    2. ./configure ${CONFIGURE_FLAGS}
    3. make

To clean the repository run `./bootstrap.sh clean`.
```
## REST command to delete an error

    curl -c cjar -k -X POST -H "Content-Type: application/json" \
    -d '{"data": [ "root", "<root password>" ] }' https://<BMC IP>/login

    curl -c cjar -b cjar -k -H "Content-Type: application/json" -X POST \
    -d '{"data": []}' \
    https://<BMC IP>/xyz/openbmc_project/logging/entry/<entry num>/action/Delete

## REST command to delete all errors

    curl -c cjar -k -X POST -H "Content-Type: application/json" \
    -d '{"data": [ "root", "<root password>" ] }' https://<<BMC IP>/login

    curl -c cjar -b cjar -k -H "Content-Type: application/json" \
    -X POST https://<<BMC IP>/xyz/openbmc_project/logging/action/deleteAll \
    -d "{\"data\": [] }"

## Adding application specific error yaml
This document captures steps for adding application specific error yaml files
and generating local elog-errors.hpp for application use.
The framework should cater for continious integration (CI) build, openbmc
bitbake build and local repository build.

### Continious Integration (CI) build
 * In continious integration build "make" is called on the repository that is
   modified rather than bitbake.
 * CI build process does not use SDK rather pulls dependent packages based on
   the dependency list set in the configure.ac script.

### Recipe build
 * Native recipe copies error yaml files to shared location.
 * phosphor-logging builds elog-errors.hpp by parsing the error yaml files from
   the shared location.

### Local repository build
 * Copies local error yaml files to the share location in SDK
 * Make generates elog-errors.hpp by parsing the error yaml files from the
   shared location.

## Implementation details
### Makefile changes
**Reference**
 * https://github.com/openbmc/openpower-debug-collector/blob/master/Makefile.am
#### Export error YAML to shared location
*Modify Makefile.am to export newly added error yaml to shared location*
```
yamldir = ${datadir}/phosphor-dbus-yaml/yaml
nobase_yaml_DATA = \
    org/open_power/Host.errors.yaml
```
#### Generate elog-errors.hpp using elog parser from SDK location
 * Add a conditional check "GEN_ERRORS" which is disabled for recipe build and
   enabled for local build.
 * If "GEN_ERRORS" is enabled generate elog-errors.hpp header file.
```
# Generate phosphor-logging/elog-errors.hpp
if GEN_ERRORS
ELOG_MAKO ?= elog-gen-template.mako.hpp
ELOG_DIR ?= ${OECORE_NATIVE_SYSROOT}${datadir}/phosphor-logging/elog
ELOG_GEN_DIR ?= ${ELOG_DIR}/tools/
ELOG_MAKO_DIR ?= ${ELOG_DIR}/tools/phosphor-logging/templates/
YAML_DIR ?= ${OECORE_NATIVE_SYSROOT}${datadir}/phosphor-dbus-yaml/yaml
phosphor-logging/elog-errors.hpp:
    @mkdir -p ${YAML_DIR}/org/open_power/
    @cp ${top_srcdir}/org/open_power/Host.errors.yaml ${YAML_DIR}/org/open_power/Host.errors.yaml
    @mkdir -p `dirname $@`
    @chmod 777 $(ELOG_GEN_DIR)/elog-gen.py
    $(AM_V_at)$(PYTHON) $(ELOG_GEN_DIR)/elog-gen.py -y ${YAML_DIR} -t ${ELOG_MAKO_DIR} -m ${ELOG_MAKO} -o $@
endif
```
#### Update BUILT_SOURCES
 * Append elog-errors.hpp to BUILT_SOURCES list and put it in conditional check
   GEN_ERRORS so that the elog-errors.hpp is generated only during local
   repository build.
```
if GEN_ERRORS
nobase_nodist_include_HEADERS += \
            phosphor-logging/elog-errors.hpp
endif
if GEN_ERRORS
BUILT_SOURCES += phosphor-logging/elog-errors.hpp
endif
```
#### Conditional check for native build
 * As the same Makefile is used both for Recipe and Native recipe build add a
   check to ensure that only installing of error yaml files happens during
   native build. It is not required to build repository during native build.
```
if !INSTALL_ERROR_YAML
endif
```
### Configure changes
**Reference**
 * https://github.com/openbmc/openpower-debug-collector/blob/master/configure.ac

#### Add argument to enable/disable installing error yaml file
 * Install error yaml argument is enabled in the recipe for native recipe build
   and disabled bur bitbake build.
 * Do not check for compiler, packages that are present only in the target build
   when the argument is disabled.

```
# Check for compiler, packages only when install_error_yaml optional feature
# is disabled
AC_ARG_ENABLE([install_error_yaml],
    AS_HELP_STRING([--enable-install_error_yaml], [Enable installing error yaml file]),
    [], [install_error_yaml=no])
AM_CONDITIONAL([INSTALL_ERROR_YAML], [test "x$enable_install_error_yaml" = "xyes"])
AS_IF([test "x$enable_install_error_yaml" != "xyes"], [
..
..
])
```
#### Add argument to enable/disable generating elog-errors.hpp header file
```
# Generate elog-errors.hpp if gen_errors optional feature is enabled
AC_ARG_ENABLE([gen_errors],
    AS_HELP_STRING([--enable-gen_errors], [Enable elog-errors.hpp generation ]),
    [],[gen_errors=yes])
AM_CONDITIONAL([GEN_ERRORS], [test "x$enable_gen_errors" != "xno"])
```

### Recipe changes
**Reference**
* https://github.com/openbmc/openbmc/blob/master/meta-openbmc-machines/meta-openpower/common/recipes-phosphor/debug/openpower-debug-collector.bb

### Extend recipe for native and nativesdk
* Extend the recipe for native and native sdk builds
```
BBCLASSEXTEND += "native nativesdk"
```
### Remove dependencies for native and native sdk build
* For native and native sdk build remove dependecy on packages that recipe build
  depends on as shown below.
* There is no need of phosphor-logging dependency for a native and native sdk
  build which caters for installing error yaml files
```
# Do not depend on phosphor-logging for native build
DEPENDS_remove_class-native = "phosphor-logging"

# Do not depend on phosphor-logging for native SDK build
DEPENDS_remove_class-nativesdk = "phosphor-logging"
```
### Add install_error_yaml argument during naive build
* Enable install_error_yaml argument variable for native and native sdk build
* Disable install_error_yaml argument variable for target build
* These arguments are passed to the configure script of the repository based on
  the type of the build.
* Example: --enable-install_error-yaml is passed to native and native sdk
  builds configure script.

```
# Provide a means to enable/disable install_error_yaml feature
PACKAGECONFIG ??= "install_error_yaml"
PACKAGECONFIG[install_error_yaml] = " \
        --enable-install_error_yaml, \
        --disable-install_error_yaml, ,\
        "

# Enable install_error_yaml during native and native SDK build
PACKAGECONFIG_add_class-native = "install_error_yaml"
PACKAGECONFIG_add_class-nativesdk = "install_error_yaml"

# Disable install_error_yaml during target build
PACKAGECONFIG_remove_class-target = "install_error_yaml"
```

### Disable generatring elog-errors.hpp for bitbake build
* Disable gen_errors argument for bitbake build as the application uses the
  elog-errors.hpp generated by phosphor-logging
* Argument is enabled for local repository build
```
 Disable generating elog error header file during bitbake. Applications
# should be using the elog header generated by phosphor-logging recipe
EXTRA_OECONF += "--disable-gen_errors"
```

## Local build
* During local build use --prefix=/usr for the configure script.
**Reference**
* https://github.com/openbmc/openpower-debug-collector/blob/master/README.md
