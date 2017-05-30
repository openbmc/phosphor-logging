## Introduction
Document describes steps to be followed for application specific non dbus
errors.
1. Use  stdbus++ to generate exception header and implmentation file.
2. Use elog-errors.hpp from **phosphor-logging** for the metadata

### Error yaml files
1. Define the error yaml and metadata yaml file in the application repository

###Use stdbus++ to generate exceptions from the error yaml file
Modify the Makefile.am as shown bellow for generation of error.cpp and erorr.hpp
```
org/open_power/Host/error.hpp: ${top_srcdir}/org/open_power/Host.errors.yaml
    @mkdir -p `dirname $@`
    $(SDBUSPLUSPLUS) -r $(srcdir) error exception-header org.open_power.Host > $@
```
```
org/open_power/Host/error.cpp: ${top_srcdir}/org/open_power/Host.errors.yaml
    @mkdir -p `dirname $@`
    $(SDBUSPLUSPLUS) -r $(srcdir) error exception-cpp org.open_power.Host > $@
```
### Copy the error yaml file to known location for metadata generation
Refer to ** openpower-debug-collector-error-native.bb** captured below
```
SUMMARY = "Copy error yaml files to known path for elog parsing"

PR = "r1"

inherit native
inherit obmc-phosphor-license
inherit openpower-dbus-interfaces

PROVIDES += "openpower-debug-collector-error-native"
require openpower-debug-collector.inc

S = "${WORKDIR}/git"

do_install_append() {
    SRC=${S}/org/open_power
    DEST=${op_error_yaml_dir}/org/open_power
    install -d ${DEST}
    install ${SRC}/Host.errors.yaml ${DEST}
}
```
### Establish dependency with phosphor-logging
Establish debendency with phosphor-logging to ensure that error yaml files are
copied to a known location before phosphor logging parses the error yaml files
Refer to **phosphor-logging-error-logs-native.bbappend**
```
DEPENDS += "openpower-debug-collector-error-native"
```
### Reporting error
To report (commit) the error follow the below format
**report<stdbuserror>(metadata)**

#### For example
```
#include <xyz/openbmc_project/Software/Version/error.hpp>
#include <phosphor-logging/elog-errors.hpp>

using error =
 sdbusplus::xyz::openbmc_project::Software::Version::Error::ManifestFileFailure;
using metadata =
phosphor::logging::xyz::openbmc_project::Software::Version::ManifestFileFailure;
report<error>(metadata::PATH(tarFilePath.c_str())););
```