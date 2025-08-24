# YARS Material and Texture Rendering - Current State

## Date: 2025-08-23

## Problem Statement
The YARS simulator is rendering successfully but without any textures or colors. The expected output (as shown in screenshot.png) should display:
- Ground with concrete/stone texture
- Walls with brick texture  
- Colored robot bodies (green, purple, yellow, blue)
- White sensor arrays
- Colored sensor rays

## Work Completed

### 1. Fixed Resource Group Initialization Issue ✅
**Problem**: MaterialManager was trying to access "YARS" resource group before it existed  
**Solution**: Added resource group existence checks in MaterialManager.cpp
```cpp
bool hasYARSResourceGroup = Ogre::ResourceGroupManager::getSingleton().resourceGroupExists("YARS");
```

### 2. Created MaterialManager System ✅
**Location**: `src/yars/view/gui/MaterialManager.cpp` and `.h`
**Features**:
- Maps legacy YARS materials to RTSS-compatible versions
- Creates 16 default materials including colors and textures
- Supports vertex color materials for sensor rays
- Handles texture loading for ground and wall materials

### 3. Material Mappings Established ✅
**Key Mappings**:
```cpp
_materialAliases["YARS/DryGroundSmall"] = "RTSS_Ground";  
_materialAliases["YARS/Wall1"] = "RTSS_Wall";
_materialAliases["YARS/Red"] = "RTSS_Red";
_materialAliases["YARS/Green"] = "RTSS_Green";
_materialAliases["YARS/Blue"] = "RTSS_Blue";
_materialAliases["YARS/TraceLine"] = "RTSS_TraceLine";
```

### 4. Fixed SceneGraphEnvironmentNode ✅
**File**: `src/yars/view/gui/SceneGraphEnvironmentNode.cpp`
**Change**: Now uses MaterialManager to resolve material names properly
```cpp
materialName = MaterialManager::instance()->resolveMaterialName(materialName);
```

### 5. Added Vertex Color Support ✅
**Feature**: TraceLine materials now support vertex colors for sensor rays
```cpp
if (params.useVertexColors) {
    pass->setVertexColourTracking(Ogre::TVC_DIFFUSE | Ogre::TVC_AMBIENT);
    pass->setLightingEnabled(false);
}
```

## Current Issues

### 1. TraceLine Shader Error ⚠️
**Error**: `RenderSystem does not support FixedFunction, but technique of 'YARS/TraceLine' has no Vertex Shader`
**Cause**: Legacy YARS/TraceLine material is being loaded from materials file and used instead of RTSS_TraceLine
**Impact**: Sensor ray visualization may fail but doesn't crash the simulation

### 2. Texture Loading Not Verified ❓
**Status**: Materials are created with texture references but actual texture application needs verification
**Textures Present**: 
- `materials/ground.jpg` - Concrete texture
- `materials/wall.jpg` - Brick texture
- All textures exist in the materials directory

### 3. Material Resolution Priority Issue ⚠️
**Problem**: When both legacy and RTSS materials exist, legacy materials take precedence
**Example**: YARS/TraceLine is found and used instead of being redirected to RTSS_TraceLine

## Test Results

### Build Status: ✅ SUCCESS
- Compiles without errors
- Only Ogre3D header warnings (normal)

### Runtime Status: ✅ RUNNING
- Simulation runs without crashing
- Frame rendering successful
- Ground material correctly resolved to RTSS_Ground
- YARS materials loaded successfully after resource group initialization

### Material System Status:
```
MaterialManager: Created 16 default materials
- RTSS_Gray ✅
- RTSS_Red ✅  
- RTSS_Green ✅
- RTSS_Blue ✅
- RTSS_White ✅
- RTSS_Black ✅
- RTSS_ProximitySensor ✅
- RTSS_LDRSensor ✅
- RTSS_Wood ✅
- RTSS_Ground ✅ (with ground.jpg texture)
- RTSS_Wall ✅ (with wall.jpg texture)
- RTSS_Chain ✅
- RTSS_Wheel ✅
- RTSS_Simple ✅
- RTSS_TraceLine ✅ (vertex colors)
- RTSS_Flare ✅
```

## Next Steps Required

### 1. Fix TraceLine Material Override
**Approach**: Need to ensure RTSS_TraceLine is used instead of legacy YARS/TraceLine
**Options**:
- Override the legacy material after loading
- Modify material loading order
- Force RTSS technique creation for YARS/TraceLine

### 2. Verify Texture Application
**Method**: Use YARS image export to capture rendered frame
**Command**: Need to identify correct capture command from YARS options

### 3. Check Color Materials on Robot Bodies
**Verify**: Robot spheres should use colored materials (Red, Green, Blue, etc.)
**Current**: Materials are mapped but visual verification needed

## Files Modified

1. `/src/yars/view/gui/MaterialManager.cpp` - Core material management
2. `/src/yars/view/gui/MaterialManager.h` - Added useVertexColors flag
3. `/src/yars/view/gui/SceneGraphEnvironmentNode.cpp` - Fixed material resolution
4. `/src/yars/view/gui/OgreHandler.cpp` - Resource group initialization timing

## Technical Notes

### RTSS (Runtime Shader System) Integration
- All materials use DEFAULT scheme with RTSS on-demand generation
- Shader cache configured at `./shader_cache`
- OpenGL 3.3 core renderer requires shaders (no fixed-function pipeline)

### Resource Loading Order
1. MaterialManager creates default RTSS materials
2. YARS resource group created and initialized  
3. Legacy YARS materials loaded from .material files
4. MaterialManager creates RTSS techniques for legacy materials

### Material Resolution Flow
1. SceneGraph requests material (e.g., "YARS/DryGroundSmall")
2. MaterialManager::resolveMaterialName() maps to RTSS equivalent ("RTSS_Ground")
3. Ogre uses the RTSS material with proper shader support

## Validation Approach

To validate texture and color rendering:
1. Export rendered frame using YARS capture functionality
2. Compare with reference screenshot.png
3. Check for:
   - Ground texture visible
   - Wall texture visible  
   - Robot colors correct
   - Sensor rays visible

## Summary

The material system refactoring is largely complete with proper RTSS shader support. The main remaining issue is ensuring legacy materials are properly overridden with RTSS equivalents, particularly for YARS/TraceLine. Once this is resolved, textures and colors should render correctly as the material definitions and texture files are all in place.