# Metal Render-System Spike — Task 2 Findings

Branch: `scratch/metal-spike` (throwaway; all `src/` edits restored after this run — see "Restoration" at the end).

## Setup performed

1. `build/plugins.cfg`: added `Plugin=RenderSystem_Metal` next to `RenderSystem_GL3Plus`.
   - **Caveat discovered**: `cmake --build .` re-runs `configure_file` and regenerates
     `build/plugins.cfg` from `src/cfg/plugins.cfg.in` on every invocation that triggers a
     reconfigure, silently wiping this edit. Had to re-add the line after the build and
     before running the binary. Phase 1 must add the line to `plugins.cfg.in` itself, not
     rely on editing the generated file.
2. `src/yars/view/gui/OgreHandler.cpp` (~line 146): replaced `renderers.front()` with a
   by-name loop hardcoding `"Metal Rendering Subsystem"` (confirmed exact string at
   `ext/ogre-source/RenderSystems/Metal/src/OgreMetalRenderSystem.mm:112`), throwing if not
   found. Guarded with `#if 1`/`#else`.
3. `src/yars/view/gui/SdlWindow.cpp`:
   - Window creation flags: dropped `SDL_WINDOW_OPENGL` (both `SDL_CreateWindow` call
     sites).
   - `SDL_GL_CreateContext` call: replaced with `glcontext = nullptr` (guarded).
   - `params["externalWindowHandle"]`: switched from `OSX_cocoa_view()` (NSView) to a new
     `OSX_cocoa_window()` helper (NSWindow*); dropped `macAPI` / `macAPICocoaUseNSView`
     entirely for the Metal path (Metal RS ignores them per
     `OgreMetalRenderWindow.mm`).
   - `SDL_GL_SwapWindow(_sdlWindow)` in the per-frame render loop: guarded out with
     `#if 0`.
4. `src/yars/view/gui/OSX_wrap.{h,mm}`: added `OSX_cocoa_window(SDL_SysWMinfo&)`, mirroring
   `OSX_cocoa_view()` but returning the `NSWindow*` handle instead of `.contentView`.

Build: `cmake --build build -j 8` succeeded (existing pre-existing deprecation warnings
only: `OgreVector3.h` migration notice, `setLogDetail` deprecated, `NSOpenGLContext`
deprecated). No new compiler errors from the spike hacks.

## Scene 1: `xml/braitenberg.xml`

Command: `timeout 60s ./bin/yars --iterations 300 --xml ../xml/braitenberg.xml`

**Outcome: ran to completion, no crash, no window content ever rendered.**

stdout (verbatim, relevant lines):
```
added wheel left hinge is active: 1
added wheel right hinge is active: 1
Shadow setup failed: InvalidParametersException: Unsupported texture format in MetalTextureBuffer::upload at /Volumes/Eregion/projects/yars/ext/ogre-source/RenderSystems/Metal/src/OgreMetalHardwarePixelBuffer.mm (line 302)
ShadowMapper: initialised with light=Vector3(-0.57735, -0.57735, -0.57735)
Maximum number of physics iterations (300) reached.
Good bye.
```

`build/ogre.log` (verbatim, relevant lines):
```
13:11:59: Warning: ScriptCompiler - deprecated symbol in YARS.material(425): separateUV is no longer supported.
13:11:59: Warning: ShaderGenerator: No supported language found. Falling back to 'null'
13:11:59: Warning: material YARS/ShadowReceiver has no supportable Techniques and will be blank. Explanation: 
Pass 0: vertex program YARS/ShadowReceiverVP cannot be used - not supported
13:11:59: Can't assign material to ManualObject 'main body'. Material 'top' not found in group 'General'. ...
   [... ~90 more "Material '<x>' not found in group 'General'" lines for every ManualObject
        submesh in the scene — main body, wheels, walls, ray sensors, etc. This cascade is
        caused by the RTSS "null" fallback: with no shader language available, the
        ShaderGenerator-backed default/scheme materials never get created, so every
        material name lookup used by these ManualObjects resolves to nothing.]
13:11:59: OgreHandler::step caught and swallowed: InvalidStateException: Cannot begin frame - no viewport selected. in _beginFrame at /Volumes/Eregion/projects/yars/ext/ogre-source/OgreMain/src/OgreRenderSystem.cpp (line 504)
```

