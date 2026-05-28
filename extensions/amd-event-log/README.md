# OEM Logging Extension Framework

## Overview

This extension provides a generic framework for attaching vendor-specific (OEM)
metadata to OpenBMC log entries.

It builds on top of the `Entry.Oem` property and enables extensions to
contribute structured metadata without modifying core logging interfaces or
workflows.

---

## Design Goals

- Enable vendor-specific metadata in a structured way
- Avoid changes to core logging APIs and data models
- Provide a pluggable extension mechanism
- Maintain separation of concerns between core and OEM logic
- Ensure robustness of the logging pipeline

---

## Architecture

### Entry.Oem Property

OEM metadata is stored in the `Entry.Oem` property using a vendor-scoped
structure. This allows multiple vendors to independently attach metadata to the
same log entry.

```text
"<Vendor>" -> { "<Key>": <Value> }
```

### Example

```json
{
  "VendorA": {
    "Key1": "Value1"
  },
  "VendorB": {
    "Key2": 123
  }
}
```

### Current Implementation Note

The current extension provides an AMD-specific provider, which populates the
`"Amd"` section:

```json
{
  "Amd": {
    "Key": "Value"
  }
}
```

---

## Extension Model

The framework uses a **provider-based model** where extensions register callback
functions that are invoked during log creation.

### Key Concepts

- Providers are registered during startup
- Each provider receives log context
- Providers populate OEM metadata
- Results are aggregated into a single container
- The logging service attaches the final data to `Entry.Oem`

---

## Data Flow

At a high level, OEM data is populated as part of log creation:

- Logging framework invokes registered OEM providers
- Providers contribute vendor-specific metadata
- Data is aggregated and attached to `Entry.Oem`

---

## Implementation Notes

- Providers should only populate the OEM container
- Providers must not modify core log entry fields
- The logging framework owns the lifecycle of OEM data
- Provider invocation must not impact logging reliability

---

## Error Handling

- Provider failures must not affect log creation
- Exceptions should be handled internally
- Logging of failures should be minimal and non-disruptive

---

## AEL Metadata (Future Direction)

The AEL (AMD Event Log) namespace represents a structured metadata model that
can be built on top of this framework.

### Example Fields

- `AEL.VERSION`
- `AEL.TYPE`
- `AEL.AFID`
- `AEL.FRU_LIST`
- `AEL.SEVERITY_OVERRIDE`

### Example Schema (Illustrative)

```json
{
  "type": "object",
  "required": ["AEL.TYPE"],
  "properties": {
    "AEL.VERSION": {
      "type": "string",
      "description": "Schema version of metadata (e.g., 1.0)"
    },
    "AEL.TYPE": {
      "type": "string",
      "description": "Event classification (e.g., Event, Error)"
    },
    "AEL.AFID": {
      "type": "string",
      "description": "Application Fault ID (optional)"
    },
    "AEL.FRU_LIST": {
      "type": "string",
      "description": "Associated FRU inventory paths"
    },
    "AEL.SEVERITY_OVERRIDE": {
      "type": "string",
      "description": "Optional severity override"
    }
  }
}
```
