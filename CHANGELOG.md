# Changelog

All notable changes to YARS are documented in this file.

## [Unreleased] - modernization-2026 Branch

### Added
- OGRE 14 support with GL3Plus render system
- Runtime Shader System (RTSS) integration for automatic shader generation
- Custom font shaders for overlay text rendering
- Custom sky dome shaders (YARS/SkyVP, YARS/SkyFP)
- SDL2 + OGRE 14 window integration for macOS
- CMake-configured resource paths (resources.cfg, rtss.cfg)
- RTSSMaterialListener for on-demand shader generation
- Missing robot and wall materials (top, bottom, body, face1-6, etc.)

### Changed
- Upgraded from OGRE 1.x to OGRE 14
- Migrated to GL3Plus render system (requires OpenGL 3.3+)
- Updated CMake minimum version to 3.16
- Updated Boost minimum version to 1.70
- Cameras and lights now attached to SceneNodes (OGRE 14 requirement)
- Scene manager creation uses "DefaultSceneManager" type
- Font definitions updated for OGRE 14 format (added `font` keyword)
- Materials updated to work with RTSS (removed fixed-function features)

### Removed
- Fixed-function pipeline support (not available in GL3Plus)
- Stencil shadows (disabled pending shader-based implementation)
- normalise_normals material setting (deprecated in OGRE 14)
- separateUV cubemap setting (deprecated)

### Fixed
- Plugin loading on macOS using installPlugin() instead of loadPlugin()
- STBI codec initialization for image loading (PNG, JPG)
- Resource path configuration using absolute paths
- Font material shader generation for overlay rendering

### Known Issues
- OSD text rendering may not display correctly (shader work in progress)
- Shadows are disabled (need shader-based shadow implementation)
- Some textures missing (GreenSkin.jpg, snake*.jpg, etc.)
- Linux visualization not yet implemented

## [0.7.28] - Previous Release

### Added
- Velocity can now be negative for actuators

## [0.7.27]

### Changed
- Simulator frequency is now an attribute of `<simulator>`

## [0.7.26]

### Changed
- Moved behaviour frequency definition from simulator tag to controller tag

## [0.7.25]

### Changed
- Renamed `<movable>` to `<robot>` and `<movables>` to `<robots>`

For older changes, see [version.h](src/yars/defines/version.h).
