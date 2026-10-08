#!/usr/bin/env python3
"""Verify that misuse of the lg2 API fails to compile with the expected
diagnostic.

Compiles lg2_compile_fail.cpp once per CASE_* macro, reusing the compiler
command that meson recorded in compile_commands.json for test/lg2_test.cpp so
that all include paths and flags match a real lg2 user.

Usage: lg2_compile_fail.py <build-dir> <path-to-lg2_compile_fail.cpp>
"""

import json
import os
import shlex
import subprocess
import sys

# Map of CASE_* macro suffix to text that must appear in the compiler output.
# Header-name validation reports errors by calling the non-constexpr
# header_str::report_error() from a consteval constructor, so the diagnostic
# text is compiler-specific; both GCC and Clang mention report_error.
CASES = {
    "TRAILING_HEADER": "Found header field without expected data.",
    "TRAILING_HEADER_FLAG": "Found header field without expected data.",
    "FLAG_BEFORE_HEADER": "Found value without expected header field.",
    "MISALIGNED_PAIRS": "Found value without expected header field.",
    "LOWERCASE_HEADER": "report_error",
    "RESERVED_HEADER": "report_error",
    "PROHIBITED_FLAG": "Prohibited flag found for value type.",
}

REFERENCE_SOURCE = "lg2_test.cpp"


def base_command(build_dir):
    """Return (directory, args) for compiling a file like lg2_test.cpp,
    without the output, dependency-file, and source-file arguments."""
    with open(os.path.join(build_dir, "compile_commands.json")) as f:
        entries = json.load(f)

    for entry in entries:
        if os.path.basename(entry["file"]) == REFERENCE_SOURCE:
            break
    else:
        sys.exit(f"{REFERENCE_SOURCE} not found in compile_commands.json")

    if "arguments" in entry:
        raw = entry["arguments"]
    else:
        raw = shlex.split(entry["command"])

    args = []
    skip_next = False
    for arg in raw:
        if skip_next:
            skip_next = False
        elif arg in ("-o", "-MF", "-MQ", "-MT"):
            skip_next = True
        elif arg in ("-c", "-MD", "-MMD") or arg.startswith(
            "-fdiagnostics-color"
        ):
            continue
        elif os.path.basename(arg) == REFERENCE_SOURCE:
            continue
        else:
            args.append(arg)

    return entry["directory"], args + ["-fdiagnostics-color=never"]


def main():
    if len(sys.argv) != 3:
        sys.exit(f"usage: {sys.argv[0]} <build-dir> <source>")

    build_dir = sys.argv[1]
    source = os.path.abspath(sys.argv[2])
    directory, args = base_command(build_dir)

    def compile_source(*defines):
        return subprocess.run(
            args + ["-fsyntax-only", *defines, source],
            cwd=directory,
            capture_output=True,
            text=True,
        )

    failures = 0

    result = compile_source()
    if result.returncode != 0:
        print("FAIL control case: valid code did not compile")
        print(result.stderr)
        failures += 1
    else:
        print("PASS control case")

    for name, expected in CASES.items():
        result = compile_source(f"-DCASE_{name}")
        if result.returncode == 0:
            print(f"FAIL {name}: compiled successfully")
            failures += 1
        elif expected not in result.stderr:
            print(f"FAIL {name}: diagnostic does not contain '{expected}'")
            print(result.stderr[-4000:])
            failures += 1
        else:
            print(f"PASS {name}")

    sys.exit(1 if failures else 0)


if __name__ == "__main__":
    main()
