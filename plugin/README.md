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

Plugin requests are represented by `plugin::Request` objects and may originate
from:

- Event extension metadata (`_EXTENSIONS`)
- Platform-specific request providers
- OEM-specific request providers
- Plugin request extension callbacks
- Future native event support

All plugin requests follow the same flow:

```text
plugin::Request
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
plugin::Request
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

### plugin::Request

`plugin::Request` represents a request to associate a plugin with a log entry.

```cpp
plugin::Request
{
    interface =
        "xyz.openbmc_project.Logging.Extension.CPER.Processed",

    data = {
        ...
    }
};
```

A request contains:

- Plugin interface identifier
- Plugin-specific metadata payload

Requests are transient and exist only during plugin processing.

---

### Descriptor

A descriptor contains validated, schema-backed data required to construct a
runtime plugin instance.

Descriptors:

- Encapsulate plugin creation data
- Separate request translation from runtime creation
- Allow validation before instantiation
- Provide a typed representation of extension metadata

Descriptors are transient transport objects used only during plugin creation.

---

### PluginFactory

Each plugin provides a factory responsible for:

1. Translating `plugin::Request` into a descriptor
2. Creating runtime plugins from descriptors

```text
plugin::Request
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

---

### PluginRegistry

The registry maintains the mapping between plugin interfaces and their
associated factories.

Responsibilities:

- Factory registration
- Factory lookup
- Descriptor generation support
- Runtime plugin creation support

A plugin interface is considered supported when a corresponding factory is
registered with the registry.

The registry owns plugin factories but does not own descriptors, runtime
plugins, or log entries.

---

### PluginManager

The plugin manager coordinates descriptor generation and runtime plugin
creation.

Responsibilities:

- Translate requests into descriptors
- Resolve factories associated with descriptors
- Create runtime plugin instances
- Register built-in plugin implementations

`PluginManager` is a helper for runtime plugin construction and does not retain
ownership of created plugins.

---

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
common `plugin::Request` representation before descriptor generation.

```text
                Event
                  │
    ┌─────────────┼─────────────┐
    │             │             │
    ▼             ▼             ▼

_EXTENSIONS   OEM Hook   Platform Hook

                  │
                  ▼
      Plugin Request Extensions
                  │
                  ▼
            plugin::Request
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

Request extension callbacks may augment plugin metadata before descriptor
generation. This allows platform and OEM code to contribute plugin-specific
information without modifying plugin creation or lifecycle management flows.

This allows new plugin request mechanisms to be added without modifying the
descriptor, plugin creation, ownership, or lifecycle portions of the framework.

---

## Ownership Model

```text
Logging Manager
       │
       ▼
  PluginService
       │
       ├── PluginRegistry
       │
       └── PluginManager

Entry
  │
  └── Plugin(s)
```

Ownership rules:

- Logging Manager owns `PluginService`
- PluginService owns `PluginRegistry`
- PluginService owns `PluginManager`
- PluginRegistry owns registered factories
- Entry owns runtime plugin instances
- Descriptor objects are transient and discarded after plugin creation

The framework intentionally separates:

```text
PluginRegistry  -> Factory registration and lookup
PluginManager   -> Descriptor and plugin creation
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
    "xyz.openbmc_project.Logging.Extension.CPERProcessed": {
      "DiagnosticDataType": "xyz.openbmc_project.Logging.CPER.Types.ContentType.CPER",
      "NotificationType": "8f87f311-c998-4d9e-a0c4-fb4dfc2c8c14",
      "SectionType": "dc3ea0b0-a144-4797-b95b-53fa242b6e1d",
      "Oem": {
        "AMD": {
          "Processor": "0"
        }
      }
    },

    "xyz.openbmc_project.Logging.Extension.CPER.Raw": {
      "Data": [1, 2, 3, 4]
    }
  }
}
```

The `_EXTENSIONS` property acts as a transport mechanism for plugin-specific
metadata.

During log creation, each supported extension is translated into a
`plugin::Request` and subsequently processed through the standard descriptor and
runtime plugin creation flow.

```text
_EXTENSIONS
      │
      ▼
plugin::Request
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

## Extension Interface Model

Extension interfaces provide the schema used to transport plugin metadata
through events.

Each extension interface becomes an independent `plugin::Request` during log
creation.

For example:

```text
xyz.openbmc_project.Logging.Extension.CPER.Processed
        │
        ▼
ProcessedDescriptor
        │
        ▼
ProcessedPlugin

xyz.openbmc_project.Logging.Extension.CPER.Raw
        │
        ▼
RawDescriptor
        │
        ▼
   RawPlugin
```

This model allows multiple extensions to be associated with a single log entry
while keeping plugin implementations independent from each other.

---

## Plugin Request Extension Hooks

Plugin request extension callbacks provide a generic mechanism for augmenting
plugin requests prior to descriptor creation.

```text
_EXTENSIONS
      │
      ▼
plugin::Request
      │
      ▼
Plugin Request Extension Hooks
      │
      ▼
Descriptor
      │
      ▼
   Plugin
```

Request extension callbacks may:

- Add implementation-specific metadata
- Inject platform-specific information
- Inject OEM-specific information
- Enrich requests using external context

Request extension callbacks should not modify plugin-defined schema properties
and should only contribute additional metadata required by the extension
provider.

This allows new metadata providers to integrate with the framework without
modifying plugin factories, runtime plugins, or Logging Manager flows.
