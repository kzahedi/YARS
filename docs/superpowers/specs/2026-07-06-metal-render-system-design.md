# Metal Render System — Design

**Date:** 2026-07-06
**Status:** Approved (brainstorm session 2026-07-06)
**Sequencing:** After the CI and hardening sub-projects; independent of
the raycast and JSON work. Long-running.

## Motivation

Apple has kept OpenGL deprecated since 2018; the GL3+ renderer works
today but can break in any macOS release. Ogre 14 ships a Metal
RenderSystem. This is insurance work, scheduled deliberately — not an
emergency.

## Mode decision

**Coexistence, GL default.** The Metal plugin is built alongside
GL3Plus; a runtime switch selects the renderer; GL3+ stays the default
until Metal reaches full parity and has soaked in regular use. Flipping
the default is a deliberate later commit, not part of this work.

## Phase 0 — pre-flight spike (go/no-go gate)

A throwaway branch answers three unknowns cheaply before any real port:

1. **RTSS on Metal:** does Ogre 14's Runtime Shader System generate
   working shaders for the Metal render system on our scenes, or is it
   GLSL-centric in practice?
2. **SDL2 + Metal windowing:** does SDL2's Metal path
   (`SDL_Metal_CreateView` / CAMetalLayer) integrate with Ogre's Metal
   window the way the Cocoa/NSOpenGLContext path does today?
3. **The shadow auto-param:** does `texture_worldviewproj_matrix` work
   on Metal? Its GL3+/macOS-arm64 breakage is what forced the
   `ShadowMapper` C++ matrix-push workaround (see
   `docs/planning/shadows_state.md`).

Deliverable: one textured scene with one shadow rendering under Metal.
**Gate:** if RTSS-on-Metal is fundamentally broken, stop, document the
finding, re-evaluate (and consider reporting upstream to OGRECave).

## Phase 1 — plugin coexistence

- Build `RenderSystem_Metal` into the Ogre install
  (`ext/ogre/install`) alongside GL3Plus on macOS.
- Extend `src/cfg/plugins.cfg.in` to list both plugins on macOS.
- Add a `--renderer metal|gl` CLI flag (default `gl`) that selects the
  render system at `OgreHandler` initialization.
- Linux untouched: `gl` is the only valid value there.

## Phase 2 — feature parity, in dependency order

1. Basic scene + RTSS materials (textures, lighting).
2. Overlays and fonts (OSD).
3. Image export (`--capture` frame path) — this is also the validation
   instrument for the remaining steps.
4. Video capture (FFmpeg pipeline fed from Metal-rendered frames).
5. Shadows, last and deepest. The Phase 0 finding decides the shape:
   - If the auto-param works on Metal: the receiver uses it directly;
     `ShadowMapper` becomes GL-only legacy.
   - If not: `ShadowMapper` stays; the custom caster/receiver shaders
     get MSL twins alongside the GLSL originals.

## Acceptance

- The three reference scenes (braitenberg, falling objects,
  hexapod-with-shadow) exported as PNGs under Metal, visually
  equivalent to the GL references
  (`docs/planning/state-2026-06-11-*.png` and successors).
- Video capture produces a playable mp4 under Metal.
- Performance at least on par with GL3+ on the same scenes.
- GL path unchanged: existing image-export references still match under
  `--renderer gl` (default).

## Out of scope

- SDL3 migration (dropped from the 2026-07-06 batch; revisit after this
  work).
- Removing GL3+ or flipping the default renderer.
- Any Linux rendering change.
