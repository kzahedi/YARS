# Shadows — state (2026-05-21 evening, v5.1 planar projected shadows)

**Status:** working. Every shadow-casting entity in YARS (walls, robot
parts, balls, boxes, capsules, cylinders, meshes, plies) casts a clean
hard-edged shadow on the floor, projected exactly along the light
direction `(-1, -1, -1)`.

**Architecture:** SIGGRAPH 97 planar projected shadows. For each
caster, a "shadow proxy" entity sharing the caster's mesh is created
and attached to its own scene node. Each frame, the proxy node's
world transform is set to `planarShadowMatrix(floor, light) *
caster.worldTransform`. The proxy renders with a translucent black
material that draws on top of the floor.

1. **`PlanarShadowProjector`** (`src/yars/view/gui/PlanarShadowProjector.{h,cpp}`)
   owns the list of `(casterNode, proxyNode, proxyEntity)` triples
   and updates each proxy's world transform per frame from
   `OgreHandler::step()`.
2. **`YARS/PlanarShadow`** material (`materials/YARSPlanarShadow.material`)
   with `materials/planar_shadow.{vert,frag}` — explicit GLSL programs
   emit `vec4(0, 0, 0, 0.5)` and use `alpha_blend` + `depth_write off`
   + `depth_bias -1` so proxies render slightly above the floor
   without occluding casters.
3. **Scene graph registration.** Each `SceneGraph*Node` class that
   creates a shadow-casting visible object calls
   `OgreHandler::instance()->getPlanarShadowProjector()->registerCaster(...)`
   after the visible Entity / ManualObject is set up. Hooked:
   `SceneGraphBoxNode`, `SceneGraphSphereNode`, `SceneGraphCapsuleNode`,
   `SceneGraphCylinderNode`, `SceneGraphMeshNode`, `SceneGraphPlyNode`.
4. **Frame convention.** YARS scene data is Z-up but the scene root
   applies a -90° X rotation that converts to Ogre Y-up for
   rendering. `casterNode->_getFullTransform()` returns the
   post-rotation Y-up world transform; proxies attach to the
   SceneManager's unrotated root (also Y-up). So the projector
   math lives in Ogre Y-up: floor plane `(UNIT_Y, 0)`, light
   direction `(-1, -1, -1)`.

**Why this is robust:** unlike texture-based shadow mapping (the
previous v5 RTT pipeline, deleted in commit `3dd7a77`), the shadow's
position on the floor is **literally** the geometric projection of
the caster's mesh along the light direction. No UV math, no
projection matrices, no FBO Y-flip cancellations to get wrong. The
math `M[i][j] = (i==j ? n·l : 0) - l[i] * n_extended[j]` exactly
encodes "where would this point land if a ray went from the light
through it and hit the floor".

**Skipped (deliberately):**
- `SceneGraphMuscleNode`, `SceneGraphSoftPlyNode` — geometry
  changes per frame; the proxy's `convertToMesh()` snapshot taken at
  registration time wouldn't follow. To support these we'd need to
  rebuild the proxy mesh per frame or write a custom proxy renderer.
- `SceneGraphLDRSensor`, `SceneGraphProximitySensor`,
  `SceneGraphJointAxisVisualisationNode`,
  `SceneGraphTraceLineObject` — visualisations, not part of the
  physical scene.
- `SceneGraphEnvironmentNode` — the floor itself (it's the
  receiver).

**Limitations:**
- Floor-only receiver. Shadows don't appear on walls. To extend, add
  one projector per receiving plane (each wall) and register
  casters with each.
- Hard-edged. No soft shadows. PCF would require returning to a
  texture-based approach.
- Each caster duplicates the mesh's vertex data (the proxy shares
  the Ogre::Mesh resource, but Ogre's mesh manager keeps both
  Entities). Modest memory cost for YARS scenes.

