#!/usr/bin/env python3

import argparse
import json
import sys


def is_leaf(node):
    return isinstance(node, dict) and "afid" in node


def split_patterns(pattern):
    return pattern.split("|")


def is_prefix(pattern):
    return pattern.endswith("*")


def check_overlap(patterns):
    """Detect overlapping patterns (basic prefix conflict check)."""
    expanded = []
    for p in patterns:
        expanded.extend(split_patterns(p))

    for i in range(len(expanded)):
        for j in range(i + 1, len(expanded)):
            a = expanded[i]
            b = expanded[j]

            if a == "*" or b == "*":
                continue

            if is_prefix(a) and is_prefix(b):
                if a[:-1].startswith(b[:-1]) or b[:-1].startswith(a[:-1]):
                    raise ValueError(
                        f"Overlapping prefix patterns: '{a}' and '{b}'"
                    )

            elif is_prefix(a):
                if b.startswith(a[:-1]):
                    raise ValueError(f"Pattern overlap: '{a}' matches '{b}'")

            elif is_prefix(b):
                if a.startswith(b[:-1]):
                    raise ValueError(f"Pattern overlap: '{b}' matches '{a}'")

            elif a == b:
                raise ValueError(f"Duplicate exact pattern: '{a}'")


def validate_leaf(node, path):
    if not isinstance(node["afid"], int):
        raise ValueError(f"afid must be int at {path}")

    if node["afid"] < 0:
        raise ValueError(f"afid must be non-negative at {path}")

    if not isinstance(node["originOfCondition"], str):
        raise ValueError(f"originOfCondition must be string at {path}")

    origins = [o.strip() for o in node["originOfCondition"].split(",")]

    if not origins:
        raise ValueError(
            f"originOfCondition must contain at least one value at {path}"
        )

    for origin in origins:
        if not origin:
            raise ValueError(f"Empty originOfCondition entry at {path}")


def validate_node(node, path="root"):
    if is_leaf(node):
        validate_leaf(node, path)
        return

    if not isinstance(node, dict):
        raise ValueError(f"Invalid node type at {path}")

    keys = list(node.keys())

    check_overlap(keys)

    for key, child in node.items():

        if not isinstance(key, str):
            raise ValueError(f"Invalid key type at {path}: {key}")

        if "=" not in key:
            raise ValueError(
                f"Expected KEYWORD=pattern format at " f"{path}->{key}"
            )

        keyword, pattern = key.split("=", 1)

        if not keyword.strip():
            raise ValueError(f"Empty keyword at {path}->{key}")

        if not pattern.strip():
            raise ValueError(f"Empty pattern at {path}->{key}")

        validate_node(child, f"{path}->{key}")


def validate_lookup_structure(lookup):
    if not isinstance(lookup, dict):
        raise ValueError("lookup must be a dictionary")

    for message, node in lookup.items():

        if not isinstance(node, dict):
            raise ValueError(f"Invalid message node: {message}")

        if is_leaf(node):
            raise ValueError(
                f"Message '{message}' must contain "
                "at least one KEYWORD=pattern level"
            )

        validate_node(node, message)


def validate_json(data):
    """Top-level validation."""

    if "lookup" not in data:
        raise ValueError("Missing 'lookup' section")

    validate_lookup_structure(data["lookup"])

    if "fallthrough_afid" not in data:
        raise ValueError("Missing 'fallthrough_afid'")

    if not isinstance(data["fallthrough_afid"], int):
        raise ValueError("'fallthrough_afid' must be integer")


def main():
    parser = argparse.ArgumentParser(description="Validate AEL LUT JSON")
    parser.add_argument("--input", required=True, help="Path to LUT JSON file")

    args = parser.parse_args()

    try:
        with open(args.input, "r") as f:
            data = json.load(f)

        validate_json(data)

        print(f"[AEL] Validation PASSED: {args.input}")
        sys.exit(0)

    except Exception as e:
        print(f"[AEL] Validation FAILED: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()
