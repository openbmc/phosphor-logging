# AMD Event Log Extension (AEL)

## Overview

The AMD Event Log (AEL) is a phosphor-logging extension that enables structured
AMD-specific metadata to be attached to OpenBMC log entries.

It builds on top of the existing `xyz.openbmc_project.Logging.Entry.Oem`
interface and provides a consistent and extensible way to describe events
without modifying base logging interfaces.

---

## What AEL Does

AEL allows attaching structured OEM metadata to existing OpenBMC log entries.

It enables:

- Adding AMD-specific information without changing base interfaces
- Supporting both native and forwarded events
- Including structured payloads (inline or external)
- Providing context so consumers can interpret events correctly

---

## How It Works

AEL integrates with phosphor-logging using the OEM extension interface:

- Uses the same DBus object path as the base log entry
- Attaches:
  - `xyz.openbmc_project.Logging.Entry.Oem`
- Stores metadata in:
  - `OemAdditionalData (dict<string, string>)`
- Uses a namespaced key format:
  - `AEL.*`

---

## AEL Metadata Structure

### Required Fields

- `AEL.VERSION`
- `AEL.TYPE`

---

### Event Types

- `Event`
- `Error`
- `Diagnostic`

---

### Full Schema Definition

```json
{
  "type": "object",
  "required": ["AEL.TYPE"],
  "properties": {
    "AEL.VERSION": {
      "type": "string",
      "description": "Schema version of AEL metadata (e.g., 1.0)"
    },
    "AEL.NAMESPACE": {
      "type": "string",
      "description": "Vendor namespace (e.g., AMD). Used for multi-vendor support"
    },
    "AEL.TYPE": {
      "type": "string",
      "enum": ["Event", "Error", "Diagnostic"],
      "description": "Semantic classification of the event"
    },
    "AEL.SUBTYPE": {
      "type": "string",
      "enum": ["None", "CPER", "RedfishEvent", "Vendor", "Opaque"],
      "description": "Defines how payload is interpreted or decoded"
    },
    "AEL.ORIGIN.TYPE": {
      "type": "string",
      "enum": ["Native", "Forwarded"],
      "description": "Indicates whether the event originated locally (Native) or was forwarded from another component"
    },
    "AEL.ORIGIN.COMPONENT": {
      "type": "string",
      "description": "Optional identifier of the originating component (e.g., AMC, CPU, GPU, NIC)"
    },
    "AEL.ORIGIN.IDENTIFIER": {
      "type": "string",
      "description": "Optional instance identifier for the component (e.g., GPU0, AMC1)"
    },
    "AEL.AFID": {
      "type": "string",
      "description": "Application Fault ID (optional)"
    },
    "AEL.FRU_LIST": {
      "type": "string",
      "description": "Comma-separated list of FRU inventory paths associated with the event"
    },
    "AEL.DATA.FORMAT": {
      "type": "string",
      "enum": ["Inline", "URI"],
      "description": "Payload representation"
    },
    "AEL.DATA.INLINE": {
      "type": "string",
      "description": "JSON encoded structured payload"
    },
    "AEL.DATA.URI": {
      "type": "string",
      "description": "URI pointing to external detailed data"
    },
    "AEL.SEVERITY_OVERRIDE": {
      "type": "string",
      "description": "Optional override of the base log severity"
    },
    "AEL.CORRELATION_ID": {
      "type": "string",
      "description": "Identifier used to correlate related events"
    }
  }
}
```

---

## AEL.ORIGIN Definition

The origin of an event is represented using flattened keys under the
`AEL.ORIGIN.*` namespace.

### Fields

- `AEL.ORIGIN.TYPE` (Required)
- `AEL.ORIGIN.COMPONENT` (Optional)
- `AEL.ORIGIN.IDENTIFIER` (Optional)

### Example

```text
AEL.ORIGIN.TYPE=Native
```

```text
AEL.ORIGIN.TYPE=Forwarded
AEL.ORIGIN.COMPONENT=AMC
```

```text
AEL.ORIGIN.TYPE=Forwarded
AEL.ORIGIN.COMPONENT=GPU
AEL.ORIGIN.IDENTIFIER=GPU0
```

---

## Decoding Behavior (SUBTYPE)

| SUBTYPE      | Description                       |
| ------------ | --------------------------------- |
| None         | Standard event                    |
| CPER         | Decode using CPER schema          |
| RedfishEvent | Already structured (pass-through) |
| Vendor       | Vendor-specific payload           |
| Opaque       | Store only                        |

---

## Flow

1. Create base log entry
2. Build AEL metadata (map<string, string>)
3. Validate schema
4. Attach Entry.Oem interface if metadata exists
5. Set OemAdditionalData

---

## Mandatory Field Rules

### Event / Error

- Required: `AEL.TYPE`
- Recommended: `AEL.AFID`
- Optional: `AEL.FRU_LIST`, `AEL.DATA.*`

### Diagnostic

- Required: `AEL.TYPE`, `AEL.SUBTYPE`

If structured payload present:

- `AEL.DATA.FORMAT`
- `AEL.DATA.URI` or `AEL.DATA.INLINE`

---

## Consumer Guidance (IMPORTANT)

- Consumers MUST use `SUBTYPE` to determine decoding logic
- Consumers MUST NOT assume presence of `DATA`
- `RedfishEvent` MUST NOT be reinterpreted
- `Vendor` requires component-specific parsing logic
- `Opaque` is not intended for parsing
- `DATA.FORMAT` must be checked before access
- `URI` payload is authoritative when present
- `ORIGIN` is for tracing only

---

## Key Rules

- Do not modify base DBus interface
- Attach OEM interface only when needed
- Use `AEL.*` prefix for metadata keys
- `SUBTYPE` defines decoding behavior
- `ORIGIN` should be portable and simple
- `DATA` supports inline and external payloads

---