Note: `Hexapod.material` / `HexapodMainBody.material` / `Hexapod.001.material` script-compile
errors ("missing property value", "normalise_normals not recognized") also appear in this
scene's log even though braitenberg.xml doesn't use the hexapod model — these are resource
group preload errors, pre-existing and **unrelated to Metal** (same materials, same errors,
independent of render system; not investigated further per time-box).

**Stage that fails**: RTSS shader generation, immediately at RTSS/ShaderGenerator init
(`"No supported language found. Falling back to 'null'"`) — confirms the established fact
that RTSS has no MSL writer. Everything downstream cascades from this: no shader-generated
materials exist, so nothing binds to any of the manual scene objects, and `renderOneFrame()`
throws on the very first frame with "no viewport selected" (only the first occurrence is
logged — `OgreHandler::step()` swallows repeats via a `reported` static flag — so this
exception is presumably thrown on every subsequent frame for all 300 iterations, silently).
Additionally, the shadow-map setup independently fails at texture creation
(`Unsupported texture format in MetalTextureBuffer::upload`) before shadows are even
attempted — this is a *second*, independent Metal incompatibility, distinct from the RTSS
gap.

No crash: the SDL window is created and shown (`SDL_ShowWindow`/`SDL_RaiseWindow`/
`_visible = true` all execute with no `SDL_CreateWindow failed` / `SDL_GetWindowWMInfo
failed` output), so a window almost certainly appears on screen, but Ogre never
successfully begins a frame into it — expected content is a blank/undefined window (likely
just whatever the bare NSWindow shows, since no Metal drawable ever gets presented; this
spike had no way to screenshot/visually confirm in this run, and per policy screenshotting
was not attempted since the exception makes the outcome unambiguous from the logs).

## Scene 2: `xml/hexapod_shadow_test.xml`

Command: `timeout 60s ./bin/yars --iterations 300 --xml ../xml/hexapod_shadow_test.xml`

**Outcome: ran to completion, no crash, no window content ever rendered — but a materially
different failure signature than Scene 1.**

stdout (verbatim, relevant lines):
```
[... hinge-active lines for all hexapod legs ...]
Shadow setup failed: InvalidParametersException: Unsupported texture format in MetalTextureBuffer::upload at /Volumes/Eregion/projects/yars/ext/ogre-source/RenderSystems/Metal/src/OgreMetalHardwarePixelBuffer.mm (line 302)
ShadowMapper: initialised with light=Vector3(-0.57735, -0.57735, -0.57735)
Maximum number of physics iterations (300) reached.
Good bye.
```

`build/ogre.log` (verbatim, relevant lines — this is the important new finding):
```
13:13:05: Warning: ShaderGenerator: No supported language found. Falling back to 'null'
13:13:05: Warning: material YARS/ShadowReceiver has no supportable Techniques and will be blank. ...
13:13:05: Error: Metal Compiler in MetalDefaultVP:
DefaultShaders.metal:5:3: error: unknown type name 'vec4'
DefaultShaders.metal:5:14: error: type 'int' is not valid for attribute 'position'
DefaultShaders.metal:6:3: error: unknown type name 'vec2'
DefaultShaders.metal:11:3: error: unknown type name 'in'
DefaultShaders.metal:11:11: error: expected ';' at end of declaration list
DefaultShaders.metal:12:3: error: unknown type name 'in'
DefaultShaders.metal:12:11: error: expected ';' at end of declaration list
DefaultShaders.metal:17:3: error: unknown type name 'mat4'
DefaultShaders.metal:18:3: error: unknown type name 'mat4'
DefaultShaders.metal:24:34: error: type 'Vertex' is not valid for attribute 'stage_in'
DefaultShaders.metal:25:34: error: type 'const constant Uniform &' is not valid for attribute 'buffer'
DefaultShaders.metal:25:43: note: type 'Uniform' cannot be used in buffer pointee type
DefaultShaders.metal:33:27: error: type 'RasterizerData' is not valid for attribute 'stage_in'
13:13:05: Error: retriving entry point 'default_vp' in shader MetalDefaultVP
13:13:05: Program 'MetalDefaultVP' is not supported: Vertex Program MetalDefaultVP failed to compile. See compile log above for details.
13:13:05: Error: Metal Compiler in MetalDefaultFP:
OgreUnifiedShader.h:62:13: error: unknown type name 'location'
OgreUnifiedShader.h:62:27: error: expected function body after function declarator
DefaultShaders.metal:5:3: error: unknown type name 'vec4'
[... same set of vec4/vec2/mat4/in/stage_in errors repeated for the fragment shader ...]
13:13:05: Error: retriving entry point 'default_fp' in shader MetalDefaultFP
13:13:05: Program 'MetalDefaultFP' is not supported: Fragment Program MetalDefaultFP failed to compile. See compile log above for details.
13:13:05: OgreHandler::step caught and swallowed: InvalidParametersException: Named constants have not been initialised, perhaps a compile error in _findNamedConstantDefinition at /Volumes/Eregion/projects/yars/ext/ogre-source/OgreMain/src/OgreGpuProgramParams.cpp (line 1269)
```

