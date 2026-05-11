# OGRE Integration Log

## Mission
Integrate OGRE 3D rendering into YARS starting from the April 2019 codebase.

## Critical Lessons from Previous Attempts
- Previous attempts got stuck in loops of 4-5 recurring bugs
- Need to track every approach tried and its outcome
- If an approach fails twice, try something fundamentally different

---

## Session: 2026-01-30

### Phase 1: Research & Planning

#### Step 1.1: Analyze Original OGRE Requirements
**Time:** 2026-01-30 14:15
**Status:** COMPLETE

**Findings:**

1. **OGRE Components Required:**
   - OgreOverlay
   - Plugin_ParticleFX
   - RenderSystem_GL (old OpenGL renderer, NOT GL3Plus)

2. **API Usage Pattern (OGRE 1.x style):**
   - `Ogre::Root` with plugins.cfg
   - `loadPlugin("Plugin_ParticleFX")` and `loadPlugin("RenderSystem_GL")`
   - `Ogre::SceneManager` with `ST_GENERIC`
   - `Ogre::OverlaySystem`
   - `ConfigFile::SectionIterator`

3. **macOS-specific issues identified:**
   - Plugin loading is commented out on macOS (lines 33-40 in OgreHandler.cpp)
   - This suggests plugin loading was problematic on macOS

4. **Key Files:**
   - `src/yars/view/gui/OgreHandler.cpp` - OGRE initialization
   - `src/yars/view/gui/SdlWindow.cpp` - SDL2 + OGRE window management
   - Custom `cmake/FindOGRE.cmake` - Expects pkg-config or manual paths

---

#### Step 1.2: Research OGRE Version Options
**Time:** 2026-01-30 14:20
**Status:** COMPLETE

**Version Timeline:**
- OGRE 1.12 → OGRE 13 → OGRE 14.5.0 (latest, Jan 2026)

**Key API Changes (1.x → 13/14):**
1. Fixed-function rendering REMOVED - must use shaders
2. Cameras/Lights should attach to SceneNodes
3. RenderSystem API modernized for programmable pipeline
4. OGRE 14 is 84% source compatible with OGRE 13

**Critical Insight:**
Previous attempt likely failed because YARS materials used **fixed-function** rendering which is removed in OGRE 13+. The solution is to use the **RTShader System (RTSS)** which automatically converts fixed-function materials to shaders.

**Decision: Use OGRE 14.5.0**
Rationale:
- Latest stable with good documentation
- Has RTShader System for automatic material conversion
- Build as git submodule (like previous attempt)
- Minimal dependencies option available

**Package Manager Status:**
- Homebrew: No OGRE formula
- MacPorts: Not available on this system
- vcpkg: Possible alternative
- Git submodule: Preferred approach

---

#### Step 1.3: Plan OGRE Integration Strategy
**Time:** 2026-01-30 14:30
**Status:** COMPLETE

**Strategy Overview:**

The integration will be done in **small, testable increments**. Each step will be validated before moving to the next. If a step fails twice with the same approach, we MUST try a fundamentally different approach.

**Phase A: Build OGRE Library (no YARS changes yet)**
1. Add OGRE 14 as git submodule
2. Configure OGRE build with minimal dependencies
3. Build OGRE and verify it works standalone
4. **Checkpoint:** OGRE library compiles and links

**Phase B: Minimal OGRE Integration (create window only)**
1. Update cmake to find/link OGRE
2. Create simplest possible OGRE window (no scene)
3. **Checkpoint:** Empty window opens and closes without crash

**Phase C: Basic Rendering (one cube)**
1. Add basic scene with one colored cube
2. Use RTSS for material (avoid fixed-function)
3. **Checkpoint:** Single cube renders on screen

**Phase D: Camera and Controls**
1. Add camera movement
2. Integrate SDL2 input
3. **Checkpoint:** Can move camera around cube

**Phase E: YARS Scene Integration**
1. Connect YARS physics objects to OGRE scene
2. Update materials to use RTSS
3. **Checkpoint:** Braitenberg robot visible

**Phase F: Full Feature Restoration**
1. Add textures
2. Add traces
3. Add overlays
4. **Checkpoint:** Full visualization working

**Anti-Loop Rules:**
- If cmake configuration fails, check OGRE_HOME and paths before anything else
- If linking fails, verify library architecture (arm64) first
- If runtime crashes, run with debugger to get exact crash location
- If materials don't render, enable RTSS debug logging
- NEVER spend more than 30 minutes on one error without trying new approach

---

### Phase A: Build OGRE Library

#### Step A.1: Add OGRE as Git Submodule
**Time:** 2026-01-30 14:35
**Status:** COMPLETE

