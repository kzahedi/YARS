# Shadows v5 — comprehensive plan (2026-05-21 PM)

After three sessions of failed attempts at making `SHADOWTYPE_TEXTURE_*`
on Ogre 14 + GL3+ core produce correct shadows, this document
consolidates **every** approach worth trying — from cheap one-day
experiments to multi-week engine rewrites — ranked by effort vs.
expected payoff. Pick the path; the steps are concrete.

## TL;DR

1. **Free first move: rebase `ext/ogre-source` onto current master**
   (`922f5b6`, ~63 commits ahead of our pin). Several touch RTSS. May
   fix our problem for free. Probably won't, but it's a 5-minute test.
2. **If that doesn't help, build approach A (custom top-down RTT)**
   — bypasses Ogre's shadow framework entirely, ~200-300 LOC, no
   dependency on broken auto-params, works for the floor *and* walls.
3. **Guaranteed fallback: planar projected shadows (approach B)**
   — ~100 LOC, crisp, perfect, but floor only. The "we definitely
   ship something" option.

Avoid: trying to make Ogre's built-in `texture_worldviewproj_matrix`
work on this platform; trying depth shadows again until issue #3442
is patched; porting to OgreNext as a shortcut.

## 0. Snapshot — where we are

* **Caster pass works** (proven: dumped 2048² shadow texture to PNG —
  clean diamond outline + robot dot on white).
* **Receiver UV math is broken** (proven: hand-edited fragment shader
  outputs `lShadowFactor` uniformly ≈1.0 across the visible ground,
  even directly under the robot; UVs (debug-visualised as RGB) are
  smooth and in [0,1] but sample white interior, not the silhouettes).
* **Switching custom receiver → RTSS-generated receiver does not
  help** — RTSS uses the same broken `ACT_TEXTURE_WORLDVIEWPROJ_MATRIX_ARRAY`
  auto-param.
* **Switching camera setup does not help** — `DefaultShadowCameraSetup`,
  `FocusedShadowCameraSetup`, `PlaneOptimalShadowCameraSetup`, and a
  hand-rolled `YarsFixedShadowCameraSetup` all exhibit the same
  symptom.
* **Depth shadows blow up** — `SAMPLER2DSHADOW` macro doesn't expand
  on GL3+ core (issue #3442 family).
* **Stencil shadows blow up** — `OGRE_UNIFORMS` / `MAIN_PARAMETERS`
  don't expand in the stencil-volume extrude shaders (same issue
  family).

Current commit: `8149349` — shadows set to `SHADOWTYPE_NONE`, all the
correct RTSS wiring left in place (light count seeded, template
sub-render-states added, init order corrected, ground material has
explicit lighting attributes). Re-enabling once the underlying bug is
fixed is a one-line change.

## 1. Quick wins — each ≤1 day

### 1A. Rebase Ogre submodule onto current master

* **What:** Update `ext/ogre-source` from `211d902` (Aug 2025) to
  `922f5b6` (HEAD). 63 commits behind. Several touch RTSS:
  * `eeecde7 GL3Plus: autodetect shader sampler bindings`
  * `5e3fdb8 RTSS: cloneShaderBasedTechniques - also copy non-FFP marks`
  * other RTSS-area fixes since Aug.
* **Effort:** ~30 min for the rebase + rebuild. Probably some
  `OgreManualObject.cpp` patch conflict (we have a local null-guard).
* **Expected payoff:** Low-to-medium. There's no specific commit
  matching our symptom in the changelog, but RTSS area churn is high
  and our bug isn't documented anywhere — could be a silent fix.
* **Risk:** Other regressions from a Bigger Ogre. Easy to revert.
* **Verdict:** Do it first. It's basically free.

### 1B. Apply paroj's workaround for issue #3442

* **What:** Issue #3442 (open, Sep 2025) describes an `OGRE_UNIFORMS`
  macro-expansion-order bug. paroj's workaround: move `#ifdef` blocks
  *outside* `OGRE_UNIFORMS(...)`. The shaders that affect us are in
  `ext/ogre-source/Media/Main/`:
  * `ShadowExtrudeDirLightFinite.vert` (and friends) — for stencil
  * `Ogre/ShadowBlend*.vert/frag` — for stencil
  * `DepthShadowmap.material`-referenced shaders — for depth shadows
* **Effort:** ~1 hour. Read the source, rewrite the `OGRE_UNIFORMS`
  blocks, rebuild.
