## Context

YARS has 21 XML simulation configs and 20+ CLI options. The codebase was recently modernized (C++17, smart pointers, namespace org, Observer pattern removal). During that work, `NativeOgreWindow::setupOSD()` was stubbed out with a comment "skipped for stability testing". No systematic verification exists that all simulations run correctly with GUI and physics.

Controllers are C++ shared libraries compiled from `contrib/controller/` and loaded at runtime via `--lib <path>`. Almost all XML configs reference a controller module; only 4 are truly standalone without any library:
- `braitenberg_nocontroller.xml`
- `braitenberg_noise.xml`
- `falling_objects.xml`
- `test_capture.xml`

All other "self-contained" configs (braitenberg variants, joints, muscle) require `contrib/controller/` compiled as a `.dylib` and passed via `--lib`.

External-process configs (TCPIP, named pipes, Go, Python, Julia, MATLAB) require separate runtime processes — these are tested for compilation only, not execution.

The Vulkan renderer (`src/yars/view/vulkan/`) is a parallel WIP renderer with its own CMakeLists.txt and standalone test executables. It is not integrated into the main YARS binary.

## Goals / Non-Goals

**Goals:**
- Restore OSD rendering via Ogre overlay system
- Compile `contrib/controller/` as a shared library
- Run all controller-based XML configs with GUI + frame capture
- Run all truly standalone XMLs headlessly (--nogui) to verify physics
- Assemble per-simulation MP4 videos from captured frames for visual review
- Compile all external-process controller sources (Go, Python scripts checked for syntax)
- Build Vulkan test executables and document their status

**Non-Goals:**
- End-to-end testing of TCPIP / named pipe / language bridge controllers (requires live external processes)
- Automating visual correctness checking (human reviews videos)
- Integrating Vulkan into the main YARS binary (that's a future change)
- CI pipeline setup (out of scope here)

## Decisions

### D1: OSD Restoration — Use Existing Ogre Overlay Infrastructure

`ColoredTextAreaOverlayElement.cpp/h` already exists in `src/yars/view/gui/`. The Ogre overlay system was removed from `setupOSD()` for stability during modernization. We restore it by reconnecting the existing overlay element classes.

**Alternative considered**: Leave OSD stub, implement OSD via SDL2 text rendering instead.
**Rejected because**: The Ogre overlay approach is already partially implemented and integrates with the existing material/font system. SDL2 text would be a new dependency path.

### D2: Controller Library — Compile contrib/ as Single Shared Library

Compile all controllers in `contrib/controller/` as a single shared library (e.g., `libYarsControllers.dylib`). Pass it to YARS via `--lib`. This is the existing runtime loading mechanism.

**Alternative considered**: Compile each controller as a separate `.dylib`.
**Rejected because**: YARS's `--lib` expects a single library path; this is how the system already works.

### D3: Video Generation — framesDirectory + ffmpeg

Use `--framesDirectory <path>` to export PNG frames, then assemble with `ffmpeg`. This is more reliable than the built-in `VideoCapture` which requires specific codec availability.

**Alternative considered**: Use YARS built-in `--capture` / `--captureName`.
**Rejected because**: Built-in video capture has codec dependencies (vcodec) that may not be configured; frame export + ffmpeg is simpler and universally available on macOS with Homebrew.

Frame rate: capture every frame at sim frequency. ffmpeg target: 30fps MP4 (H.264).

### D4: Simulation Run Duration — Fixed Iteration Count

Each simulation runs for a fixed number of iterations (e.g., 500 physics steps = 5 seconds at 100Hz) rather than wall time. This gives deterministic, reproducible video length.

**Rationale**: YARS `--iterations N` terminates cleanly after N steps.

### D5: Vulkan Assessment — Build Only, No Integration

Build Vulkan test executables and run `vulkan_geometry_test` as a smoke test. Document status. Do not attempt to integrate Vulkan into main YARS binary in this change.

## Risks / Trade-offs

- **OSD Ogre overlay may have changed API** since the code was written (Ogre 13.x vs original version) → Mitigation: check Ogre 13.x overlay API compatibility before restoring
- **contrib/controller CMakeLists.txt may need updating** for C++17 / macOS → Mitigation: inspect and fix before compiling
- **hexapod.xml has hardcoded wrong path** (`/Users/zahedi/projects/...`) → Fix path to use relative or correct absolute path for Mac Mini
- **ffmpeg may not be installed** → Check with `which ffmpeg`; document install if missing
- **Vulkan shaders may not compile** on current macOS/MoltenVK setup → Document failures, don't block other work

## Migration Plan

1. Compile contrib/controller library → verify with one braitenberg XML
2. Fix OSD in NativeOgreWindow
3. Run all compatible XMLs, generate videos → human review
4. Assess Vulkan build status
5. Document any external controller compilation issues

Rollback: OSD change is isolated to `NativeOgreWindow::setupOSD()` — reverting one function is safe.

## Open Questions

- Does `braitenberg_noise.xml` use a controller module or only noise on sensors? (Confirmed: noise only — no controller tag)
- What is the expected OSD content? (Time display minimum; keyboard shortcuts optional)
- Does `hexapod_crbm.xml` use NMODE controller from contrib, or an external binary?
