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

### C++17 Migration (Boost Replacement)
- Replaced `boost::thread` with `std::thread` in MainLoopThread.h, yarsMain.cpp
- Replaced `boost::function` with `std::function` in KeyboardShortcut.h, KeyHandler.cpp
- Replaced `boost::filesystem` with `std::filesystem` in Directories.h/cpp, FileSystemOperations.h/cpp, XsdGraphvizExporter.h
- Replaced `boost::date_time` with `std::chrono` in Timer.h
- Replaced `boost::format` with `std::put_time` and `std::ostringstream`
- Replaced `boost::tokenizer` and `BOOST_FOREACH` with C++17 string operations
- Replaced `boost::replace_all_copy` with `std::regex_replace` in CSVLogger.cpp
- Replaced `fs::initial_path()` with `fs::current_path()` (std::filesystem)
- Replaced `fs::system_complete()` with `fs::absolute()` (std::filesystem)
- Removed boost::filesystem and boost::thread from CMake dependencies
- Remaining Boost dependencies: program_options (no C++17 equivalent), circular_buffer (used in GnuplotLogger.h, DataTrace.h)

### Code Modernization (C++11/14/17)
- Replaced NULL with nullptr in modified files:
  - KeyHandler.cpp
  - FileSystemOperations.cpp
  - Directories.cpp
  - YarsConfiguration.cpp
  - KeyboardShortcut.cpp
  - KeyboardShortcuts.cpp

### Removed
- Fixed-function pipeline support (not available in GL3Plus)
- Stencil shadows (disabled pending shader-based implementation)
- normalise_normals material setting (deprecated in OGRE 14)
- separateUV cubemap setting (deprecated)
- boost::filesystem dependency (replaced with std::filesystem)
- boost::thread dependency (replaced with std::thread)

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
