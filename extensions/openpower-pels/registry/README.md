# Platform Event Log Message Registry

On the BMC, PELs are created from the standard event logs provided by
phosphor-logging using a message registry that provides the PEL related fields.
The message registry is a JSON file.

## Contents

- [Component IDs](#component-ids)
- [Message Registry](#message-registry-fields)
- [Modifying and Testing](#modifying-and-testing)

## Component IDs

A component ID is a 2 byte value of the form 0xYY00 used in a PEL to:

1. Provide the upper byte (the YY from above) of an SRC reason code in `BD`
   SRCs.
2. Reside in the section header of the Private Header PEL section to specify the
   error log creator's component ID.
3. Reside in the section header of the User Header section to specify the error
   log committer's component ID.
4. Reside in the section header in the User Data section to specify which parser
   to call to parse that section.

Component IDs are specified in the message registry either as the upper byte of
the SRC reason code field for `BD` SRCs, or in the standalone `ComponentID`
field.

Component IDs will be unique on a per-repository basis for errors unique to that
repository. When the same errors are created by multiple repositories, those
errors will all share the same component ID. The master list of component IDs is
[here](O_component_ids.json). That file can used by PEL parsers to display a
name for the component ID. The 'O' in the name is the creator ID value for BMC
created PELs.

## Message Registry Fields

The message registry schema is [here](schema/schema.json), and the message
registry itself is [here](message_registry.json). The schema will be validated
either during a bitbake build or during CI, or eventually possibly both.

In the message registry, there are fields for specifying:

### Name

This is the key into the message registry, and is the Message property of the
OpenBMC event log that the PEL is being created from.

```json
"Name": "xyz.openbmc_project.Power.Fault"
```

### Subsystem

This field is part of the PEL User Header section, and is used to specify the
subsystem pertaining to the error. It is an enumeration that maps to the actual
PEL value. If the subsystem isn't known ahead of time, it can be passed in at
the time of PEL creation using the 'PEL_SUBSYSTEM' AdditionalData field. In this
case, 'Subsystem' isn't required, though 'PossibleSubsystems' is.

```json
"Subsystem": "power_supply"
```

### PossibleSubsystems

This field is used by scripts that build documentation from the message registry
to know which subsystems are possible for an error when it can't be hardcoded
using the 'Subsystem' field. It is mutually exclusive with the 'Subsystem'
field.

```json
"PossibleSubsystems": ["memory", "processor"]
```

### Severity

This field is part of the PEL User Header section, and is used to specify the
PEL severity. It is an optional field, if it isn't specified, then the severity
of the OpenBMC event log will be converted into a PEL severity value.

It can either be the plain severity value, or an array of severity values that
are based on system type, where an entry without a system type will match
anything unless another entry has a matching system type.

```json
"Severity": "unrecoverable"
```

```json
Severity":
[
    {
        "System": "system1",
        "SevValue": "recovered"
    },
    {
        "Severity": "unrecoverable"
    }
]
```

The above example shows that on system 'system1' the severity will be recovered,
and on every other system it will be unrecoverable.

### Mfg Severity

This is an optional field and is used to override the Severity field when a
specific manufacturing isolation mode is enabled. It has the same format as
Severity.

```json
"MfgSeverity": "unrecoverable"
```

### Event Scope

This field is part of the PEL User Header section, and is used to specify the
event scope, as defined by the PEL spec. It is optional and defaults to "entire
platform".

```json
"EventScope": "entire_platform"
```

### Event Type

This field is part of the PEL User Header section, and is used to specify the
event type, as defined by the PEL spec. It is optional and defaults to "not
applicable" for non-informational logs, and "misc_information_only" for
informational ones.

```json
"EventType": "na"
```

### Action Flags

This field is part of the PEL User Header section, and is used to specify the
PEL action flags, as defined by the PEL spec. It is an array of enumerations.

The action flags can usually be deduced from other PEL fields, such as the
severity or if there are any callouts. As such, this is an optional field and if
not supplied the code will fill them in based on those fields.

In fact, even if supplied here, the code may still modify them to ensure they
are correct. The rules used for this are
[here](../README.md#action-flags-and-event-type-rules).

```json
"ActionFlags": ["service_action", "report", "call_home"]
```

### Mfg Action Flags

This is an optional field and is used to override the Action Flags field when a
specific manufacturing isolation mode is enabled.

```json
"MfgActionFlags": ["service_action", "report", "call_home"]
```

### Component ID

This is the component ID of the PEL creator, in the form 0xYY00. For `BD` SRCs,
this is an optional field and if not present the value will be taken from the
upper byte of the reason code. If present for `BD` SRCs, then this byte must
match the upper byte of the reason code.

```json
"ComponentID": "0x5500"
```

### SRC Type

This specifies the type of SRC to create. The type is the first 2 characters of
the 8 character ASCII string field of the PEL. The allowed types are `BD`, for
the standard OpenBMC error, and `11`, for power related errors. It is optional
and if not specified will default to `BD`.

Note: The ASCII string for BD SRCs looks like: `BDBBCCCC`, where:

- BD = SRC type
- BB = PEL subsystem as mentioned above
- CCCC SRC reason code

For `11` SRCs, it looks like: `1100RRRR`, where RRRR is the SRC reason code.

```json
"Type": "11"
```

### SRC Reason Code

This is the 4 character value in the latter half of the SRC ASCII string. It is
treated as a 2 byte hex value, such as 0x5678. For `BD` SRCs, the first byte is
the same as the first byte of the component ID field in the Private Header
section that represents the creator's component ID.

```json
"ReasonCode": "0x5544"
```

### SRC Symptom ID Fields

The symptom ID is in the Extended User Header section and is defined in the PEL
spec as the unique event signature string. It always starts with the ASCII
string. This field in the message registry allows one to choose which SRC words
to use in addition to the ASCII string field to form the symptom ID. All words
are separated by underscores. If not specified, the code will choose a default
format, which may depend on the SRC type.

For example: ["SRCWord3", "SRCWord9"] would be:
`<ASCII_STRING>_<SRCWord3>_<SRCWord9>`, which could look like:
`B181320_00000050_49000000`.

```json
"SymptomIDFields": ["SRCWord3", "SRCWord9"]
```

### SRC words 6 to 9

In a PEL, these SRC words are free format and can be filled in by the user as
desired. On the BMC, the source of these words is the AdditionalData fields in
the event log. The message registry provides a way for the log creator to
specify which AdditionalData property field to get the data from, and also to
define what the SRC word means for use by parsers. If not specified, these SRC
words will be set to zero in the PEL.

```json
"Words6to9":
{
    "6":
    {
        "description": "Failing unit number",
        "AdditionalDataPropSource": "PS_NUM"
    }
}
```

### Documentation Fields

The documentation fields are used by PEL parsers to display a human readable
description of a PEL. They are also the source for the Redfish event log
messages.

#### Message

This field is used by the BMC's PEL parser as the description of the error log.
It will also be used in Redfish event logs. It supports argument substitution
using the %1, %2, etc placeholders allowing any of the SRC user data words 6 - 9
to be displayed as part of the message. If the placeholders are used, then the
`MessageArgSources` property must be present to say which SRC words to use for
each placeholder.

```json
"Message": "Processor %1 had %2 errors"
```

#### MessageArgSources

This optional field is required when the Message field contains the %X
placeholder arguments. It is an array that says which SRC words to get the
placeholders from. In the example below, SRC word 6 would be used for %1, and
SRC word 7 for %2.

```json
"MessageArgSources":
[
    "SRCWord6", "SRCWord7"
]
```

#### Description

A short description of the error. This is required by the Redfish schema to
generate a Redfish message entry, but is not used in Redfish or PEL output.

```json
"Description": "A power fault"
```

#### Notes

This is an optional free format text field for keeping any notes for the
registry entry, as comments are not allowed in JSON. It is an array of strings
for easier readability of long fields.

```json
"Notes": [
    "This entry is for every type of power fault.",
    "There is probably a hardware failure."
]
```

### Callout Fields

The callout fields allow one to specify the PEL callouts (either a hardware FRU,
a symbolic FRU, or a maintenance procedure) in the entry for a particular error.
These callouts can vary based on system type, as well as a user specified
AdditionalData property field. Callouts will be added to the PEL in the order
they are listed in the JSON. If a callout is passed into the error, say with
CALLOUT_INVENTORY_PATH, then that callout will be added to the PEL before the
callouts in the registry.

There is room for up to 10 callouts in a PEL.

#### Callouts example based on the system type

```json
"Callouts":
[
    {
        "System": "system1",
        "CalloutList":
        [
            {
                "Priority": "high",
                "LocCode": "P1-C1"
            },
            {
                "Priority": "low",
                "LocCode": "P1"
            }
        ]
    },
    {
        "CalloutList":
        [
            {
                "Priority": "high",
                "Procedure": "SVCDOCS"
            }
        ]

    }
]

```

The above example shows that on system 'system1', the FRU at location P1-C1 will
be called out with a priority of high, and the FRU at P1 with a priority of low.
On every other system, the maintenance procedure SVCDOCS is called out.

#### Callouts example based on an AdditionalData field

```json
"CalloutsUsingAD":
{
    "ADName": "PROC_NUM",
    "CalloutsWithTheirADValues":
    [
        {
            "ADValue": "0",
            "Callouts":
            [
                {
                    "CalloutList":
                    [
                        {
                            "Priority": "high",
                            "LocCode": "P1-C5"
                        }
                    ]
                }
            ]
        },
        {
            "ADValue": "1",
            "Callouts":
            [
                {
                    "CalloutList":
                    [
                        {
                            "Priority": "high",
                            "LocCode": "P1-C6"
                        }
                    ]
                }
            ]
        }
    ]
}

```

This example shows that the callouts were selected based on the 'PROC_NUM'
AdditionalData field. When PROC_NUM was 0, the FRU at P1-C5 was called out. When
it was 1, P1-C6 was called out. Note that the same 'Callouts' array is used as
in the previous example, so these callouts can also depend on the system type.

If it's desired to use a different set of callouts when there isn't a match on
the AdditionalData field, one can use CalloutsWhenNoADMatch. In the following
example, the 'air_mover' callout will be added if 'PROC_NUM' isn't 0.
'CalloutsWhenNoADMatch' has the same schema as the 'Callouts' section.

```json
"CalloutsUsingAD":
{
    "ADName": "PROC_NUM",
    "CalloutsWithTheirADValues":
    [
        {
            "ADValue": "0",
            "Callouts":
            [
                {
                    "CalloutList":
                    [
                        {
                            "Priority": "high",
                            "LocCode": "P1-C5"
                        }
                    ]
                }
            ]
        },
    ],
    "CalloutsWhenNoADMatch": [
        {
            "CalloutList": [
                {
                    "Priority": "high",
                    "SymbolicFRU": "air_mover"
                }
            ]
        }
    ]
}

```

#### CalloutType

This field can be used to modify the failing component type field in the callout
when the default doesn\'t fit:

```json
{

    "Priority": "high",
    "Procedure": "FIXIT22"
    "CalloutType": "config_procedure"
}
```

The defaults are:

- Normal hardware FRU: hardware_fru
- Symbolic FRU: symbolic_fru
- Procedure: maint_procedure

#### Symbolic FRU callouts with dynamic trusted location codes

A special case is when one wants to use a symbolic FRU callout with a trusted
location code, but the location code to use isn\'t known until runtime. This
means it can\'t be specified using the 'LocCode' key in the registry.

In this case, one should use the 'SymbolicFRUTrusted' key along with the
'UseInventoryLocCode' key, and then pass in the inventory item that has the
desired location code using the 'CALLOUT_INVENTORY_PATH' entry inside of the
AdditionalData property. The code will then look up the location code for that
passed in inventory FRU and place it in the symbolic FRU callout. The normal FRU
callout with that inventory item will not be created. The symbolic FRU must be
the first callout in the registry for this to work.

```json
{
  "Priority": "high",
  "SymbolicFRUTrusted": "AIR_MOVR",
  "UseInventoryLocCode": true
}
```

### Capturing the Journal

The PEL daemon can be told to capture pieces of the journal in PEL UserData
sections. This could be useful for debugging problems where a BMC dump which
would also contain the journal isn't available.

The 'JournalCapture' field has two formats, one that will create one UserData
section with the previous N lines of the journal, and another that can capture
any number of journal snippets based on the journal's SYSLOG_IDENTIFIER field.

```json
"JournalCapture": {
    "NumLines": 30
}
```

```json
"JournalCapture":
{
    "Sections": [
        {
            "SyslogID": "phosphor-bmc-state-manager",
            "NumLines": 20
        },
        {
            "SyslogID": "phosphor-log-manager",
            "NumLines": 15
        }
    ]
}
```

The first example will capture the previous 30 lines from the journal into a
single UserData section.

The second example will create two UserData sections, the first with the most
recent 20 lines from phosphor-bmc-state-manager, and the second with 15 lines
from phosphor-log-manager.

If a UserData section would make the PEL exceed its maximum size of 16KB, it
will be dropped.

## Modifying and Testing

The general process for adding new entries to the message registry is:

1. Update message_registry.json to add the new errors.
2. If a new component ID is used (usually the first byte of the SRC reason
   code), document it in O_component_ids.json.
3. Validate the file. It must be valid JSON and obey the schema. The
   `validate_registry.py` script in `extensions/openpower-pels/registry/tools`
   will validate both, though it requires the python-jsonschema package to do
   the schema validation. This script is also run to validate the message
   registry as part of CI testing.

   ```sh
   ./tools/validate_registry.py -s schema/schema.json -r message_registry.json
   ```

4. One can test what PELs are generated from these new entries without writing
   any code to create the corresponding event logs:

   1. Copy the modified message_registry.json into `/etc/phosphor-logging/` on
      the BMC. That directory may need to be created.
   2. Use busctl to call the Create method to create an event log corresponding
      to the message registry entry under test.

      ```sh
      busctl call xyz.openbmc_project.Logging /xyz/openbmc_project/logging \
      xyz.openbmc_project.Logging.Create Create ssa{ss} \
      xyz.openbmc_project.Common.Error.Timeout \
      xyz.openbmc_project.Logging.Entry.Level.Error 1 "TIMEOUT_IN_MSEC" "5"
      ```

   3. Check the PEL that was created using peltool.
   4. When finished, delete the file from `/etc/phosphor-logging/`.
