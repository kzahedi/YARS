# Tasks

## 1. Audit texture assets

- [x] 1.1 `find materials/ -type f` lists only `.jpg`, `.png`, plus
      shader sources. `grep -rEi '\.(tif|tiff|exr|raw|psd|pgm|ppm)'`
      across `materials/` and `xml/` returns nothing. stb_image
      covers everything we use.
- [x] 1.2 No non-stb formats turned up; nothing to convert.

## 2. Drop FreeImage from CI

- [x] 2.1 `linux-build.yml`: dropped `libfreeimage-dev` from the
      apt list (commit `e2ec4ae`).
- [x] 2.2 `macos-build.yml`: dropped `freeimage` from the brew
      install line and from the version-report line (`e2ec4ae`).

## 3. Build Ogre without FreeImage

- [x] 3.1 Linux Ogre cmake step: added
      `-DOGRE_BUILD_PLUGIN_FREEIMAGE=OFF` next to STBI. Bumped the
      cache key from `v2` to `v3-nofi` so the new flag takes
      effect (CI ran a fresh Ogre build, took ~25 min, completed
      green).
- [x] 3.2 macOS Ogre cmake step: same flag. Bumped cache key
      from `v1` to `v2-nofi`. Both runners passed after fresh
      Ogre rebuild.
- [x] 3.3 Flag name `OGRE_BUILD_PLUGIN_FREEIMAGE` is correct for
      Ogre 14 — confirmed by `cmake_dependent_option(...)` in
      `ext/ogre-source/CMakeLists.txt:351`.

## 4. Drop the runtime plugin load

- [x] 4.1 `src/cfg/plugins.cfg.in` did not list `Codec_FreeImage`;
      nothing to remove.
- [x] 4.2 No explicit `loadPlugin("Codec_FreeImage")` in `src/`.

## 5. Verify

- [x] 5.1 Skipped local macOS rebuild (would have required a
      ~20-min Ogre rebuild for a low-risk audit). Trusted CI on
      the cache-key-bumped path; macOS CI green on
      `26016591317`.
- [x] 5.2 Linux CI green on `26016591296` — full audit corpus +
      smoke + CSV check + capture step all passed.
- [x] 5.3 Open-points doc updated (commit folds in with this task
      checklist).