**Files:**
- `src/yars/view/gui/PlanarShadowProjector.{h,cpp}` — projector
- `src/yars/view/gui/OgreHandler.cpp` — instantiates projector;
  exposes `getPlanarShadowProjector()`
- `src/yars/view/gui/SceneGraph*Node.cpp` — register casters
- `materials/YARSPlanarShadow.material` +
  `materials/planar_shadow.{vert,frag}` — translucent-black proxy
  material

**Implementation history:**
- v1 — v3: attempts to make Ogre's `SHADOWTYPE_TEXTURE_*` /
  `SHADOWTYPE_STENCIL_*` paths work on GL3+ core, all failed.
  Documented in `shadows_attempts_log.md`.
- v4: empirical UV swap in a custom GLSL receiver. Worked for one
  scene but broke on others. Reverted.
- v5 RTT pipeline: hand-rolled top-down render-to-texture with our
  own UV math. Worked architecturally but `texture_worldviewproj_matrix`
  vs custom UV calculation never produced correctly-placed shadows
  across both `braitenberg.xml` and `falling_objects.xml`.
- v5.1 (current): switched to planar projected shadows. Each
  caster's shadow is the exact geometric projection of its mesh,
  guaranteed correct by construction.

See `docs/superpowers/plans/2026-05-21-shadows-v5.md` for the full
research + plan that led to this implementation.

---

## Historical (pre-v5)

# Shadows — state of investigation (2026-05-20, updated)

**Current state:** shadows are **re-enabled** and visible. The setup
uses `FocusedShadowCameraSetup` + a 50×50 ground mesh (was 500×500)
+ an empirical `uv = uv.yx` swap in the receiver fragment shader.

**Why these choices:**

1. **Ground mesh shrunk from 500×500 to 50×50** — the original 500×500
   ground caused `FocusedShadowCameraSetup` to span a massive area,
   making the 8×8 arena's wall silhouettes microscopic in the shadow
   texture (~1% of texture extent). With 50×50, the focused camera
   produces a shadow texture where the arena walls form a prominent
   diamond filling most of the texture.

2. **YX swap in the receiver shader** — even with the correctly-sized
   ground, the receiver's `texture_worldviewproj_matrix` produces UVs
   rotated 90° from where the caster pass writes silhouettes. Applying
   `uv = uv.yx` aligns them. The root cause was not traced to a
   specific Ogre 14 implementation detail despite extensive
   diagnostics, but the fix produces correct results.

3. **`FocusedShadowCameraSetup`** — auto-fits the shadow frustum to
   the intersection of eye-camera frustum and shadow-caster geometry.
   With the small ground, this gives stable, well-focused shadows
   for both top-down (braitenberg.xml) and side-view (falling_objects.xml)
   cameras.

**Visible results (2026-05-20):**
- braitenberg.xml: robot casts a clear shadow on the floor directly
  below it.
- falling_objects.xml: balls cast clear shadows on the floor in the
  light-direction.
- No artifacts/misprojected silhouettes from earlier attempts.

**Caveats:**
- Wall shadows are still subtle because YARS arena walls are only
  0.5m tall (shadow strip ~0.5m wide at 45° light angle).
- The YX swap is empirical; the underlying Ogre 14 quirk warrants
  a deeper dive if anyone wants to clean this up.
- The 50×50 ground may show the horizon at extreme camera angles
  (falling_objects.xml's low side-view shows it; this is mostly
  cosmetic).

**Files involved:**
- `src/yars/view/gui/OgreHandler.cpp::__setupShadows()` — full setup
- `src/yars/view/gui/SceneGraphEnvironmentNode.cpp` — ground mesh 50×50
- `materials/YARSShadowCaster.material` / `shadow_caster.{vert,frag}`
- `materials/YARSShadowReceiver.material` / `shadow_receiver.{vert,frag}`

## Historical (2026-05-19): the original buggy state

---

## What we have working (do not undo)

