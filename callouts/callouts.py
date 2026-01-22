#!/usr/bin/env python3

import argparse
import os

from mako.template import Template

import yaml

script_dir = os.path.dirname(os.path.realpath(__file__))


def main():
    parser = argparse.ArgumentParser(description="Callout code generator")

    parser.add_argument(
        "-i",
        "--callouts_yaml",
        dest="callouts_yaml",
        default=os.path.join(script_dir, "callouts-example.yaml"),
        help="input callouts yaml",
    )
    parser.add_argument(
        "-o",
        "--output",
        dest="output",
        default="callouts-gen.hpp",
        help="output file name (default: callouts-gen.hpp)",
    )

    args = parser.parse_args()

    with open(args.callouts_yaml, "r") as fd:
        calloutsMap = yaml.safe_load(fd)

        # Render the mako template
        template = os.path.join(script_dir, "callouts-gen.hpp.mako")
        t = Template(filename=template)
        with open(args.output, "w") as fd:
            fd.write(t.render(calloutsMap=calloutsMap))


if __name__ == "__main__":
    main()
