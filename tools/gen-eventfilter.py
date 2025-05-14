#!/usr/bin/env python3

import argparse
import json
import os
import sys

import jsonschema
from mako.template import Template

import yaml

# Determine the script's directory to find the schema file relative to it.
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
SCHEMA_FILE = os.path.join(
    SCRIPT_DIR, "phosphor-logging", "schemas", "eventfilter.schema.yaml"
)
TEMPLATE_FILE = os.path.join(
    SCRIPT_DIR, "phosphor-logging", "templates", "event-filter.mako.cpp"
)


def main() -> int:
    """
    Validates a JSON filter file against the eventfilter schema.
    """
    parser = argparse.ArgumentParser(
        description="Validate an event filter JSON file against the schema."
    )
    parser.add_argument(
        "filter",
        type=str,
        help="Path to the JSON filter file to validate.",
    )
    args = parser.parse_args()

    with open(args.filter, "r") as f:
        filter_data = json.load(f)

    with open(SCHEMA_FILE, "r") as f:
        schema_data = yaml.safe_load(f)

    jsonschema.validate(instance=filter_data, schema=schema_data)

    template = Template(filename=TEMPLATE_FILE)
    output = template.render(data=filter_data)
    print(output)

    return 0


if __name__ == "__main__":
    sys.exit(main())