### C++ wiring (in `src/yars/view/gui/OgreHandler.cpp`)
The shadow setup block currently calls `setShadowTechnique(NONE)`,
but the previous-known-good block is preserved in git history at
commit `12c7cd7`. To re-enable, restore that block and fix the
placement bug below. Key pieces that must be present:

- `setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE)`
- `setShadowTextureSize(4096)` — crisp edges; ~64 MB memory cost
- `setShadowTextureCount(1)`
- `setShadowFarDistance(200.0f)`
- `setShadowCasterRenderBackFaces(false)` — without this, walls
  self-shadow their own outward face.
- `setShadowCameraSetup(new FocusedShadowCameraSetup())` — see
  caveat below.
- **Both** `SceneManager::setShadowTextureCasterMaterial(MaterialPtr)`
  AND `SceneManager::setShadowTextureReceiverMaterial(MaterialPtr)`.
  Per-Technique `setShadowCasterMaterial` is NOT enough; Ogre 14
  falls back to a global fixed-function default.

### Material wiring (in `materials/`)
- `YARSShadowCaster.material` defines `YARS/TextureShadowCaster`
  with explicit GLSL (`shadow_caster.vert`, `shadow_caster.frag`).
  Required because `Ogre/TextureShadowCaster` is fixed-function and
  fails on GL3+ core ("technique has no Vertex Shader").
- `YARSShadowReceiver.material` defines `YARS/TextureShadowReceiver`
  with explicit GLSL (`shadow_receiver.vert`, `shadow_receiver.frag`).
  Required because `Ogre/TextureShadowReceiver` is fixed-function
  and silently breaks the modulating pass on GL3+ core.
- Both materials tag their technique with
  `scheme ShaderGeneratorDefaultScheme` so
  `SGTechniqueResolverListener` does not lazily create a duplicate
  RTSS-generated technique that would override our explicit shaders.

---

## The bug (still open)

### Symptom
- Caster pass works: shadow texture contains correct wall silhouettes
  (verified by rendering the shadow texture directly to the ground
  via `gl_FragCoord/1200` screen-space sampling — see the
  "Visual debug techniques" section below).
- Receiver pass projects to UVs that vary smoothly in [0,1] across
  the visible ground, but those UVs always land on the **white
  interior** of the diamond, never on the dark wall silhouettes
  at the diamond's edges.
- Result: no visible shadows in the modulative pass.

### The shape of the misalignment
- With `texture_viewproj_matrix * obj_vertex` (the original "buggy"
  state in commit `12c7cd7`): shadows DO appear on the floor as
  diagonal stripes crossing the entire arena. These are the wall
  silhouettes mapped onto the ground via the **wrong** projection —
  treating object-space coordinates as world-space coordinates.
  Visually misleading but at least visible.
- With `texture_worldviewproj_matrix * obj_vertex` (the canonical
  Ogre RTSS math — see "Ogre source references" below): no
  shadows visible anywhere. UVs are mathematically in range but
  always sample the diamond interior.
- With explicit `texture_viewproj_matrix * (world_matrix * vertex)`:
  same as above (mathematically equivalent).
- With Y-flip / X-flip / XY-swap applied on top of either of the
  above: no shadows. So it is **not** a simple axis-convention
  mismatch.

### Best-supported hypothesis (not yet verified)

The visible ground covers only a small subset of the shadow texture
UV range, and that subset lies entirely inside the diamond
interior. The wall silhouettes (the diamond *border*) are at UVs
the ground never reaches.

Two factors feed this:

1. **Ground mesh is 500×500 units** (see
   `src/yars/view/gui/SceneGraphEnvironmentNode.cpp:23`,
   `createPlane(..., 500, 500, 50, 50, ...)`). The visible arena is
   ~8×8. So the visible ground is a tiny central patch of a huge
   mesh.
