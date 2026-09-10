#!/usr/bin/env python3

import argparse
import json
import sys


def is_leaf(node):
    return isinstance(node, dict) and not any(
        isinstance(v, dict) for v in node.values()
    )


def split_patterns(pattern):
    return pattern.split("|")


def match_pattern(value, pattern):
    if pattern == "*":
        return True

    idx = pattern.find("*")

    if idx == -1:
        return value == pattern

    prefix = pattern[:idx]
    suffix = pattern[idx + 1 :]

    if len(value) < len(prefix) + len(suffix):
        return False

    return value.startswith(prefix) and value.endswith(suffix)


def patterns_overlap(a, b):

    if a == b:
        return True

    if "*" not in a and "*" not in b:
        return False

    if "*" in a and match_pattern(b, a):
        return True

    if "*" in b and match_pattern(a, b):
        return True

    return False


def check_overlap(patterns):

    expanded = []

    for p in patterns:
        expanded.extend(split_patterns(p))

    for i in range(len(expanded)):
        for j in range(i + 1, len(expanded)):

            a = expanded[i]
            b = expanded[j]

            if patterns_overlap(a, b):
                raise ValueError(f"Overlapping patterns: '{a}' and '{b}'")


def validate_pattern(pattern, path):
    count = pattern.count("*")

    if count > 1:
        raise ValueError(
            f"Pattern '{pattern}' at {path} contains {count} wildcards. "
            f"Only one '*' per pattern token is supported."
        )


def validate_leaf(node, path):
    # Validate afid presence
    if "afid" not in node:
        raise ValueError(f"Missing 'afid' at {path}")

    # Validate AFID type
    if not isinstance(node["afid"], int):
        raise ValueError(f"afid must be int at {path}")

    if node["afid"] < 0:
        raise ValueError(f"afid must be non-negative at {path}")

    # Validate originOfCondition presence
    if "originOfCondition" not in node:
        raise ValueError(f"Missing 'originOfCondition' at {path}")

    # Validate originOfCondition type
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

    if len(origins) != len(set(origins)):
        raise ValueError(
            f"Duplicate originOfCondition entries at {path}: {origins}"
        )


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
                f"Expected KEYWORD=pattern format at {path}->{key}"
            )

        keyword, raw_pattern = key.split("=", 1)

        if not keyword.strip():
            raise ValueError(f"Empty keyword at {path}->{key}")

        if not raw_pattern.strip():
            raise ValueError(f"Empty pattern at {path}->{key}")

        for token in split_patterns(raw_pattern):
            if not token:
                raise ValueError(
                    f"Empty pattern token in key '{key}' at {path}"
                )
            validate_pattern(token, f"{path}->{key}")

        validate_node(child, f"{path}->{key}")


def validate_lookup_structure(lookup):
    if not lookup:
        raise ValueError(
            "Invalid LUT: lookup must contain at least one message entry"
        )

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
