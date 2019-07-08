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

## Remote Logging via Rsyslog
The BMC has the ability to stream out local logs (that go to the systemd journal)
via rsyslog (https://www.rsyslog.com/).

The BMC will send everything. Any kind of filtering and appropriate storage
will have to be managed on the rsyslog server. Various examples are available
on the internet. Here are few pointers :
https://www.rsyslog.com/storing-and-forwarding-remote-messages/
https://www.rsyslog.com/doc/rsyslog%255Fconf%255Ffilter.html
https://www.thegeekdiary.com/understanding-rsyslog-filter-options/

#### Configuring rsyslog server for remote logging
The BMC is an rsyslog client. To stream out logs, it needs to talk to an rsyslog
server, to which there's connectivity over a network. REST API can be used to
set the remote server's IP address and port number.

The following presumes a user has logged on to the BMC (see
https://github.com/openbmc/docs/blob/master/rest-api.md).

Set the IP:
```
curl -b cjar -k -H "Content-Type: application/json" -X PUT \
    -d '{"data": <IP address>}' \
    https://<BMC IP address>/xyz/openbmc_project/logging/config/remote/attr/Address
```

Set the port:
```
curl -b cjar -k -H "Content-Type: application/json" -X PUT \
    -d '{"data": <port number>}' \
    https://<BMC IP address>/xyz/openbmc_project/logging/config/remote/attr/Port
```

#### Querying the current configuration
```
curl -b cjar -k \
    https://<BMC IP address>/xyz/openbmc_project/logging/config/remote
```

#### Setting the hostname
Rsyslog can store logs separately for each host. For this reason, it's useful to
provide a unique hostname to each managed BMC. Here's how that can be done via a
REST API :
```
curl -b cjar -k -H "Content-Type: application/json" -X PUT \
    -d '{"data": "myHostName"}' \
    https://<BMC IP address>//xyz/openbmc_project/network/config/attr/HostName
```

#### Disabling remote logging
Remote logging can be disabled by writing 0 to the port, or an empty string("")
to the IP.

#### Changing the rsyslog server
When switching to a new server from an existing one (i.e the address, or port,
or both change), it is recommended to disable the existing configuration first.


## Adding application specific error YAML
* This document captures steps for adding application specific error YAML files
  and generating local elog-errors.hpp header file for application use.
* Should cater for continuous integration (CI) build, bitbake image build, and
  local repository build.

#### Continuous Integration (CI) build
 * Make is called on the repository that is modified.
 * Dependent packages are pulled based on the dependency list specified in the
   configure.ac script.

#### Recipe build
 * Native recipes copy error YAML files to shared location.
 * phosphor-logging builds elog-errors.hpp by parsing the error YAML files from
   the shared location.

#### Local repository build
 * Copies local error YAML files to the shared location in SDK
 * Make generates elog-errors.hpp by parsing the error YAML files from the
   shared location.

#### Makefile changes
**Reference**
 * https://github.com/openbmc/openpower-debug-collector/blob/master/Makefile.am

###### Export error YAML to shared location
*Modify Makefile.am to export newly added error YAML to shared location*
```
yamldir = ${datadir}/phosphor-dbus-yaml/yaml
nobase_yaml_DATA = \
    org/open_power/Host.errors.yaml
```

###### Generate elog-errors.hpp using elog parser from SDK location
 * Add a conditional check "GEN_ERRORS"
 * Disable the check for recipe bitbake image build
 * Enable it for local repository build
 * If "GEN_ERRORS" is enabled, build generates elog-errors.hpp header file.
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
      @cp ${top_srcdir}/org/open_power/Host.errors.yaml \
        ${YAML_DIR}/org/open_power/Host.errors.yaml
      @mkdir -p `dirname $@`
      @chmod 777 $(ELOG_GEN_DIR)/elog-gen.py
      $(AM_V_at)$(PYTHON) $(ELOG_GEN_DIR)/elog-gen.py -y ${YAML_DIR} \
        -t ${ELOG_MAKO_DIR} -m ${ELOG_MAKO} -o $@
  endif
```

###### Update BUILT_SOURCES
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

###### Conditional check for native build
 * As the same Makefile is used both for recipe image build and native recipe
   build, add a conditional to ensure that only installation of error yaml files
   happens during native build. It is not required to build repository during
   native build.
```
   if !INSTALL_ERROR_YAML
   endif
```

#### Autotools changes
**Reference**
 * https://github.com/openbmc/openpower-debug-collector/blob/master/configure.ac

###### Add option(argument) to enable/disable installing error yaml file
 * Install error yaml option(argument) is enabled for native recipe build
   and disabled for bitbake build.

 * When install error yaml option is disabled do not check for target specific
   packages in autotools configure script.

###### Add option(argument) to install error yaml files
```
AC_ARG_ENABLE([install_error_yaml],
    AS_HELP_STRING([--enable-install_error_yaml],
    [Enable installing error yaml file]),[], [install_error_yaml=no])
AM_CONDITIONAL([INSTALL_ERROR_YAML],
    [test "x$enable_install_error_yaml" = "xyes"])
AS_IF([test "x$enable_install_error_yaml" != "xyes"], [
..
..
])
```

###### Add option(argument) to enable/disable generating elog-errors header file
```
AC_ARG_ENABLE([gen_errors],
    AS_HELP_STRING([--enable-gen_errors], [Enable elog-errors.hpp generation ]),
    [],[gen_errors=yes])
AM_CONDITIONAL([GEN_ERRORS], [test "x$enable_gen_errors" != "xno"])
```

#### Recipe changes
**Reference**
* https://github.com/openbmc/openbmc/blob/master/meta-openbmc-machines\
/meta-openpower/common/recipes-phosphor/debug/openpower-debug-collector.bb

###### Extend recipe for native and nativesdk
* Extend the recipe for native and native SDK builds
```
BBCLASSEXTEND += "native nativesdk"
```
###### Remove dependencies for native and native SDK build
* Native recipe caters only for copying error yaml files to shared location.
* For native and native SDK build remove dependency on packages that recipe
  build depends

###### Remove dependency on phosphor-logging for native build
```
DEPENDS_remove_class-native = "phosphor-logging"
```

###### Remove dependency on phosphor-logging for native SDK build
```
DEPENDS_remove_class-nativesdk = "phosphor-logging"
```

###### Add install_error_yaml argument during native build
* Add package config to enable/disable install_error_yaml feature.

###### Add package config to enable/disable install_error_yaml feature
```
PACKAGECONFIG ??= "install_error_yaml"
PACKAGECONFIG[install_error_yaml] = " \
        --enable-install_error_yaml, \
        --disable-install_error_yaml, ,\
        "
```
###### Enable install_error_yaml check for native build
```
PACKAGECONFIG_add_class-native = "install_error_yaml"
PACKAGECONFIG_add_class-nativesdk = "install_error_yaml"
```
###### Disable install_error_yaml during target build
```
PACKAGECONFIG_remove_class-target = "install_error_yaml"
```

###### Disable generating elog-errors.hpp for bitbake build
* Disable gen_errors argument for bitbake image build as the application uses
  the elog-errors.hpp generated by phosphor-logging
* Argument is enabled by default for local repository build in the configure
  script of the local repository.
```
 XTRA_OECONF += "--disable-gen_errors"
```

#### Local build
* During local build use --prefix=/usr for the configure script.

## Event Log Extensions

The extension concept is a way to allow code that creates other formats of
error logs besides phosphor-logging's event logs to still reside in the
phosphor-log-manager application.

The extension code lives in the `extensions/<extension>` subdirectories,
and is enabled with a `--enable-<extension>` configure flag.  The
extension code won't compile unless enabled with this flag.

Extensions can register themselves to have functions called at the following
points using the REGISTER_EXTENSION_FUNCTION macro.
* On startup
   * Function type void(internal::Manager&)
* After an event log is created
   * Function type void(args)
   * The args are:
     * const std::string& - The Message property
     * uin32_t - The event log ID
     * uint64_t - The event log timestamp
     * Level - The event level
     * const AdditionalDataArg& - the additional data
     * const AssociationEndpointsArg& - Association endpoints (callouts)
* Before an event log is deleted, to check if it is allowed.
   * Function type void(std::uint32_t, bool&) that takes the event ID
* After an event log is deleted
   * Function type void(std::uint32_t) that takes the event ID

Using these callback points, they can create their own event log for each
OpenBMC event log that is created, and delete these logs when the corresponding
OpenBMC event log is deleted.

In addition, an extension has the option of disabling phosphor-logging's
default error log capping policy so that it can use its own.  The macro
DISABLE_LOG_ENTRY_CAPS() is used for that.

### Motivation

The reason for adding support for extensions inside the phosphor-log-manager
daemon as opposed to just creating new daemons that listen for D-Bus signals is
to allow interactions that would be complicated or expensive if just done over
D-Bus, such as:
* Allowing for custom old log retention algorithms.
* Prohibiting manual deleting of certain logs based on an extension's
  requirements.

### Creating extensions

1. Add a new flag to configure.ac to enable the extension:
```
AC_ARG_ENABLE([foo-extension],
              AS_HELP_STRING([--enable-foo-extension],
                             [Create Foo logs]))
AM_CONDITIONAL([ENABLE_FOO_EXTENSION],
               [test "x$enable_foo_extension" == "xyes"])
```
2. Add the code in `extensions/<extension>/`.
3. Create a makefile include to add the new code to phosphor-log-manager:
```
phosphor_log_manager_SOURCES += \
        extensions/foo/foo.cpp
```
3. In `extensions/extensions.mk`, add the makefile include:
```
if ENABLE_FOO_EXTENSION
include extensions/foo/foo.mk
endif
```
4. In the extension code, register the functions to call and optionally disable
   log capping using the provided macros:
```
DISABLE_LOG_ENTRY_CAPS();

void fooStartup(internal::Manager& manager)
{
    // Initialize
}

REGISTER_EXTENSION_FUNCTION(fooStartup);

void fooCreate(const std::string& message, uint32_t id, uint64_t timestamp,
               Entry::Level severity, const AdditionalDataArg& additionalData,
               const AssociationEndpointsArg& assocs)
{
    // Create a different type of error log based on 'entry'.
}

REGISTER_EXTENSION_FUNCTION(fooCreate);

void fooRemove(uint32_t id)
{
    // Delete the extension error log that corresponds to 'id'.
}

REGISTER_EXTENSION_FUNCTION(fooRemove);
```

**Reference**
* https://github.com/openbmc/openpower-debug-collector/blob/master/README.md
