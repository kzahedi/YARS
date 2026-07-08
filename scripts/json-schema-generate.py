#!/usr/bin/env python3
"""Regenerate schema/yars-config.schema.json from the xml/ corpus.

Walks every canonical config (and robot fragment) under xml/, infers one
schema definition per element — scalar attributes with their observed
JSON types, child elements, tagged containers, presence flags — and
emits a draft-07 JSON Schema. Curated rules that cannot be inferred
(required version, enums, colour patterns) are applied at the end.

Run after adding new elements/attributes to the Data* classes and to the
corpus, then review the schema diff. Check with
scripts/json-schema-check.py.
"""

import collections
import glob
import json
import os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ENUM_KEYS = {"mode", "project"}
TAGGED = {"sensors", "actuators"}
PLURAL = {"robots": "robot"}

elements = collections.defaultdict(lambda: {
    "attrs": collections.defaultdict(set),
    "enums": collections.defaultdict(set),
    "children": set(),
    "tagged_tags": set(),
    "flags": set(),
    "has_children_key": False,
})


def jtype(value):
    if isinstance(value, bool):
        return "boolean"
    if isinstance(value, (int, float)):
        return "number"
    return "string"


def visit(elem, obj):
    e = elements[elem]
    for k, v in obj.items():
        if k in ("#tag", "$include"):
            continue
        if k == "#children":
            e["has_children_key"] = True
            for entry in v:
                visit(entry["#tag"], entry)
        elif v is None:
            e["flags"].add(k)
        elif isinstance(v, dict):
            e["children"].add(k)
            visit(k, v)
        elif isinstance(v, list):
            e["children"].add(k)
            if k in TAGGED and v and isinstance(v[0], dict) and "#tag" in v[0]:
                for entry in v:
                    elements[k]["tagged_tags"].add(entry["#tag"])
                    visit(entry["#tag"], entry)
            elif k in PLURAL:
                for entry in v:
                    visit(PLURAL[k], entry)
            else:
                for entry in v:
                    visit(k, entry)
        else:
            e["attrs"][k].add(jtype(v))
            if isinstance(v, str):
                e["enums"][k].add(v)


def main():
    for f in glob.glob(os.path.join(ROOT, "xml", "**", "*.json"),
                       recursive=True):
        document = json.load(open(f))
        if "yars" in document:
            visit("yars", document["yars"])
        else:
            visit("robot", document)  # robot fragment

    defs = {}
    for name, e in sorted(elements.items()):
        props = {
            "$include": {
                "type": "string",
                "description": "Merge another JSON file (path relative to "
                               "this file); sibling keys override.",
            },
            # Pinned so a tagged-array entry only matches the definition
            # its #tag names (catches unknown/legacy tag names).
            "#tag": {"const": name},
        }
        for k, types in sorted(e["attrs"].items()):
            components = set(k.split("_")) | {k}
            if "colour" in components:
                props[k] = {"type": "string",
                            "pattern": "^#[0-9a-fA-F]{6}([0-9a-fA-F]{2})?$"}
                continue
            type_list = sorted(types)
            prop = {"type": type_list[0] if len(type_list) == 1 else type_list}
            if k in ENUM_KEYS and e["enums"][k]:
                prop = {"enum": sorted(e["enums"][k])}
            props[k] = prop
        for flag in sorted(e["flags"]):
            props[flag] = {
                "type": "null",
                "description": "Presence flag (canonical form is null).",
            }
        for child in sorted(e["children"]):
            if child in TAGGED and elements[child]["tagged_tags"]:
                tags = sorted(elements[child]["tagged_tags"])
                props[child] = {
                    "type": "array",
                    "items": {"allOf": [
                        {"required": ["#tag"]},
                        {"anyOf": [{"$ref": f"#/definitions/{t}"}
                                   for t in tags]},
                    ]},
                }
            elif child in PLURAL:
                props[child] = {
                    "type": "array",
                    "items": {"$ref": f"#/definitions/{PLURAL[child]}"},
                }
            else:
                ref = {"$ref": f"#/definitions/{child}"}
                props[child] = {"anyOf": [ref,
                                          {"type": "array", "items": ref}]}
        if e["has_children_key"]:
            props["#children"] = {
                "type": "array",
                "items": {"type": "object", "required": ["#tag"]},
            }
        defs[name] = {"title": name, "type": "object",
                      "additionalProperties": False, "properties": props}

    # Curated, non-inferable rules.
    defs["yars"]["required"] = ["version"]
    defs["yars"]["properties"]["version"] = {
        "type": "string",
        "pattern": "^\\d+\\.\\d+\\.\\d+$",
        "description": "Config schema version (XmlChangeLog); the binary "
                       "rejects configs newer than it supports or older "
                       "than the last crucial change.",
    }

    schema = {
        "$schema": "http://json-schema.org/draft-07/schema#",
        "$id": "https://github.com/kzahedi/YARS/schema/yars-config.schema.json",
        "title": "YARS simulation configuration",
        "description": "Canonical concise YARS JSON config format (see "
                       "docs/JSON_Format.md). Generated by "
                       "scripts/json-schema-generate.py from the xml/ "
                       "corpus, with curated rules on top.",
        "type": "object",
        "additionalProperties": False,
        "required": ["yars"],
        "properties": {
            "$schema": {"type": "string"},
            "yars": {"$ref": "#/definitions/yars"},
        },
        "definitions": defs,
    }
    out = os.path.join(ROOT, "schema", "yars-config.schema.json")
    with open(out, "w") as handle:
        json.dump(schema, handle, indent=2)
        handle.write("\n")
    print(f"{len(defs)} element definitions -> {out}")


if __name__ == "__main__":
    main()