**Stage that fails**: RTSS shader generation fails identically to Scene 1
(`"No supported language found. Falling back to 'null'"`), but this scene additionally
reaches Ogre's own **bundled built-in default Metal shader** (`MetalDefaultVP`/
`MetalDefaultFP`, source file `DefaultShaders.metal`, used as the fixed-function-emulation
fallback for entities with no assignable material/technique) — and that shader **fails to
compile as actual Metal Shading Language**. The compiler errors show the bundled
`DefaultShaders.metal` is written using GLSL-flavored syntax
(`vec4`, `vec2`, `mat4`, `in`, `layout(location = 0)`) filtered through
`OgreUnifiedShader.h` macros that clearly target GLSL/HLSL cross-compilation, not real MSL
— `IN(a, b)` expands to `in a;` (invalid MSL), and the `layout(location = ...)` qualifier is
GLSL-only. This is a **new, independent finding beyond the RTSS gap**: even Ogre's own
fallback Metal shader in this Ogre fork/version is non-functional out of the box; it was
apparently never actually exercised/validated end-to-end. This is a deeper defect than
"RTSS has no MSL writer" — it means the Metal RS's most basic no-material rendering path is
broken too.

The final swallowed exception differs from Scene 1 (`"Named constants have not been
initialised, perhaps a compile error"` vs. `"no viewport selected"`), consistent with this
scene's content reaching further into the render pipeline (default-shader compilation
attempt) before failing, whereas braitenberg.xml's materials/objects fail earlier
(no viewport ever gets a valid frame started) and never reach the default-shader path.

Shadow setup fails identically to Scene 1, before any shadow-specific rendering is
attempted: `Shadow setup failed: InvalidParametersException: Unsupported texture format in
MetalTextureBuffer::upload`.

## Step 4 (auto-param question) — not reached

Per the brief, Step 4 (testing a `texture_worldviewproj_matrix` auto-param receiver
material) applies "if shadows misrender." Here shadows don't misrender in the v6 sense
(wrong placement/orientation) — they never get created at all: `ShadowMapper` setup
throws `InvalidParametersException: Unsupported texture format in MetalTextureBuffer::upload`
before any shadow texture exists, independent of and prior to the RTSS/material questions.
This is a Metal shadow-texture-format incompatibility, not a shader-auto-param question, so
Step 4 as scoped (which assumes shadows render but are misplaced) does not apply. Per the
time-box, no shader/material workaround was attempted.

## Summary of failure stages

| Scene | Plugin load | Window create | RTSS material gen | Shadow setup | First frame |
|---|---|---|---|---|---|
| braitenberg.xml | OK | OK (no crash) | FAIL (no MSL writer → "null" fallback) | FAIL (unsupported texture format) | FAIL ("no viewport selected", every frame, only 1st logged) |
| hexapod_shadow_test.xml | OK | OK (no crash) | FAIL (same) + bundled default Metal shader also fails to compile (MSL syntax errors in `DefaultShaders.metal`) | FAIL (same) | FAIL ("Named constants have not been initialised", every frame, only 1st logged) |

Overall: the yars binary never crashes and always exits cleanly after the requested
iteration count in both scenes — the failure mode is a silently-blank/non-rendering window
for the entire run, not a crash. No visual/screenshot confirmation was performed (not
needed to characterize the failure — the exception traces are unambiguous and consistent
across both scenes).

## Restoration

`git restore src/` run at the end; `build/plugins.cfg` (generated/untracked) reverted to
GL3Plus-only by removing the added `Plugin=RenderSystem_Metal` line manually.