* **Expected payoff:** Unlocks **either** stencil shadows or
  depth-format texture shadows. Stencil is the more interesting unlock
  because YARS already has the `prepareForShadowVolume()` infrastructure
  intact and stencil avoids the UV-math bug entirely.
* **Risk:** The patch may need to live in our vendored copy of Ogre
  permanently. Document it.
* **Verdict:** Do it second. Issue #3442 is exactly our preprocessor
  problem.

### 1C. Diagnostic: confirm `requiresTextureFlipping()`

* **What:** Add a one-shot `std::cerr` print of
  `mActiveRenderTarget->requiresTextureFlipping()` during the caster
  pass. If it's `false` on macOS arm64 GL3+, the Y-flip cancellation
  is broken in our specific Ogre + platform combo — file an upstream
  issue with a minimal repro.
* **Effort:** 20 min.
* **Expected payoff:** Won't fix anything by itself, but tells us
  whether we have a platform bug worth filing.
* **Verdict:** Bundle with 1A — cheap data.

### 1D. CSMShadows-style recipe (modified to single cascade)

* **What:** The `Samples/CSMShadows/` sample uses
  `SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED` with `PF_FLOAT32_R`
  (NOT depth-format) and **hand-written receiver materials** that do
  manual depth comparison with a plain `sampler2D`. This sidesteps
  the `SAMPLER2DSHADOW` macro-expansion problem AND (potentially)
  the auto-param UV bug if the receiver does its own UV computation.
* **Files to study:**
  * `ext/ogre-source/Samples/CSMShadows/include/CSMShadows.h:124-155`
  * `ext/ogre-source/Samples/CSMShadows/include/ShadowCameraSetupStableCSM.cpp`
  * `ext/ogre-source/Samples/Media/CSMShadows/materials/scripts/CSM.material`
* **Effort:** ~6-8 hours. Simplify to 1 cascade, port the receiver
  material to YARS materials. Same approach as approach A below but
  inside Ogre's shadow framework.
* **Expected payoff:** Medium. Same UV-math caveats apply if the
  CSM material relies on `texture_worldviewproj_matrix`. Need to
  read the CSM material to know.
* **Verdict:** Try after 1A+1B. If 1A or 1B unblocks something, may
  not need this.

### 1E. Browse Ogre forum threads from a different network

* **What:** Threads identified by research but unreachable from this
  network (HTTP/2 issue):
  * `forums.ogre3d.org/viewtopic.php?t=97229` "SHADOW MAPPING"
  * `forums.ogre3d.org/viewtopic.php?t=80787` "shadows and gl3plus renderer [SOLVED?]"
  * `forums.ogre3d.org/viewtopic.php?t=96589` "Integrated Texture Shadows full self shadowing"
  * `forums.ogre3d.org/viewtopic.php?t=97451` "1.8.2 to 14.3 issues"
* **Effort:** 30 min on a phone/different connection.
* **Expected payoff:** One of these may have the exact fix.
* **Verdict:** Try in parallel with 1A. Zero engineering cost.

## 2. Real fixes — 1-3 days each

### Approach A. Custom top-down RTT with hand-rolled UVs ⭐ (RECOMMENDED)

The cleanest path to working shadows. Bypass Ogre's shadow framework
entirely. We control every pixel of the pipeline.

**Architecture:**

```
+----------------------------+
| Ogre SceneManager          |
|                            |
|   eye Camera --(renders)-->| -> screen
|                            |
|   light Camera (ortho top) | -> shadow_rt (custom RTT)
|     |                      |
|     +-- RenderTargetListener: forces every renderable to use a
|         BLACK_ON_WHITE material variant
|                            |
|   ground material samples shadow_rt with:
|     uv = (worldXZ - arenaMin) / arenaSize
+----------------------------+
```

**Step-by-step:**

1. Set `SHADOWTYPE_NONE` (already done in `8149349`).
2. Create a second `Camera` in the same scene manager — `lightCam`.
   Set ortho projection (e.g. 12×12 m), positioned at `(0, +50, 0)`
   in Ogre coords, looking straight down (`-UNIT_Y`). This bakes in
   the orthographic-down assumption; for a directional light from
   `(-1,-1,-1)` we can either tilt this camera *or* keep it
   top-down and accept that walls cast slightly-extruded shadows
   when sampled by the floor.
3. Create a `TexturePtr` via `TextureManager::createManual` with
   `TU_RENDERTARGET`, format `PF_R8` (or `PF_BYTE_RGB`), 1024² or
   2048².
