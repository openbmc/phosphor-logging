# Platform Event Log Message Registry
On the BMC, PELs are created from the standard event logs provided by
phosphor-logging using a message registry that provides the PEL related fields.
The message registry is a JSON file.

## Contents
* [Component IDs](#component-ids)
* [Message Registry](#message-registry-fields)

## Component IDs
A component ID is a 2 byte value of the form 0xYY00 used in a PEL to:
1. Provide the upper byte (the YY from above) of an SRC reason code in `BD`
   SRCs.
2. Reside in the section header of the Private Header PEL section to specify
   the error log creator's component ID.
3. Reside in the section header of the User Header section to specify the error
   log committer's component ID.
4. Reside in the section header in the User Data section to specify which
   parser to call to parse that section.

Component IDs are specified in the message registry either as the upper byte of
the SRC reason code field for `BD` SRCs, or in the standalone `ComponentID`
field.

Component IDs will be unique on a per-repository basis for errors unique to
that repository.  When the same errors are created by multiple repositories,
those errors will all share the same component ID.  The master list of
component IDs is [here](ComponentIDs.md).

## Message Registry Fields
The message registry schema is [here](schema/schema.json), and the message
registry itself is [here](message_registry.json).  The schema will be validated
either during a bitbake build or during CI, or eventually possibly both.

In the message registry, there are fields for specifying:

### Name
This is the key into the message registry, and is the Message property
of the OpenBMC event log that the PEL is being created from.

```
"Name": "xyz.openbmc_project.Power.Fault"
```

### Subsystem
This field is part of the PEL User Header section, and is used to specify
the subsystem pertaining to the error.  It is an enumeration that maps to the
actual PEL value.

```
"Subsystem": "power_supply"
```

### Severity
This field is part of the PEL User Header section, and is used to specify
the PEL severity.  It is an optional field, if it isn't specified, then the
severity of the OpenBMC event log will be converted into a PEL severity value.

```
"Severity": "unrecoverable"
```

### Mfg Severity
This is an optional field and is used to override the Severity field when a
specific manufacturing isolation mode is enabled.

```
"MfgSeverity": "unrecoverable"
```

### Event Scope
This field is part of the PEL User Header section, and is used to specify
the event scope, as defined by the PEL spec.  It is optional and defaults to
"entire platform".

```
"EventScope": "entire_platform"
```

### Event Type
This field is part of the PEL User Header section, and is used to specify
the event type, as defined by the PEL spec.  It is optional and defaults to
"not applicable" for non-informational logs, and "misc_information_only" for
informational ones.

```
"EventType": "na"
```

### Action Flags
This field is part of the PEL User Header section, and is used to specify the
PEL action flags, as defined by the PEL spec.  It is an array of enumerations.

The action flags can usually be deduced from other PEL fields, such as the
severity or if there are any callouts.  As such, this is an optional field and
if not supplied the code will fill them in based on those fields.

In fact, even if supplied here, the code may still modify them to ensure they
are correct.

```
"ActionFlags": ["service_action", "report", "call_home"]
```

### Mfg Action Flags
This is an optional field and is used to override the Action Flags field when a
specific manufacturing isolation mode is enabled.

```
"MfgActionFlags": ["service_action", "report", "call_home"]
```

### Component ID
This is the component ID of the PEL creator, in the form 0xYY00.  For `BD`
SRCs, this is an optional field and if not present the value will be taken from
the upper byte of the reason code.  If present for `BD` SRCs, then this byte
must match the upper byte of the reason code.

```
"ComponentID": "0x5500"
```

### SRC Type
This specifies the type of SRC to create.  The type is the first 2 characters
of the 8 character ASCII string field of the PEL.  The allowed types are `BD`,
for the standard OpenBMC error, and `11`, for power related errors.  It is
optional and if not specified will default to `BD`.

Note: The ASCII string for BD SRCs looks like: `BDBBCCCC`, where:
* BD = SRC type
* BB = PEL subsystem as mentioned above
* CCCC SRC reason code

For `11` SRCs, it looks like: `1100RRRR`, where RRRR is the SRC reason code.

```
"Type": "11"
```

### SRC Reason Code
This is the 4 character value in the latter half of the SRC ASCII string.  It
is treated as a 2 byte hex value, such as 0x5678.  For `BD` SRCs, the first
byte is the same as the first byte of the component ID field in the Private
Header section that represents the creator's component ID.

```
"ReasonCode": "0x5544"
```

### SRC Symptom ID Fields
The symptom ID is in the Extended User Header section and is defined in the PEL
spec as the unique event signature string.  It always starts with the ASCII
string.  This field in the message registry allows one to choose which SRC words
to use in addition to the ASCII string field to form the symptom ID. All words
are separated by underscores.  If not specified, the code will choose a default
format, which may depend on the SRC type.

For example: ["SRCWord3", "SRCWord9"] would be:
`<ASCII_STRING>_<SRCWord3>_<SRCWord9>`, which could look like:
`B181320_00000050_49000000`.

```
"SymptomIDFields": ["SRCWord3", "SRCWord9"]
```

### SRC words 6 to 9
In a PEL, these SRC words are free format and can be filled in by the user as
desired.  On the BMC, the source of these words is the AdditionalData fields in
the event log.  The message registry provides a way for the log creator to
specify which AdditionalData property field to get the data from, and also to
define what the SRC word means for use by parsers.  If not specified, these SRC
words will be set to zero in the PEL.

```
"Words6to9":
{
    "6":
    {
        "description": "Failing unit number",
        "AdditionalDataPropSource": "PS_NUM"
    }
}
```

### SRC Power Fault flag
The SRC has a bit in it to indicate if the error is a power fault.  This is an
optional field in the message registry and defaults to false.

```
"PowerFault: false
```

### Documentation Fields
The documentation fields are used by PEL parsers to display a human readable
description of a PEL.  They are also the source for the Redfish event log
messages.

#### Message
This field is used by the BMC's PEL parser as the description of the error log.
It will also be used in Redfish event logs.  It supports argument substitution
using the %1, %2, etc placeholders allowing any of the SRC user data words 6 -
9 to be displayed as part of the message.  If the placeholders are used, then
the `MessageArgSources` property must be present to say which SRC words to use
for each placeholder.

```
"Message": "Processor %1 had %2 errors"
```

#### MessageArgSources
This optional field is required when the Message field contains the %X
placeholder arguments. It is an array that says which SRC words to get the
placeholders from.  In the example below, SRC word 6 would be used for %1, and
SRC word 7 for %2.

```
"MessageArgSources":
[
    "SRCWord6", "SRCWord7"
]
```

#### Description
A short description of the error.  This is required by the Redfish schema to generate a Redfish message entry, but is not used in Redfish or PEL output.

```
"Description": "A power fault"
```

#### Notes
This is an optional free format text field for keeping any notes for the
registry entry, as comments are not allowed in JSON.  It is an array of strings
for easier readability of long fields.

```
"Notes": [
    "This entry is for every type of power fault.",
    "There is probably a hardware failure."
]
```
