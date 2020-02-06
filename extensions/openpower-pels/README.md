# OpenPower Platform Event Log (PEL) extension

This extension will create PELs for every OpenBMC event log. It is also
possible to point to the raw PEL to use in the OpenBMC event, and then that
will be used instead of creating one.

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

#### ESEL

This keyword's data contains a full PEL in string format.  This is how hostboot
sends down PELs when it is configured in IPMI communication mode.  The PEL is
handled just like the PEL obtained using the RAWPEL keyword.

The syntax is:

```
ESEL=
"00 00 df 00 00 00 00 20 00 04 12 01 6f aa 00 00 50 48 00 30 01 00 33 00 00..."
```

Note that there are 16 bytes of IPMI SEL data before the PEL data starts.

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

## Default UserData sections for BMC created PELs

The extension code that creates PELs will add these UserData sections to every
PEL:

- The AdditionalData property contents
  - If the AdditionalData property in the OpenBMC event log has anything in it,
    it will be saved in a UserData section as a JSON string.

- System information
  - This section contains various pieces of system information, such as the
    full code level and the BMC, chassis, and host state properties.

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
