# Event Extensions Framework

The Event Extensions framework allows log entries to be augmented with
additional D-Bus interfaces.

## Class Overview

### Request

Represents an extension requested by an event.

Contains the extension interface name and extension-specific metadata.

### Registry

Maintains the mapping between extension interface names and extension creation
callbacks.

### Manager

Framework entry point owned by LogManager.

Responsible for resolving requests and creating runtime extension instances.

### Extension

Base class for runtime event extensions.

Extension instances are owned by Entry.

## Ownership Model

```text
+------------+          +-------+
| LogManager |          | Entry |
+------------+          +-------+
       |                    |
       | owns               | owns
       v                    v
+------------+      +---------------+
|  Manager   |      |   Extensions  |
+------------+      +---------------+
       |
       | owns
       v
+------------+
|  Registry  |
+------------+
```
