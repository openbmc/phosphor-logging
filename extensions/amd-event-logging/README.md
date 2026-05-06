# AMD Event Logging (AEL)

## What is AEL?

AMD Event Logging (AEL) is a lightweight extension on top of OpenBMC logging
that converts standard BMC log entries into structured **AMD‑specific events**.

AEL acts as a **translator and correlator**:

- OpenBMC logs → explain _what happened_
- Metadata services → provide _diagnostic details_
- AEL → combines both into **AMD events**

AEL does not modify the existing logging pipeline. It listens to logs,
correlates them with metadata, and publishes structured D‑Bus objects for
consumers such as Redfish.

---

## Key Idea

**Logs stay minimal. Metadata stays external. AEL correlates them.**

---

## Where This Fits

AEL is implemented by the `amd-log-manager` service and integrates with:

- phosphor-logging (log source)
- DBus metadata providers (CPER, RAS, telemetry)
- bmcweb (Redfish exposure)

---

## How It Works

```text
BMC component logs event
    ↓
AEL observes log
    ↓
Filter → AMD Error Type
    ↓
Registry → metadata requirements
    ↓
DBus lookup using correlation keys
    ↓
AMD Event created
```

---

## Producer Requirements

Use standard phosphor-logging APIs and optionally include correlation keys.

```yaml
AdditionalData:
  ERROR_CLASS: MCA
  SOCKET: 0
  CORRELATION_ID: ras-evt-7821
```

---

## AEL Configuration Model

### 1. Filters (Trigger)

Maps BMC log → AMD error type

```json
{
  "BmcEvent": "xyz.openbmc_project.Hw.IO.LinkError",
  "AMDErrorType": "AMD.IO.LinkFailure"
}
```

**Filters are only for classification.**

---

### 2. AMD Error Registry (Core Logic)

Defines what an AMD error means

- AFIDs (diagnosed conditions)
- FRU classes (not instances)
- Metadata requirements

```json
{
  "AMD.IO.LinkFailure": {
    "AFIDResolution": { "Mode": "Static" },
    "MetadataContract": {
      "Sources": [
        { "Type": "ExtendedData", "Keys": ["LINK_ID"] },
        { "Type": "Service", "Provider": "xyz.openbmc_project.AMD.IO" }
      ]
    }
  }
}
```

### How AEL Uses This

1. Filter selects AMD Error Type
2. Registry defines:
   - AFIDs to generate
   - Metadata sources
3. AEL fetches metadata and constructs event

---

## Correlation Model (CPER Example)

```text
Log AdditionalData:
  CORRELATION_ID = ras-evt-7821

Metadata (from CPER service):
  CorrelationId = "ras-evt-7821"
  ... (CPER-derived fields)
```

**AEL workflow:**

- Uses the **AMD Error Registry** to determine the metadata provider
- Extracts `CORRELATION_ID` from the log entry
- Retrieves metadata using a **provider-defined fast lookup mechanism**
- Builds the corresponding AMD Event

---

### Metadata Lookup

- `CorrelationId` is a **logical key** used to link logs and metadata
- Metadata providers expose a **fast lookup** (e.g. in-memory or indexed store)
- AEL performs **direct lookup using CorrelationId**

---

### Summary

- Logs carry a lightweight identifier (`CORRELATION_ID`)
- Metadata remains external and provider-owned
- AEL performs **efficient, direct correlation** using the identifier

---

### How AEL Finds the Metadata Service

AEL relies on the **AMD Error Registry**, which defines the metadata provider:

```json
{
  "Type": "Service",
  "Provider": "xyz.openbmc_project.AMD.CPER"
}
```

This ensures:

- Explicit ownership of metadata
- No hardcoded service knowledge in AEL
- Deterministic lookup behavior

---

> **Correlation is based on identity (ID), not location (path).**

## What Gets Created

```text
/com/amd/event/<event-id>
 └── logging/
     ├── afids/
     └── sections/
```

- **Event** → incident boundary
- **AFIDs** → diagnosed conditions
- **Sections** → processed metadata views

Raw diagnostics are not stored.

---

## Extending AEL

1. Add filter mapping
2. Update registry
3. Provide metadata via DBus

No core AEL file changes required.

---

## DBus Contract Reference

AEL relies on metadata providers exposing structured DBus objects.

For detailed DBus definitions and schema expectations, refer to:

👉 Platform Design Interface (PDI) documentation for AMD-specific DBus
interfaces

---

## Design Rules

- Keep logs minimal
- Use correlation keys only
- Do not attach large payloads to logs
- Expose metadata via DBus services

---

## Summary

AEL is a **correlation system** that connects logs and metadata to produce rich
AMD events.
