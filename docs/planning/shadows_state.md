# Shadows — state (2026-05-22, v6 shadow mapping)

**Status:** working. The robot, walls, balls, boxes, capsules, cylinders,
meshes, and PLY casters all project shadows onto the floor and onto each
other via Ogre's `SHADOWTYPE_TEXTURE_MODULATIVE` framework with custom
GLSL caster + receiver shaders.

**Architecture.** Texture-based shadow mapping. Ogre renders all casters
to a 2048×2048 R8G8B8 shadow texture from the light's POV (one ortho
camera produced by `FocusedShadowCameraSetup` aligned to the directional
light at `(-1,-1,-1)`), then re-renders every receiver in a second
modulating pass that samples the shadow texture and darkens shadowed
fragments.

1. **Caster shader** (`materials/shadow_caster.{vert,frag}` +
   `materials/YARSShadowCaster.material`). Writes
   `vDepth = gl_Position.z / gl_Position.w * 0.5 + 0.5` into the R channel.
   Set via `setShadowTextureCasterMaterial`.

2. **Receiver shader** (`materials/shadow_receiver.{vert,frag}` +
   `materials/YARSShadowReceiver.material`). Samples the shadow map and
   outputs `vec4(k, k, k, 1.0)` where `k = shadowDarkness` (0.35) for
   shadowed fragments, `1.0` otherwise. `scene_blend modulate` makes
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
   after the NDC→UV remap. Ogre's `getProjectionMatrix()` includes a
   per-render-target Y-flip for FBOs (GL FBO row 0 is at the bottom of
   the texture, opposite of the sampler convention), and we apply the
   same projection matrix on the receiver side as the caster side, so
   the silhouette lands at row `(1-v)` rather than `v`. Without the
   flip, walls' top faces register as in-shadow and the robot's shadow
   appears in the wrong arena quadrant.

5. **Shadow camera setup.** `FocusedShadowCameraSetup` fits the ortho
   frustum to visible casters. `SceneManager::setShadowFarDistance(20)`
   + `Light::setShadowFarDistance(15)` produce a 30×30 frustum at the
   light's POV — enough for braitenberg's 8×8 arena and roughly fits
   most YARS scenes. In sparse-caster scenes (e.g. `falling_objects.xml`
   with only two balls) the focused frustum is small, so the receiver
   pass covers only the floor area near those casters; shadows in such
   scenes appear under the balls but not across the entire floor.

## Tuning knobs

| Param | Value | What it does |
|-------|-------|--------------|
| `shadowDarkness` | 0.35 | Modulation factor in `shadow_receiver.frag` |
| `shadowBias` | 0.0015 | Combats shadow acne in the depth comparison |
| Shadow texture | 2048², `PF_R8G8B8` | Resolution / format |
| `Light::setShadowFarDistance` | 15m | Ortho frustum size at the light |
| `SceneManager::setShadowFarDistance` | 20m | Max distance from camera that receivers run |
| `setShadowCasterRenderBackFaces` | false | Front-face-only casters (prevents self-shadow on receivers) |

## What's gone

- `PlanarShadowProjector` and the planar-shadow proxy nodes (removed; planar
  projection couldn't cast shadows onto non-floor receivers — see goal
  constraint #5).
- `materials/YARSPlanarShadow.material`, `planar_shadow.{vert,frag}` (removed).
- `SceneGraph*Node::registerCaster` calls (removed).

## Validated

- `braitenberg.xml`: robot casts a clear soft-gray shadow on the floor;
  walls cast shadows along the floor near their bases. Visual at
  `docs/planning/state-2026-05-22-braitenberg.png` if captured.
- `falling_objects.xml`: balls cast subtle elliptical shadows on the
  floor below them. Coverage is limited by the focused shadow frustum
  (only the area near the casters receives the modulating pass).
- CSV regression: `braitenberg_logging.xml` at 2000 iterations matches
  `xml/reference_logfile.macos-arm64.csv` byte-for-byte.
- `yars_tests`: 35/35 passing.
