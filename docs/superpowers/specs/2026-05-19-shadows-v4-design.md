# Shadows v4 — Texture-Modulative with Hand-Rolled GLSL Caster

**Status:** design, awaiting user review
**Date:** 2026-05-19
**Branch:** `feat/linux-ci-verification` (= `master` at 6303d18)
**Driver:** Three prior shadow attempts (stencil + two texture-modulative
variants) failed. The last attempt (`a10c1b1`) produced a non-empty mp4
that was visually broken — sky-blue everywhere, sawtooth border, tiny
robot. Root cause hypothesis from `docs/planning/v0.8.7-open-points.md`:
RTSS is not emitting a working depth-only shader variant for the shadow
caster pass when applied to the stock fixed-function
`Ogre/TextureShadowCaster` material.

## Goal

Real dynamic shadow maps from YARS's single directional "sun" light,
cast by robots and walls onto the ground (and onto each other), using
Ogre 14's `SHADOWTYPE_TEXTURE_MODULATIVE` technique with a hand-written
GLSL 330-core shadow-caster material that bypasses RTSS entirely on the
caster pass.

## Non-goals

- Cascaded shadow maps / PSSM (one shadow texture is enough at the
  scale YARS scenes operate at).
- Soft shadows / PCF antialiasing (single-sample hard edges are
  acceptable for v4; can come later).
- Spot or point-light shadows (YARS scenes have only the directional
  sun; multi-light shadowing is out of scope).
- Touching the RTSS receiver path — receivers continue to use the
  existing RTSS-generated materials unchanged.

## Architecture

Three layers, each with one job:

1. **Caster pipeline (new).** A dedicated `materials/` script defines
   `YARS/TextureShadowCaster` with explicit `vertex_program_ref` /
   `fragment_program_ref` pointing at two new GLSL 330-core programs
   (`shadow_caster.vert`, `shadow_caster.frag`). Because the passes
   already have explicit shaders, RTSS's auto-technique listener never
   fires for this material.

2. **Receiver pipeline (modified).** Every YARS material created by
   `MaterialManager` gets a per-pass call to
   `setShadowCasterMaterial("YARS/TextureShadowCaster")`. This tells
   Ogre to swap to our caster when rendering the shadow texture.
   Receivers still render through their existing RTSS-generated
   techniques. `Material::setReceiveShadows(true)` is Ogre's default
   so no explicit call is needed; transparent/effect materials that
   should not receive shadows (proximity sensor overlays, traces,
   flares) already opt out via `MaterialParams::receiveShadows{false}`
   plus `setShadowCasterMaterial` is skipped when
   `params.castShadows` is false.

3. **Scene config (modified).** `OgreHandler::__setScene` swaps the
   current `SHADOWTYPE_NONE` for
   `SHADOWTYPE_TEXTURE_MODULATIVE`, with deliberate defaults:
   - shadow texture 1024×1024, count 1
   - shadow far distance 50 (scene-scale appropriate)
   - shadow colour `(0.5, 0.5, 0.5)` — mid-grey, modulative.
     This is the first-pass tuning value; may need adjustment after
     visual review (lighter = subtler shadows, darker = harder).
   - the directional sun already has the correct world-space
     direction `(-1, -1, -1)`; no change to the light.

## Components

### New files

```
materials/shadow_caster.vert        # GLSL 330 core — clip-space transform
materials/shadow_caster.frag        # GLSL 330 core — write black
materials/YARSShadowCaster.material # Defines YARS/TextureShadowCaster
```

`shadow_caster.vert`:

```glsl
#version 330 core
uniform mat4 worldViewProjMatrix;
in vec4 vertex;
void main() { gl_Position = worldViewProjMatrix * vertex; }
```

`shadow_caster.frag`:

```glsl
#version 330 core
out vec4 FragColor;
void main() { FragColor = vec4(0.0, 0.0, 0.0, 1.0); }
```

`YARSShadowCaster.material`:

```
vertex_program YARS/ShadowCasterVP glsl
{
  source shadow_caster.vert
  default_params { param_named_auto worldViewProjMatrix worldviewproj_matrix }
}

fragment_program YARS/ShadowCasterFP glsl
{
  source shadow_caster.frag
}

material YARS/TextureShadowCaster
{
  receive_shadows off
  technique
  {
    pass
    {
      vertex_program_ref YARS/ShadowCasterVP {}
      fragment_program_ref YARS/ShadowCasterFP {}
    }
  }
}
```

### Modified files

`src/yars/view/gui/MaterialManager.cpp`:

- `_createBasicRTSSMaterial(...)` — after `pass = technique->createPass()`,
  if `params.castShadows` (default true) is true, set
  `pass->setShadowCasterMaterial("YARS/TextureShadowCaster")`.
