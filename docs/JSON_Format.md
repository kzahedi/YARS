# YARS JSON Configuration Format

YARS simulations are described by a single JSON file (conventionally kept
in `xml/`, the historical directory name from the XML era). This document
specifies the canonical format as of YARS 0.12.0: the structural rules,
the reader extensions (comments, includes, duplicate-key detection), the
version check, editor integration, and the migration path from older
formats.

Quick orientation:

```json
{
  "yars": {
    "version": "0.8.41",
    "simulator": { "frequency": 100, "solver_iterations": 10 },
    "screens":   { "...": "window, camera, OSD setup" },
    "environment": { "...": "ground, static objects, lights" },
    "robots":    [ { "...": "body, sensors, actuators, controller" } ],
    "logging":   { "...": "CSV/file/console output" }
  }
}
```

- Authoritative element reference: [`schema/yars-config.schema.json`](../schema/yars-config.schema.json)
  (90 element definitions; drives editor validation, see
  [Editor integration](#editor-integration)).
- Working examples: every file under [`xml/`](../xml/).
- Reader implementation: [`src/yars/configuration/json/JsonParser.{h,cpp}`](../src/yars/configuration/json/JsonParser.h).

## The root object

A config is a JSON object with a single required key `yars`:

```json
{
  "$schema": "../schema/yars-config.schema.json",
  "yars": {
    "version": "0.8.41",
    "...": "..."
  }
}
```

- `version` (required) is the **config schema version**, tracked by
  `XmlChangeLog` (`src/yars/defines/version.h`). It is independent of the
  binary version. On load, YARS rejects configs *newer* than the schema
  version it supports, and configs *older than the last crucial schema
  change* (with a printed changelog of what changed in between).
  Non-critical drift produces a warning only.
- `$schema` (optional) is ignored by the reader; editors use it to locate
  the JSON Schema when the repository `.vscode/settings.json` wiring is
  not in effect.
- The XML-era `rosiml` root (and the always-arrays wrapper) are
  **rejected since 0.14** with a hint to run
  `scripts/json-canonicalize.py`, which still reads every historical
  shape and rewrites it canonically.

## Structural rules

The format maps JSON onto XML-style elements (named nodes with attributes
and children). Six value shapes cover everything; they may be mixed
freely within one file.

### 1. Scalars are attributes

```json
"simulator": { "frequency": 100 }
```

Strings, numbers, and booleans are attribute values. Use native JSON
types: `100`, `0.5`, `true` — not `"100"`, `"0.5"`, `"true"` (the legacy
all-strings form is accepted but not canonical).

### 2. An object is a single child element

```json
"camera": {
  "position": { "x": 0.0, "y": 0.0, "z": 10 },
  "lookAt":   { "x": 0.0, "y": 0.01, "z": 0.0 }
}
```

### 3. An array is a repeated child element

```json
"box": [
  { "name": "wall 1", "...": "..." },
  { "name": "wall 2", "...": "..." }
]
```

A one-element array is equivalent to a plain object; the canonical form
uses the object. The `robots` container is always a flat array of robot
definitions:

```json
"robots": [ { "name": "walker", "...": "..." } ]
```

### 4. `elem_attr` shorthand for single-attribute elements

An element with exactly one attribute collapses into one key, joining
the element and attribute name with an underscore:

```json
"object_name": "main body",        // <object name="main body"/>
"solver_iterations": 10,           // <solver iterations="10"/>
"distance_meter": 0.5,             // <distance meter="0.5"/>
"texture_name": "YARS/DryGroundSmall"
```

The key is split at the first underscore. Attribute names never contain
one, and the few multi-word element names (`object_velocity`,
`source_anchor`, ...) only ever carry object values — the shorthand
applies to scalar values only, so there is no ambiguity. Elements whose
names contain an underscore never use the shorthand.

### 5. Tagged arrays for ordered, mixed-type containers

`sensors` and `actuators` hold children of *different* types whose
*order matters* — sensor order defines the controller channel order.
They are written as flat arrays in which every entry names its own
element via `"#tag"`:

```json
"sensors": [
  { "#tag": "proximity",  "name": "left 1",  "object_name": "main body", "...": "..." },
  { "#tag": "deflection", "name": "knee",    "object_name": "knee joint", "...": "..." }
],
"actuators": [
  { "#tag": "hinge", "name": "wheel left hinge",  "mode": "active", "type": "velocity", "...": "..." }
]
```

The schema requires `#tag` on every entry, so a forgotten tag is flagged
while editing.

**`#children` variant.** When an element carries attributes *and* an
ordered mixed-type child list, the list goes under a `#children` key
(a plain array cannot coexist with attributes). Example — a muscle
sensor whose repeated `domain`/`mapping` pairs must interleave in
order (`xml/muscle_tcpip.json`):

```json
{
  "#tag": "muscle",
  "name": "muscle sensor",
  "#children": [
    { "#tag": "object",  "name": "muscle" },
    { "#tag": "domain",  "min": 0, "max": 5.0 },
    { "#tag": "mapping", "min": 0, "max": 5.0 },
    { "#tag": "domain",  "min": 0, "max": 2.0 },
    { "#tag": "mapping", "min": 0, "max": 2.0 }
  ]
}
```

### 6. Presence flags are `null`

Some elements are pure on/off markers — their presence enables a
behaviour (XML's `<external/>`). They are written as `null`; omit the
key to disable:

```json
"sensor": [
  { "target": "left 1", "precision": 3, "external": null, "internal": null }
]
```

(The legacy `"external": {}` form is still accepted.)

## Element names

A few legacy element names were replaced with descriptive ones. Since
0.14 the reader **rejects** the legacy spellings with a migration hint;
`scripts/json-canonicalize.py` upgrades them:

| Canonical | Legacy | Meaning |
|-----------|--------|---------|
| `light` | `ldr` | light sensor (light dependent resistor) |
| `object_velocity` | `ov` | object velocity sensor |
| `object_angular_velocity` | `oav` | object angular velocity sensor |
| `source_anchor` / `destination_anchor` | `srcAnchor` / `dstAnchor` | muscle attachment anchors |
| `top`, `left`, `bottom`, `right`, `front`, `back` (box faces) | `first`..`sixth` | box face textures (+z, −x, −z, +x, +y, −y with z up) |

Names that look cryptic but are domain-standard stay: `cfm`/`erp`
(Bullet/ODE constraint parameters), `pid` (controller gains), `osd`
(on-screen display).

## Data types

| Kind      | Form                                  | Example |
|-----------|---------------------------------------|---------|
| Number    | JSON number                           | `"frequency": 100`, `"z": 0.01` |
| Boolean   | JSON `true`/`false`                   | `"visualise": true` |
| String    | JSON string                           | `"name": "main body"` |
| Colour    | `"#RRGGBB"` or `"#RRGGBBAA"` string   | `"colour": "#FFFF00"` |
| Angle     | Number, degrees                       | `"gamma": 90` |
| Version   | `"major.minor.patch"` string          | `"version": "0.8.41"` |

Colours keep the CSS-style `#` prefix (bare hex is accepted for
backward compatibility). Angles in `pose` elements (`alpha`, `beta`,
`gamma`) are degrees; distances are meters, masses kilograms
(`mass_kg`), frequencies hertz.

## Comments

Line (`//`) and block (`/* ... */`) comments are permitted anywhere:

```jsonc
"simulator": {
  "frequency": 100      // physics steps per second
}
```

The repository's VS Code settings associate `xml/**/*.json` with
`jsonc`, so editors do not flag them. Note that
`scripts/json-canonicalize.py` strips comments when it rewrites a file.

## Duplicate keys are errors

JSON parsers normally keep the *last* of two identical keys, which would
silently delete an element from a hand-edited config. YARS rejects the
file instead:

```
ERROR: config.json: duplicate key 'box' — use an array for repeated elements
```

## Includes: `$include`

Any object may be assembled from another JSON file:

```json
"robot": {
  "$include": "robots/hexapod-12dof.json",
  "controller": { "frequency": 10, "module": "Python", "...": "..." }
}
```

Semantics:

- The path is relative to the *including* file's directory.
- The included file must contain a JSON object. It is taken as the base;
  sibling keys are **shallow overrides** — a key that exists in the base
  is replaced at its original position, a new key is appended.
- Includes nest (an included file may itself use `$include`); cycles are
  a hard error.

Shared robot definitions live in [`xml/robots/`](../xml/robots/):
`hexapod-12dof.json` and `hexapod-24dof.json` are each referenced by
several top-level configs (see `xml/hexapod*.json` for the pattern,
including `xml/hexaboard.json`, which overrides the `sensors` section of
an included robot).

## Config sections

A complete config usually contains, under `yars`, in this order:

### `simulator`

```json
"simulator": { "frequency": 100, "solver_iterations": 10 }
```

Physics update rate (Hz) and constraint-solver iterations per step.

### `screens`

Window setup, camera, on-screen display, sky, and followable objects:

```json
"screens": {
  "screen": {
    "name": "Braiternberg", "show": true, "follow": false,
    "size": { "width": 800, "height": 800 },
    "camera": {
      "position": { "x": 0.0, "y": 0.0, "z": 10 },
      "lookAt":   { "x": 0.0, "y": 0.01, "z": 0.0 }
    },
    "osd": {
      "time":  { "colour": "#FFFF00", "font": "Time",  "size": 25 },
      "robot": { "colour": "#FFFFFF", "font": "Robot", "size": 25 }
    }
  },
  "sky_name": "YARS/CloudySky"
}
```

### `environment`

Ground plane, static geometry, and light sources:

```json
"environment": {
  "ground": {
    "fixed": true, "fog": false,
    "visualisation": { "texture_name": "YARS/DryGroundSmall" }
  },
  "box": [
    {
      "name": "wall 1",
      "dimension": { "width": 7.75, "height": 0.5, "depth": 0.25 },
      "pose": { "x": 4, "y": 0, "z": -0.15, "alpha": 0, "beta": 0, "gamma": 90 },
      "physics": { "mass_kg": 0 }
    }
  ],
  "pointLightSource": [ { "...": "..." } ]
}
```

Shapes: `box`, `sphere`, `cylinder`, `capsule`, `ply` (mesh files under
`ply/`). A `pose` gives position (`x`, `y`, `z`, meters) and orientation
(`alpha`, `beta`, `gamma`, degrees); a body-fixed `dimension` names its
axes per shape (`radius`, `height`, ...). `mass_kg: 0` makes an object
static.

### `robots`

Each `robot` has a `body` (rigid bodies, possibly `composite`),
`actuators`, `sensors`, and optionally a `controller`:

```json
"robots": [
  {
    "name": "Braitenberg",
    "body": {
      "composite": {
        "name": "composite",
        "geometry": {
          "cylinder": {
            "name": "main body", "visualise": true,
            "dimension": { "radius": 0.1, "height": 0.09 },
            "pose": { "x": 0, "y": 0, "z": 0.01 },
            "physics": { "mass_kg": 1.0 }
          }
        }
      }
    },
    "actuators": [
      {
        "#tag": "hinge",
        "name": "wheel left hinge", "mode": "active", "type": "velocity",
        "source_name": "wheel left", "destination_name": "main body",
        "force_max": 10.0, "velocity_max": 10.0,
        "pose": { "x": 0.0, "y": 0.0, "z": 0.0, "gamma": 90.0 },
        "mapping": { "min": -1, "max": 1 },
        "pid": { "p": 0.1, "i": 0.001, "d": 0.001 }
      }
    ],
    "sensors": [
      {
        "#tag": "proximity",
        "name": "left 1", "object_name": "main body",
        "pose": { "x": 0.0, "y": 0.1, "z": 0, "alpha": -90, "beta": 90 },
        "distance_meter": 0.5,
        "mapping": { "min": 1, "max": -1 },
        "openingAngles": { "x": 25, "y": 2.5 }
      }
    ],
    "controller": {
      "frequency": 10,
      "module": "Braitenberg3b",
      "parameter": { "name": "debug", "value": true }
    }
  }
]
```

Actuator types include `hinge`, `slider`, `fixed`, `generic`, `muscle`;
sensor types include `proximity`, `ldr`, `deflection`, `position`,
`velocity`, `binaryContact`, and more — see the schema definitions and
the `xml/` corpus. A sensor/actuator `mapping` (and sensor `domain`)
rescales raw values into the controller's range. **The order of the
`sensors` and `actuators` arrays defines the controller channel
order** — this is why they are ordered tagged arrays.

### `logging`

Loggers select values (`object`, `sensor`, `actuator`, `controller`
entries) and sinks (`csv`, `file`, `console`, `gnuplot`, `blender`):

```json
"logging": {
  "object": [
    { "target": "main body", "precision": 3,
      "use": [ { "value": "x" }, { "value": "y" } ] }
  ],
  "sensor": [
    { "target": "left 1", "precision": 3, "external": null, "internal": null }
  ],
  "csv": {
    "name": "braitenberg",
    "target": [ { "name": "main body" }, { "name": "wheel left" } ]
  }
}
```

### `traces`

Motion traces rendered in the GUI:

```json
"traces": {
  "trace": [
    {
      "target": "main body", "length": 5, "max": 300, "time": 5,
      "particles": false, "project": "xz",
      "init":  { "colour": "#ff0000",   "size": 0.01 },
      "final": { "colour": "#ff000000", "size": 0.0 }
    }
  ]
}
```

`project` (`xy`/`xz`/`yz`) draws the trace projected onto a plane; the
trace fades from `init` to `final` colour/size — see
`xml/braitenberg_trace_projection.json`.

## Editor integration

Live validation and completion work out of the box in VS Code: the
repository ships [`.vscode/settings.json`](../.vscode/settings.json)
which associates `xml/**/*.json` with `jsonc` and binds:

- [`schema/yars-config.schema.json`](../schema/yars-config.schema.json)
  to full configs,
- [`schema/yars-robot-fragment.schema.json`](../schema/yars-robot-fragment.schema.json)
  to robot fragments under `xml/robots/`.

The schema catches unknown/typo'd element and attribute names, wrong
types, malformed colours, a missing `version`, and missing `#tag`
entries — while you type. VS Code also warns on duplicate keys.

Headless (CI or other editors):

```bash
pip install jsonschema
python3 scripts/json-schema-check.py            # whole corpus
python3 scripts/json-schema-check.py my.json    # specific files
```

The schema is generated from the config corpus by
`scripts/json-schema-generate.py` (plus curated rules such as the
required `version`): after adding a new element or attribute to the
`Data*` classes and the corpus, rerun it and review the diff. Schema validation is advisory (the runtime does not
enforce it); a config the schema rejects may still run.

## Tooling and legacy formats

Since 0.14 the reader accepts only the canonical shape; retired shapes
fail with an explicit migration hint. `scripts/json-canonicalize.py`
still reads every historical era and rewrites it canonically:

| Era | Root | Reader (≥ 0.14) |
|-----|------|------------------|
| canonical (≥ 0.12) | `"yars"` | ✅ accepted |
| 0.10–0.11 (plural wrappers, `{}` flags, legacy names) | `"yars"` | structural forms still parse; retired element names are rejected with a hint |
| stage 1 | `"rosiml"` | ❌ rejected with hint |
| always-arrays (0.9.x, from `XmlToJson`) | `"rosiml"` | ❌ rejected with hint |

- **Canonicalize** any accepted config (idempotent):

  ```bash
  python3 scripts/json-canonicalize.py my_config.json
  ```

- **Migrate from XML**: XML support ended with the `v0.9.0-last-xml`
  release tag; its `--convert` option writes `<file>.json` next to a
  legacy `.xml` input. Canonicalize the result with the script above.

## Error behaviour

| Problem | Behaviour |
|---------|-----------|
| Malformed JSON, missing `yars` root, missing `#tag` in `#children` | clean error with file and position |
| Duplicate object key | clean error naming the key |
| `$include` missing file / non-object / cycle | clean error |
| Config `version` newer than binary supports | hard error |
| Config `version` older than last crucial change | hard error with changelog |
| Missing `version` | silently accepted (schema flags it) |
| Missing *required attribute* of an element | may crash inside `Data*::add()` — schema validation catches this while editing; runtime enforcement is planned via the binding tables |
