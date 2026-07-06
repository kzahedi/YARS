# Metal Render System Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Bring up Ogre 14's Metal render system alongside GL3+ on macOS behind a `--renderer` flag (GL stays default), reaching feature parity: materials, overlays, image export, video capture, shadows.

**Architecture:** Phase 0 is a throwaway spike answering three go/no-go unknowns (RTSS-on-Metal, SDL2 Metal windowing, the shadow auto-param). Phase 1 wires plugin coexistence. Phase 2 walks parity in dependency order, using the existing PNG-export references as the acceptance instrument. Linux is untouched.

**Tech Stack:** Ogre 14 (`ext/ogre-source` fork kzahedi/ogre), `RenderSystem_Metal`, SDL2 (`SDL_Metal_CreateView`), RTSS, MSL where hand-written shaders are needed.

**Spec:** `docs/superpowers/specs/2026-07-06-metal-render-system-design.md`

## Global Constraints

- Branch: `feat/metal-renderer` for real work; spike happens on `scratch/metal-spike` (deleted afterwards, findings survive as a committed doc).
- macOS-only feature; every task must leave the Linux build untouched and the macOS GL path bit-identical in behavior (default renderer stays `gl` until a separate, later decision).
- ALWAYS build/run in `./build`; ALWAYS timeout yars runs; never claim success unless yars compiles and runs.
- GUI-heavy plan: visual acceptance = image export compared against the GL-rendered references (`docs/planning/state-2026-06-11-*.png` show target look; exact comparison images are regenerated per Task).
- Ogre rebuilds are expensive (~20+ min): reuse `ext/ogre/build` incrementally; only reconfigure when flags change.
- Commit per task; no pushes without user approval.
- Key existing code: `src/yars/view/gui/OgreHandler.cpp` (render system selection + Root init), `src/yars/view/gui/SdlWindow.cpp` (SDL↔Ogre window glue), `src/cfg/plugins.cfg.in` (plugin list), `src/yars/view/gui/ShadowMapper.*` + shadow materials (see `docs/planning/shadows_state.md` — READ IT FIRST).

---

### Task 1 (Phase 0): Build Ogre with the Metal render system

**Files:**
- No committed changes; scratch branch `scratch/metal-spike`.

**Interfaces:**
- Produces: `ext/ogre/install/lib/macosx/Release/RenderSystem_Metal.framework` (or `.dylib` — record which), needed by every later task.

- [ ] **Step 1: Reconfigure the existing Ogre build with Metal ON**

```bash
cd /Volumes/Eregion/projects/yars
git checkout -b scratch/metal-spike
cmake -S ext/ogre-source -B ext/ogre/build -DOGRE_BUILD_RENDERSYSTEM_METAL=ON
cmake --build ext/ogre/build -j 8
cmake --install ext/ogre/build
ls ext/ogre/install/lib/macosx/Release/ | grep -i metal
```

Expected: `RenderSystem_Metal.framework` listed. If the Metal render system fails to compile on the pinned fork commit, record the exact error in the findings doc (Task 4) — that alone may be a NO-GO.

- [ ] **Step 2: Face the pre-verified RTSS fact (this is the make-or-break constraint)**

Already established by inspecting the submodule (2026-07-06): **the vendored Ogre fork has NO MSL/Metal program writer in RTSS.** `Components/RTShaderSystem/src/` contains only `OgreShaderCGProgramWriter` and `OgreShaderGLSLProgramWriter`; `OgreShaderProgramManager.cpp:232-238` handles only `"hlsl"`/`"glsl"`; the Metal render system advertises only the `"metal"` shader profile (`OgreMetalRenderSystem.mm:245`) and registers `MetalProgramFactory` for language `"metal"` (`OgreMetalProgramFactory.mm:37`). There is no cross-compilation path. YARS's rendering depends on RTSS-generated shaders, so **the default expectation for this spike is NO-GO** unless one of these pans out:

1. Upstream OGRECave has since gained an MSL RTSS writer or a SPIRV-Cross path that can be pulled into the fork (check upstream master briefly);
2. Writing an MSL `ProgramWriter` for the subset of RTSS output YARS uses is judged affordable — if so, that becomes its own preliminary project and must be costed in the findings doc, NOT started in this plan.

The spike's Step 3 render attempt is still worth one time-boxed try (it documents the concrete failure mode for the findings doc / a possible upstream issue), but do not sink days into it.

---

### Task 2 (Phase 0): Spike — one textured scene under Metal

