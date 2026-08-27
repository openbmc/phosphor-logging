# AMD Event Log Extension

## Overview

This extension provides AMD-specific functionality for the phosphor-logging
framework.

It builds on the extension infrastructure provided by phosphor-logging and
enables AMD-specific processing during log creation.

The extension framework allows AMD-specific logic to enrich log entries without
modifying the core logging implementation.

---

## AMD Metadata Namespace

AMD-specific metadata is stored under the `"AMD"` namespace within the runtime
metadata object managed by phosphor-logging.

### Example

```json
{
  "AMD": {
    "Key": "Value"
  }
}
```

### Guidelines

- Only the `"AMD"` namespace is owned and modified by AMD extensions.
- Metadata should be structured and compact.
- Existing fields should remain backward compatible.
- Consumers should ignore unrecognized fields.

---

## Runtime Metadata

The AMD extension framework supports enriching log entries with AMD-specific
runtime metadata.

Metadata is generated during log creation and stored under the `"AMD"` namespace
as structured JSON.

### AEL Metadata

The AMD Event Log (AEL) extension uses this framework to contribute AMD-specific
event information derived from the log context.

### AEL Core Fields

| Field          | Description                                        |
| -------------- | -------------------------------------------------- |
| `AEL.VERSION`  | Version of the AEL schema                          |
| `AEL.AFID`     | AMD Field ID identifying the error                 |
| `AEL.FRU_LIST` | Inventory object path(s) associated with the fault |
| `AEL.RACK_ID`  | Rack identifier associated with the event          |
| `AEL.REDFISH`  | Optional pre-rendered AMD OEM Redfish payload      |

### Example

```json
{
  "AMD": {
    "AEL.VERSION": "1.0",
    "AEL.AFID": "4660",
    "AEL.FRU_LIST": "/xyz/openbmc_project/inventory/system/chassis"
  }
}
```

### Redfish Projection

The AEL framework supports projecting AMD runtime metadata into an AMD OEM
Redfish representation.

By default, the framework generates a Redfish payload from available AEL
metadata.

Applications may optionally provide a fully rendered OEM Redfish payload using
the `AEL.REDFISH` field. When present, the supplied payload is treated as the
authoritative AMD OEM representation and automatic projection is skipped.

This supports two usage models:

- Metadata-driven projection using AEL fields such as `AEL.AFID`,
  `AEL.FRU_LIST`, and `AEL.RACK_ID`.
- Direct passthrough of a fully rendered AMD OEM Redfish payload using
  `AEL.REDFISH`.

#### Metadata-Based Projection

Input:

```json
{
  "AMD": {
    "AEL.AFID": "12001",
    "AEL.FRU_LIST": "/xyz/openbmc_project/inventory/system/chassis"
  }
}
```

Projected Output:

```json
{
  "@odata.type": "#AMD_Message.v1_0_0.AMD_Message",
  "AMDFieldIdentifiers": [
    {
      "AFID": 12001,
      "Description": "Compute Tray Error"
    }
  ]
}
```

#### Pre-rendered OEM Payload (Passthrough)

Input:

```json
{
  "AMD": {
    "AEL.REDFISH": {
      "@odata.type": "#AMD_Message.v1_0_0.AMD_Message",
      "AMDFieldIdentifiers": [
        {
          "AFID": 12001,
          "Description": "Compute Tray Error"
        }
      ]
    }
  }
}
```

Output:

```json
{
  "@odata.type": "#AMD_Message.v1_0_0.AMD_Message",
  "AMDFieldIdentifiers": [
    {
      "AFID": 12001,
      "Description": "Compute Tray Error"
    }
  ]
}
```

When `AEL.REDFISH` is present, the payload is used directly and no additional
AEL metadata processing or Redfish projection is performed.

---

## AEL Reverse Lookup Table

- Provides mapping of Redfish events to AMD-specific identifiers (AFID,
  originOfCondition)
- Uses a JSON-defined reverse LUT generated at build time
- Eliminates runtime parsing via compile-time generated C++ code
- Ensures deterministic and efficient lookup behavior
- Supports OEM customization through Yocto-based JSON override

---

## Extensibility

The AMD extension infrastructure is designed to support additional AMD-specific
logging features using the same runtime metadata framework.

Extensions may contribute new metadata, diagnostics, or log enrichment features
under the `"AMD"` namespace while maintaining a consistent schema for consumers.
