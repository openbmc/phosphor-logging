#!/bin/sh
/usr/bin/env python3 extensions/openpower-pels/registry/tools/validate_registry.py \
    -s extensions/openpower-pels/registry/schema/schema.json \
    -r extensions/openpower-pels/registry/message_registry.json