4. Attach `lightCam` to the texture's render target via
   `getBuffer()->getRenderTarget()->addViewport(lightCam)`.
5. Set this viewport's clear color to white (`1,1,1,1`) — "no
   shadow" baseline.
6. Install a `RenderTargetListener` on the shadow RTT:
   * `preRenderTargetUpdate`: set every visible scene
     entity's material scheme to `YARS_SHADOW_CAST` (or
     similar). For each entity, store its current scheme and
     restore it in `postRenderTargetUpdate`.
   * Alternatively: use Ogre's scheme system — set
     `mViewport->setMaterialScheme("yars-shadow-cast")` on
     the shadow viewport and provide a black-on-white "shadow
     cast" technique for every YARS material via a single
     `MaterialManager::Listener` that auto-generates it.
7. Update the floor material:
   * Add a second `texture_unit` referencing the shadow RTT.
   * Add a custom GLSL vertex+fragment shader pair (we
     already have `shadow_receiver.{vert,frag}` we can repurpose).
   * Vertex shader: pass `worldPos.xz` to the fragment.
   * Fragment shader: `uv = (worldPos.xz - arenaCenter) /
     arenaSize + 0.5;` then sample `shadow_rt` at that UV;
     multiply `gl_FragColor` by `mix(shadowStrength, 1.0,
     shadowSample)`.

**LOC estimate:** 200-300 (mostly the listener + the GLSL).

**Pros:**
* No dependency on `texture_worldviewproj_matrix` — UV math is in
  our code.
* No dependency on RTSS shadow_mapping SRS, no `OGRE_UNIFORMS` —
  we write plain GLSL.
* No dependency on Ogre's `setShadowTexture*` framework.
* Trivially extends to wall shadow receivers (add a second texture
  unit + UV math per wall) or to per-wall RTTs.
* Renders at any size we want, at any frequency we want, in any
  format we want.

**Cons:**
* Top-down orthographic only by default; need extra work for true
  directional-light shadows from oblique angles. Acceptable for YARS.
* Two render passes per frame (the cost is fine for our scene size).
* Need a "shadow cast" material variant per renderable. Worst case
  we add a `MaterialManager::Listener` that creates one on demand.

**Files to create/modify:**
* `src/yars/view/gui/OgreHandler.cpp` — create RTT, listener
* `src/yars/view/gui/ShadowMapping.{h,cpp}` (new) — listener + RTT
  management
* `materials/YARSGroundShadow.material` (new) — floor receiver with
  custom shader
* `materials/yars_ground_shadow.{vert,frag}` (new)
* Plus a "shadow cast" technique applied to all opaque materials.

**Estimated wall-clock: 1-2 days including testing.**

### Approach B. Planar projected shadows

Floor-only, but **guaranteed to work and look great**. Use as a
fallback if A turns out to be more invasive than expected.

**Math:** For a plane `(n, d)` and a directional light direction `l`,
the shadow matrix `M` that projects geometry onto the plane is:

```
M[i][j] = (i == j ? n·l : 0) - l[i] * n[j]
```

For our floor `(n=(0,1,0), d=0)` and light direction `(-1,-1,-1)`:

```c++
Ogre::Matrix4 planarShadowMatrix(const Ogre::Plane& plane,
                                 const Ogre::Vector4& light)
{
    float d = plane.normal.x*light.x + plane.normal.y*light.y +
              plane.normal.z*light.z + plane.d*light.w;
    Ogre::Matrix4 m;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
        {
            float pj = (j==0?plane.normal.x:j==1?plane.normal.y:
                        j==2?plane.normal.z:plane.d);
            float li = (i==0?light.x:i==1?light.y:i==2?light.z:light.w);
            m[i][j] = (i==j?d:0) - li * pj;
        }
    return m;
}
```

**Step-by-step:**

1. Set `SHADOWTYPE_NONE`.
2. For every shadow-casting entity in the scene graph
   (`SceneGraphBoxNode`, `Capsule`, `Cylinder`, `Mesh`, `Muscle`,
   `Ply`, `SoftPly`), after creating its primary entity, create a
   second `Entity` from the same mesh — call it `shadowProxy` —
   with material `YARS/PlanarShadow` (defined below) and depth bias
   `-1` to avoid z-fight.
3. Attach the shadow proxy to a new `SceneNode` whose transform is
   updated every frame to `planarShadowMatrix * caster.world`.
   Easy way: cache the proxy's `SceneNode` and after computing the
   caster's world matrix each frame, set the proxy node's transform
   directly.
