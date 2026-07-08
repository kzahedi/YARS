#!/usr/bin/env python3
"""Canonicalize a YARS JSON config to the concise element shape.

The historical XmlToJson converter emitted every child element as an
array ("always-arrays" shape), even when only one child is present:

    "lookAt": [{"x": "0.0", "y": "0.01", "z": "0.0"}]

JsonParser now also accepts a plain object for a single child element:

    "lookAt": {"x": "0.0", "y": "0.01", "z": "0.0"}

This script rewrites configs into that concise form: every array of
length 1 whose sole entry is an object is unwrapped to the object,
recursively. Arrays with more than one entry (true collections such as
repeated sensors, hinges, robots, ...) are left as arrays. "#children"
arrays are never unwrapped: they are ordered, heterogeneous child lists
and the parser requires them to stay arrays.

Attribute values are also converted from the all-strings shape XmlToJson
emitted to native JSON types: "true"/"false" become booleans and numeric
strings become numbers. JsonParser::toAttributeString stringifies these
back before the atoi/atof/"true"-compare accessors run, so the simulation
sees identical values. Keys whose values are identifiers or hex colours
(a colour like "000000" would otherwise silently become the number 0)
are never converted; see STRING_KEYS.

Usage: scripts/json-canonicalize.py <file.json> [more.json ...]
       (rewrites each file in place; exits non-zero on parse errors)
"""

import json
import sys

# Attribute keys whose values are text/identifiers even when they happen
# to look numeric or boolean (e.g. colour "000000", a robot named "true").
STRING_KEYS = {
    "#tag",
    "cfg",
    "colour",
    "filename",
    "font",
    "mode",
    "module",
    "name",
    "project",
    "target",
    "texture",
    "type",
    "version",
}


def typed(key, value):
    if key in STRING_KEYS or not isinstance(value, str):
        return value
    if value == "true":
        return True
    if value == "false":
        return False
    try:
        return int(value)
    except ValueError:
        pass
    try:
        return float(value)
    except ValueError:
        pass
    return value


def canonicalize(node):
    if isinstance(node, dict):
        out = {}
        for key, value in node.items():
            if (
                isinstance(value, list)
                and len(value) == 1
                and isinstance(value[0], dict)
                and key != "#children"
            ):
                out[key] = canonicalize(value[0])
            elif isinstance(value, (dict, list)):
                out[key] = canonicalize(value)
            else:
                out[key] = typed(key, value)
        return out
    if isinstance(node, list):
        return [canonicalize(item) for item in node]
    return node


def main(paths):
    status = 0
    for path in paths:
        try:
            with open(path, "r", encoding="utf-8") as handle:
                document = json.load(handle)
        except (OSError, json.JSONDecodeError) as error:
            print(f"FAIL {path}: {error}", file=sys.stderr)
            status = 1
            continue
        with open(path, "w", encoding="utf-8") as handle:
            json.dump(canonicalize(document), handle, indent=2)
            handle.write("\n")
        print(f"OK   {path}")
    return status


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(__doc__, file=sys.stderr)
        sys.exit(2)
    sys.exit(main(sys.argv[1:]))
