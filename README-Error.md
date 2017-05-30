## Introduction
Document captures steps for adding application specific error yaml files

### Write app specific error yaml files
Write error yamls for your application and place them in your application
repository.
Refer to **openpower-debug-collector**
```
org/open_power/Host.errors.yaml
```
### Makefile rules to run sdbus++ on error yaml files
Modify the Makefile.am as shown bellow for generation of error.cpp and erorr.hpp
```
org/open_power/Host/error.hpp: ${srcdir}/org/open_power/Host.errors.yaml
    @mkdir -p `dirname $@`
    $(SDBUSPLUSPLUS) -r $(srcdir) error exception-header org.open_power.Host > $@
```
```
org/open_power/Host/error.cpp: ${srcdir}/org/open_power/Host.errors.yaml
    @mkdir -p `dirname $@`
    $(SDBUSPLUSPLUS) -r $(srcdir) error exception-cpp org.open_power.Host > $@
```
### Write native recipe to copy error yaml files
Application specific error yaml files need to be copied to a know location
so that phosphor-logging can generate error metadata by parsing the error
yaml files.
Refer to **openpower-debug-collector-error-native.bb** captured below
```
PROVIDES += "openpower-debug-collector-error-native"
SRC_URI += "git://github.com/openbmc/openpower-debug-collector"
SRCREV = "a28f2a5be69fb4849b0d35e838f706a8dd6cfcde"
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
***report<stdbusexception>(metadata)***
#### *example*
```
#include <xyz/openbmc_project/Software/Version/error.hpp>
#include <phosphor-logging/elog-errors.hpp>

using error =
 sdbusplus::xyz::openbmc_project::Software::Version::Error::ManifestFileFailure;
using metadata =
phosphor::logging::xyz::openbmc_project::Software::Version::ManifestFileFailure;
report<error>(metadata::PATH(tarFilePath.c_str())););
```
