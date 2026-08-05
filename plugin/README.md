# phosphor-logging Plugin Framework

## Overview

The phosphor-logging plugin framework allows log entries to be extended with
plugin-specific functionality while keeping the Logging Manager independent of
plugin implementations.

The framework provides a common mechanism for:

1. Producing plugin requests
2. Generating plugin descriptors
3. Creating runtime plugin instances
4. Associating plugins with log entries

Plugin requests are represented by `PluginInfo` objects and may originate from:

- Event extension metadata (`_EXTENSIONS`)
- Platform-specific hooks
- OEM-specific hooks
- Future native event support

All plugin requests follow the same flow:

```text
PluginInfo
    │
    ▼
Descriptor
    │
    ▼
Plugin
    │
    ▼
Log Entry
```

This allows new plugin types and plugin request sources to be added without
introducing plugin-specific logic into the Logging Manager.

---

## Architecture

The framework separates plugin request processing from runtime plugin creation.

```text
Plugin Request
      │
      ▼
PluginInfo
      │
      ▼
PluginRegistry
      │
      ▼
PluginFactory::createDescriptor()
      │
      ▼
Descriptor
      │
      ▼
PluginManager
      │
      ▼
PluginFactory::create()
      │
      ▼
Runtime Plugin
      │
      ▼
Log Entry
```

---

## Core Components

### PluginInfo

`PluginInfo` represents a request to associate a plugin with a log entry.

```text
PluginInfo
{
    interface =
        "xyz.openbmc_project.Logging.Diagnostic.CPER",

    data = {
        ...
    }
}
```

`PluginInfo` objects are transient and exist only during request processing.

### Descriptor

A descriptor contains validated, plugin-specific data required to construct a
runtime plugin instance.

Descriptors:

- Encapsulate plugin creation data
- Separate request processing from runtime creation
- Allow validation before instantiation

Descriptors are transient transport objects used only during plugin creation.

### PluginFactory

Each plugin provides a factory responsible for:

1. Translating `PluginInfo` into a descriptor
2. Creating runtime plugins from descriptors

```text
PluginInfo
    │
    ▼
createDescriptor()
    │
    ▼
Descriptor
    │
    ▼
create()
    │
    ▼
Plugin
```

### PluginRegistry

The registry maintains the mapping between plugin interfaces and their
associated factories.

Responsibilities:

- Factory registration
- Factory lookup
- Descriptor generation support
- Runtime plugin creation support

The registry owns plugin factories but does not own descriptors, runtime
plugins, or log entries.

### PluginManager

The plugin manager creates runtime plugins from descriptors.

Responsibilities:

- Resolve the factory associated with a descriptor
- Invoke the selected factory
- Create runtime plugin instances

`PluginManager` is a helper for runtime plugin construction and does not retain
ownership of created plugins.

### Runtime Plugin

A plugin is the runtime representation of a log entry extension.

Plugins may:

- Expose plugin-specific D-Bus interfaces
- Maintain runtime state
- Manage plugin-specific resources

Plugins remain associated with their owning log entry for the lifetime of that
entry.

---

## Plugin Request Sources

Plugin requests can originate from multiple sources and are normalized into a
common `PluginInfo` representation before descriptor generation.

```text
                Event
                  │
    ┌─────────────┼─────────────┐
    │             │             │
    ▼             ▼             ▼

_EXTENSIONS   OEM Hook   Platform Hook

    │             │             │
    └─────────────┴─────────────┘
                  │
                  ▼
              PluginInfo
                  │
                  ▼
              Descriptor
                  │
                  ▼
                Plugin
                  │
                  ▼
               Log Entry
```

This allows new plugin request mechanisms to be added without modifying the
descriptor, plugin creation, ownership, or lifecycle portions of the framework.

---

## Ownership Model

```text
Logging Manager
 │
 ├── PluginRegistry
 │
 ├── PluginManager
 │
 └── Entry
       │
       └── Plugin(s)
```

Ownership rules:

- Logging Manager owns `PluginRegistry`
- Logging Manager owns `PluginManager`
- PluginRegistry owns registered factories
- Entry owns runtime plugin instances
- Descriptor objects are transient and discarded after plugin creation

The framework intentionally separates:

```text
PluginRegistry  -> Factory registration and lookup
PluginManager   -> Plugin construction
Plugin          -> Runtime functionality
Entry           -> Runtime ownership
```

---

## Event Extensions

Plugins may be requested through event extension metadata.

Example:

```json
{
  "_EXTENSIONS": {
    "xyz.openbmc_project.Logging.Diagnostic.CPER": {
      "NotificationType": "8f87f311-c998-4d9e-a0c4-fb4dfc2c8c14",
      "SectionType": "dc3ea0b0-a144-4797-b95b-53fa242b6e1d",
      "Oem": {
        "AMD": {
          "CperSection": {
            "AID": "0x1234",
            "Socket": 0,
            "Die": 1
          }
        }
      }
    }
  }
}
```

The `_EXTENSIONS` property acts as a transport mechanism for plugin-specific
metadata.

During log creation, supported extensions are translated into one or more
`PluginInfo` requests and subsequently processed through the standard descriptor
and runtime plugin creation flow.

```text
_EXTENSIONS
      │
      ▼
PluginInfo
      │
      ▼
Descriptor
      │
      ▼
Plugin
```

The Logging Manager remains independent of extension-specific schemas and
delegates interpretation of extension metadata to the corresponding plugin
implementation.

Unsupported extensions are ignored.

---

## Hook-Based Plugin Requests

In addition to `_EXTENSIONS` metadata, platforms may register hooks that inspect
event data and contribute additional plugin
