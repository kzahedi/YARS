## Context

YARS currently ships with working braitenberg and hexapod examples. The legacy MPI experiments use older XML dialects:
- **RoSiML** (very old): used by `ant/xml/ant_basic.xml` and `spider/xml/spider.xml` — completely different root element, different structure
- **rosiml 0.8.38/0.8.39**: used by hexapod version4 and hexaboard — close to current 0.8.41 but may have minor element differences

The current YARS parser targets rosiml 0.8.41. An existing `hexapod.xml` already works at 0.8.41 and uses mesh-based PLY bodies.

## Goals / Non-Goals

**Goals:**
- Port `ant_basic.xml` to rosiml 0.8.41 and run it without an external controller
- Port `hexapod_reduced.xml` (version4) to rosiml 0.8.41 and run it without an external controller
- Each simulation SHALL launch, display a 3D robot, and step physics without errors

**Non-Goals:**
- Porting all legacy experiments (spider, walker, nmode variants)
- Implementing trained/evolved controllers — open-loop sine or no-controller is sufficient
- Matching original simulation behavior exactly (joint parameters may be adjusted)

## Decisions

### Decision 1: Hexapod — adapt version4 rosiml 0.8.38 → 0.8.41

**Rationale**: `hexapod_reduced.xml` already uses modern rosiml syntax with ply bodies and meshes. The existing `hexapod.xml` in the YARS repo serves as a reference for 0.8.41 structure. Differences are likely minor (attribute names, screen element changes). Copying and diffing against the working file is the fastest path.

**Alternative considered**: Use the existing `hexapod.xml` as-is — rejected because that file already exists; we want a second distinct hexapod config from the MPI experiments.

### Decision 2: Ant — rewrite from RoSiML to rosiml 0.8.41

**Rationale**: The old RoSiML format uses `<movable>`, `<compound>`, `<cappedCylinder>`, `<hinge>` elements under a completely different schema. These do not map to YARS's current parser. The ant geometry is simple (torso + 4 legs × 2 segments = 9 bodies, 8 joints), so a clean rewrite using `<capsule>` or `<cylinder>` bodies is faster than building a compatibility shim.

**Alternative considered**: Add RoSiML parser compatibility — rejected as too invasive and the XML format was intentionally replaced.

### Decision 3: No-controller mode via `<noController/>`

Both experiments will use the existing `<noController/>` tag (already supported in YARS, used in `braitenberg_nocontroller.xml`). This lets joints hang free under gravity/physics, which is sufficient to verify the simulation loads and runs correctly.

### Decision 4: No new mesh assets needed for ant

The ant uses simple geometric primitives (box torso, capped cylinders for legs). YARS supports `<cylinder>` and `<capsule>` primitives natively. No new mesh files are needed.

## Risks / Trade-offs

- [Schema drift] Some elements in hexapod 0.8.38 may have been renamed or removed by 0.8.41 → Mitigation: diff against working `hexapod.xml` and fix element by element
- [Ant geometry] Joint anchors and orientations are defined in world-space in old format, need careful translation to relative poses → Mitigation: use existing ant_basic.xml values and test incrementally
- [Parser strictness] YARS may reject unknown attributes silently vs. crash → Mitigation: run with `--iterations 10` first to catch early errors

## Open Questions

- Does `hexapod_reduced.xml` use any mesh assets (`*.mesh`, `*.ply`) that are not present in the YARS `ext/` or build output? If so, may need to fall back to primitive geometry for that hexapod too.
