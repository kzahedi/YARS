# Tasks

## 1. Header includes

- [x] 1.1 `OgreHandler.h`: collapsed the `#if __APPLE__ / #else` include block. Both platforms now use the single `<OGRE/...>` form (commit a09255a)
- [x] 1.2 Dropped `Ogre::STBIPlugin *_stbiPlugin;` (a09255a). Also removed the unused `Ogre::GL3PlusPlugin *_GLPlugin;` and `Ogre::ParticleFXPlugin *_particlePlugin;` members since no code now constructs them
- [x] 1.3 `ShaderManager.h`: removed the `OgreShaderFFPTransform.h` + `OgreShaderFFPTexturing.h` includes entirely; neither is needed once the FFPFactory registration is gone (a09255a)

## 2. Plugin / codec loading

- [x] 2.1 `OgreHandler.cpp`: dropped the macOS-only `new Ogre::GL3PlusPlugin(); install();` block. Single code path on both platforms now: `_root = new Ogre::Root("plugins.cfg", "ogre.cfg", "")` (a09255a)
- [x] 2.2 `ShaderManager.cpp`: dropped the macOS-only FFP factory registration block (a09255a). RTSS auto-registers FFP_Transform + FFP_Texturing in Ogre 14. The GLSL 150 shader profile call is now unconditional (was previously `#if !defined(__APPLE__)`)

## 3. CMake / link

- [x] 3.1 `src/yars/CMakeLists.txt`: dropped the `if(APPLE)` static-link block and the separate `if(NOT APPLE …)` dynamic-link block. Single block now uses `${OGRE_ROOT}/lib/libOgreMain${CMAKE_SHARED_LIBRARY_SUFFIX}` (resolves to `.so` on Linux, `.dylib` on macOS)
- [x] 3.2 Kept the macOS `-framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo` link flags. SDL2's framework dependencies cover Cocoa indirectly but Ogre's GL3+ context creation needs the others explicitly
- [ ] 3.3 Verify `SDL2::SDL2` imported target works on macOS via Homebrew's `pkg-config sdl2`. **Pending macOS CI run completion.**

## 4. Plugin config

- [x] 4.1 `plugins.cfg.in` already lists `RenderSystem_GL3Plus`, `Plugin_ParticleFX`, `Codec_STBI` — verified during the Linux work. `Codec_FreeImage` is not currently listed and may need to be added if any material script needs FreeImage's format support beyond what stb_image covers. **Defer until needed.**
- [x] 4.2 `OGRE_PLUGINS_DIR` resolution via `cmake/CreateConfigFiles.cmake` falls back to `${OGRE_ROOT}/lib/OGRE` when the variable isn't otherwise defined; same path on both platforms

## 5. macOS local verification (pending — needs your Mac)

- [ ] 5.1 On your Mac, delete `ext/ogre/install/` to remove the cached Ogre 13.6.4 install
- [ ] 5.2 Build Ogre 14 from the submodule using the same cmake invocation as `.github/workflows/macos-build.yml` (no `OGRE_STATIC=ON` — dynamic libs now)
- [ ] 5.3 Configure and build YARS — expect a clean build
- [ ] 5.4 Launch `./build/bin/yars --xml ../xml/braitenberg.xml` on macOS; confirm window opens, scene renders, mouse + scroll input work
- [ ] 5.5 Capture screenshot to `docs/planning/macos-screenshots/braitenberg.png` and commit

## 6. CI update

- [x] 6.1 `macos-build.yml`: removed `OGRE_MACOS_TAG` env var and the "Pin Ogre submodule to v13.6.4" step (a09255a)
- [ ] 6.2 Confirm the macOS CI run is green against the unpinned submodule. **In flight at commit time of this update.**
- [x] 6.3 Reconciled cache keys: Linux uses `ogre14-ubuntu22-glx-…-v2`, macOS uses `ogre14-macos14-glx-…-v1`. Both keyed on `.gitmodules` + Ogre's OgreMain CMakeLists hash, so a submodule bump invalidates both

## 7. Documentation

- [x] 7.1 Updated `CLAUDE.md` Ogre integration section: now reads Ogre 14.x dynamic linking with plugins.cfg, `OGRE_GLSUPPORT_USE_EGL=OFF`, `find_package(OGRE CONFIG)`, GLSL 1.50 RTSS target (commit de42dfa)
- [x] 7.2 Wrote `docs/macOS_Build.md` parallel to `docs/Linux_Build.md`. Covers Homebrew deps, Ogre 14 build with the framework-install layout note, `find_package(OGRE CONFIG)` rationale, `OGRE_BUILD_PLUGIN_DOT_SCENE=OFF`, Metal renderer choice, and a "refreshing a stale Ogre install" section for upgrading from 13.6.4
