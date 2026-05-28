# AMD Event Log Extension

## Overview

This extension provides AMD-specific logic for populating metadata in OpenBMC
log entries.

It builds on the generic `Entry.Oem` property defined in the phosphor-logging
framework and contributes AMD-specific structured data during log creation.

For details on the generic OEM property design, refer to the
[phosphor-logging README](https://github.com/openbmc/phosphor-logging/blob/master/README.md)

---

## AMD OEM Metadata

AMD-specific metadata is stored under the "Amd" namespace within the
`Entry.Oem` property.

### Example

```json
{
  "AMD": {
    "Key": "Value"
  }
}
```

This allows AMD to attach platform-specific context to log entries without
impacting the standard logging schema.

---

## Implementation Notes

The AMD extension contributes metadata to the `Entry.Oem` property under
the "AMD" namespace during log creation.

- AMD-specific data is populated as part of the logging flow
- Only the "AMD" namespace is modified
- Standard log entry properties are not altered
- Metadata should remain structured, compact, and extensible

---

## AEL Metadata (AMD Event Log)

The AMD Event Log (AEL) defines a structured metadata model used within
the "AMD" namespace.

### Example Fields

- `AEL.VERSION`
- `AEL.TYPE`
- `AEL.AFID`
- `AEL.FRU_LIST`
- `AEL.SEVERITY_OVERRIDE`

### Example

```json
{
  "AMD": {
    "AEL.VERSION": "1.0",
    "AEL.TYPE": "Error",
    "AEL.AFID": "0x1234",
    "AEL.FRU_LIST": "/xyz/openbmc_project/inventory/system/chassis",
    "AEL.SEVERITY_OVERRIDE": "Critical"
  }
}
```

---

## Notes

- The phosphor-logging framework owns the lifecycle and aggregation of `Entry.Oem`
- This extension focuses only on AMD-specific metadata population
- All generic behavior is defined in the base README
