## 1. STBI codec include

- [ ] 1.1 Confirm with Ogre 14 source that `OGRE_BUILD_PLUGIN_STBI=ON` installs a top-level `OgreSTBICodec.h` (check `ext/ogre/install/include/OGRE/` after a fresh install run); document the actual installed path
- [ ] 1.2 If the top-level header is installed: change `OgreHandler.h:17` from `#include <OGRE/Codec_STBI/OgreSTBICodec.h>` to `#include <OgreSTBICodec.h>` (option (c))
- [ ] 1.3 If not: fall back to option (a) — add `ext/ogre-source/PlugIns/STBICodec/include` to the GUI target's include path
- [ ] 1.4 Audit other Ogre plugin includes in `OgreHandler.h` (`OgreGL3PlusPlugin.h`, `OgreParticleFXPlugin.h`) and simplify any that use the same `OGRE/Plugins/.../X.h` pattern when a flatter form works

## 2. YARS_INSTALL_PATH macro

- [ ] 2.1 Replace `cmake/SetDefinitions.cmake:2` with `add_compile_definitions(YARS_INSTALL_PATH="${CMAKE_INSTALL_PREFIX}")`
- [ ] 2.2 Grep for any other `add_definitions(-D…='"…"')` shell-quoted patterns in `cmake/` and convert them to `add_compile_definitions` form
- [ ] 2.3 Verify macOS build still compiles and runs (no behaviour change expected)

## 3. CI Verification

- [ ] 3.1 Push to a feature branch; `Linux Build & Headless Audit` workflow runs
- [ ] 3.2 Confirm the workflow advances past `[5%] Building YarsGUI` without the STBI header error
- [ ] 3.3 Confirm the GCC warning `character constant too long for its type` no longer appears in the build log
- [ ] 3.4 If the workflow advances and then fails on a different Linux-only issue: stop, open a new follow-up `fix-linux-*` proposal for that issue, do not bundle into this change

## 4. Documentation

- [ ] 4.1 Note in `docs/Linux_Build.md` that `OGRE_BUILD_PLUGIN_STBI=ON` is required (it is in the Ogre build step in the CI workflow; mention it for local builders)
- [ ] 4.2 No status doc — acceptance is "the green CI run on this branch"