2. **`FocusedShadowCameraSetup`** sizes the shadow camera frustum to
   the visible scene, so the four walls project to the four edges
   of the shadow texture (the diamond border). With a 0.5m wall in
   an 8m arena and 45° light direction `(-1,-1,-1)`, the wall
   shadow on the floor extends only ~0.5m — about 6% of the floor.

Under that hypothesis, the receiver's UVs for the *visible* central
ground should map to the *center* of the shadow texture (the
diamond interior, all white). Wall silhouettes are at the diamond
border, which corresponds to ground points within ~0.5m of each
wall, but those ground points may either be outside the focused
shadow camera's view OR be sampling a thin border region that's
visually indistinguishable from full brightness at the test
resolution.

### What the next session should try (ranked)

1. **Runtime matrix dump.** Stop guessing. Log
   `shadowCamera->getViewMatrix()`,
   `shadowCamera->getProjectionMatrixWithRSDepth()`, and the
   ground's world matrix, both during the caster pass and during
   the receiver pass. Plug a known world point through both
   pipelines by hand. Compare. If the matrices are consistent, the
   bug is the hypothesis above (UV mismatch is real but small) and
   the fix is about widening the shadow camera frustum or biasing
   the receiver UVs. If the matrices diverge, the bug is
   elsewhere.
2. **Swap shadow camera setup.** Replace
   `FocusedShadowCameraSetup` with the default
   `DefaultShadowCameraSetup`. If wall silhouettes now occupy a
   larger fraction of the shadow texture, shadows should appear
   under the canonical `texture_worldviewproj_matrix` receiver
   math. Quick test, low risk.
3. **Boost wall heights** in `xml/braitenberg.xml` from 0.5 to 2.0
   temporarily. Bigger wall silhouettes → bigger shadow regions →
   if anything renders, the placement error (or correctness) will
   be obvious. Diagnostic, not a fix.

### Approaches we tried that did NOT work

- `texture_worldviewproj_matrix * vertex` (canonical Ogre RTSS) →
  UVs vary in [0,1] but always sample diamond interior → no
  shadows.
- `texture_viewproj_matrix * (worldMatrix * vertex)` (equivalent
  to the above) → same result.
- Y-flip of UV (`uv.y = 1 - uv.y` after divide; or
  `oShadowUV.y = oShadowUV.w - oShadowUV.y` before divide) → no
  shadows in any variant.
- X-flip of UV → no shadows.
- XY-swap of UV → no shadows.
- 2×2 grid of all four (xy / yx / x-flip / y-flip) tested in a
  single shader — all four quadrants returned ~1.0 sampling.

---

## Ogre source references

Files in `ext/ogre-source/`. Use these to verify any future change
matches what Ogre's own RTSS does.

- `Components/RTShaderSystem/src/OgreShaderExIntegratedPSSM3.cpp:251`
  — RTSS binds `ACT_TEXTURE_WORLDVIEWPROJ_MATRIX_ARRAY` for shadow
  receivers (the canonical auto-param name for combined
  light-projection × world).
- `Components/RTShaderSystem/src/OgreShaderExIntegratedPSSM3.cpp:315`
  — RTSS vertex shader does
  `oLightPos = mul(worldViewProjMatrices[i], objSpacePosition)`.
  Input is OBJECT space.
- `OgreMain/src/OgreAutoParamDataSource.cpp:703-734` —
  `getTextureViewProjMatrix(i)` =
  `CLIPSPACE2DTOIMAGESPACE * Frustum::getProjectionMatrixWithRSDepth()
   * Frustum::getViewMatrix()`. Note that the projection is pulled
  from the Frustum directly, NOT from `AutoParamDataSource::getProjectionMatrix()`,
  so it does NOT get the per-render-target Y-flip applied.
- `OgreMain/src/OgreAutoParamDataSource.cpp:736-750` —
  `getTextureWorldViewProjMatrix(i)` =
  `getTextureViewProjMatrix(i) * getWorldMatrix()`.