**Files:**
- Scratch edits to `src/cfg/plugins.cfg.in`, `src/yars/view/gui/OgreHandler.cpp`, `src/yars/view/gui/SdlWindow.cpp` — NOT committed to main history; they inform Phase 1's real implementation.

- [ ] **Step 1: Hack plugin + render-system selection (spike quality)**

In `plugins.cfg.in`: add `Plugin=RenderSystem_Metal` next to the GL3Plus line.
In `OgreHandler.cpp`: find where the render system is chosen (`grep -n "getRenderSystemByName\|setRenderSystem\|GL3Plus" src/yars/view/gui/OgreHandler.cpp`) and hard-code `"Metal Rendering Subsystem"` (verify exact name: `grep -rn "Metal Rendering" ext/ogre-source/RenderSystems/Metal/src/`).

- [ ] **Step 2: Window glue**

In `SdlWindow.cpp`, the current GL handoff sets `SDL_WINDOW_OPENGL` (lines 384/393) and passes `externalWindowHandle` = the NSView from `OSX_cocoa_view()` plus `macAPI=cocoa` (lines 466-467). Metal differs on BOTH counts (verified in `ext/ogre-source/RenderSystems/Metal/src/OgreMetalRenderWindow.mm` — note the filename, there is no `OgreMetalWindow.mm`): it parses only `externalWindowHandle` (line 202), `FSAA` (193), and `gamma` (197); it ignores `macAPI` entirely; and it casts `externalWindowHandle` to an **NSWindow\*** (line 206), taking `.contentView` itself (line 232). So for Metal: create the SDL window WITHOUT `SDL_WINDOW_OPENGL`, and pass the **NSWindow** (`syswm_info.info.cocoa.window`), NOT the NSView — passing the view is a crash. Spike-hack this behind `#if 1`.

- [ ] **Step 3: Render the spike scenes**

```bash
cd /Volumes/Eregion/projects/yars/build && cmake --build . -j 8
timeout 60s ./bin/yars --iterations 300 --xml ../xml/braitenberg.xml
timeout 60s ./bin/yars --iterations 300 --xml ../xml/hexapod_shadow_test.xml
```

Record for each: does a window open; do textured objects render; do overlays/fonts render; do shadows appear and are they PLACED CORRECTLY (the v6 placement bug class); any Ogre exceptions in the log.

- [ ] **Step 4: The auto-param question**

If shadows misrender: create a minimal receiver material variant using `texture_worldviewproj_matrix` as auto-param (copy the GL receiver material, see `docs/planning/shadows_state.md` for file locations) and record whether Metal computes it correctly — this decides Task 7's architecture (auto-param vs keeping `ShadowMapper`).

---

### Task 3 (Phase 0): Findings doc + GO/NO-GO

**Files:**
- Create: `docs/planning/metal-spike-findings.md` (committed to `feat/metal-renderer`, not the scratch branch)

- [ ] **Step 1: Write the findings doc** with a hard verdict per unknown:
  1. RTSS-on-Metal: works / works-with-caveats (list) / broken (evidence).
  2. SDL2+Metal windowing: exact param set that worked, or the failure.
  3. `texture_worldviewproj_matrix` on Metal: correct / broken.
  4. **Verdict: GO or NO-GO.** NO-GO ⇒ stop this plan, update `docs/planning/v0.8.7-open-points.md`, consider an upstream OGRECave issue with the spike evidence. GO ⇒ proceed.

- [ ] **Step 2: Clean up the spike**

```bash
git restore src/ cmake/   # discard spike hacks FIRST — they must not leak into the real branch
git checkout feat/metal-renderer 2>/dev/null || git checkout -b feat/metal-renderer
git add docs/planning/metal-spike-findings.md
git commit -m "docs(planning): Metal spike findings — GO/NO-GO for the port"
git branch -D scratch/metal-spike
```

The remaining tasks assume GO and use the spike's recorded working parameters.

---

### Task 4 (Phase 1): `--renderer` flag + real render-system selection

**Files:**
- Modify: `src/yars/view/gui/OgreHandler.cpp` (render-system selection)
- Modify: the CLI definition file — find it: `grep -rn "add_option\|add_flag" src/yars --include="*.cpp" | grep -i "capture\|nogui" | head` (the file defining `--nogui`/`--capture` gets `--renderer`)
- Modify: the runtime data/config carrying CLI values to the GUI layer (follow how `--capture` travels: `grep -rn "captureName" src/yars/configuration/ | head`)
- Modify: `src/cfg/plugins.cfg.in`

