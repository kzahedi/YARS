## Why

YARS has accumulated ~20 XML simulation configurations and 20+ CLI options with no systematic verification that they all work correctly — physics, rendering, OSD, video export, and external controllers alike. With the ongoing modernization, regressions can go undetected. This change establishes a verified baseline across all simulation scenarios before further development proceeds.

## What Changes

- **OSD restored**: `NativeOgreWindow::setupOSD()` is currently stubbed out; the Ogre overlay system will be reconnected to display simulation time and status
- **Simulation audit**: Each self-contained XML config is run and confirmed crash-free with correct physics output
- **Video generation**: Each working simulation runs with frame capture enabled; frames are assembled into MP4 videos for human visual review
- **External controller compilation**: Go, Python, Julia, MATLAB, TCP/IP, and named-pipe controller variants are confirmed to compile or have their dependencies documented
- **`--nogui` verification**: Headless mode is confirmed to run all self-contained simulations without crashing
- **Vulkan renderer status**: Vulkan test executables are built and assessed for readiness

## Capabilities

### New Capabilities

- `osd-rendering`: On-screen display showing simulation time/status via Ogre overlay system
- `simulation-audit`: Systematic verification that each XML configuration runs correctly
- `video-export-per-sim`: Per-simulation frame capture and MP4 assembly for visual review
- `controller-build-verification`: Compilation check for all external controller variants
- `vulkan-renderer-status`: Build and smoke-test of the Vulkan rendering path

### Modified Capabilities

<!-- None: this is a verification/fix pass, no existing spec-level requirements are changing -->

## Impact

- `src/yars/view/gui/NativeOgreWindow.cpp` — OSD reconnection
- `src/yars/view/gui/ColoredTextAreaOverlayElement.cpp/h` — already exists, needs wiring
- `xml/*.xml` — hexapod.xml path fix (wrong machine path for Python controller)
- `src/yars/view/vulkan/` — build verification only, no logic changes
- Build system: may need ffmpeg present for video assembly
- All XML configs in `xml/` and `xml/joints/`
