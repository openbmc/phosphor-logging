# AMD Event Log Extension

## Overview

This extension provides AMD-specific logic for populating metadata in OpenBMC
log entries.

It builds on the generic `Entry.Oem` property defined in the phosphor-logging
framework and contributes AMD-specific structured data during log creation.

For details on the generic OEM property design, refer to the
[phosphor-logging README](https://github.com/openbmc/phosphor-logging/blob/master/README.md)

---

## AMD OEM Namespace

AMD-specific metadata is stored under the "Amd" key within the `Entry.Oem`
property.

### Example

```json
{
  "AMD": {
    "Key": "Value"
  }
}
```

### Guidelines

- Only the "AMD" namespace is owned and modified by this extension
- Standard log entry fields remain unchanged
- Metadata should be:
  - Structured
  - Compact
  - Backward compatible

---

## AEL (AMD Event Log) Metadata

The AMD Event Log (AEL) defines a structured and partially standardized schema
used within the "Amd" namespace.

AEL provides a set of well-known fields that are expected to be understood by
consumers, while still allowing additional extension fields when required.

---

## AEL Core Fields

| Field          | Description                                        |
| -------------- | -------------------------------------------------- |
| `AEL.VERSION`  | Version of the AEL schema                          |
| `AEL.AFID`     | AMD Feild ID identifying the error                 |
| `AEL.FRU_LIST` | Inventory object path(s) associated with the fault |

These fields are expected to be stable and documented.

---

## AEL Example

```json
{
  "Amd": {
    "AEL.VERSION": "1.0",
    "AEL.AFID": "0x1234",
    "AEL.FRU_LIST": "/xyz/openbmc_project/inventory/system/chassis"
  }
}
```

---

## AEL Reverse Lookup Table

- Provides mapping of Redfish events to AMD-specific identifiers (AFID,
  originOfCondition)
- Uses a JSON-defined reverse LUT generated at build time
- Eliminates runtime parsing via compile-time generated C++ code
- Ensures deterministic and efficient lookup behavior
- Supports OEM customization through Yocto-based JSON override

---

### Rules

- Fields prefixed with `AEL.` are reserved for AMD-defined semantics
- New fields may be introduced without breaking existing consumers
- Consumers should ignore unknown fields

---

## Implementation Notes

- Metadata is populated during log creation
- The phosphor-logging framework owns:
  - Storage
  - Lifecycle
  - Aggregation of `Entry.Oem`
- This extension is responsible only for:
  - Populating AMD-specific metadata
  - Maintaining AEL schema consistency

---