**Interfaces:**
- Produces: `--renderer gl|metal` (default `gl`); `OgreHandler` selects `"OpenGL 3+ Rendering Subsystem"` or `"Metal Rendering Subsystem"` accordingly. macOS-only: passing `metal` on Linux exits with a clear error.

- [ ] **Step 1: Add the CLI option** following the exact pattern of an existing string option in the same file (copy `--captureName`'s definition/plumbing end-to-end, rename accordingly, default value `"gl"`, validate against {`gl`,`metal`}).

- [ ] **Step 2: Plugin list**: `src/cfg/plugins.cfg.in` is a FLAT list with no platform conditionals (only `@OGRE_PLUGINS_DIR@` is substituted, via plain `configure_file` at `cmake/CreateConfigFiles.cmake:51`) — there is no existing idiom to copy, and an unconditional `Plugin=RenderSystem_Metal` line would abort Ogre::Root on Linux. Introduce one: in `CreateConfigFiles.cmake`, `set(METAL_PLUGIN_LINE "")`, then inside `if(APPLE)` (and ideally only if the Metal framework exists in `ext/ogre/install`) `set(METAL_PLUGIN_LINE "Plugin=RenderSystem_Metal")`; put `@METAL_PLUGIN_LINE@` on its own line in the template. Verify the generated Linux-style `plugins.cfg` (configure with the variable unset) contains no Metal line and no blank-line breakage.

- [ ] **Step 3: Selection in OgreHandler**: replace the hard-coded selection with a lookup keyed by the flag; unknown/unavailable render system ⇒ throw with a message listing available ones. Apply the SDL window-creation differences (no `SDL_WINDOW_OPENGL` for Metal; the spike's recorded param set) conditioned on the same flag in `SdlWindow.cpp`.

- [ ] **Step 4: Test both paths**

```bash
cd /Volumes/Eregion/projects/yars/build && cmake --build . -j 8
timeout 60s ./bin/yars --iterations 300 --xml ../xml/braitenberg.xml                    # default gl
timeout 60s ./bin/yars --iterations 300 --renderer gl    --xml ../xml/braitenberg.xml
timeout 60s ./bin/yars --iterations 300 --renderer metal --xml ../xml/braitenberg.xml
timeout 60s ./bin/yars --iterations 300 --renderer bogus --xml ../xml/braitenberg.xml   # expect clean error
```

Expected: first three render, fourth errors clearly. Regression: `--nogui` CSV run still bit-exact.

- [ ] **Step 5: Commit**

```bash
# add named files only — never a blanket src/ add on this branch
git add src/yars/view/gui/OgreHandler.cpp src/yars/view/gui/SdlWindow.cpp \
        src/yars/configuration/ProgramOptions.cpp \
        src/yars/configuration/container/ProgramOptionsConfiguration.h \
        src/cfg/plugins.cfg.in cmake/CreateConfigFiles.cmake
git commit -m "feat(gui): --renderer gl|metal flag; Metal plugin coexists, GL stays default"
```

(Adjust the list to the files actually touched — the point is naming them, not `git add src/`.)

---

### Task 5 (Phase 2a): Materials/RTSS + overlays parity under Metal

**Files:**
- Modify: whatever the spike identified (RTSS scheme registration in `OgreHandler.cpp`/`SceneGraph*.cpp`, material scripts under `src/yars/view/gui/materials/` — locate: `find src -name "*.material" | head`)

- [ ] **Step 1: Fix material/RTSS issues recorded in the spike findings** so that `braitenberg.xml`, `braitenberg_zoo.xml`, and `falling_objects.xml` render fully textured under `--renderer metal` — same object set, textures, and OSD text as GL.

- [ ] **Step 2: Acceptance via image export.** Generate PNG pairs (locate the frame-export mechanism: `grep -rn "writeContentsToFile\|saveImage\|screenshot" src/yars/view/gui/*.cpp | head`) for each scene under `--renderer gl` and `--renderer metal` at the same iteration, and compare visually (side-by-side): identical scene content; minor driver-level AA/gamma differences acceptable, missing textures/objects/overlays are NOT.

- [ ] **Step 3: Commit** (`feat(gui): Metal parity — materials, RTSS, overlays`), attaching the PNG pairs to the PR description later.

---

### Task 6 (Phase 2b): Image export + video capture under Metal

**Files:**
- Modify: capture path if needed — `src/yars/view/gui/` (find via `grep -rn "toggleCaptureVideo\|CaptureVideo" src/yars/view/gui/ | head`)

- [ ] **Step 1: Image export**: verify the export API works under Metal (Ogre's `RenderTarget::writeContentsToFile` is render-system-agnostic, but pixel-format/row-order bugs are common — export under both renderers and diff dimensions + visual content).

- [ ] **Step 2: Video capture**:

Prerequisite: the entire video path compiles only under `YARS_USE_CAPTURE_VIDEO` (default OFF — `cmake/SetOptions.cmake:48-50`); without it the command below silently produces no mp4 and you would misdiagnose a Metal failure. Check and enable first:

```bash
grep YARS_USE_CAPTURE_VIDEO /Volumes/Eregion/projects/yars/build/CMakeCache.txt
# if OFF: cmake -B build -DYARS_USE_CAPTURE_VIDEO=ON && cmake --build build -j 8
cd /Volumes/Eregion/projects/yars/build
timeout 120s ./bin/yars --iterations 1800 --renderer metal --capture --captureDirectory . --captureName metal-test.mp4 --xml ../xml/test_capture.xml
ffprobe -v error -show_entries stream=codec_name,width,height metal-test-0000.mp4
```

Expected: playable H.264 mp4 (segment-suffixed name), correct dimensions, frames not flipped/black (open it).

- [ ] **Step 3: Commit** (`feat(gui): image export + video capture verified under Metal`).

---

### Task 7 (Phase 2c): Shadows under Metal

**Files:**
- Modify: shadow materials + `src/yars/view/gui/ShadowMapper.*` (paths per `docs/planning/shadows_state.md`)

- [ ] **Step 1: Choose the architecture from the spike finding:**
  - Auto-param WORKS on Metal ⇒ Metal receiver material uses `texture_worldviewproj_matrix` directly; `ShadowMapper`'s manual matrix push becomes GL-only (guard by active render system name).
  - Auto-param BROKEN ⇒ keep `ShadowMapper` for both; port the caster/receiver shaders to MSL twins registered in the same materials as additional `delegate`/language-specific programs (follow how the GLSL programs are declared; Ogre materials support per-syntax program declarations — mirror the existing `glsl` blocks with `metal` ones, same param names so `ShadowMapper` needs no changes).

- [ ] **Step 2: Acceptance = the six criteria in `docs/planning/shadow_goal.md`** run under `--renderer metal` on `xml/hexapod_shadow_test.xml`, PNG-exported and compared against the GL shadow reference. Shadow placement must be correct (this was the historic bug class — check contact points under legs and the obstacle).

- [ ] **Step 3: Commit** (`feat(gui): shadows under Metal — <auto-param|ShadowMapper+MSL> path`).

---

### Task 8: Final verification + docs

- [ ] **Step 1: Full matrix locally**: for each of {gl, metal} × {braitenberg_zoo, falling_objects, hexapod_shadow_test}: GUI run 500 iterations, PNG export (`--framesDirectory` — note this is the frame-export flag; `--capture` is the separate ffmpeg video path), clean exit. `--nogui` CSV regression bit-exact (renderer flag is irrelevant headless — verify `--nogui --renderer metal` doesn't initialize any GL/Metal and still matches).

- [ ] **Step 1b: Performance parity check (spec acceptance)**: same scene, both renderers, timed:

```bash
cd /Volumes/Eregion/projects/yars/build
time ./bin/yars --iterations 5000 --renderer gl    --xml ../xml/braitenberg_zoo.xml
time ./bin/yars --iterations 5000 --renderer metal --xml ../xml/braitenberg_zoo.xml
```

Expected: Metal wall-clock ≤ GL within ~10%. Record both numbers in the findings doc; a materially slower Metal blocks any future default flip.

- [ ] **Step 2: CI**: confirm both workflows green (they exercise only the GL path — that is by design; add `-DOGRE_BUILD_RENDERSYSTEM_METAL=ON` to the macOS workflow's Ogre build block at `macos-build.yml:47-51` so the plugin at least compiles in CI, and change the cache key's trailing `-v2-nofi` to `-v3-nofi` at `macos-build.yml:45` to force the rebuild).

- [ ] **Step 3: Update docs**: `CLAUDE.md` (renderer flag, Metal status), `docs/planning/v0.8.7-open-points.md` (Metal available behind flag; default-flip decision deferred). Commit, hand off for push approval.
