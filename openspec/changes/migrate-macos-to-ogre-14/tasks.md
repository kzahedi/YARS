# Tasks

## 1. Header includes

- [ ] 1.1 `src/yars/view/gui/OgreHandler.h`: replace Apple framework
  paths (`<RenderSystems/GL3Plus/OgreGL3PlusPlugin.h>` etc.) with the
  same `<OGRE/...>` paths used by the Linux branch — Ogre 14 install
  is layout-portable across platforms now
- [ ] 1.2 Drop the `#if __APPLE__ Ogre::STBIPlugin *_stbiPlugin; #endif`
  member declaration; the codec is loaded via plugin loader, no class
  needed
- [ ] 1.3 `src/yars/view/gui/ShaderManager.h`: drop `#if __APPLE__`
  guard around `OgreShaderFFPTransform.h` / `OgreShaderFFPTexturing.h`
  includes (they're private in Ogre 14 — remove the includes entirely)

## 2. Plugin / codec loading

- [ ] 2.1 `src/yars/view/gui/OgreHandler.cpp`: drop the macOS-only
  static-plugin install block (`new Ogre::GL3PlusPlugin()` etc.). Rely
  on `Ogre::Root("plugins.cfg", "ogre.cfg", "")` to auto-load from
  `plugins.cfg` on both platforms
- [ ] 2.2 `src/yars/view/gui/ShaderManager.cpp`: drop the macOS-only
  `FFPTexturingFactory()` / `FFPTransformFactory()` registration —
  Ogre 14 RTSS auto-registers these internally on both platforms

## 3. CMake / link

- [ ] 3.1 `src/yars/CMakeLists.txt`: drop the `if(APPLE)` static-link
  block (lines that link `libOgreMainStatic.a`,
  `RenderSystem_GL3PlusStatic.framework`, etc.). Use the same
  `${OGRE_ROOT}/lib/libOgreMain.so ...` pattern the Linux branch uses,
  with `.so` swapped to `.dylib` via CMAKE_SHARED_LIBRARY_SUFFIX
- [ ] 3.2 Remove the macOS-only `-framework Cocoa -framework OpenGL`
  link flags only if SDL2 already pulls them in (verify with a clean
  build); otherwise keep
- [ ] 3.3 Verify `SDL2::SDL2` imported target works on macOS via
  Homebrew's `pkg-config sdl2`

## 4. Plugin config

- [ ] 4.1 `src/cfg/plugins.cfg.in`: confirm `Plugin=RenderSystem_GL3Plus`,
  `Plugin=Plugin_ParticleFX`, `Plugin=Codec_STBI`,
  `Plugin=Codec_FreeImage` are all listed. The Linux build already
  lists the first three; macOS may want `Codec_FreeImage` for JPEG/PNG
  parity if material scripts depend on it.
- [ ] 4.2 Verify `OGRE_PLUGINS_DIR` resolves correctly on macOS via
  `cmake/CreateConfigFiles.cmake` (the existing
  `${OGRE_ROOT}/lib/OGRE` path should work)

## 5. macOS local verification

- [ ] 5.1 On the contributor's Mac, delete `ext/ogre/install/` to
  remove the cached Ogre 13.6.4 install
- [ ] 5.2 Build Ogre 14 from the submodule via the same cmake invocation
  the CI workflow uses (but without `OGRE_STATIC=ON`)
- [ ] 5.3 Configure and build YARS — expect a clean build
- [ ] 5.4 Launch YARS GUI on macOS; confirm window opens, scene
  renders, mouse + scroll input work
- [ ] 5.5 Capture screenshot to `docs/planning/macos-screenshots/braitenberg.png`

## 6. CI update

- [ ] 6.1 `.github/workflows/macos-build.yml`: remove the `OGRE_MACOS_TAG`
  env var and the `Pin Ogre submodule to ...` step
- [ ] 6.2 Confirm the macOS CI run is green against the unpinned
  submodule (Ogre 14)
- [ ] 6.3 Reconcile macOS + Linux Ogre cache keys if convenient

## 7. Documentation

- [ ] 7.1 Update `CLAUDE.md` macOS section: Ogre version reads from the
  submodule pin (currently 14.4), not the legacy 13.6.4
- [ ] 7.2 Update `docs/macOS_Build.md` (if it exists) or write one if
  not — same `apt`-equivalent dep list for brew
