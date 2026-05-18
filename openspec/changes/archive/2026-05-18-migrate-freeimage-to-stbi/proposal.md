# Migrate from libfreeimage to stb_image (Ogre's STBI codec)

## Why

Homebrew has deprecated `freeimage`:

> freeimage has been deprecated because it has outstanding CVEs and
> requires patches to build! It will be disabled on 2027-01-10.

Both CI workflows currently install it:

- Linux: `libfreeimage-dev`
- macOS: `freeimage` (already shows the deprecation warning on every
  build)

After 2027-01-10 the macOS workflow will start failing the
`brew install` step. We have ~9 months of runway. The Linux package
is also a transitive Ogre dependency: Ogre's `Codec_FreeImage`
plugin links against it.

YARS itself does not call FreeImage directly. The plugin is only
used by Ogre to decode/encode texture image formats. Ogre 14 also
ships `Codec_STBI` (stb_image), which is already enabled in our
build (`OGRE_BUILD_PLUGIN_STBI=ON` in `.github/workflows/linux-build.yml`)
and listed in `plugins.cfg.in`.

stb_image covers PNG, JPEG, BMP, TGA, GIF, HDR, PSD, PIC, PNM — the
formats YARS textures and meshes actually use. The formats stb_image
does *not* cover (TIFF, OpenEXR, RAW) we don't use.

## What changes

1. **Drop `libfreeimage-dev` from `.github/workflows/linux-build.yml`**.
2. **Drop `freeimage` from `.github/workflows/macos-build.yml`**.
3. **Build Ogre without the FreeImage codec.** Pass
   `-DOGRE_BUILD_PLUGIN_FREEIMAGE=OFF` to the Ogre cmake step in
   both workflows. (Confirm the flag name in Ogre 14's cmake;
   `OGRE_BUILD_PLUGIN_*` is the consistent naming we already use
   for `STBI`, `DOT_SCENE`, `ASSIMP`.)
4. **Remove `Codec_FreeImage` from `plugins.cfg`** if it's still
   listed.
5. **Audit `materials/` and the texture loader path** for any file
   format stb_image can't read. If found, either convert the asset
   or call out the regression and revisit.

## Non-goals

- Replacing Ogre's image codec system. We just swap which codec
  plugin handles texture decode.
- Moving to a third-party image library (libpng, libjpeg-turbo).
  stb_image is header-only, already a transitive dep, and good
  enough for our texture set.

## Open questions

- Does the Ogre cmake variable for disabling the FreeImage plugin
  actually exist? If not, we may need to remove the plugin
  loadtime entry only. Quick check during implementation.
- Are there any material scripts that reference `.tif` / `.exr`
  files? `grep -ri '\.tif\|\.exr' materials/` will answer this.
