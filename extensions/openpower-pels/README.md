# OpenPower Platform Event Log (PEL) extension

This extension will create PELs for every OpenBMC event log. It is also
possible to point to the raw PEL to use in the OpenBMC event, and then that
will be used instead of creating one.

## Contents
* [Passing in data when creating PELs](#passing-pel-related-data-within-an-openbmc-event-log)
* [Default UserData sections for BMC created PELs](#default-userdata-sections-for-bmc-created-pels)
* [The PEL Message Registry](#the-pel-message-registry)
* [Action Flags and Event Type Rules](#action-flags-and-event-type-rules)
* [D-Bus Interfaces](#d-bus-interfaces)

## Passing PEL related data within an OpenBMC event log

An error log creator can pass in data that is relevant to a PEL by using
certain keywords in the AdditionalData property of the event log.

### AdditionalData keywords

#### RAWPEL

This keyword is used to point to an existing PEL in a binary file that should
be associated with this event log.  The syntax is:
```
RAWPEL=<path to PEL File>
e.g.
RAWPEL="/tmp/pels/pel.5"
```
The code will assign its own error log ID to this PEL, and also update the
commit timestamp field to the current time.

#### _PID

This keyword that contains the application's PID is added automatically by the
phosphor-logging daemon when the `commit` or `report` APIs are used to create
an event log, but not when the `Create` D-Bus method is used.  If a caller of
the `Create` API wishes to have their PID captured in the PEL this should be
used.

This will be added to the PEL in a section of type User Data (UD), along with
the application name it corresponds to.

The syntax is:
```
_PID=<PID of application>
e.g.
_PID="12345"
```

### FFDC Intended For UserData PEL sections

When one needs to add FFDC into the PEL UserData sections, the
`CreateWithFFDCFiles` D-Bus method on the `xyz.openbmc_project.Logging.Create`
interface must be used when creating a new event log. This method takes a list
of files to store in the PEL UserData sections.

That API is the same as the 'Create' one, except it has a new parameter:

```
std::vector<std::tuple<enum[FFDCFormat],
                       uint8_t,
                       uint8_t,
                       sdbusplus::message::unix_fd>>
```

Each entry in the vector contains a file descriptor for a file that will
be stored in a unique UserData section.  The tuple's arguments are:

- enum[FFDCFormat]: The data format type, the options are:
    - 'json'
        - The parser will use nlohmann::json\'s pretty print
    - 'bson'
        - The parser will use nlohmann::json\'s pretty print
    - 'text'
        - The parser will output ASCII text
    - 'custom'
        - The parser will hexdump the data, unless there is a parser registered
          for this component ID and subtype.
- uint8_t: subType
  - Useful for the 'custom' type.  Not used with the other types.
- uint8_t: version
  - The version of the data.  Can be incremented if format changes.
- unixfd - The file descriptor for the opened file that contains the
    contents.  The file descriptor can be closed and the file can be deleted if
    desired after the method call.

An example of saving JSON data to a file is:

```
nlohmann::json json = ...;
auto jsonString = json.dump();
// write jsonString to file
```

Upon receiving this data, the PEL code will create UserData sections for each
entry in that vector with the follow UserData fields:

- Section header component ID:
    - If the type field from the tuple is "custom", use the component ID from
      the message registry.
    - Otherwise, set the component ID to the phosphor-logging component ID so
      that the parser knows to use the built in parsers (e.g. json) for the
      type.
- Section header subtype: The subtype field from the tuple.
- Section header version: The version field from the tuple.
- Section data: The data from the file.

If there is a peltool parser registered for the custom type (method is TBD),
that will be used by peltool to print the data, otherwise it will be hexdumped.

## Default UserData sections for BMC created PELs

The extension code that creates PELs will add these UserData sections to every
PEL:

- The AdditionalData property contents
  - If the AdditionalData property in the OpenBMC event log has anything in it,
    it will be saved in a UserData section as a JSON string.

## The PEL Message Registry

The PEL message registry is used to create PELs from OpenBMC event logs.
Documentation can be found [here](registry/README.md).

## `Action Flags` and `Event Type` Rules

The `Action Flags` and `Event Type` PEL fields are optional in the message
registry, and if not present the code will set them based on certain rules
layed out in the PEL spec.  In fact, even if they were specified, the checks
are still done to ensure consistency across all the logs.

These rules are:
1. Always set the `Report` flag, unless the `Do Not Report` flag is already on.
2. Always clear the `SP Call Home` flag, as that feature isn't supported.
3. If the severity is `Non-error Event`:
    - Clear the `Service Action` flag.
    - Clear the `Call Home` flag.
    - If the `Event Type` field is `Not Applicable`, change it to `Information
      Only`.
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
