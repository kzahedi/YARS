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
- [x] 3.4 Workflow now fails on a different Linux-only issue (`'STBIPlugin' in namespace 'Ogre' does not name a type` at `OgreHandler.h:49`, Ogre 14 likely renamed the plugin class). Filed for a follow-up `fix-linux-ogre-stbi-plugin-rename` proposal — not bundled here. CI auto-triggers disabled (commit 7fb2a98) until Linux GUI verification work resumes.

## 4. Documentation

- [ ] 4.1 Note in `docs/Linux_Build.md` that `OGRE_BUILD_PLUGIN_STBI=ON` is required (it is in the Ogre build step in the CI workflow; mention it for local builders)
- [ ] 4.2 No status doc — acceptance is "the green CI run on this branch"