4. `YARS/PlanarShadow` material: black, alpha blended, depth_write off,
   scene_blend alpha_blend, simple `vec4(0,0,0,0.5)` fragment shader
   (RTSS-generated trivially OK).

**LOC estimate:** ~100. About 30 lines for the math + 50 for the
per-caster proxy creation + 20 for the material.

**Pros:**
* Guaranteed to work — no Ogre shadow code involved at all.
* Crisp, exact placement on the floor.
* Cheap (~equal cost to rendering casters again, no shadow texture).
* No new RTT, no new pipeline.

**Cons:**
* Floor receiver only. Walls and the robot's underside don't get
  shadows. For the YARS arena, this is mostly fine — the floor is
  the dominant receiver.
* Hard-edged only — no soft shadows.
* Self-shadowing on the floor (robot shadow draws on top of robot
  if proxy renders second). Manage with render queue ordering.
* Each caster needs a "proxy" entity twin in the scene graph.

**Verdict:** Backup option. Always works. Use if A becomes more than
2 days of work.

### Approach C. Hand-written GLSL stencil shadow volumes

Use YARS' existing `prepareForShadowVolume()` infrastructure that's
already wired up in every `SceneGraph*Node`. Bypass Ogre's broken
built-in extrude shaders.

**Step-by-step:**

1. Write our own `YARS/ShadowExtrudeDirLight.{vert,frag}` shader
   pair without `OGRE_UNIFORMS` or `MAIN_PARAMETERS`. Vertex shader
   takes vertex + edge-list metadata, extrudes silhouette edges
   along the light direction.
2. Replace Ogre's built-in `Ogre/ShadowExtrudeDirLight` material
   reference with `YARS/ShadowExtrudeDirLight`.
3. Set `SHADOWTYPE_STENCIL_ADDITIVE` (or `_MODULATIVE`).
4. Restore the `OgreManualObject.cpp` null-guard patch we already
   have.
5. The `ManualObject`-built geometry (walls, robot body parts)
   needs to have its edge list correctly built — we already call
   `prepareForShadowVolume()`.

**LOC estimate:** ~500-800 GLSL + a small C++ wiring patch.

**Pros:**
* Pixel-perfect shadows on every surface (floor, walls, casters
  themselves).
* No texture, no UV math, no projection mismatch.

**Cons:**
* Stencil shadows are notorious for edge cases — open meshes
  break, silhouette edge extraction is fragile.
* Hard-edged only.
* Slower than texture shadows for high-poly scenes (irrelevant
  for YARS).

**Verdict:** Only if we want the most accurate possible result and
have a day to dedicate. Approach A is more flexible.

### Approach D. Per-receiver blob shadow decals

Cheap and dirty: a soft-circle texture rendered on the floor under
each dynamic caster.

**LOC estimate:** ~50 (a sprite per caster + position update).
**Pros:** Trivial.
**Cons:** Ugly. Doesn't help walls. Doesn't reflect shape.
**Verdict:** Use as a stand-in *during* development of A or B.

## 3. Multi-day investments

### Approach E. Port to OgreNext (Ogre 2.x/3.x)

OgreNext has a completely different shadow path: compositor-driven
`OgreShadowNode*`, HLMS instead of `.material`, no FFP, no RTSS. The
shadow code is reportedly working modernly.

**Cost:**
* Material system rewrite: every `.material` file → HLMS `.material.json` + `.any` datablocks.
* Scene management API differences.
* No more `Observer`-style or legacy v1 APIs.
* Some YARS code paths (the `ManualObject`-based scene graph) may need restructuring.

**Estimated wall-clock: 2-4 weeks**, depending on which features
break.

**Verdict:** Don't do this just to get shadows. Do it if there's
broader value (better performance, Vulkan/Metal, modern PBR). Not a
near-term option.

### Approach F. Replace Ogre with bgfx / raylib / magnum / sokol_gfx / hand-rolled GL

YARS uses Ogre only for visualization. Physics is Bullet (separate).
Replacing the renderer means:

* Re-implementing the scene graph (currently Ogre-based via
  `SceneGraph*Node` classes).
* Re-implementing the camera / window / SDL2 bridge.
* Re-implementing materials, textures, lights, mesh loading.
* Keeping the rest of YARS (config, physics, controllers) untouched.

**Cost: 3-6 weeks** depending on engine choice. raylib has the
simplest learning curve; bgfx has the most performance/features;
magnum is the closest to Ogre in scope.

