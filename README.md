# phosphor-logging

The phosphor logging repository provides mechanisms for event and journal
logging.

## Table Of Contents

- [Building](#to-build)
- [Structured Logging](#structured-logging)
- [Event Logs](#event-logs)
- [Event Log Extensions](#event-log-extensions)
- [Remote Logging](#remote-logging-via-rsyslog)
- [Boot Fail on Hardware Errors](#boot-fail-on-hardware-errors)

## To Build

To build this package, do the following steps:

1. meson builddir
2. ninja -c builddir

## Structured Logging

phosphor-logging provides APIs to add program logging information to the
systemd-journal and it is preferred that this logging data is formatted in a
structured manner (using the facilities provided by the APIs).

See [Structured Logging](./docs/structured-logging.md) for more details on this
API.

## Event Logs

OpenBMC event logs are a collection of D-Bus interfaces owned by
phosphor-log-manager that reside at `/xyz/openbmc_project/logging/entry/X`,
where X starts at 1 and is incremented for each new log.

The interfaces are:

- [xyz.openbmc_project.Logging.Entry]
  - The main event log interface.
- [xyz.openbmc_project.Association.Definitions]
  - Used for specifying inventory items as the cause of the event.
  - For more information on associations, see the [associations
    doc][associations-doc].
- [xyz.openbmc_project.Object.Delete]
  - Provides a Delete method to delete the event.
- [xyz.openbmc_project.Software.Version]
  - Stores the code version that the error occurred on.

On platforms that make use of these event logs, the intent is that they are the
common event log representation that other types of event logs can be created
from. For example, there is code to convert these into both Redfish and IPMI
event logs, in addition to the event log extensions mentioned
[below](#event-log-extensions).

The logging daemon has the ability to add `callout` associations to an event log
based on text in the AdditionalData property. A callout is a link to the
inventory item(s) that were the cause of the event log. See [callout
doc][callout-doc] for details.

### Creating Event Logs In Code

The preferred method for creating event logs is specified in the project-level
[event log design][event-log-design]. Events are defined using YAML in the
phosphor-dbus-interfaces repository, such as the
[Logging.Cleared][logging-cleared] event, which will generate a C++ class for
the event. Then a call to `lg2::commit` is made on a constructed event to add it
to the event log.

```cpp
lg2::commit(sdbusplus::event::xyz::openbmc_project::Logging::Cleared(
    "NUMBER_OF_LOGS", count));
```

The above function will return the object path of the created log entry. This
log-entry can be resolved with the helper `lg2::resolve` function.

```cpp
lg2::resolve(logPath);
```

### Event Log Filtering

Vendors customizing phosphor-logging for their platforms may decide that they
would like to prevent certain events from being added to the event log. This is
especially true for informational / tracing events. The `lg2::commit` supports a
compile-time event filtering mechanism that can accomplish this.

The meson option `event-filter` can be used to specify a file containing
filtering policy. When left unspecified, the [default
policy][default-policy-json] of "allow all" is enabled. For both events and
errors, a default policy of "allowed" or "blocked" can be specified and an
additional set of events can be given for which the non-defaulted action should
be taken. A JSON-Schema is available for the [policy
JSON][filter-policy-schema].

[default-policy-json]:
  https://github.com/openbmc/phosphor-logging/blob/master/tools/phosphor-logging/default-eventfilter.json
[filter-policy-schema]:
  https://github.com/openbmc/phosphor-logging/blob/master/tools/phosphor-logging/schemas/eventfilter.schema.yaml

### Deprecated Methods for Creating Event Logs

There are two other, but now deprecated, methods to creating event logs in
OpenBMC code. The first makes use of the systemd journal to store metadata
needed for the log, and the second is a plain D-Bus method call.

[event-log-design]:
  https://github.com/openbmc/docs/blob/master/designs/event-logging.md#phosphor-logging
[logging-cleared]:
  https://github.com/openbmc/phosphor-dbus-interfaces/blob/6a8507d06e172d8d29c0459f0a0d078553d2ecc7/yaml/xyz/openbmc_project/Logging.events.yaml#L4

#### Journal Based Event Log Creation [deprecated]

Event logs can be created by using phosphor-logging APIs to commit sdbusplus
exceptions. These APIs write to the journal, and then call a `Commit` D-Bus
method on the logging daemon to create the event log using the information it
put in the journal.

The APIs are found in `<phosphor-logging/elog.hpp>`:

- `elog()`: Throw an sdbusplus error.
- `commit()`: Catch an error thrown by elog(), and commit it to create the event
  log.
- `report()`: Create an event log from an sdbusplus error without throwing the
  exception first.

Any errors passed into these APIs must be known to phosphor-logging, usually by
being defined in `<phosphor-logging/elog-errors.hpp>`. The errors must also be
known by sdbusplus, and be defined in their corresponding error.hpp. See below
for details on how get errors into these headers.

Example:

```cpp
#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/elog.hpp>
#include <xyz/openbmc_project/Common/error.hpp>
...
using InternalFailure =
    sdbusplus::xyz::openbmc_project::Common::Error::InternalFailure;
...
if (somethingBadHappened)
{
    phosphor::logging::report<InternalFailure>();
}

```

Alternatively, to throw, catch, and then commit the error:

```cpp
try
{
    phosphor::logging::elog<InternalFailure>();
}
catch (InternalFailure& e)
{
    phosphor::logging::commit<InternalFailure>();
}
```

Metadata can be added to event logs to add debug data captured at the time of
the event. It shows up in the AdditionalData property in the
`xyz.openbmc_project.Logging.Entry` interface. Metadata is passed in via the
`elog()` or `report()` functions, which write it to the journal. The metadata
must be predefined for the error in the metadata YAML so that the daemon knows
to look for it in the journal when it creates the event log.

Example:

```cpp
#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/elog.hpp>
#include <xyz/openbmc_project/Control/Device/error.hpp>
...
using WriteFailure =
    sdbusplus::xyz::openbmc_project::Control::Device::Error::WriteFailure;
using metadata =
    phosphor::logging::xyz::openbmc_project::Control::Device::WriteFailure;
...
if (somethingBadHappened)
{
    phosphor::logging::report<WriteFailure>(metadata::CALLOUT_ERRNO(5),
                              metadata::CALLOUT_DEVICE_PATH("some path"));
}
```

In the above example, the AdditionalData property would look like:

```cpp
["CALLOUT_ERRNO=5", "CALLOUT_DEVICE_PATH=some path"]
```

Note that the metadata fields must be all uppercase.

##### Event Log Definition [deprecated]

As mentioned above, both sdbusplus and phosphor-logging must know about the
event logs in their header files, or the code that uses them will not even
compile. The standard way to do this to define the event in the appropriate
`<error-category>.errors.yaml` file, and define any metadata in the
`<error-category>.metadata.yaml` file in the appropriate `*-dbus-interfaces`
repository. During the build, phosphor-logging generates the elog-errors.hpp
file for use by the calling code.

In much the same way, sdbusplus uses the event log definitions to generate an
error.hpp file that contains the specific exception. The path of the error.hpp
matches the path of the YAML file.

For example, if in phosphor-dbus-interfaces there is
`xyz/openbmc_project/Control/Device.errors.yaml`, the errors that come from that
file will be in the include: `xyz/openbmc_project/Control/Device/error.hpp`.

In rare cases, one may want one to define their errors in the same repository
that uses them. To do that, one must:

1. Add the error and metadata YAML files to the repository.
2. Run the sdbus++ script within the makefile to create the error.hpp and .cpp
   files from the local YAML, and include the error.cpp file in the application
   that uses it. See [openpower-occ-control] for an example.
3. Tell phosphor-logging about the error:
   1. Run phosphor-logging's `elog-gen.py` script on the local yaml to generate
      an elog-errors.hpp file that just contains the local errors, and check
      that into the repository and include it where the errors are needed.
   2. Create a recipe that copies the local YAML files to a place that
      phosphor-logging can find it during the build. See sample [LED
      YAML][led-link] for an example.

#### D-Bus Event Log Creation [deprecated]

There is also a [D-Bus method][log-create-link] to create event logs:

- Service: xyz.openbmc_project.Logging
- Object Path: /xyz/openbmc_project/logging
- Interface: xyz.openbmc_project.Logging.Create
- Method: Create
  - Method Arguments:
    - Message: The `Message` string property for the
      `xyz.openbmc_project.Logging.Entry` interface.
    - Severity: The `severity` property for the
      `xyz.openbmc_project.Logging.Entry` interface. An
      `xyz.openbmc_project.Logging.Entry.Level` enum value.
    - AdditionalData: The `AdditionalData` property for the
      `xyz.openbmc_project.Logging.Entry` interface, but in a map instead of in
      a vector of "KEY=VALUE" strings. Example:

```cpp
    std::map<std::string, std::string> additionalData;
    additionalData["KEY"] = "VALUE";
```

Unlike the previous APIs where errors could also act as exceptions that could be
thrown across D-Bus, this API does not require that the error be defined in the
error YAML in the D-Bus interfaces repository so that sdbusplus knows about it.
Additionally, as this method passes in everything needed to create the event
log, the logging daemon doesn't have to know about it ahead of time either.

That being said, it is recommended that users of this API still follow some
guidelines for the message field, which is normally generated from a combination
of the path to the error YAML file and the error name itself. For example, the
`Timeout` error in `xyz/openbmc_project/Common.errors.yaml` will have a Message
property of `xyz.openbmc_project.Common.Error.Timeout`.

The guidelines are:

1. When it makes sense, one can still use an existing error that has already
   been defined in an error YAML file, and use the same severity and metadata
   (AdditionalData) as in the corresponding metadata YAML file.

2. If creating a new error, use the same naming scheme as other errors, which
   starts with the domain, `xyz.openbmc_project`, `org.open_power`, etc,
   followed by the capitalized category values, followed by `Error`, followed by
   the capitalized error name itself, with everything separated by "."s. For
   example: `xyz.openbmc_project.Some.Category.Error.Name`.

3. If creating a new common error, still add it to the appropriate error and
   metadata YAML files in the appropriate D-Bus interfaces repository so that
   others can know about it and use it in the future. This can be done after the
   fact.

[xyz.openbmc_project.logging.entry]:
  https://github.com/openbmc/phosphor-dbus-interfaces/blob/master/yaml/xyz/openbmc_project/Logging/Entry.interface.yaml
[xyz.openbmc_project.association.definitions]:
  https://github.com/openbmc/phosphor-dbus-interfaces/blob/master/yaml/xyz/openbmc_project/Association/Definitions.interface.yaml
[associations-doc]:
  https://github.com/openbmc/docs/blob/master/architecture/object-mapper.md#associations
[callout-doc]:
  https://github.com/openbmc/phosphor-dbus-interfaces/blob/master/yaml/xyz/openbmc_project/Common/Callout/README.md
[xyz.openbmc_project.object.delete]:
  https://github.com/openbmc/phosphor-dbus-interfaces/blob/master/yaml/xyz/openbmc_project/Object/Delete.interface.yaml
[xyz.openbmc_project.software.version]:
  https://github.com/openbmc/phosphor-dbus-interfaces/blob/master/yaml/xyz/openbmc_project/Software/Version.errors.yaml
[openpower-occ-control]: https://github.com/openbmc/openpower-occ-control
[led-link]:
  https://github.com/openbmc/openbmc/tree/master/meta-phosphor/recipes-phosphor/leds
[log-create-link]:
  https://github.com/openbmc/phosphor-dbus-interfaces/blob/master/yaml/xyz/openbmc_project/Logging/Create.interface.yaml

## Event Log Extensions

The extension concept is a way to allow code that creates other formats of error
logs besides phosphor-logging's event logs to still reside in the
phosphor-log-manager application.

The extension code lives in the `extensions/<extension>` subdirectories, and is
enabled with a `--enable-<extension>` configure flag. The extension code won't
compile unless enabled with this flag.

Extensions can register themselves to have functions called at the following
points using the REGISTER_EXTENSION_FUNCTION macro.

- On startup
  - Function type void(internal::Manager&)
- After an event log is created
  - Function type void(args)
  - The args are:
    - const std::string& - The Message property
    - uin32_t - The event log ID
    - uint64_t - The event log timestamp
    - Level - The event level
    - const AdditionalDataArg& - the additional data
    - const AssociationEndpointsArg& - Association endpoints (callouts)
- Before an event log is deleted, to check if it is allowed.
  - Function type void(std::uint32_t, bool&) that takes the event ID
- After an event log is deleted
  - Function type void(std::uint32_t) that takes the event ID

Using these callback points, they can create their own event log for each
OpenBMC event log that is created, and delete these logs when the corresponding
OpenBMC event log is deleted.

In addition, an extension has the option of disabling phosphor-logging's default
error log capping policy so that it can use its own. The macro
DISABLE_LOG_ENTRY_CAPS() is used for that.

### Motivation

The reason for adding support for extensions inside the phosphor-log-manager
daemon as opposed to just creating new daemons that listen for D-Bus signals is
to allow interactions that would be complicated or expensive if just done over
D-Bus, such as:

- Allowing for custom old log retention algorithms.
- Prohibiting manual deleting of certain logs based on an extension's
  requirements.

### Creating extensions

1. Add a new flag to configure.ac to enable the extension:

   ```autoconf
   AC_ARG_ENABLE([foo-extension],
                 AS_HELP_STRING([--enable-foo-extension],
                                [Create Foo logs]))
   AM_CONDITIONAL([ENABLE_FOO_EXTENSION],
                  [test "x$enable_foo_extension" == "xyes"])
   ```

2. Add the code in `extensions/<extension>/`.
3. Create a makefile include to add the new code to phosphor-log-manager:

   ```make
   phosphor_log_manager_SOURCES += \
           extensions/foo/foo.cpp
   ```

4. In `extensions/extensions.mk`, add the makefile include:

   ```make
   if ENABLE_FOO_EXTENSION
   include extensions/foo/foo.mk
   endif
   ```

5. In the extension code, register the functions to call and optionally disable
   log capping using the provided macros:

   ```cpp
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

### Extension List

The supported extensions are:

- OpenPower PELs
  - Enabled with --enable-openpower-pel-extension
  - Detailed information can be found in
    [extensions/openpower-pels](extensions/openpower-pels/README.md).

## Remote Logging via Rsyslog

The BMC has the ability to stream out local logs (that go to the systemd
journal) via rsyslog (<https://www.rsyslog.com/>).

The BMC will send everything. Any kind of filtering and appropriate storage will
have to be managed on the rsyslog server. Various examples are available on the
internet. Here are few pointers :
<https://www.rsyslog.com/storing-and-forwarding-remote-messages/>
<https://www.rsyslog.com/doc/rsyslog%255Fconf%255Ffilter.html>
<https://www.thegeekdiary.com/understanding-rsyslog-filter-options/>

### Configuring rsyslog server for remote logging

The BMC is an rsyslog client. To stream out logs, it needs to talk to an rsyslog
server, to which there's connectivity over a network. REST API can be used to
set the remote server's IP address and port number.

The following presumes a user has logged on to the BMC (see
<https://github.com/openbmc/docs/blob/master/rest-api.md>).

Set the IP:

```sh
curl -b cjar -k -H "Content-Type: application/json" -X PUT \
    -d '{"data": <IP address>}' \
    https://<BMC IP address>/xyz/openbmc_project/logging/config/remote/attr/Address
```

Set the port:

```sh
curl -b cjar -k -H "Content-Type: application/json" -X PUT \
    -d '{"data": <port number>}' \
    https://<BMC IP address>/xyz/openbmc_project/logging/config/remote/attr/Port
```

#### Querying the current configuration

```sh
curl -b cjar -k \
    https://<BMC IP address>/xyz/openbmc_project/logging/config/remote
```

#### Setting the hostname

Rsyslog can store logs separately for each host. For this reason, it's useful to
provide a unique hostname to each managed BMC. Here's how that can be done via a
REST API :

```sh
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

## Boot Fail on Hardware Errors

phosphor-logging supports a setting, which when set, will result in the software
looking at new phosphor-logging entries being created, and if a CALLOUT\* is
found within the entry, ensuring the system will not power on. Entries with
severities of Informational or Debug will not block boots, even if they have
callouts.

The full design for this can be found in
[docs](https://github.com/openbmc/docs/blob/master/designs/fail-boot-on-hw-error.md).

To enable this function:

```sh
busctl set-property xyz.openbmc_project.Settings /xyz/openbmc_project/logging/settings xyz.openbmc_project.Logging.Settings QuiesceOnHwError b true
```

To check if an entry is blocking the boot:

```sh
obmcutil listbootblock
```

Resolve or clear the corresponding entry to allow the system to boot.
