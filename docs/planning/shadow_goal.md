# Shadow rendering goal

The visual target for YARS shadows is the **pre-2019 stencil-shadow look**, an
example of which is preserved as
[`hexapod_reference.png`](hexapod_reference.png) (the wooden-hexapod
screenshot the user provided on 2026-05-22). Until what's rendered today
matches that look, the shadow system is not done.

For contrast, see
[`current_corner_artifact.png`](current_corner_artifact.png) — the
broken-walls failure mode from the depth_write experiment in commit
`dd50295` (reverted in `eb45d81`).

## What "right" looks like (from the hexapod reference image)

Concretely, examining the reference:

1. **Soft, even gray.** Shadows are a uniform mid-gray, not pure black.
   They look like a dim version of the floor texture where the floor's own
   pattern is still faintly visible through the shadow.

2. **No mesh pattern, no wireframe.** The shadows look smooth — flat patches
   of gray with clean silhouette edges. There is *no* visible triangulation
   inside a shadow, *no* hexagonal/quad pattern from the caster mesh's
   topology. A sphere casts a smooth ellipse; a robot leg casts a smooth
   tapered shape.

3. **Per-element fidelity.** Each separate caster body part has its own
   shadow on the floor. The hexapod's six legs each cast their own narrow
   shadow; the body casts a body-shaped shadow. They aren't merged into a
   single under-the-robot blob.

4. **Directional, offset from the caster.** Shadows extend in the light
   direction. A leg whose base is at world (x, y, z) and which reaches up to
   y=h has its shadow's far end at approximately (x − h, 0, z − h) for the
   light direction (−1, −1, −1). So the shadow lies *next to* the caster
   on the floor, not directly under it.

5. **Floor only.** Shadows appear on the floor mesh and nothing else. They
   do not appear on the walls. They do not occlude or alter wall faces.

6. **No double-darkening in corners.** Where two casters' shadows would
   overlap (e.g. two robot legs close together), the overlap region is
   either the same intensity as the individual shadows, or only marginally
   darker — *not* twice as dark.

7. **No artifacts on caster faces.** Shadows render on the floor only; they
   never bleed onto or through the casters themselves. There must be no
   "holes" in walls or robot parts where the sky/clear-color shows through.

8. **Static and dynamic casters both work.** Walls cast clean shadows on
   the floor adjacent to their bases. Robot parts and balls cast shadows
   that follow them as they move. The hexapod legs animate frame-to-frame
   and their shadows animate with them.

## What "wrong" looks like (current artifacts, as of `eb45d81`)

The current implementation (planar projected shadow proxies with
`alpha_blend` + `depth_write off`) has these problems:

1. **Mesh pattern inside curved shadows.** Sphere and capsule shadows
   show a visible hexagonal / triangulation pattern because hundreds of
   overlapping projected triangles each alpha-blend with the previous,
   stacking into a darker patch with the triangle wireframe visible.
   *Goal: smooth flat gray, no pattern.*

2. **Double-dark corners.** Where two wall shadows overlap inside a
   corner of the arena, the alpha-stacking produces a noticeably darker
   patch. *Goal: corner is the same darkness as the individual wall
   shadows.*

3. **Holes in walls** (introduced by the depth_write attempt, now
   reverted). When `depth_write on` was enabled to fix #1 and #2, the
   shadow proxies' depth values occluded wall faces near the floor,
   making patches of sky color show through the walls.
   *Goal: walls render solidly; shadows only on the floor.*

4. **Wall self-shadowing near base.** Wall shadows project as a strip
   immediately inside each wall. From a side-view camera, the strip
   appears to climb up the wall's inside face. *Goal: wall shadows are
   clearly on the floor, not on the wall itself.*

5. **Shadows missing for some camera angles.** With a very small Y-lift
   (5mm), shadows lost the depth fight at oblique angles and rendered
   nothing. Bumping to 5cm helped but the 5cm gap is visible from
   grazing angles. *Goal: shadows are always visible and look attached
   to the floor with no visible gap.*

## What approach should we use?

The reference image looks like a stencil-shadow-volume rendering: pixel-
perfect silhouettes, per-pixel shadow testing, no overdraw stacking. That
matches pre-2019 YARS which used `SHADOWTYPE_STENCIL_ADDITIVE`.

The current planar-projected-proxy approach fundamentally can't avoid the
alpha-stacking artifact (#1) without a depth trick (which causes #3), or a
non-additive blend mode (which Ogre doesn't cleanly expose for this case),
or a much-more-complex algorithm (silhouette extraction → render only
silhouette → fill polygon).

**Candidate approaches, ranked by likelihood of matching the goal:**

1. **Stencil shadow volumes** (the pre-2019 path). Pixel-perfect by design.
   Was previously blocked on macOS GL3+ by `OGRE_UNIFORMS` macro expansion
   in the stencil-volume extrude shaders and by `ManualObject` null-guard
   crashes. The null-guard is already patched in our vendored Ogre. The
   macro issue would need a custom GLSL extrude shader. Several days of
   work but produces the goal look directly.

2. **Hand-written silhouette projection.** For each caster mesh, compute
   the silhouette edges from the light's POV CPU-side, then render only
   those edges as a flat filled polygon (a 2D shape on the floor). No
   triangle overlap because the silhouette is one outline. Medium
   complexity. Produces smooth shadows.

3. **Render-to-texture shadow map with proper depth comparison.** A
   1024² shadow map rendered from light POV, sampled per floor pixel.
   The previous v5 RTT pipeline tried this and failed on Ogre's broken
   `texture_worldviewproj_matrix` auto-param. Could be revived with a
   custom matrix uniform (we partially built this in commit `5bf684e`
   but it was difficult to debug).

4. **Pre-rendered shadow textures on each caster + UV-projected onto the
   floor.** Each caster renders its silhouette into its own small
   per-caster texture; the floor material samples one decal texture per
   caster. Complex.

The user has authorized "heavy" approaches. **Recommended: pursue option 1
(stencil shadows)** because it directly matches the reference image and
because the pre-2019 YARS code already had stencil infrastructure intact
(see `docs/planning/shadows_attempts_log.md` for the previous attempt and
what blocked it).

## Acceptance criteria

The shadow rendering is "done" when **both** of these hold:

1. A screenshot of `xml/braitenberg.xml` from the default top-down camera
   shows: clear wall shadow strips along the arena perimeter (no
   double-dark corners); the robot casts a visible shadow whose shape
   roughly matches the robot's silhouette projected along the light
   direction; the floor texture is visible everywhere except where
   shadows are; no holes through walls.

2. A screenshot of `xml/falling_objects.xml` from the default side-view
   camera shows: each ball casts a smooth elliptical shadow on the
   floor, offset in the light direction; **no hexagonal/wireframe
   pattern inside the shadow**; shadows do not appear on the walls or
   on the balls themselves.

CSV regression (`braitenberg_logging.xml` at 2000 iterations vs
`reference_logfile.macos-arm64.csv`) must remain 0-diff. `yars_tests`
must remain 35/35.

## Constraints

- macOS arm64, Ogre 14 (from vendored submodule), GL3+ core profile.
- C++17. Cannot use C++20-only features.
- The custom shaders must compile against GL3+ core (no fixed-function,
  no compatibility profile).
- Must not break the existing rendering pipeline for the floor, walls,
  robot, sensors, or trace lines.