**Action:** Added OGRE v14.3.0 as git submodule in `ext/ogre`
**Command:** `git submodule add https://github.com/OGRECave/ogre.git ext/ogre`
**Checkout:** `git checkout v14.3.0`

---

#### Step A.2: Configure OGRE Build
**Time:** 2026-01-30 14:40
**Status:** COMPLETE

**CMake Options Used:**
```cmake
-DCMAKE_BUILD_TYPE=Release
-DOGRE_BUILD_SAMPLES=OFF
-DOGRE_BUILD_TOOLS=OFF
-DOGRE_BUILD_TESTS=OFF
-DOGRE_INSTALL_DOCS=OFF
-DOGRE_BUILD_COMPONENT_OVERLAY=ON
-DOGRE_BUILD_COMPONENT_RTSHADERSYSTEM=ON
-DOGRE_BUILD_PLUGIN_PFX=ON
-DOGRE_BUILD_RENDERSYSTEM_GL3PLUS=ON
-DOGRE_BUILD_RENDERSYSTEM_GL=OFF
-DOGRE_STATIC=OFF
```

**Features Enabled:**
- OpenGL 3+ renderer (modern, shader-based)
- Overlay component (for text overlays)
- RTShader System (for fixed-function → shader conversion)
- Particle FX plugin
- Bullet integration (bonus)

**Note:** Using GL3Plus instead of GL because GL (OpenGL 2.x fixed-function) is deprecated on macOS.

---

#### Step A.3: Build OGRE Library
**Time:** 2026-01-30 14:45
**Status:** COMPLETE

**Command:** `make -j4`
**Duration:** ~5 minutes
**Result:** SUCCESS

**Built Frameworks (in ext/ogre/build/lib/macosx/):**
- Ogre.framework (main library)
- OgreOverlay.framework
- OgreRTShaderSystem.framework
- OgreBites.framework (includes CameraMan, Trays)
- RenderSystem_GL3Plus.framework
- Plugin_ParticleFX.framework
- And others (Terrain, Paging, etc.)

**CHECKPOINT A PASSED:** OGRE library compiled successfully

---

### Phase B: Minimal OGRE Integration

#### Step B.1: Update YARS CMake to Link OGRE
**Time:** 2026-01-30 14:55
**Status:** COMPLETE

**Changes Made:**
1. Updated `cmake/IncludePackages.cmake`:
   - Set up local OGRE build paths using symlinks to source directories
   - Created wrapper include structure: `ext/ogre/build/include/OGRE/` with symlinks to preserve `<OGRE/Ogre.h>` style includes
   - Added framework include directories for proper subdirectory handling (Overlay, RenderSystems, Plugins)

2. Include structure created:
   - `ext/ogre/build/include/OGRE/` → symlink to `OgreMain/include`
   - `ext/ogre/build/include/OGRE/Overlay/` → symlinks to Overlay headers
   - `ext/ogre/build/include/OGRE/RenderSystems/GL3Plus/` → symlinks to GL3Plus headers
   - `ext/ogre/build/include/OGRE/Plugins/ParticleFX/` → symlinks to ParticleFX headers

---

#### Step B.2: Fix OGRE 14 API Compatibility
**Time:** 2026-01-30 15:00
**Status:** COMPLETE

**API Changes Required:**

1. **OgreHandler.h/cpp:**
   - Changed `Ogre::GLPlugin` → `Ogre::GL3PlusPlugin`
   - Changed `#include <OGRE/RenderSystems/GL/OgreGLPlugin.h>` → `<OGRE/RenderSystems/GL3Plus/OgreGL3PlusPlugin.h>`
   - Changed `Ogre::ST_GENERIC` → `""` (empty string for default scene manager)
   - Changed `cf.getSectionIterator()` → `cf.getSettingsBySection()` (range-based)
   - Changed `lightSun->setPosition()` → `lightNode->setPosition()` (lights on nodes)
   - Changed plugin loading to use `installPlugin()` instead of `loadPlugin()`

2. **SdlWindow.h/cpp:**
   - Added `Ogre::SceneNode *_cameraNode` member
   - Changed all camera position/orientation methods to use the node:
     - `_camera->setPosition()` → `_cameraNode->setPosition()`
     - `_camera->lookAt()` → `_cameraNode->lookAt()`
     - `_camera->getPosition()` → `_cameraNode->getPosition()`
     - `_camera->getDirection()` → `_cameraNode->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z`
     - `_camera->yaw()/pitch()` → `_cameraNode->yaw()/pitch()`
     - `_camera->moveRelative()` → `_cameraNode->translate(..., TS_LOCAL)`

