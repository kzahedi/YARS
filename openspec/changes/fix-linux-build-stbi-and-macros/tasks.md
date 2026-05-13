## 1. STBI codec include

- [x] 1.1 Confirmed actual installed path from CI run 25736692735: `ext/ogre/install/include/OGRE/Plugins/STBICodec/OgreSTBICodec.h` (no top-level `OgreSTBICodec.h` on Linux; on macOS it ships in `Codec_STBIStatic.framework/Headers/`)
- [x] 1.2 N/A — top-level header is not installed on Linux; option (c) does not apply
- [x] 1.3 Changed `OgreHandler.h:17` to `#include <OGRE/Plugins/STBICodec/OgreSTBICodec.h>` (matches the actual install layout; consistent with neighbouring `OGRE/Plugins/ParticleFX/...` include)
- [x] 1.4 `OgreGL3PlusPlugin.h` and `OgreParticleFXPlugin.h` already use the `OGRE/Plugins/.../X.h` / `OGRE/RenderSystems/.../X.h` pattern correctly — no change needed

## 2. YARS_INSTALL_PATH macro

- [x] 2.1 Replace `cmake/SetDefinitions.cmake:2` with `add_compile_definitions(YARS_INSTALL_PATH="${CMAKE_INSTALL_PREFIX}")`
- [x] 2.2 Grep for any other `add_definitions(-D…='"…"')` shell-quoted patterns in `cmake/` and convert them to `add_compile_definitions` form (none found beyond 2.1)
- [x] 2.3 Verify macOS build still compiles and runs (no behaviour change expected)

## 3. CI Verification

- [x] 3.1 Pushed to `feat/linux-ci-verification`; `Linux Build & Headless Audit` workflow ran (run 25791333734)
- [x] 3.2 Workflow advanced past `[5%] Building YarsGUI` — STBI header error resolved
- [x] 3.3 GCC `character constant too long for its type` warnings no longer appear in build log — `add_compile_definitions` fix confirmed
- [x] 3.4 Subsequent Linux failures (STBIPlugin class removed in Ogre 14, CMP0167 policy, hardcoded macOS SDL2/Ogre paths, FFP factory removal, plugins.cfg pointing at /usr/local, GLSL IN/OUT macros, EGL/virtio-gpu DRI2 auth) were all fixed in the same `feat/linux-ci-verification` branch rather than spinning off separate proposals. CI auto-triggers re-enabled with `OGRE_GLSUPPORT_USE_EGL=OFF` added to the Ogre build step.

## 4. Documentation

- [x] 4.1 Documentation deferred — `add-linux-port-verification` covers `docs/Linux_Build.md` refresh in task 6.2.
- [x] 4.2 Acceptance superseded by `add-linux-port-verification`: green CI run + Ubuntu/UTM GUI verification with screenshot.

This change is effectively **complete**; all originally-scoped issues (STBI header path, YARS_INSTALL_PATH macro) are resolved on `feat/linux-ci-verification`. The broader Linux GUI verification work continues under `add-linux-port-verification`.
