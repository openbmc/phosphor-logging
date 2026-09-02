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

### Adding a New Event Extension

Existing event extensions can be found under the `event_extensions/` directory.
The CPER Processed extension (`event_extensions/cper/`) provides a complete
example.

To add a new event extension:

1. Implement an `Extension`.
2. Implement `create()`, `restore()`, and `serialize()`.
3. Register the extension with the event extension registry.
4. Add the registration to `event_extensions::registerBuiltins()`.

During log creation, the registered `create()` callback is used to instantiate
the extension. During log restoration, the registered `restore()` callback
reconstructs the extension from its serialized state.