**Verdict:** Out of scope for a shadow fix. Worth filing as a future
"engine evaluation" project if Ogre keeps fighting us across other
features too.

### Approach G. Switch Ogre's render system to Vulkan / Metal

Ogre 14 has both Vulkan and Metal render systems. Both go through
the same `OgreUnifiedShader.h` and RTSS code paths — research
indicates the same macro-expansion issues likely affect them.

**Verdict:** Don't expect a fix from this.

## 4. Recommended path

The cheapest experiment that's most likely to fix everything:

| # | Action | ETA | Likelihood of fix |
|---|--------|-----|-------------------|
| 1 | Rebase `ext/ogre-source` to current master (1A) | 30 min | Low (~10%) |
| 2 | Add `requiresTextureFlipping()` debug print (1C) | 20 min | Diagnostic only |
| 3 | Re-enable shadows with rebased Ogre, confirm symptom unchanged | 30 min | — |
| 4 | Browse the 4 forum threads from another network (1E) | offline | Medium |
| 5 | Apply paroj's `OGRE_UNIFORMS` workaround in stencil-shadow shaders (1B) | 1-2 hr | Unlocks stencil |
| 6 | Test stencil shadows with patched shaders | 2 hr | High **if** 1B works |
| **7** | **If 1-6 fail: build approach A (custom RTT)** | **1-2 days** | **High (~90%)** |
| 8 | If approach A is delayed: ship approach B (planar) as fallback | 1 day | Definite |

**Default plan if no preference:** do 1-3 in parallel (1A + 1C +
attempt to read forum), then 4-6 if needed, then A. Approach B as
the "we definitely ship this week" backstop.

## 5. Risks and what to NOT do

* **Don't** keep adding empirical UV transforms to custom shaders.
  We've proven the bug is upstream of the receiver.
* **Don't** invest in OgreNext or engine replacement just to get
  shadows. Bigger projects with their own value can fund those.
* **Don't** try depth shadows again until issue #3442 is patched
  (locally or upstream). Confirmed broken.
* **Don't** force-merge the Ogre rebase if it breaks the rest of
  YARS. Maintain a separate branch for it.

## 6. Source links

### Confirmed upstream issues
* [#3442 OGRE_UNIFORMS macro-expansion bug (open, Sep 2025)](https://github.com/OGRECave/ogre/issues/3442) — this is our `OGRE_UNIFORMS` / `MAIN_PARAMETERS` blocker.
* [#3379 Introduce Shadow scheme (open, paroj Jun 2025)](https://github.com/OGRECave/ogre/issues/3379) — paroj acknowledges integrated-shadow + RTSS needs architectural rework.
* [#1643 Auto-generated shadow-caster material leaks (open)](https://github.com/OGRECave/ogre/issues/1643) — confirms our `setShadowTextureCasterMaterial` workaround is the recommended path.
* [#2182 shadow-caster material un-initialised (open)](https://github.com/OGRECave/ogre/issues/2182).

### Working Ogre 14 references
* [Samples/CSMShadows](https://github.com/OGRECave/ogre/tree/master/Samples/CSMShadows) — the closest "just works on GL3+" recipe.
* [Samples/Simple/Shadows.h](https://github.com/OGRECave/ogre/blob/master/Samples/Simple/include/Shadows.h) — what variants work.
* [Custom shadow mapping wiki](https://wiki.ogre3d.org/Custom+Shadow+Mapping) — reference for approach A.

### Planar / non-Ogre approaches
* [Planar projected shadows tutorial (Erkaman)](https://erkaman.github.io/planar_proj_shadows/) — math for approach B.
* [SIGGRAPH 97 Projection Shadows notes](https://www.opengl.org/archives/resources/code/samples/advanced/advanced97/notes/node100.html) — classic reference.
* [GLSL Programming: Shadows on Planes](https://en.wikibooks.org/wiki/GLSL_Programming/Unity/Shadows_on_Planes) — same trick in GLSL.

### Forum threads to try from a different network
* [t=97229 SHADOW MAPPING](https://forums.ogre3d.org/viewtopic.php?t=97229)
* [t=80787 shadows and gl3plus renderer [SOLVED?]](https://forums.ogre3d.org/viewtopic.php?t=80787)
* [t=96589 Integrated Texture Shadows](https://forums.ogre3d.org/viewtopic.php?t=96589)
* [t=97451 Ogre 1.8.2 to 14.3 issues](https://forums.ogre3d.org/viewtopic.php?t=97451)
