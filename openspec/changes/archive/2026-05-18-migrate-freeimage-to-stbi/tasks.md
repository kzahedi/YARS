# Tasks

## 1. Audit texture assets

- [ ] 1.1 `grep -rEi '\.(tif|tiff|exr|raw)' materials/ xml/` —
      confirm no asset uses a format stb_image can't decode.
- [ ] 1.2 If any non-stb formats turn up, decide per-asset: convert
      to PNG/JPEG or document the regression. Skip the migration
      until resolved.

## 2. Drop FreeImage from CI

- [ ] 2.1 `.github/workflows/linux-build.yml`: remove
      `libfreeimage-dev` from the `apt-get install` list.
- [ ] 2.2 `.github/workflows/macos-build.yml`: remove `freeimage`
      from the `brew install` line and from the `brew list
      --versions` reporting line.

## 3. Build Ogre without FreeImage

- [ ] 3.1 Add `-DOGRE_BUILD_PLUGIN_FREEIMAGE=OFF` to the
      Ogre build step in `linux-build.yml` (next to the existing
      `-DOGRE_BUILD_PLUGIN_STBI=ON`). Bump the Ogre cache key
      (`ogre14-ubuntu22-glx-...-v2` → `v3`) so the new flag
      actually takes effect.
- [ ] 3.2 Same flag in `.github/workflows/macos-build.yml` if
      that workflow builds Ogre (it does — same cache pattern,
      bump that cache key too).
- [ ] 3.3 If the flag name is wrong for Ogre 14 (`OGRE_BUILD_FREEIMAGE`
      perhaps, or no toggle at all), fall back to dropping the
      runtime entry only.

## 4. Drop the runtime plugin load

- [ ] 4.1 `src/cfg/plugins.cfg.in`: confirm `Codec_FreeImage` is not
      listed. If it is, drop it. (Currently appears not to be.)
- [ ] 4.2 Search the C++ source for any explicit
      `_root->loadPlugin("Codec_FreeImage")` and drop it.

## 5. Verify

- [ ] 5.1 Local macOS build + run. Confirm braitenberg textures
      still render correctly. Capture a screenshot to compare
      against `docs/planning/macos-screenshots/braitenberg.png`.
- [ ] 5.2 Linux CI passes the full audit corpus + texture
      regression.
- [ ] 5.3 Cross off the freeimage item in
      `docs/planning/v0.8.7-open-points.md`.
