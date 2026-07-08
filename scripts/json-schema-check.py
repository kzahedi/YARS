#!/usr/bin/env python3
"""Validate YARS JSON configs against schema/yars-config.schema.json.

Full configs (root key "yars") validate against the config schema;
robot fragments under xml/robots/ validate against the robot
definition. Requires the `jsonschema` package (pip install jsonschema).

Usage: scripts/json-schema-check.py [file.json ...]
       (default: every .json under xml/)
"""

import glob
import json
import os
import sys

try:
    from jsonschema import Draft7Validator
except ImportError:
    print("jsonschema package missing: pip install jsonschema", file=sys.stderr)
    sys.exit(2)

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def main(paths):
    schema = json.load(
        open(os.path.join(ROOT, "schema", "yars-config.schema.json"))
    )
    config_validator = Draft7Validator(schema)
    fragment_schema = {
        "definitions": schema["definitions"],
        "$ref": "#/definitions/robot",
    }
    fragment_validator = Draft7Validator(fragment_schema)

    if not paths:
        paths = sorted(glob.glob(os.path.join(ROOT, "xml", "**", "*.json"),
                                 recursive=True))
    status = 0
    for path in paths:
        document = json.load(open(path))
        validator = config_validator if "yars" in document else fragment_validator
        errors = list(validator.iter_errors(document))
        if errors:
            status = 1
            location = "/".join(str(p) for p in errors[0].absolute_path)
            print(f"FAIL {path}: {len(errors)} error(s); first at "
                  f"'{location}': {errors[0].message[:160]}")
        else:
            print(f"OK   {path}")
    return status


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