- `createRTSSForLegacyMaterials()` — inside `_createRTSS`, after RTSS
  technique creation, iterate every pass of the new RTSS technique and
  call `setShadowCasterMaterial(...)` unless one is already set or the
  material is the caster itself (avoid recursion).

`src/yars/view/gui/OgreHandler.cpp::__setScene` (around line 296–330):

- Replace the comment block + `setShadowTechnique(SHADOWTYPE_NONE)` with:

```cpp
_sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
_sceneManager->setShadowTextureSize(1024);
_sceneManager->setShadowTextureCount(1);
_sceneManager->setShadowFarDistance(50.0f);
_sceneManager->setShadowColour(Ogre::ColourValue(0.5f, 0.5f, 0.5f));
```

The existing resource-group plumbing (`YARS_OGRE_MEDIA_DIR`,
`OgreInternal` for `spot_shadow_fade.dds`, null-checked
`prepareForShadowVolume` loops in `SceneGraph*Node.cpp`) stays — it's
already correct for this technique.

## Data flow per frame

1. Ogre starts a shadow-map render pass for the directional sun.
   For each shadow-casting renderable in range, it temporarily
   substitutes that renderable's material with
   `YARS/TextureShadowCaster`.
2. Caster vertex shader transforms vertex positions by
   `worldViewProjMatrix`; caster fragment shader writes black.
3. Shadow texture now contains "black where occluders are, untouched
   elsewhere" from the light's POV.
4. Main scene render proceeds as today — RTSS-generated materials,
   directional lighting, etc.
5. A modulating pass darkens receivers using the shadow texture and
   the configured shadow colour.

## Error handling

- If the new `.material` / `.vert` / `.frag` files fail to load (file
  missing, GLSL compile error), Ogre raises during resource init.
  Catch the throw in `OgreHandler::setupSceneManager` around the
  caster-material lookup and **fall back to `SHADOWTYPE_NONE`** with a
  loud `std::cerr` message. The scene must still render.
- If `setShadowCasterMaterial(...)` throws inside `MaterialManager`,
  catch per-material and log, continue with the rest.
- `_sceneManager->setShadowTechnique(...)` itself wraps in
  `try`/`catch` and reverts to `SHADOWTYPE_NONE` on any throw.

## Testing & verification

**Mandatory order; "build passes" alone is not success.**

1. **Build** — `cd build && make -j8`. Must compile clean.
2. **Headless regression** —
   `./bin/yars --iterations 1000 --xml ../xml/braitenberg_logging.xml --nogui`.
   CSV diff against `xml/reference_logfile.macos-arm64.csv` must be
   bit-identical (shadows don't touch physics).
3. **GUI smoke test** —
   `timeout 10 ./bin/yars --xml ../xml/braitenberg.xml`.
   Must print "Good bye" and exit cleanly. No segfault, no missing
   plugins.
4. **Visual verification (mandatory before claiming success)** —
   capture a frame via `--writeFrames` or `--capture`, open the image,
   and confirm visually:
   - Ground texture visible (not sky-blue replacing it).
   - Walls visible with proper shading.
   - **Dark patch under each robot/cylinder on the ground.**
   - No sawtooth border, no missing geometry.

   "The mp4 has nonzero bytes" is not a substitute for opening a
   frame. This is the lesson from the v3 failure.

5. **Both runners** — push to `feat/linux-ci-verification`, wait for
   the CI mp4 artefact, eyeball both Linux and macOS frames.

## Risks and unknowns

- **YARS materials lack explicit `iteration once_per_light` setup.**
  Modulative texture shadows in Ogre 14 don't strictly require it for
  a single directional light, but if shadows render only on the
  ambient pass and not on the lit surfaces, we'll see weird banding.
  Mitigation: visually inspect the GUI first frame.
- **Manual objects in YARS use `OT_TRIANGLE_STRIP` / `OT_TRIANGLE_FAN`.**
  Ogre's shadow map render supports these, but the
  `prepareForShadowVolume` path (already in tree for the stencil
  attempt) was specifically a stencil thing. The texture path
  shouldn't care — but we'll know from the visual test.
- **Transparent materials (proximity / LDR sensors, trace lines,
  flares) must not cast shadows.** Already covered by the existing
  `castShadows = false` flags. Sanity-check the design respects that
  by skipping `setShadowCasterMaterial` when `params.castShadows` is
  false.

## Rollback

A single revert of the shadow-enable commit returns to
`SHADOWTYPE_NONE`. The .material/.vert/.frag files are inert when
unreferenced; they can stay in tree.

## Out of scope (explicitly deferred)

- PSSM / cascaded shadow maps
- Soft shadows
- Shadow bias / acne tuning (use Ogre defaults first; revisit only if
  the visual test shows acne)
- Spot/point light shadows
- Multi-light shadow accumulation
