# phosphor-logging Plugin Framework

## Overview

The phosphor-logging plugin framework enables log entries to be extended with
plugin-specific functionality while keeping the logging manager independent of
plugin implementation details.

The framework introduces a common mechanism for:

- Requesting plugins from event metadata
- Generating plugin descriptors
- Creating runtime plugin instances
- Associating plugins with log entries

This design allows new plugin types to be added without requiring
plugin-specific logic in the logging manager.

---

## Architecture

A plugin progresses through the following lifecycle:

```text
Producer
    │
    ▼
Event Extension Metadata
    │
    ▼
PluginInfo
    │
    ▼
PluginFactory
    │
    ▼
Descriptor
    │
    ▼
Log Entry Creation
    │
    ▼
Runtime Plugin
```

The framework separates plugin request processing from runtime plugin creation.

---

## Core Concepts

### PluginInfo

`PluginInfo` represents a plugin creation request.

A plugin request contains:

- Plugin type
- Plugin-specific metadata

Example:

```text
PluginInfo
{
    type = cper,
    data = {
        ...
    }
}
```

Plugin requests are generated from event extension metadata and consumed by
plugin factories.

---

### Descriptor

A descriptor contains the information required to create a runtime plugin
instance.

Descriptors are immutable runtime inputs generated from `PluginInfo` objects.

Benefits:

- Encapsulates plugin configuration
- Decouples request processing from runtime creation
- Allows validation before plugin instantiation

---

### PluginFactory

Each plugin type provides a factory implementation.

Factories are responsible for:

1. Translating `PluginInfo` into a descriptor.
2. Creating runtime plugin instances from descriptors.

Example flow:

```text
PluginInfo
    │
    ▼
Factory::createDescriptor()
    │
    ▼
Descriptor
    │
    ▼
Factory::create()
    │
    ▼
Runtime Plugin
```

---

### PluginRegistry

The plugin registry owns registered plugin factories.

Responsibilities:

- Plugin factory registration
- Plugin type lookup
- Descriptor generation
- Runtime plugin creation

The logging manager interacts with plugins through the registry instead of
directly referencing plugin implementations.

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
        "Source": "/xyz/openbmc_project/inventory/system"
      }
    }
  }
}
```

During log creation:

```text
Event Extension Metadata
    │
    ▼
PluginInfo
    │
    ▼
Descriptor Generation
    │
    ▼
Runtime Plugin Creation
```

Unsupported extensions are ignored.

---

## Descriptor Generation

Descriptor generation occurs before log entry creation.

```text
PluginInfo
    │
    ▼
PluginRegistry
    │
    ▼
PluginFactory::createDescriptor()
    │
    ▼
DescriptorList
```

The logging manager uses generated descriptors when constructing runtime plugins
for a log entry.

---

## Runtime Plugin Creation

Runtime plugin creation occurs during log entry creation.

```text
Descriptor
    │
    ▼
PluginFactory::create()
    │
    ▼
Plugin Instance
```

Plugins are associated with the owning log entry and remain available for the
lifetime of that entry.

---
