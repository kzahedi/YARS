# Shadows — state (2026-06-11, v6.1 shadow mapping)

**Status:** working, all six acceptance criteria in `shadow_goal.md` met.
The robot, walls, balls, boxes, capsules, cylinders, meshes, and PLY
casters project shadows onto the floor **and onto each other / onto
non-floor receivers** via Ogre's `SHADOWTYPE_TEXTURE_MODULATIVE`
framework with custom GLSL caster + receiver shaders.

**Architecture.** Texture-based shadow mapping. Ogre renders all casters
to a 2048×2048 `PF_FLOAT32_R` shadow texture from the light's POV (one
ortho camera produced by `YarsFixedShadowCameraSetup` aligned to the
directional light at `(-1,-1,-1)`), then re-renders every receiver in a
second modulating pass that samples the shadow texture and darkens
shadowed fragments.

1. **Caster shader** (`materials/shadow_caster.{vert,frag}` +
   `materials/YARSShadowCaster.material`). Writes
   `vDepth = gl_Position.z / gl_Position.w * 0.5 + 0.5` into the R
   channel (single-channel write, matching the `PF_FLOAT32_R` target).
   Set via `setShadowTextureCasterMaterial`.

2. **Receiver shader** (`materials/shadow_receiver.{vert,frag}` +
   `materials/YARSShadowReceiver.material`). Samples the shadow map with
   a **3×3 PCF** kernel (nine binary depth compares one texel apart,
   averaged into a `lit` factor) and outputs `vec4(k,k,k,1.0)` where
   `k = mix(shadowDarkness, 1.0, lit)`. `scene_blend modulate` makes
   shadowed pixels darken to `k × original_colour` on the framebuffer.
   Set via `setShadowTextureReceiverMaterial`.

3. **`ShadowMapper`** (`src/yars/view/gui/ShadowMapper.{h,cpp}`).
   `Ogre::ShadowTextureListener` that fires after the caster pass but
   before the receiver pass. Reads the shadow camera Ogre just used,
   computes `proj * view` from `getProjectionMatrix() * getViewMatrix()`,
   and pushes the result into the receiver material as the
   `shadowViewProjMatrix` uniform on both the vertex and fragment
   programs. This is the critical workaround for Ogre's broken
   `texture_worldviewproj_matrix` auto-param on macOS arm64 GL3+ core —
   that auto-param computes UVs that don't agree with where the caster
   pass actually wrote silhouettes.

4. **Y-flip compensation.** The receiver shader applies `uv.y = 1.0 - uv.y`
   after the NDC→UV remap. Verified empirically 2026-06-11 under the
   custom projection matrix from `YarsFixedShadowCameraSetup` (A/B frame
   captures of braitenberg + falling_objects): the per-render-target FBO
   flip still applies even with `setCustomProjectionMatrix` on the shadow
   camera (GL FBO row 0 is at the bottom of the texture). WITH the flip,
   the robot's shadow hugs its base and the airborne ball's shadow lies
   on the floor below/offset from the ball; WITHOUT it, those shadows
   vanish and a phantom blob appears mirrored across the arena. Do not
   re-guess this — `shadows_attempts_log.md` documents six failed
   flip/swap attempts.

5. **Shadow camera setup.** `YarsFixedShadowCameraSetup`
   (`OgreHandler.cpp:39`) — world-anchored and light-aligned, ignoring
   the eye camera entirely. It positions an ortho camera along
   `-lightDir` looking at the origin with a fixed 60×60 frustum
   (half-extent 30 m, `OgreHandler.cpp:98`). This guarantees full-arena
   coverage in **every** scene and removes camera-dependent drift. At
   2048² that is ~2.9 cm/texel, smoothed by PCF. (Replaced
   `FocusedShadowCameraSetup`, which fit the frustum to the *viewer's*
   view — sparse-caster scenes like `falling_objects.xml` got a tight
   frustum that missed most of the floor, and coverage drifted when
   orbiting.)

## Tuning knobs

| Param | Value | What it does |
|-------|-------|--------------|
| `shadowDarkness` | 0.35 | Modulation factor in `shadow_receiver.frag` |
| `shadowBias` | 0.0002 | Depth-compare offset vs. acne (~4 cm world over the 1..200 frustum, with float depth) |
| Shadow texture | 2048², `PF_FLOAT32_R` | Resolution / format (float depth — 8-bit `PF_R8G8B8` gave only 256 levels and lost subtle shadows) |
| Fixed frustum half-extent | 30 m (60×60) | `YarsFixedShadowCameraSetup`, `OgreHandler.cpp:98` |
| `SceneManager::setShadowFarDistance` | 100m | Eye-camera cull distance for the modulating pass only (no longer shapes the shadow camera) |
| `setShadowCasterRenderBackFaces` | false | Front-face-only casters (prevents self-shadow on receivers) |
| PCF kernel | 3×3 (9-tap) | Soft penumbra edges in `shadow_receiver.frag` |
| `tex_address_mode` | border | Border PCF taps return lit — do NOT change to clamp/wrap (`YARSShadowReceiver.material:54`) |

## What's gone

- `FocusedShadowCameraSetup` (replaced by `YarsFixedShadowCameraSetup`).
- `Light::setShadowFarDistance(15)` (only fed the focused frustum fit).
- `PlanarShadowProjector` and the planar-shadow proxy nodes (removed in
  v6; planar projection couldn't cast shadows onto non-floor receivers).
- `materials/YARSPlanarShadow.material`, `planar_shadow.{vert,frag}`.
- `SceneGraph*Node::registerCaster` calls.

## Validated (2026-06-11)

- `braitenberg.xml`: robot casts a clear soft-gray shadow on the floor,
  offset along the light direction; walls cast shadows along the floor
  near their bases; floor texture visible through shadows. Capture:
  `docs/planning/state-2026-06-11-braitenberg.png`.
- `falling_objects.xml`: balls cast smooth elliptical shadows on the
  floor below them — full-arena coverage now that the frustum is
  world-anchored (this scene was broken in v6 by the focused frustum +
  8-bit precision). Capture: `docs/planning/state-2026-06-11-falling.png`.
- `hexapod_shadow_test.xml`: distinct per-leg shadows on the floor AND a
  visible shadow patch on the adjacent white obstacle face (acceptance
  criterion 3 — shadow on a non-floor receiver). Capture:
  `docs/planning/state-2026-06-11-hexapod.png`.
- CSV regression: `braitenberg_logging.xml` at 2000 iterations matches
  `xml/reference_logfile.macos-arm64.csv` byte-for-byte.
- `yars_tests`: 35/35 passing.
