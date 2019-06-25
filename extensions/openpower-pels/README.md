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

#### _PID

This keyword that contains the application's PID is added automatically by the
phosphor-logging daemon when the `create` or `report` APIs are used to create
an event log, but not when the `Create` D-Bus method is used.  If a caller of
the `Create` API wishes to have their PID captured in the PEL this should be
used.

This will be added to the PEL in a user data (UH) section, along with the
application name it corresponds to.

The syntax is:
```
_PID=<PID of application>
e.g.
_PID="12345"
```

## The PEL Message Registry

## D-Bus Interfaces
