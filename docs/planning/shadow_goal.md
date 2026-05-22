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

5. **Cast on every receiving surface, not just the floor.** Examining
   the hexapod reference carefully, the robot's leg clearly casts a
   shadow onto the **white obstacle** on the right side of the image —
   not just on the sandy floor. Shadows in YARS must fall on:
   - the floor mesh
   - arena walls / boxes / obstacles
   - other dynamic objects (one robot leg shadowing another, a ball
     shadowing the robot beneath it, etc.)
   They must NOT appear on the casters themselves in a way that creates
   self-shadowing artifacts.

   **This requirement is the single most important constraint.** It
   rules out planar-projection shadows (which only work for one
   receiver plane) as the final approach. The shadow system must
   handle arbitrary 3D receivers.

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

The reference image clearly shows shadows on the **floor AND on the white
obstacle**, with pixel-perfect silhouettes and no overdraw stacking. That
rules out planar-projection (single receiver only) and points squarely at
stencil shadow volumes or proper shadow mapping.

The current planar-projected-proxy approach is **fundamentally
inadequate** — even if we fix the alpha-stacking / wall-transparency
artifacts, shadows still only render on the floor. To match the reference,
shadows must fall on every receiver, which the planar approach cannot do.

**Candidate approaches that CAN handle arbitrary 3D receivers:**

1. **Stencil shadow volumes** (the pre-2019 YARS path).
   - **How it works:** for each caster, extrude its silhouette edges
     along the light direction into a closed volume; mark every pixel
     inside that volume in the stencil buffer; render a darken pass
     over only stencil-marked pixels. Per-pixel exact, works on any
     receiver geometry.
   - **Pros:** pixel-perfect; matches the reference exactly; YARS
     already has the infrastructure intact (`prepareForShadowVolume()`
     calls in every `SceneGraph*Node`; edge-list building; the
     `OgreManualObject.cpp` null-guard patch already lives in our
     vendored Ogre submodule).
   - **Cons:** previously blocked on macOS GL3+ core by `OGRE_UNIFORMS`
     macro expansion in Ogre's built-in stencil-volume extrude shaders.
     Workaround: hand-write our own GLSL extrude shaders that don't
     depend on Ogre's unified-shader macros.
   - **Estimated effort:** 2–4 days.

2. **Shadow mapping (RTT depth shadow).**
   - **How it works:** render the scene depth from the light's POV
     into a depth texture; in the receiver shaders, sample that texture
     to determine whether each pixel is in shadow.
   - **Pros:** can be soft (PCF / VSM / etc.); works on any receiver.
   - **Cons:** previous attempts (the v5 RTT pipeline) all failed on
     Ogre 14's broken `texture_worldviewproj_matrix` auto-param on
     macOS arm64 GL3+. We partially built a custom-matrix workaround
     in commit `5bf684e` but couldn't get it placed correctly. Every
     YARS material would need a shadow-receiving shader variant, which
     is invasive.
   - **Estimated effort:** 3–5 days, more if we hit Ogre auto-param
     issues again.

3. **OgreNext / port to a newer Ogre.** OgreNext (2.x/3.x) has a
   completely different, working shadow system. The cost is porting
   YARS to OgreNext's HLMS materials and different scene-graph API.
   - **Estimated effort:** 2–4 weeks.

4. **Switch the renderer entirely** (bgfx / raylib / hand-rolled GL).
   Same effort scale as option 3.

**Strong recommendation: option 1 (stencil shadow volumes with
hand-written GLSL extrude shaders).** Direct match to the reference look,
existing infrastructure mostly in place, manageable effort, no scene-
graph rewrite required.

## Acceptance criteria

The shadow rendering is "done" when **all** of these hold:

1. **braitenberg.xml** (default top-down camera): the robot casts a
   visible shadow whose shape matches the robot's silhouette projected
   along the light direction. Wall shadows fall on the floor near each
   wall's base. No double-darkening in corners. No holes through walls.
   Floor texture visible everywhere outside the shadow regions.

2. **falling_objects.xml** (default side-view camera): each ball casts
   a smooth elliptical shadow on the floor (offset in the light
   direction), **no mesh / wireframe pattern inside the shadow**. If a
   ball is positioned such that its shadow would fall on a wall or on
   another ball, the shadow appears on that surface too — not just on
   the floor.

3. **The hexapod test case** (any scene with the wooden hexapod robot
   and a wall obstacle): each leg of the hexapod casts its own
   distinct shadow on the floor. When a leg is positioned such that
   its shadow falls on the white obstacle, the shadow appears on the
   obstacle's face — matching the reference image.

4. CSV regression (`braitenberg_logging.xml` at 2000 iterations vs
   `reference_logfile.macos-arm64.csv`): 0-diff.

5. `yars_tests`: 35/35 pass.

6. Build clean, no new warnings.

## Constraints

- macOS arm64, Ogre 14 (from vendored submodule), GL3+ core profile.
- C++17. Cannot use C++20-only features.
- The custom shaders must compile against GL3+ core (no fixed-function,
  no compatibility profile).
- Must not break the existing rendering pipeline for the floor, walls,
  robot, sensors, or trace lines.