3. **ColoredTextAreaOverlayElement.cpp:**
   - Simplified `updateColours()` - private members no longer accessible
   - Removed access to `mColoursChanged`, `mRenderOp`, `mAllocSize`
   - Color functionality temporarily reduced (text displays, colors simplified)

---

#### Step B.3: Fix Linking Issues
**Time:** 2026-01-30 15:15
**Status:** COMPLETE

**Issues Resolved:**

1. **SDL2 not found:**
   - Changed `target_link_libraries(... SDL2)` → `target_link_libraries(... ${SDL2_LIBRARY})`

2. **OGRE frameworks not found:**
   - Added `target_link_options(${YARS_TARGET} PRIVATE "-F${OGRE_FRAMEWORK_DIR}")`

3. **Boost.Thread symbols missing:**
   - Added `thread` to Boost components in find_package
   - Added `Boost::thread` to target_link_libraries

---

### CHECKPOINT B PASSED: YARS Compiles with Visualization

**Time:** 2026-01-30 15:30
**Result:** SUCCESS

**Executable:** `/Users/zahedi/code/YARS/build/bin/yars` (6.1MB)
**Help Output:** Works correctly

**Summary of All Code Changes for OGRE 14:**
- 6 files modified
- Main themes: node-based cameras/lights, modern API replacements, plugin loading changes
- No major architectural changes to YARS itself

---

### Phase C: Runtime Testing

#### Step C.1: Test Visualization
**Time:** 2026-01-30 15:35
**Status:** COMPLETE (partial)

**Results:**

1. **plugins.cfg Fix:**
   - Created empty plugins.cfg (plugins loaded programmatically)
   - Resolved "RenderSystem_GL not found" error

2. **Scene Manager Fix:**
   - Changed from `""` to `"DefaultSceneManager"`
   - Resolved "No factory found for scene manager" error

3. **OpenGL Context Error:**
   - Error: "OpenGL 3.0 is not supported in initialiseContext"
   - This is expected - the terminal session has no display
   - Visualization requires running with a connected display (e.g., from Terminal.app)

4. **Physics Test (--nogui):**
   - Works correctly
   - Robot velocity data logged properly
   - Physics simulation runs independently of visualization

---

## Summary: OGRE 14 Integration Status

**Date:** 2026-02-01
**Status:** ✅ COMPLETE - GUI RENDERING WORKING

### What Works:
- ✅ YARS compiles with OGRE 14.3.0 visualization
- ✅ All OGRE API changes implemented
- ✅ Physics simulation runs correctly
- ✅ Controller libraries work
- ✅ GUI window opens and renders on macOS
- ✅ Textured materials render with RTSS-generated shaders
- ✅ braitenberg.xml and braitenberg_zoo.xml run successfully

### Key OGRE 14 Changes Made:
1. Cameras and lights attach to SceneNodes
2. RenderSystem_GL3Plus instead of GL
3. Plugin loading via installPlugin()
4. Modern ConfigFile iteration API
5. DefaultSceneManager type name
6. Simplified ColoredTextAreaOverlayElement

### Files Modified:
- `cmake/IncludePackages.cmake` - OGRE paths and includes
- `src/yars/CMakeLists.txt` - Framework paths, Boost.Thread
- `src/yars/view/gui/OgreHandler.h` - GL3Plus plugin
- `src/yars/view/gui/OgreHandler.cpp` - Plugin loading, scene manager, API updates
- `src/yars/view/gui/SdlWindow.h` - Camera node member
- `src/yars/view/gui/SdlWindow.cpp` - Node-based camera positioning
- `src/yars/view/gui/ColoredTextAreaOverlayElement.cpp` - OGRE 14 API compatibility

---

## Session: 2026-02-01

### GUI Rendering Fixes

**Issues Fixed:**

1. **YarsErrorHandler SIGSEGV** - Static `push()` methods accessed `_me` before calling `instance()`
2. **Controller not found** - `--lib` option wasn't adding to library search paths
3. **Empty ShaderGeneratorDefaultScheme techniques** - Materials in Chain.material had empty techniques, bypassing RTSS shader generation
4. **Material aliasing** - Textured materials were aliased to solid colors, losing textures

**Solution:**
- Fixed YarsErrorHandler to use `instance()` in all push methods
- Made `_libPathCandidates` static in Directories class for shared paths
- Removed empty ShaderGeneratorDefaultScheme techniques from Chain.material
- Let RTSS generate shader techniques on-demand, preserving textures
- Added `validateMaterial()` call in OgreHandler's handleSchemeNotFound

**Commit:** `b86f200 fix: OGRE 14 GUI rendering with RTSS shader generation`

### Next Steps:
1. Add integration and multi-robot tests
2. Generate video output
3. Clean up remaining Observer pattern references