- `OgreMain/src/OgreMatrix4.cpp:55-59` —
  `Matrix4::CLIPSPACE2DTOIMAGESPACE` bakes a Y-flip in
  (`-0.5` on Y). This is what cancels with the render-target's
  Y-flip during the caster pass.
- `RenderSystems/GLSupport/include/OgreGLRenderTexture.h:7` —
  `requiresTextureFlipping() const override { return true; }` —
  every GL render-to-texture target. Confirms the caster's
  projection IS Y-flipped during the FBO render.
- `OgreMain/src/OgreAutoParamDataSource.cpp:366-374` — the
  per-render-target Y-flip is applied in
  `AutoParamDataSource::getProjectionMatrix()` (used by the caster
  pass via `worldviewproj_matrix`). It is NOT applied in
  `getTextureViewProjMatrix()`. In theory the bias-matrix Y-flip
  cancels the render-target Y-flip; end-to-end consistent.
- `OgreMain/src/OgreTextureShadowRenderer.cpp:1098` — during the
  receiver pass, `setTextureProjector(shadowCamera, 0)` is called
  before rendering each receiver. So `mCurrentTextureProjector[0]`
  IS the shadow camera at receiver-pass time, and
  `texture_*_matrix` auto-params resolve against it.

---

## Visual debug techniques used

All run with
`./bin/yars --xml ../xml/braitenberg.xml --geometry "1200x1200+0+0"
--capture --iterations 500` and frame extracted via ffmpeg.

1. **UV visualization.** Replace receiver fragment shader with
   `FragColor = vec4(uv.x, uv.y, 0.0, 1.0)`. Modulative blend
   multiplies the framebuffer, so dark uv values darken the
   floor. Reveals which UV range the ground actually samples.
2. **Shadow-texture-content visualization, world-projected.**
   Replace with `FragColor = vec4(v, v, v, 1.0)` where
   `v = texture(shadowMap, uv).r`. Any caster-written black pixel
   the receiver samples will make the floor black at that
   location. If the floor stays bright, the receiver is sampling
   only white texels.
3. **Shadow-texture-content visualization, screen-projected.**
   `vec2 screenUV = gl_FragCoord.xy / vec2(1200.0, 1200.0)` and
   sample. This shows the raw shadow texture stretched across the
   floor, ignoring the receiver's projection math. Confirms what
   the caster actually wrote, independent of the receiver's UV
   computation.
4. **World-vs-object position comparison.** Pass both
   `worldMatrix * vertex` and `vertex` to the fragment shader as
   colors, split screen vertically. If the two halves show
   different gradients, the world matrix is non-identity for that
   renderable.
5. **UV variant grid.** Quad-split the screen into xy / yx /
   x-flip / y-flip variants. Lets one frame test four hypotheses.

---

## Additional findings (filled in 2026-05-19 PM)

### Scene-specific facts that shape the bug

- **Ground does NOT cast shadows.** `SceneGraphEnvironmentNode.cpp:26`
  calls `_entity->setCastShadows(false)` on the ground entity. That
  is why the **interior of the diamond in the shadow texture is
  white**, not black. Only walls + dynamic objects (robot) write
  silhouettes. If the ground also cast shadows, the entire diamond
  would be filled black and modulative shadows would always blacken
  the floor unconditionally — wrong for the intended look. So this
  setting is correct and must stay.
- **Ground plane geometry.**
  `SceneGraphEnvironmentNode.cpp:20-23`:
  `Ogre::Plane(Ogre::Vector3::UNIT_Z, 0)`,
  `createPlane("ground", ..., plane, 500, 500, 50, 50, true, 1, 40, 40,
   Ogre::Vector3::UNIT_Y)`.
  Normal is +Z (Ogre coords — so the plane is horizontal in YARS-XY).
  Size 500×500. Texture tiles 40×. The 500×500 size is the likely
  source of the world-space-vs-object-space discrepancy noted in
  the receiver investigation.
