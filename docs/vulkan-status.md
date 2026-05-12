# Vulkan Renderer Status

## Summary

The Vulkan renderer is **wired into the main CMake build** behind the
`YARS_USE_VULKAN` option (default `OFF`). When enabled, the `YarsVulkan`
static library and its shader set build alongside the main `yars` target.
The renderer is **not yet wired into the YARS runtime** — it remains an
experimental module; the GUI path still uses Ogre 14.

## What Exists

- Full source tree at `src/yars/view/vulkan/` (~35 .cpp/.h files)
- Shader sources at `src/yars/view/vulkan/shaders/` (basic, triangle, mesh, mesh_textured)
- CMake integration via `add_subdirectory(view/vulkan)` in `src/yars/CMakeLists.txt`
  guarded by `option(YARS_USE_VULKAN "Build Vulkan renderer and tests" OFF)`

## Build Status

### Main build

```bash
cmake -S . -B build -DYARS_USE_VULKAN=ON
cmake --build build -j
```

This builds:

- `YarsVulkan` static library (always built when `YARS_USE_VULKAN=ON`)
- `compile_shaders` custom target (SPIR-V outputs in `build/.../shaders/compiled/`)
- The main `yars` executable (unaffected — does not yet link `YarsVulkan`)

### Experimental test executables (opt-in, `EXCLUDE_FROM_ALL`)

These have pre-existing transitive-link issues against `YarsConfiguration`'s
references to `FollowCamera` / `CameraFactory` (which live in `YarsGUI`)
and are therefore not built by the default `all` target. Build them
individually when needed:

```bash
cmake --build build --target vulkan_geometry_test
cmake --build build --target vulkan_static_box_test
cmake --build build --target vulkan_handler_test
cmake --build build --target vulkan_frame_export_test
cmake --build build --target vulkan_yars_frame_capture
cmake --build build --target vulkan_test
cmake --build build --target vulkan_braitenberg_capture
cmake --build build --target vulkan_braitenberg_export
cmake --build build --target vulkan_ground_plane_test
cmake --build build --target vulkan_camera_control_test
```

Only `vulkan_geometry_test` is known to link cleanly today; the rest will
require either widening their link lines (add `YarsGUI`) or fixing the
underlying layering issue where `YarsConfiguration::__printListCommandFollowModes`
reaches into `FollowCamera`.

## System Vulkan Support (macOS arm64)

- **MoltenVK**: 1.4.1 via Homebrew (Vulkan-on-Metal translation layer)
- **libvulkan**: `/opt/homebrew/lib/libvulkan.dylib` (1.4.341)
- **glm**: available
- **Shader compiler**: `glslangValidator` (preferred path uses `glslc` when
  installed, but `glslangValidator` with `-V` is used as fallback and is
  what we currently build with)

## Known Issues / Follow-ups

1. **Test-executable link layering** — `YarsConfiguration` references
   GUI-only types; until that's untangled, most vulkan test exes need
   `YarsGUI` on their link line.
2. **Runtime wiring** — `YarsVulkanView` is not yet selected by
   `YarsConfiguration::init` or `YarsMainControl`; there is no way to run
   `yars` against the Vulkan renderer at the command line.
3. **Hardcoded macOS Homebrew paths** in `src/yars/view/vulkan/CMakeLists.txt`
   (`/opt/homebrew/include/SDL2`, `/opt/homebrew/lib/libSDL2.dylib`).
   Replace with `find_package(SDL2)` for Linux/CI portability.

## Next Steps

Phase 1 (this commit): library is reachable from the build system.

Phase 2: untangle `YarsConfiguration` ↔ `FollowCamera` so the test exes
can be re-enabled in `all`, and replace the hardcoded SDL2 paths.

Phase 3: wire `YarsVulkanView` into the runtime path (see
`docs/VULKAN_MIGRATION_PLAN.md` for the longer-range plan).

## Reference

- Migration plan: `docs/VULKAN_MIGRATION_PLAN.md`
- Source: `src/yars/view/vulkan/`
