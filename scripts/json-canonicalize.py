#!/usr/bin/env python3
"""Canonicalize a YARS JSON config to the concise element shape.

The historical XmlToJson converter emitted every child element as an
array ("always-arrays" shape) with all-string attribute values and the
XML-era "rosiml" root key. JsonParser accepts that legacy shape as well
as the concise canonical form this script rewrites configs into:

- root key "yars" instead of "rosiml"
- a single child element is an object, not a one-element array:
    "lookAt": {"x": 0.0, "y": 0.01, "z": 0.0}
  (arrays with more than one entry — repeated sensors, hinges, robots,
  ... — stay arrays; "#children" arrays are handled below)
- attribute values use native JSON types: "true"/"false" become
  booleans, numeric strings become numbers. Keys whose values are
  identifiers or hex colours (a colour like "000000" would otherwise
  silently become the number 0) are never converted; see STRING_KEYS.
- an attribute-free ordered container collapses from
    "sensors": {"#children": [{"#tag": "ldr", ...}, ...]}
  to the flat tagged array
    "sensors": [{"#tag": "ldr", ...}, ...]
  ("#children" stays only where the element also carries attributes,
  e.g. <muscle name="..."> with ordered children)
- a child element with exactly one attribute collapses to an
  "elem_attr" key:
    "object": {"name": "main body"}  ->  "object_name": "main body"
- colour values gain the CSS-style "#" prefix: "#RRGGBB[AA]"

Usage: scripts/json-canonicalize.py <file.json> [more.json ...]
       (rewrites each file in place; exits non-zero on parse errors)
"""

import json
import re
import sys

# Attribute keys whose values are text/identifiers even when they happen
# to look numeric or boolean (e.g. colour "000000", a robot named "true").
# Checked against every "_"-separated component of a key, so the
# collapsed "colour_value" is protected by "colour".
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

HEX_COLOUR = re.compile(r"[0-9a-fA-F]{6}([0-9a-fA-F]{2})?\Z")


def typed(key, value):
    if not isinstance(value, str):
        return value
    components = set(key.split("_")) | {key}
    if "colour" in components and HEX_COLOUR.match(value):
        return value if value.startswith("#") else "#" + value
    if components & STRING_KEYS:
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


def is_scalar(value):
    return not isinstance(value, (dict, list))


def canonicalize(node):
    if isinstance(node, list):
        return [canonicalize(item) for item in node]
    if not isinstance(node, dict):
        return node
    out = {}
    for key, value in node.items():
        if (
            isinstance(value, list)
            and len(value) == 1
            and isinstance(value[0], dict)
            and key != "#children"
        ):
            value = value[0]
        if isinstance(value, dict):
            if set(value.keys()) == {"#children"}:
                # Attribute-free ordered container: hoist the entries.
                out[key] = canonicalize(value["#children"])
                continue
            if len(value) == 1:
                attribute, attribute_value = next(iter(value.items()))
                if (
                    is_scalar(attribute_value)
                    and "_" not in key
                    and not key.startswith("#")
                    and "_" not in attribute
                    and not attribute.startswith("#")
                ):
                    # Single-attribute child element: "object": {"name":
                    # ...} becomes "object_name": ... The combined key
                    # must type the raw value (a colour element's
                    # {"value": "000000"} would wrongly become the
                    # number 0 under the bare "value" key).
                    combined = f"{key}_{attribute}"
                    out[combined] = typed(combined, attribute_value)
                    continue
            out[key] = canonicalize(value)
        elif isinstance(value, list):
            out[key] = canonicalize(value)
        else:
            out[key] = typed(key, value)
    return out


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
        if "rosiml" in document:
            document = {"yars": document.pop("rosiml"), **document}
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
