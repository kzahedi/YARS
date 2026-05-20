# Shadow attempts log — comprehensive inventory

## Goal
Get shadows working correctly in `braitenberg.xml` and `falling_objects.xml`
using YARS image capture to validate. Shadows must be:
- World-anchored (not moving with the eye camera)
- Visually correct (placed where the light actually casts them)
- Robust across different XML scenes

## Environment

- Ogre 14.x (built from `ext/ogre-source`, installed to `ext/ogre/install`)
- OpenGL 3.3+ core profile (GL3Plus renderer)
- macOS arm64 (Mac Mini)
- YARS uses Z-up; Ogre uses Y-up
- Scene root has a `-90° X` rotation (`SceneGraph.cpp:49-50`) to convert
  YARS-Z-up scene data into Ogre Y-up rendering

## What we know works

### Caster pass
- `YARS/TextureShadowCaster` material with explicit GLSL renders caster
  silhouettes into the shadow texture correctly.
- Verified via screen-space sampling: diamond outline (walls) + dot
  (robot) clearly visible in the shadow texture.
- With ground=50×50, the diamond fills a large portion of the texture.

### Receiver pass infrastructure
- `YARS/TextureShadowReceiver` material is applied to receivers during
  the modulative pass. Confirmed by unconditional fragment output
  test (entire scene darkened uniformly).
- The shader receives valid auto-params and the shadow texture binds
  on sampler unit 0.

### Coordinate transform
- Floor entity's world matrix correctly places the 50×50 plane at
  world Y=0 in the XZ plane.
- World position visualization (`worldMatrix * vertex` output as color)
  shows floor.x ∈ [-4, 4], y=0, z ∈ [-4, 4] across the visible arena.

## Things tried, ranked by approach

### Shadow camera setup
- ✅ **`FocusedShadowCameraSetup`** — final choice. Works once ground
  mesh is 50×50.
- ✅ **`DefaultShadowCameraSetup` + `setShadowDirLightTextureOffset(0.667)`**
  — produced correct target=(0,0,0) but receiver UV still off by 0.15.
- ✅ **Custom `YarsFixedShadowCameraSetup`** — built with both
  parent-node positioning AND `setCustomViewMatrix/setCustomProjectionMatrix`.
  Caster pass writes diamond correctly centered in shadow texture, but
  receiver UV math STILL doesn't align.
- ❌ **`LiSPSMShadowCameraSetup`** — not tried.
- ❌ **`PlaneOptimalShadowCameraSetup`** — not tried, specifically designed
  for plane receivers.

### Receiver UV math
- ✅ Canonical `texture_worldviewproj_matrix * vertex` — UVs in [0,1]
  but always sample white interior of diamond. World origin → UV (0.65, 0.65)
  instead of expected (0.5, 0.5).
- ✅ `texture_viewproj_matrix * (worldMatrix * vertex)` (decomposed) —
  same result mathematically.
- ✅ Manual computation in shader using hardcoded shadow camera matrices
  matching the C++ side — single blob in wrong place; doesn't agree
  with caster pass either.
- ✅ `uv = uv.yx` (XY swap) — empirically produces visible shadows
  near the casters; root cause unknown.
- ✅ `uv.y = 1.0 - uv.y` (Y flip) — produces a blob in wrong location
  far from the caster.
- ✅ `uv.x = 1.0 - uv.x` (X flip) — produces a blob mirrored from the
  caster.
- ✅ `uv = 1.0 - uv` (XY flip) — wrong placement.
- ❌ Two-pass approach: compute UV in vertex with one matrix, override
  in fragment — not tried.

### Shadow technique
- ✅ `SHADOWTYPE_TEXTURE_MODULATIVE` — current choice.
- ❌ `SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED` — would need RTSS PSSM3
  integration; requires every material to support integrated shadows.
- ❌ `SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED` — same as above but
  modulative.
- ❌ `SHADOWTYPE_STENCIL_*` — gone in GL3+ core.

### Ground mesh size
- ❌ 500×500 — original; caused microscopic diamond.
- ✅ 30×30 — too small, visible horizon in side-view cameras.
- ✅ 50×50 — current; reasonable balance.
- ❌ 100×100 — not tried; might still hide horizon with similar shadow quality.

### Light direction
- ✅ Light direction `(-1, -1, -1)` — current.
- ❌ Tried other directions? Not really. User specifically said hardcode
  to `(-1, -1, -1)` is fine.

### Shadow texture
- ✅ Size 2048² — current. Was 4096² originally.
- ✅ Count 1 (single texture).
- ✅ Format PF_R8G8B8 (color, default for MODULATIVE).
- ❌ PF_DEPTH16 — needed for INTEGRATED depth shadows.

