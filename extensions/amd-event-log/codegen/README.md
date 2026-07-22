# AMD AEL Code Generation

## Overview

This directory contains scripts used to generate the AMD Event Log (AEL) reverse
lookup table (LUT) and associated unit tests at build time.

The code generation process converts JSON-based LUT definitions into C++ source
artifacts used by the AEL decoder.

---

## Components

| File                    | Description                              |
| ----------------------- | ---------------------------------------- |
| `generate_lut.py`       | Generates the LUT header from JSON input |
| `generate_lut_tests.py` | Generates unit tests from JSON input     |

---

## Reverse LUT Configuration

The AEL reverse lookup table is defined in:

data/reverse_lut.json

This file maps Redfish event information to AMD-specific identifiers such as
AFID and `originOfCondition`.

---

### Reverse LUT Platform Customization

Platform-specific LUT content can be provided through a Yocto layer by
overriding the default `reverse_lut.json` during the build without requiring
changes to the core implementation.

---

## Build-Time Generation

The LUT is generated during the Meson build:

reverse_lut.json (data/) ↓ generate_lut.py ↓ amd_ael_lut_gen.hpp (build dir)

The generated header is consumed by the runtime decoder.

---

## Test Generation

Unit tests are generated from the same JSON:

reverse_lut.json ↓ generate_lut_tests.py ↓ test_ael_lut_generated.cpp

This ensures consistency between runtime behavior and test coverage.

---

## Validation

Input JSON is expected to follow a strict hierarchical format:

registry → messageId → arg1 → ... → {afid, originOfCondition}

Invalid structures or conflicting patterns may result in build-time failures.

---

## Usage

These scripts are invoked automatically through Meson `custom_target`
definitions and are not intended to be run manually during normal builds.

For standalone validation or debugging, the scripts can be executed directly:

```bash
python3 generate_lut.py --input <json> --output <header>
python3 generate_lut_tests.py --input <json> --output <test>
```