- **Light direction.** `OgreHandler.cpp:271`
  `node->setDirection(Ogre::Vector3(-1, -1, -1))` — in **Ogre
  coordinates**, not YARS. Through the `YARS_TO_OGRE` macro
  (`src/yars/util/macros.h:32`, which swaps Y↔Z with a sign flip),
  this corresponds to YARS direction `(-1, +1, -1)`. Light rays go
  in this direction; the light source is at `(+1, +1, +1)` from
  origin (Ogre).
- **Wall geometry (braitenberg.xml).** Four walls at YARS positions
  `(±4, 0)` and `(0, ±4)`. Each: `height=0.5`, `width=7.75–8.25`,
  `depth=0.25`, `z=-0.15` (so the top of the wall is at `z=+0.35`).
  Walls are short. For a 45° light direction, a 0.5m wall casts a
  ~0.5m shadow on the floor — ~6% of the 8m arena width. Subtle
  even if perfectly placed.
- **YARS uses Z-up; Ogre uses Y-up.** `YARS_TO_OGRE`:
  ```cpp
  destination[0] =  source.x;   // x  ↦ x
  destination[1] =  source.z;   // z  ↦ y  (YARS up ↦ Ogre up)
  destination[2] = -source.y;   // -y ↦ z
  ```
  All Ogre-side rendering is in Ogre coords. Shadow camera, world
  matrices, projections — all Y-up. The conversion is uniform, so
  in principle it shouldn't break the shadow pipeline. Flagged in
  case it interacts with something we haven't checked.

### Concrete UV / world-coord measurements observed

These were the actual numbers from the visual UV-debug runs with
`texture_viewproj_matrix * (worldMatrix * vertex)` math (which is
mathematically equivalent to the canonical
`texture_worldviewproj_matrix * vertex`):

| Screen position             | UV observed (approx) |
| --------------------------- | -------------------- |
| Top-left of visible ground  | (0.3, 0.0)           |
| Top-right of visible ground | (1.0, 0.0)           |
| Bottom-left                 | (0.3, 1.0)           |
| Bottom-right                | (1.0, 1.0)           |
| World origin (screen center)| (~0.65, ~0.5)        |

So the visible ground covers about 70% of the U range and 100% of
the V range. The receiver's UV at world origin is **off-center**
in U (~0.65, not 0.5). This 0.15 offset in U is suspicious and is
the most concrete clue the next session has — figure out where it
comes from. Could be the `FocusedShadowCameraSetup` placing the
shadow frustum off-center, OR the world matrix being applied to
ground has an X translation, OR something else entirely.

### Shadow texture content observed (via screen-space gl_FragCoord overlay)

Rendering the shadow texture's content via
`screenUV = gl_FragCoord.xy / vec2(1200, 1200)` (i.e. stretching
the entire shadow texture across the floor as a screen-space sampler):

- A **black diamond outline** (rotated 45° square), vertices at
  approximately `(0.5, 0.0)`, `(1.0, 0.5)`, `(0.5, 1.0)`, `(0.0, 0.5)`
  in UV space. These are the four wall silhouettes from the light's
  POV.
- A **small black dot** near `(0.5, 0.7ish)` — the robot's
  silhouette.
- **White interior** of the diamond — the ground area not blocked
  by any caster.
- **White exterior** of the diamond — outside the focused shadow
  camera's view (or below z=0 / above the visible ground).

### Smoke tests that confirmed environment correctness

These eliminated possible explanations early:

- **Receiver shader IS running**: replaced fragment shader with
  `FragColor = vec4(0.3, 0.3, 0.3, 1.0)`. Floor became uniformly
  3× darker via the "scene_blend modulate" multiplication. Confirms
  the receiver pass is active and the shader is being compiled and
  linked.
- **Shadow texture IS populated**: the screen-space overlay
  (described above) showed clear silhouettes. Caster pass works.