### Caster material variations
- ✅ Explicit GLSL caster (`shadow_caster.{vert,frag}`) — current.
- ❌ Default `Ogre/TextureShadowCaster` — fixed-function, breaks GL3+.

## Symptoms still observed

User's most recent screenshot (after the 50×50 ground fix and YX swap):
- Massive elongated black shadows extending diagonally across floor
- Shadows don't correspond to any actual caster geometry
- Shadows move when camera moves (confirmed by user)
- Walls in the arena cast oddly extruded shadow shapes

This pattern matches the "wall silhouettes painted on floor at wrong
locations" failure mode from commit 12c7cd7 — the receiver pass is
sampling the shadow texture but the UV does not correspond to the
world position from the caster's projection.

## Hypotheses for the actual root cause

### H1: Y-flip mismatch between caster and receiver passes
Ogre's caster pass uses `worldviewproj_matrix` via `AutoParamDataSource::getProjectionMatrix()`,
which applies a render-target Y-flip for FBO render targets.

The receiver pass uses `texture_worldviewproj_matrix` =
`BIAS * Frustum::getProjectionMatrixWithRSDepth() * Frustum::getViewMatrix() * worldMatrix`.
The `getProjectionMatrixWithRSDepth()` does NOT apply the FBO Y-flip.
The BIAS has a Y-flip baked in (-0.5 on Y), which in theory cancels
the missing FBO flip.

If these two Y-flips don't cancel cleanly (Ogre 14 bug or version
difference), shadows would be vertically mirrored.

But the YX-swap is a 90° rotation, not just a Y flip. So this alone
doesn't explain it.

### H2: Texture coordinate convention vs render target convention
In OpenGL, glTexImage2D uses Y=0 at bottom-left; sampling uses the
same convention. FBO render targets render bottom-up.

In Ogre, the `requiresTextureFlipping()` flag tells the pipeline that
a target needs flipping. For GL FBOs this is true.

If the receiver passes computed UV in the BOTTOM-UP convention but
the texture was written in TOP-DOWN convention (or vice versa), the
result would be flipped.

### H3: Custom scene root rotation interfering
The scene root has a -90° X rotation. This affects every entity's
world matrix. The light however is NOT a child of this rotated root —
it's directly under `_sceneManager->getRootSceneNode()`.

So the LIGHT direction `(-1, -1, -1)` is in unrotated world space.
But ENTITIES' world matrices include the -90° X rotation.

When computing `texture_worldviewproj_matrix * vertex`:
- `vertex` is in entity local space
- `worldMatrix` includes the scene root rotation → world space (Y-up)
- Shadow camera view+proj computed using light direction in unrotated space

These should all be in the same world space frame after the world matrix
is applied. But maybe some auto-param uses the entity's local frame
instead of accumulated world frame? Worth checking.

### H4: AutoParamDataSource bug or different code path for shadow receivers
Maybe `getTextureViewProjMatrix(0)` returns a stale or different
projection from what's actually used during the caster pass. The
investigation doc mentions this code path uses Frustum-level methods
which don't apply the render-target Y-flip.

A quick test: explicitly compute the matrix C++-side and pass it as
a custom uniform to the receiver shader, bypassing all auto-params.
Then compare with the caster's actual texture position.

### H5: RTSS interference
The SGTechniqueResolverListener generates RTSS techniques for materials.
Maybe the receiver material's technique is being replaced or duplicated
by RTSS, and the duplicate uses different auto-params.

We added `scheme ShaderGeneratorDefaultScheme` to YARS/TextureShadowReceiver
to prevent this, but maybe it's not effective.

## What hasn't been tried (priorities for next session)

1. **Online research on Ogre 14 + texture shadow + UV alignment issues.**
   Specifically look for:
   - Ogre forum threads / GitHub issues about texture_worldviewproj_matrix
   - PR-46xx era changes that touched shadow rendering
   - Ogre 14 migration guides for shadow code
2. **Switch to depth-based shadow mapping** with `setShadowTexturePixelFormat(PF_DEPTH16)`
   and SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED. Lets RTSS handle the
   shader generation completely, bypassing the receiver pass issue.
3. **PlaneOptimalShadowCameraSetup** — designed specifically for the
   case where the receiver is a known plane (the YARS floor).
4. **Manually instrument the C++ side** — log the actual matrices Ogre
   computes for `worldviewproj_matrix` (caster) vs
   `texture_worldviewproj_matrix` (receiver) for a known vertex. Compare
   numerically.
5. **Override SGTechniqueResolverListener** to definitely-not generate
   a duplicate technique for shadow receiver.
6. **Try without the SceneGraph -90° X rotation** — temporarily render
   in YARS native Z-up to test if the rotation interacts with shadow math.
