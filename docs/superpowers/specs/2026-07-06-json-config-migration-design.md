# JSON Config Migration + Data* Strangler Refactor — Design

**Date:** 2026-07-06
**Status:** Approved (brainstorm session 2026-07-06)
**Sequencing:** Last of the 2026-07-06 sub-projects. Multi-week effort;
every stage lands as a separate mergeable branch with the tree green.

## Goal

Replace XML/Xerces/XSD with JSON as the primary configuration format,
and collapse the hand-written duplication across the ~258 `Data*`
classes (~31k LOC, 55% of the codebase) into declarative binding
tables. Expected reduction: 10–15k LOC plus the Xerces dependency.

## Decisions

- **Format end state:** JSON primary, one-shot converter, XML support
  removed afterwards. No dual-format maintenance.
- **Data* restructuring:** strangler pattern — class names and accessors
  stay frozen, so physics/view/logging need zero changes; only the guts
  (parsing, validation, copy, schema generation) are replaced.
- **JSON library:** nlohmann/json, header-only, vendored under `ext/`
  or fetched via CMake.
- **Validation:** the binding tables are the single source of truth.
  Runtime validation (required fields, types, ranges) with precise
  `file:node: message` errors replaces XSD. No external schema file to
  keep in sync. (A generated JSON Schema export for editor
  autocompletion is a possible later addition, out of scope here.)

## Stages

### Stage 0 — JSON writer + converter

Add a JSON serializer over the existing in-memory data model (the tree
XML parsing already produces). This immediately yields the converter —
`yars --convert config.xml` writes `config.json` — and the JSON format
is defined by construction from the real data model rather than
designed on paper.

### Stage 1 — binding tables + JSON reader

The strangler core. Each `Data*` class declares one table naming its
attributes, children, element types, defaults, and required flags. A
generic walker parses JSON from the tables; the same tables drive
validation and `copy()`/`resetTo()` where mechanical.

**Verification (mechanical and strong):** for all 27 corpus configs:
`load XML → dump JSON → load JSON` and the two in-memory trees must
compare equal; simulations run from JSON must produce **bit-exact
CSVs** against their XML-loaded twins on both platforms.

### Stage 2 — corpus migration

Convert the `xml/` corpus to JSON; switch CI and documentation to JSON
as the primary format. XML files remain in-tree during this stage as
round-trip test fixtures.

### Stage 3 — the harvest

1. Tag a release as the **last XML-capable version** — users with old
   configs convert with that tag's `--convert`.
2. Then delete: the SAX parser layer, the XSD generator, the
   `add(DataParseElement*)` / `createXsd(...)` bodies across the Data*
   classes, the obsolete factory classes, and the **Xerces dependency**
   (CMake, CI package installs, docs).

## Risk containment

- Physics/view/logging interfaces frozen throughout.
- Each stage independently mergeable and guarded by the bit-exact CSV
  gates (braitenberg + hexapod, both platforms) from the CI quality
  gates sub-project.
- The converter exists (Stage 0) before anything is removed (Stage 3),
  with a tagged escape hatch for stragglers.

## Out of scope

- Any change to configuration *semantics* (attribute meanings,
  defaults, units).
- JSON Schema file generation for editors.
- Code generation of Data* classes from a schema — a possible later
  layer on top of the binding tables, not part of this migration.