- **The auto-param `texture_worldviewproj_matrix` IS bound**: it is
  in the auto-param table (`OgreGpuProgramParams.cpp:135`); the
  script parser accepts it without an explicit index, defaulting to
  0 (`OgreScriptTranslator.cpp:3902-3906`); and during the
  modulative receiver pass, `setTextureProjector(shadowCamera, 0)`
  IS called (`OgreTextureShadowRenderer.cpp:1098` via
  `resolveShadowTexture(texUnit, si, 0)` at line 215).

### Receiver material details worth knowing

`materials/YARSShadowReceiver.material`:

- `receive_shadows off` on the `YARS/TextureShadowReceiver`
  material itself. This is **correct** — the receiver material is
  the modulating pass; it shouldn't trigger another receiver pass
  on itself.
- `scene_blend modulate` — output is multiplied into the
  framebuffer. So any `FragColor` channel < 1.0 darkens the
  framebuffer at that channel.
- `depth_write off` — required; the receiver pass doesn't replace
  depth.
- Texture unit: `tex_address_mode border`,
  `tex_border_colour 1 1 1 1`, `filtering linear linear none`.
  **Critical for debugging**: UVs outside [0,1] sample the white
  border colour (1.0), which equals "no shadow". So if the
  receiver shader ever produces out-of-range UVs, the fragment
  outputs `mix(SHADOW_STRENGTH, 1.0, 1.0) = 1.0` = no modulation.
  This makes "wrong UV" indistinguishable from "no shadow" in the
  final image. The current shader has an explicit guard
  (`if uv < 0 || > 1: FragColor = 1`) that makes this explicit;
  removing that guard does not change the result.

### Caster fragment shader

`materials/shadow_caster.frag`:

```glsl
void main() { FragColor = vec4(0.0, 0.0, 0.0, 1.0); }
```

Caster writes **pure black** at the silhouettes. The shadow
texture is therefore essentially a 1-bit visibility map:
0 = caster blocks light, 1 = light reaches.

### Receiver fragment shader logic

```glsl
visibility = texture(shadowMap, uv).r;            // 0 in shadow, 1 elsewhere
modulation = mix(SHADOW_STRENGTH, 1.0, visibility);
                                                  // SHADOW_STRENGTH in shadow
                                                  // 1.0 elsewhere
FragColor = vec4(modulation, modulation, modulation, 1.0);
```

With `SHADOW_STRENGTH = 0.5`, a fully shadowed pixel becomes 50%
of original brightness. With `0.3` (briefly tried during debug),
30%.

### Ogre source line number for the GL FBO flip flag

(Correcting an imprecise reference in the earlier section:)
`RenderSystems/GLSupport/include/OgreGLRenderTexture.h` — the
override `bool requiresTextureFlipping() const override { return
true; }` is on the base `GLRenderTexture` class. All
`GL3PlusFBORenderTexture` instances inherit it (no override down
the chain).

### What the receiver-pass world matrix is for the ground

When the modulative receiver pass runs on the ground:

- `AutoParamDataSource::setCurrentRenderable(groundRenderable)` is
  called.
- `getWorldMatrix()` returns `groundRenderable->getWorldTransforms(...)`,
  which is the ground entity's accumulated SceneNode transform.
- For YARS, the ground entity is attached to a SceneNode created
  in `SceneGraphEnvironmentNode`. We didn't dump the actual matrix
  values; this is item 1 of the "next session" list above.

---

## Don't-do list (carried over from previous handover)

- Don't trust byte counts (mp4 size) as evidence of correct
  rendering. Always inspect a frame.
- Don't enable `setShadowCasterRenderBackFaces(true)` — it makes
  walls self-shadow their outward face.
- Don't drop the RTSS scheme tag from the YARS shadow materials —
  the `SGTechniqueResolverListener` will silently override them.
- Don't drop either of `setShadowTextureCasterMaterial` or
  `setShadowTextureReceiverMaterial` — both are required on GL3+ core.
