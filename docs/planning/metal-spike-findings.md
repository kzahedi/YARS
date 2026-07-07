# Metal Render System — Spike Findings and GO/NO-GO

**Date:** 2026-07-07 · **Spike branch:** `scratch/metal-spike` (deleted; raw
step-by-step evidence preserved in `metal-spike-findings-raw.md`)
**Plan:** `docs/superpowers/plans/2026-07-06-metal-render-system.md` (Phase 0)

## Verdict: NO-GO

Three **independent** blockers, each alone sufficient. The plan's Phase 1+
work does not proceed; see "What GO would cost" below.

## The three unknowns, answered

### 1. RTSS on Metal — BROKEN (expected)

RTSS has no MSL program writer (CG/GLSL only — verified in the vendored
fork AND upstream OGRECave master, checked 2026-07-07). At init RTSS logs
`No supported language found. Falling back to 'null'`; every
shader-generated material then fails, and the first `renderOneFrame()`
throws `InvalidStateException: Cannot begin frame - no viewport selected`
(OgreRenderSystem.cpp:504), repeating each frame. No YARS content can
render, since all our materials go through RTSS on a programmable pipeline.

### 2. Ogre's own Metal fallback shader — ALSO BROKEN (not expected)

The hexapod scene reached Ogre's bundled Metal default programs
(`MetalDefaultVP`/`MetalDefaultFP` from `DefaultShaders.metal`), which fail
to compile as real MSL:

```
Error: Metal Compiler in MetalDefaultVP:
DefaultShaders.metal:5:3: error: unknown type name 'vec4'
```

The bundled source is GLSL-flavored (`vec4`, `mat4`, `layout(location=…)`
via `OgreUnifiedShader.h` macros) and is never translated to MSL. This
means the Metal render system in this Ogre version was likely never
exercised end-to-end even by its own defaults — the gap is deeper than the
known RTSS limitation.

### 3. Shadow texture creation — BROKEN independently

Before any shader question arises, shadow setup fails at texture creation:

```
InvalidParametersException: Unsupported texture format in
MetalTextureBuffer::upload (OgreMetalHardwarePixelBuffer.mm:302)
```

Our shadow pipeline (`PF_FLOAT32_R` shadow texture) is not supported by the
Metal pixel-buffer upload path. The auto-param question (unknown #3 in the
plan) was unreachable — shadows die before shaders.

### Partial positive: SDL2 ↔ Metal windowing WORKS

The one thing that behaved: SDL window creation without `SDL_WINDOW_OPENGL`
plus passing the NSWindow (`syswm_info.info.cocoa.window`) as
`externalWindowHandle` creates the Ogre Metal window without a crash, both
scenes, full runs. If Metal is ever revisited, the window glue is a solved
problem.

## What GO would cost (decision for the project owner)

1. **An MSL `ProgramWriter` for RTSS** (or a GLSL→SPIR-V→MSL cross-compile
   path à la SPIRV-Cross) — does not exist upstream; this is a genuine
   OGRECave-scale contribution, weeks not days.
2. **Fixing the Metal RS's own defects** in the fork (broken
   `DefaultShaders.metal`, missing texture formats in the pixel-buffer
   upload path) — unknown depth; the component appears untested end-to-end.
3. Only then does the plan's actual porting work (Phases 1–2) begin.

Given that GL3+ works today and Apple's deprecation remains
bark-without-bite since 2018, the recommendation is to **shelve Metal**
and re-evaluate if either (a) upstream ships RTSS Metal support, or
(b) a macOS release actually breaks OpenGL.

## Worth reporting upstream

The `DefaultShaders.metal` MSL-compile failure and the RTSS "null"
fallback behavior reproduce on stock Ogre (fork is v14.5.2-79 + one
YARS patch unrelated to Metal). An OGRECave issue with the two verbatim
errors above would be a fair contribution.
