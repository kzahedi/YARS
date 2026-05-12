## Why

The first run of the `Linux Build & Headless Audit` CI workflow (introduced by `add-linux-port-verification`) failed at `[5%] Building YarsGUI/SdlWindow.cpp` with:

```
OgreHandler.h:17:10: fatal error: OGRE/Codec_STBI/OgreSTBICodec.h: No such file or directory
```

Two related Linux-only build defects in the same step:

1. **Codec_STBI plugin header is unreachable on Linux.** `cmake --install` only exposes the public Ogre API; the STBI codec plugin keeps its `OgreSTBICodec.h` in `ext/ogre-source/PlugIns/STBICodec/include/`, which is not added to the include path on Linux. The macOS framework layout makes this header reachable accidentally, hiding the gap.
2. **`YARS_INSTALL_PATH` macro is malformed.** `cmake/SetDefinitions.cmake:2` defines `-DYARS_INSTALL_PATH='"${CMAKE_INSTALL_PREFIX}"'` using shell-style quoting. Apple Clang silently tolerates the resulting multi-character-constant emission, but GCC on Linux warns `"character constant too long for its type"` at `Directories.cpp:288` and `:327`, indicating the macro expands to an integer literal rather than a string.

Together these block any `add-linux-port-verification` task from closing. Fixing them is a prerequisite, not a new feature, so the work lives in its own narrow change.

## What Changes

- **STBI include path.** Add the plugin's source-tree header directory to the YARS include path so `#include <OGRE/Codec_STBI/OgreSTBICodec.h>` resolves. Options to evaluate during implementation:
  - (a) Append `ext/ogre-source/PlugIns/STBICodec/include` to `target_include_directories` for the YARS GUI target, or
  - (b) Run Ogre's install step with `OGRE_INSTALL_PLUGINS_HEADERS=ON` (if Ogre supports it) so `ext/ogre/install/include/OGRE/Codec_STBI` exists, or
  - (c) Replace the `<OGRE/Codec_STBI/...>` include with the upstream-supported `<OgreSTBICodec.h>` form (Ogre 14's `OGRE_BUILD_PLUGIN_STBI=ON` installs it without the `OGRE/Codec_STBI/` subdirectory).

  Prefer (c) — it is the smallest change and avoids hard-coding a submodule path. The other plugin includes in `OgreHandler.h` (`OgreGL3PlusPlugin.h`, `OgreParticleFXPlugin.h`) use the same patterns; if (c) succeeds, simplify those consistently.

- **`YARS_INSTALL_PATH` macro.** Replace the shell-quoted definition with `add_compile_definitions(YARS_INSTALL_PATH="${CMAKE_INSTALL_PREFIX}")` so the macro expands to a proper C-string literal on both Apple Clang and GCC. Equivalent treatment for any sibling definitions in `cmake/SetDefinitions.cmake` that use the same `'"..."'` pattern.

- **CI gate.** After both fixes land, `Linux Build & Headless Audit` (`.github/workflows/linux-build.yml`) MUST pass at least through the `Build YARS` step (it may still fail downstream on other Linux-only issues — those get their own fix proposals).

## Capabilities

### New Capabilities

<!-- None. -->

### Modified Capabilities

- `linux-port-verification`: The "YARS builds from a clean Linux checkout" requirement now also requires the GUI target to compile, not just configure. See spec delta in `specs/linux-port-verification/spec.md`.

## Impact

- `cmake/SetDefinitions.cmake` — replace `add_definitions(-D…='"…"')` with `add_compile_definitions(…="…")`.
- `cmake/IncludePackages.cmake` or `src/yars/view/gui/CMakeLists.txt` — only if option (a) or (b) is chosen; option (c) requires no CMake change.
- `src/yars/view/gui/OgreHandler.h` — adjust the `#include <OGRE/Codec_STBI/OgreSTBICodec.h>` form (option (c)).
- No runtime behaviour change on macOS (the macOS build was already passing this step). Linux build progresses past 5%.
- CI run on `.github/workflows/linux-build.yml` is the acceptance test for this change.
