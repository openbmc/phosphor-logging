# OpenPower Platform Event Log (PEL) extension

This extension will create PELs for every OpenBMC event log. It is also possible
to point to the raw PEL to use in the OpenBMC event, and then that will be used
instead of creating one.

## Contents

- [Passing in data when creating PELs](#passing-pel-related-data-within-an-openbmc-event-log)
- [Default UserData sections for BMC created PELs](#default-userdata-sections-for-bmc-created-pels)
- [The PEL Message Registry](#the-pel-message-registry)
- [Callouts](#callouts)
- [Action Flags and Event Type Rules](#action-flags-and-event-type-rules)
- [D-Bus Interfaces](#d-bus-interfaces)
- [PEL Retention](#pel-retention)
- [Adding python3 modules for PEL UserData and SRC parsing](#adding-python3-modules-for-pel-userdata-and-src-parsing)
- [Fail Boot on Host Errors](#fail-boot-on-host-errors)
- [SBE FFDC](#self-boot-engine-first-failure-data-capture-support)
- [PEL Archiving](#pel-archiving)
- [Handling PELs for hot plugged FRUs](#handling-pels-for-hot-plugged-frus)

## Passing PEL related data within an OpenBMC event log

An error log creator can pass in data that is relevant to a PEL by using certain
keywords in the AdditionalData property of the event log.

### AdditionalData keywords

#### RAWPEL

This keyword is used to point to an existing PEL in a binary file that should be
associated with this event log. The syntax is:

```ascii
RAWPEL=<path to PEL File>
e.g.
RAWPEL="/tmp/pels/pel.5"
```

The code will assign its own error log ID to this PEL, and also update the
commit timestamp field to the current time.

#### POWER_THERMAL_CRITICAL_FAULT

This keyword is used to set the power fault bit in PEL. The syntax is:

```ascii
POWER_THERMAL_CRITICAL_FAULT=<FLAG>
e.g.
POWER_THERMAL_CRITICAL_FAULT=TRUE
```

Note that TRUE is the only value supported.

#### SEVERITY_DETAIL

This is used when the passed in event log severity determines the PEL severity
and a more granular PEL severity is needed beyond what the normal event log to
PEL severity conversion could give.

The syntax is:

```ascii
SEVERITY_DETAIL=<SEVERITY_TYPE>
e.g.
SEVERITY_DETAIL=SYSTEM_TERM
```

Option Supported:

- SYSTEM_TERM, changes the Severity value from 0x50 to 0x51

#### ESEL

This keyword's data contains a full PEL in string format. This is how hostboot
sends down PELs when it is configured in IPMI communication mode. The PEL is
handled just like the PEL obtained using the RAWPEL keyword.

The syntax is:

```ascii
ESEL=
"00 00 df 00 00 00 00 20 00 04 12 01 6f aa 00 00 50 48 00 30 01 00 33 00 00..."
```

Note that there are 16 bytes of IPMI SEL data before the PEL data starts.

#### \_PID

This keyword that contains the application's PID is added automatically by the
phosphor-logging daemon when the `commit` or `report` APIs are used to create an
event log, but not when the `Create` D-Bus method is used. If a caller of the
`Create` API wishes to have their PID captured in the PEL this should be used.

This will be added to the PEL in a section of type User Data (UD), along with
the application name it corresponds to.

The syntax is:

```ascii
_PID=<PID of application>
e.g.
_PID="12345"
```

#### CALLOUT_INVENTORY_PATH

This is used to pass in an inventory item to use as a callout. See
[here for details](#passing-callouts-in-with-the-additionaldata-property)

#### CALLOUT_PRIORITY

This can be used along with CALLOUT_INVENTORY_PATH to specify the priority of
that FRU callout. If not specified, the default priority is "H"/High Priority.

The possible values are:

- "H": High Priority
- "M": Medium Priority
- "L": Low Priority

See [here for details](#passing-callouts-in-with-the-additionaldata-property)

#### CALLOUT_DEVICE_PATH with CALLOUT_ERRNO

This is used to pass in a device path to create callouts from. See
[here for details](#passing-callouts-in-with-the-additionaldata-property)

#### CALLOUT_IIC_BUS with CALLOUT_IIC_ADDR and CALLOUT_ERRNO

This is used to pass in an I2C bus and address to create callouts from. See
[here for details](#passing-callouts-in-with-the-additionaldata-property)

#### PEL_SUBSYSTEM

This keyword is used to pass in the subsystem that should be associated with
this event log. The syntax is: `PEL_SUBSYSTEM=<subsystem value in hex>` e.g.
PEL_SUBSYSTEM=0x20

### FFDC Intended For UserData PEL sections

When one needs to add FFDC into the PEL UserData sections, the
`CreateWithFFDCFiles` D-Bus method on the `xyz.openbmc_project.Logging.Create`
interface must be used when creating a new event log. This method takes a list
of files to store in the PEL UserData sections.

That API is the same as the 'Create' one, except it has a new parameter:

```cpp
std::vector<std::tuple<enum[FFDCFormat],
                       uint8_t,
                       uint8_t,
                       sdbusplus::message::unix_fd>>
```

Each entry in the vector contains a file descriptor for a file that will be
stored in a unique UserData section. The tuple's arguments are:

- enum[FFDCFormat]: The data format type, the options are:
  - 'JSON'
    - The parser will use nlohmann::json\'s pretty print
  - 'CBOR'
    - The parser will use nlohmann::json\'s pretty print
  - 'Text'
    - The parser will output ASCII text
  - 'Custom'
    - The parser will hexdump the data, unless there is a parser registered for
      this component ID and subtype.
- uint8_t: subType
  - Useful for the 'custom' type. Not used with the other types.
- uint8_t: version
  - The version of the data.
  - Used for the custom type.
  - Not planning on using for JSON/BSON unless a reason to do so appears.
- unixfd - The file descriptor for the opened file that contains the contents.
  The file descriptor can be closed and the file can be deleted if desired after
  the method call.

An example of saving JSON data to a file and getting its file descriptor is:

```cpp
nlohmann::json json = ...;
auto jsonString = json.dump();
FILE* fp = fopen(filename, "w");
fwrite(jsonString.data(), 1, jsonString.size(), fp);
int fd = fileno(fp);
```

Alternatively, 'open()' can be used to obtain the file descriptor of the file.

Upon receiving this data, the PEL code will create UserData sections for each
entry in that vector with the following UserData fields:

- Section header component ID:
  - If the type field from the tuple is "custom", use the component ID from the
    message registry.
  - Otherwise, set the component ID to the phosphor-logging component ID so that
    the parser knows to use the built in parsers (e.g. json) for the type.
- Section header subtype: The subtype field from the tuple.
- Section header version: The version field from the tuple.
- Section data: The data from the file.

If there is a peltool parser registered for the custom type (method is TBD),
that will be used by peltool to print the data, otherwise it will be hexdumped.

Before adding each of these UserData sections, a check will be done to see if
the PEL size will remain under the maximum size of 16KB. If not, the UserData
section will be truncated down enough so that it will fit into the 16KB.

## Default UserData sections for BMC created PELs

The extension code that creates PELs will add these UserData sections to every
PEL:

- The AdditionalData property contents
  - If the AdditionalData property in the OpenBMC event log has anything in it,
    it will be saved in a UserData section as a JSON string.

- System information
  - This section contains various pieces of system information, such as the full
    code level and the BMC, chassis, and host state properties.

## The PEL Message Registry

The PEL message registry is used to create PELs from OpenBMC event logs.
[Documentation](registry/README.md) is available.

## Callouts

A callout points to a FRU, a symbolic FRU, or an isolation procedure. There can
be from zero to ten of them in each PEL, where they are located in the SRC
section.

There are a few different ways to add callouts to a PEL. In all cases, the
callouts will be sorted from highest to lowest priority within the PEL after
they are added.

### Passing callouts in with the AdditionalData property

The PEL code can add callouts based on the values of special entries in the
AdditionalData event log property. They are:

- CALLOUT_INVENTORY_PATH

  This keyword is used to call out a single FRU by passing in its D-Bus
  inventory path. When the PEL code sees this, it will create a single FRU
  callout, using the VPD properties (location code, FN, CCIN) from that
  inventory item. If that item is not a FRU itself and does not have a location
  code, it will keep searching its parents until it finds one that is.

  The priority of the FRU callout will be high, unless the CALLOUT_PRIORITY
  keyword is also present and contains a different priority in which case it
  will be used instead. This can be useful when a maintenance procedure with a
  high priority callout is specified for this error in the message registry and
  the FRU callout needs to have a different priority.

```ascii
  CALLOUT_INVENTORY_PATH=
  "/xyz/openbmc_project/inventory/system/chassis/motherboard"
```

- CALLOUT_DEVICE_PATH with CALLOUT_ERRNO

  These keywords are required as a pair to indicate faulty device communication,
  usually detected by a failure accessing a device at that sysfs path. The PEL
  code will use a data table generated by the MRW to map these device paths to
  FRU callout lists. The errno value may influence the callout.

  I2C, FSI, FSI-I2C, and FSI-SPI paths are supported.

```ascii
  CALLOUT_DEVICE_PATH="/sys/bus/i2c/devices/3-0069"
  CALLOUT_ERRNO="2"
```

- CALLOUT_IIC_BUS with CALLOUT_IIC_ADDR and CALLOUT_ERRNO

  These 3 keywords can be used to callout a failing I2C device path when the
  full device path isn't known. It is similar to CALLOUT_DEVICE_PATH in that it
  will use data tables generated by the MRW to create the callouts.

  CALLOUT_IIC_BUS is in the form "/dev/i2c-X" where X is the bus number, or just
  the bus number by itself. CALLOUT_IIC_ADDR is the 7 bit address either as a
  decimal or a hex number if preceded with a "0x".

```ascii
  CALLOUT_IIC_BUS="/dev/i2c-7"
  CALLOUT_IIC_ADDR="81"
  CALLOUT_ERRNO=62
```

### Defining callouts in the message registry

Callouts can be completely defined inside that error's definition in the PEL
message registry. This method allows the callouts to vary based on the system
type or on any AdditionalData item.

At a high level, this involves defining a callout section inside the registry
entry that contain the location codes or procedure names to use, along with
their priority. If these can vary based on system type, the type provided by the
entity manager will be one of the keys. If they can also depend on an
AdditionalData entry, then that will also be a key.

See the message registry [README](registry/README.md) and
[schema](registry/schema/schema.json) for the details.

### Using the message registry along with CALLOUT\_ entries

If the message registry entry contains a callout definition and the event log
also contains one of aforementioned CALLOUT keys in the AdditionalData property,
then the PEL code will first add the callouts stemming from the CALLOUT items,
followed by the callouts from the message registry.

### Specifying multiple callouts using JSON format FFDC files

Multiple callouts can be passed in by the creator at the time of PEL creation.
This is done by specifying them in a JSON file that is then passed in as an
[FFDC file](#ffdc-intended-for-userdata-pel-sections). The JSON will still be
added into a PEL UserData section for debug.

To specify that an FFDC file contains callouts, the format value for that FFDC
entry must be set to JSON, and the subtype field must be set to 0xCA:

```cpp
using FFDC = std::tuple<CreateIface::FFDCFormat,
                        uint8_t,
                        uint8_t,
                        sdbusplus::message::unix_fd>;

FFDC ffdc{
    CreateIface::FFDCFormat::JSON,
    0xCA, // Callout subtype
    0x01, // Callout version, set to 0x01
    fd};
```

The JSON contains an array of callouts that must be in order of highest priority
to lowest, with a maximum of 10. Any callouts after the 10th will just be thrown
away as there is no room for them in the PEL. The format looks like:

```jsonl
[
    {
        // First callout
    },
    {
        // Second callout
    },
    {
        // Nth callout
    }
]
```

A callout entry can be a normal hardware callout, a maintenance procedure
callout, or a symbolic FRU callout. Each callout must contain a Priority field,
where the possible values are:

- "H" = High
- "M" = Medium
- "A" = Medium Group A
- "B" = Medium Group B
- "C" = Medium Group C
- "L" = Low

Either unexpanded location codes or D-Bus inventory object paths can be used to
specify the called out part. An unexpanded location code does not have the
system VPD information embedded in it, and the 'Ufcs-' prefix is optional (so
can be either Ufcs-P1 or just P1).

#### Normal hardware FRU callout

Normal hardware callouts must contain either the location code or inventory
path, and priority. Even though the PEL code doesn't do any guarding or
deconfiguring itself, it needs to know if either of those things occurred as
there are status bits in the PEL to reflect them. The Guarded and Deconfigured
fields are used for this. Those fields are optional and if omitted then their
values will be false.

When the inventory path of a sub-FRU is passed in, the PEL code will put the
location code of the parent FRU into the callout.

```jsonl
{
    "LocationCode": "P0-C1",
    "Priority": "H"
}

{
    "InventoryPath": "/xyz/openbmc_project/inventory/motherboard/cpu0/core5",
    "Priority": "H",
    "Deconfigured": true,
    "Guarded": true
}

```

MRUs (Manufacturing Replaceable Units) are 4 byte numbers that can optionally be
added to callouts to specify failing devices on a FRU. These may be used during
the manufacturing test process, where there may be the ability to do these
replacements. There can be up to 15 MRUs, each with its own priority, embedded
in a callout. The possible priority values match the FRU priority values.

Note that since JSON only supports numbers in decimal and not in hex, MRU IDs
will show up as decimal when visually inspecting the JSON.

```jsonl
{
  "LocationCode": "P0-C1",
  "Priority": "H",
  "MRUs": [
    {
      "ID": 1234,
      "Priority": "H"
    },
    {
      "ID": 5678,
      "Priority": "H"
    }
  ]
}
```

#### Maintenance procedure callout

The LocationCode field is not used with procedure callouts. Only the first 7
characters of the Procedure field will be used by the PEL.

```jsonl
{
  "Procedure": "PRONAME",
  "Priority": "H"
}
```

#### Symbolic FRU callout

Only the first seven characters of the SymbolicFRU field will be used by the
PEL.

If the TrustedLocationCode field is present and set to true, this means the
location code may be used to turn on service indicators, so the LocationCode
field is required. If TrustedLocationCode is false or missing, then the
LocationCode field is optional.

```jsonl
{
  "TrustedLocationCode": true,
  "Location Code": "P0-C1",
  "Priority": "H",
  "SymbolicFRU": "FRUNAME"
}
```

## `Action Flags` and `Event Type` Rules

The `Action Flags` and `Event Type` PEL fields are optional in the message
registry, and if not present the code will set them based on certain rules laid
out in the PEL spec.

These rules are:

1. Always set the `Report` flag, unless the `Do Not Report` flag is already on.
2. Always clear the `SP Call Home` flag, as that feature isn't supported.
3. If the severity is `Non-error Event`:
   - Clear the `Service Action` flag.
   - Clear the `Call Home` flag.
   - If the `Event Type` field is `Not Applicable`, change it to
     `Information Only`.
   - If the `Event Type` field is `Information Only` or `Tracing`, set the
     `Hidden` flag.
4. If the severity is `Recovered`:
   - Set the `Hidden` flag.
   - Clear the `Service Action` flag.
   - Clear the `Call Home` flag.
5. For all other severities:
   - Clear the `Hidden` flag.
   - Set the `Service Action` flag.
   - Set the `Call Home` flag.

Additional rules may be added in the future if necessary.

## D-Bus Interfaces

See the org.open_power.Logging.PEL interface definition for the most up to date
information.

## PEL Retention

The PEL repository is allocated a set amount of space on the BMC. When that
space gets close to being full, the code will remove a percentage of PELs to
make room for new ones. In addition, the code will keep a cap on the total
number of PELs allowed. Note that removing a PEL will also remove the
corresponding OpenBMC event log.

The disk capacity limit is set to 20MB, and the number limit is 3000.

The rules used to remove logs are listed below. The checks will be run after a
PEL has been added and the method to create the PEL has returned to the caller,
i.e. run when control gets back to the event loop.

### Removal Algorithm

If the size used is 95% or under of the allocated space and under the limit on
the number of PELs, nothing further needs to be done, otherwise continue and run
all 5 of the following steps. Each step itself only deletes PELs until it meets
its requirement and then it stops.

The steps are:

1. Remove BMC created informational PELs until they take up 15% or less of the
   allocated space.

2. Remove BMC created non-informational PELs until they take up 30% or less of
   the allocated space.

3. Remove non-BMC created informational PELs until they take up 15% or less of
   the allocated space.

4. Remove non-BMC created non-informational PELs until they take up 30% or less
   of the allocated space.

5. After the previous 4 steps are complete, if there are still more than the
   maximum number of PELs, remove PELs down to 80% of the maximum.

PELs with associated guard records will never be deleted. Each step above makes
the following 4 passes, stopping as soon as its limit is reached:

- Pass 1. Remove HMC acknowledged PELs.
- Pass 2. Remove OS acknowledged PELs.
- Pass 3. Remove PHYP acknowledged PELs.
- Pass 4. Remove all PELs.

After all these steps, disk capacity will be at most 90% (15% + 30% + 15% +
30%).

## Adding python3 modules for PEL UserData and SRC parsing

In order to support python3 modules for the parsing of PEL User Data sections
and to decode SRC data, setuptools is used to import python3 packages from
external repos to be included in the OpenBMC image.

Sample layout for setuptools:

setup.py src/usr/scom/plugins/ebmc/b0300.py src/usr/i2c/plugins/ebmc/b0700.py
src/build/tools/ebmc/errludP_Helpers.py

`setup.py` is the build script for setuptools. It contains information about the
package (such as the name and version) as well as which code files to include.

The setup.py template to be used for eBMC User Data parsers:

```python3
import os.path
from setuptools import setup

# To update this dict with new key/value pair for every component added
# Key: The package name to be installed as
# Value: The path containing the package's python modules
dirmap = {
    "b0300": "src/usr/scom/plugins/ebmc",
    "b0700": "src/usr/i2c/plugins/ebmc",
    "helpers": "src/build/tools/ebmc"
}

# All packages will be installed under 'udparsers' namespace
def get_package_name(dirmap_key):
    return "udparsers.{}".format(dirmap_key)

def get_package_dirent(dirmap_item):
    package_name = get_package_name(dirmap_item[0])
    package_dir = dirmap_item[1]
    return (package_name, package_dir)

def get_packages():
    return map(get_package_name, dirmap.keys())

def get_package_dirs():
    return map(get_package_dirent, dirmap.items())

setup(
        name="Hostboot",
        version="0.1",
        packages=list(get_packages()),
        package_dir=dict(get_package_dirs())
)
```

- User Data parser module
  - Module name: `xzzzz.py`, where `x` is the Creator Subsystem from the Private
    Header section (in ASCII) and `zzzz` is the 2 byte Component ID from the
    User Data section itself (in HEX). All should be converted to lowercase.
    - For example: `b0100.py` for Hostboot created UserData with CompID 0x0100
  - Function to provide: `parseUDToJson`
    - Argument list:
      1. (int) Sub-section type
      2. (int) Section version
      3. (memoryview): Data
    - Return data:
      1. (str) JSON string

  - Sample User Data parser module:

```python3
    import json
    def parseUDToJson(subType, ver, data):
        d = dict()
        ...
        # Parse and populate data into dictionary
        ...
        jsonStr = json.dumps(d)
        return jsonStr
```

- SRC parser module
  - Module name: `xsrc.py`, where `x` is the Creator Subsystem from the Private
    Header section (in ASCII, converted to lowercase).
    - For example: `bsrc.py` for Hostboot generated SRCs
  - Function to provide: `parseSRCToJson`
    - Argument list:
      1. (str) Refcode ASCII string
      2. (str) Hexword 2
      3. (str) Hexword 3
      4. (str) Hexword 4
      5. (str) Hexword 5
      6. (str) Hexword 6
      7. (str) Hexword 7
      8. (str) Hexword 8
      9. (str) Hexword 9
    - Return data:
      1. (str) JSON string

  - Sample SRC parser module:

    ```python3
    import json
    def parseSRCToJson(ascii_str, word2, word3, word4, word5, word6, word7, \
                       word8, word9):
        d = dict({'A': 1, 'B': 2})
        ...
        # Decode SRC data into dictionary
        ...
        jsonStr = json.dumps(d)
        return jsonStr
    ```

## Fail Boot on Host Errors

The fail boot on hw error [design][1] provides a function where a system owner
can tell the firmware to fail the boot of a system if a BMC phosphor-logging
event has a hardware callout in it.

It is required that when this fail boot on hardware error setting is enabled,
that the BMC fail the boot for **any** error from the host which satisfies the
following criteria:

- not SeverityType::nonError
- has a callout of any kind from the `FailingComponentType` structure

## Self Boot Engine First Failure Data Capture Support

During SBE chip-op failure SBE creates FFDC with custom data format. SBE FFDC
contains different packets, which include SBE internal failure related Trace and
user data also Hardware procedure failure FFDC created by FAPI infrastructure.
PEL infrastructure provides support to process SBE FFDC packets created by FAPI
infrastructure during hardware procedure execution failures, also add callouts,
user data section information based on FAPI processing in case non FAPI based
failure, just keeps the raw FFDC data in the user section to support SBE parser
plugins.

CreatePELWithFFDCFiles D-Bus method on the `org.open_power.Logging.PEL`
interface must be used when creating a new event log.

To specify that an FFDC file contains SBE FFDC, the format value for that FFDC
entry must be set to "custom", and the subtype field must be set to 0xCB:

```cpp
using FFDC = std::tuple<CreateIface::FFDCFormat,
                        uint8_t,
                        uint8_t,
                        sdbusplus::message::unix_fd>;

FFDC ffdc{
     CreateIface::FFDCFormat::custom,
     0xCB, // SBE FFDC subtype
     0x01, // SBE FFDC version, set to 0x01
     fd};
```

"SRC6" Keyword in the additional data section should be populated with below.

- [0:15] chip position (hex)
- [16:23] command class (hex)
- [24:31] command (hex)

e.g for GetSCOM

SRC6="0002A201"

Note: "phal" build-time configure option should be "enabled" to enable this
feature.

## PEL Archiving

When an OpenBMC event log is deleted its corresponding PEL is moved to an
archive folder. These archived PELs will be available in BMC dump. The archive
path: /var/lib/phosphor-logging/extensions/pels/logs/archive.

Highlighted points are:

- PELs whose corresponding event logs have been deleted will be available in the
  archive folder.
- Archive folder size is tracked along with logs folder size and if combined
  size exceeds warning size all archived PELs will be deleted.
- Archived PEL logs can be viewed using peltool with flag --archive.
- If a PEL is deleted using peltool its not archived.

## Handling PELs for hot plugged FRUs

The degraded mode reporting functionality (i.e. nag) implemented by IBM creates
periodic degraded mode reports when a system is running in degraded mode. This
includes when hardware has been deconfigured or guarded by hostboot, and also
when there is a fault on a redundant fan, VRM, or power supply. The report
includes the PELs created for the fails leading to the degraded mode. These PELs
can be identified by the 'deconfig' or 'guard' flags set in the SRC of BMC or
hostboot PELs.

Fans and power supplies can be hot plugged when they are replaced, and as that
FRU is no longer considered degraded the PELs that led to its replacement no
longer need be picked up in the degraded mode report.

To handle this, the PEL daemon will watch the inventory D-Bus objects that have
the 'xyz.openbmc_project.Inventory.Item.Fan' or
'xyz.openbmc_project.Inventory.Item.PowerSupply' interface. When the 'Present'
property on the 'xyz.openbmc_project.Inventory.Item' interface on these paths
change to true, the code will find all fan/PS PELs with the deconfig bit set and
that location code in the callout list and clear the deconfig flag in the PEL.
That way, when the code that does the report searches for PELs, it will no
longer find them.

[1]:
  https://github.com/openbmc/docs/blob/master/designs/fail-boot-on-hw-error.md
