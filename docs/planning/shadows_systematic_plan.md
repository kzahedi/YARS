# Shadows — systematic plan (2026-05-20)

> **RESOLVED (2026-06):** shadow mapping shipped via
> `SHADOWTYPE_TEXTURE_MODULATIVE` + `ShadowMapper`
> (`src/yars/view/gui/ShadowMapper.h`, commit `9c3f07b`); the custom
> RTT pipeline was removed in `3dd7a77`. This document is kept as a
> historical record of the approaches considered.

After multiple failed attempts at making `SHADOWTYPE_TEXTURE_MODULATIVE`
with custom caster/receiver materials work correctly on GL3+ core, the
plan is to switch to **`SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED`** —
the modern Ogre 14 approach that delegates all shadow shader generation
to RTSS. This bypasses the entire UV math / projection mismatch class
of problems.

## Research summary

### What we found online
- **Ogre 14 has a "shadow_mapping" RTSS sub-render-state** that's the
  modern way to declare a material as a shadow receiver. Materials use:
  ```
  pass {
      rtshader_system { shadow_mapping light_count 1 }
      texture_unit { texture mytex.tga }
  }
  ```
  RTSS then generates the correct shaders automatically — caster AND
  receiver — using whatever shadow camera / projection is set up. No
  custom `texture_worldviewproj_matrix` math is needed in user shaders.

- **Ogre samples use `SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED`** with
  default Ogre material wiring (no `setShadowTextureCasterMaterial`).
  RTSS handles caster generation when materials are RTSS-managed.

- Sample reference: `ext/ogre-source/Samples/Simple/include/SkeletalAnimation.h`
  - C++: `setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED)`,
    `setShadowTextureCount(N)`, `setShadowTextureSize(N)`. Nothing else.
  - Sample reference for receiver material:
    `ext/ogre-source/Samples/Media/materials/scripts/DepthShadowmap.material`.

- **Known GL3+ shadow UV issues** in older Ogre versions involved shared
  materials across multiple meshes. With INTEGRATED + per-material RTSS,
  each material gets its own auto-generated shader, sidestepping this.

### Why our current approach fails
- We use `SHADOWTYPE_TEXTURE_MODULATIVE` (NON-integrated). This routes
  shadow rendering through `OgreTextureShadowRenderer` which renders an
  explicit modulating pass over receivers.
- The receiver pass uses our custom `YARS/TextureShadowReceiver` material
  with a hand-written GLSL shader that does
  `texture_worldviewproj_matrix * vertex`.
- Empirically the receiver's projected UV is rotated 90° from where the
  caster pass writes silhouettes. Root cause was not traced despite
  extensive C++/shader-level diagnostics.
- Conclusion: writing custom GLSL shadow receiver code on GL3+ core
  with non-integrated shadows is fragile and we should stop fighting it.

## The plan

### Phase 1: Drop the custom receiver, switch to integrated shadows
**Goal: make Ogre+RTSS generate the receiver shader.**

1. **Change `setShadowTechnique` to `SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED`**
   (in `OgreHandler.cpp::__setupShadows`).
2. **Remove `setShadowTextureReceiverMaterial(...)` calls** —
   integrated mode generates receivers per-material.
3. **Update YARS receiver materials** to declare themselves as receivers:
   - Add `rtshader_system { shadow_mapping }` to the ground material's pass.
   - Add it to wall materials (if walls should receive too).
   - For materials that shouldn't receive (sky, overlays), leave unchanged.
4. **Keep `setShadowTextureCasterMaterial(YARS/TextureShadowCaster)`** for
   now since our custom caster works correctly. If RTSS can auto-generate
   a caster for INTEGRATED mode, we may be able to drop it later.
5. **Delete `YARS/TextureShadowReceiver` material and
   `shadow_receiver.{vert,frag}`** — no longer needed.

### Phase 2: Verify
1. Build, run braitenberg.xml, capture late frame, inspect shadow placement.
2. Same with falling_objects.xml.
3. Move the eye camera in the live GUI to confirm shadows are stable
   (no longer dragging across the floor).
4. Run CSV regression and `yars_tests`.

### Phase 3: Fall-back options if Phase 1 doesn't work
If RTSS-integrated shadows don't render correctly, try in order:

A. **`SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED` with depth shadows**
   - `setShadowTexturePixelFormat(PF_DEPTH16)`
   - `setShadowTextureSelfShadow(true)`
   - Uses depth-comparison samplers; physically correct.
   - Higher-quality and more robust than modulative.

B. **`PlaneOptimalShadowCameraSetup`** for the planar floor
   - Designed specifically for shadows projected onto a known plane.
   - Replaces `FocusedShadowCameraSetup` in our `__setupShadows`.
   - Should give pixel-perfect shadow placement on the floor regardless
     of camera position.

C. **Cherry-pick from Ogre's `IntegratedPSSM3` C++ sample**
   (`LightShafts.h`):
   ```cpp
   auto* rs = shaderGenerator->getRenderState(MSN_SHADERGEN);
   auto* sub = shaderGenerator->createSubRenderState("SGX_IntegratedPSSM3");
   rs->addTemplateSubRenderState(sub);
   ```
   This wires PSSM3 into RTSS for all materials globally.

### Phase 4: Document the final solution
Update `docs/planning/shadows_state.md` with the working approach.
Remove the diagnostic shaders and any leftover empirical hacks.

## What we should NOT do
- Keep adding empirical UV swaps / flips / offsets to custom shaders.
  We've tried all 4 simple variants (raw, X, Y, XY flip) and they don't
  produce correct placement.
- Continue debugging Ogre's `texture_worldviewproj_matrix` discrepancy
  at the matrix-math level. We've spent multiple sessions on it without
  isolating the root cause.
- Increase shadow texture size or change frustum size further. The
  problem is mathematical alignment, not resolution.

## Success criteria
- Robot in braitenberg.xml casts a visible shadow that stays put when
  the eye camera moves.
- Falling balls in falling_objects.xml cast visible shadows on the floor
  that follow the balls (not the camera).
- Wall shadows present (even if subtle due to 0.5m wall height).
- No "diamond outline painted on floor" misprojection artifact.
- CSV regression unchanged (0 diff lines).
- `yars_tests` passing (35/35).
