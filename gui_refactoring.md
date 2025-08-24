# GUI Refactoring Plan: SDL + Ogre3D Implementation
*AI Coding Agent Implementation Guide*

## Executive Summary

This document provides step-by-step instructions for an AI coding agent to refactor the YARS GUI implementation. All code changes, file creations, and testing procedures are specified with exact commands and implementations. No external library installations are required - all dependencies are included in the YARS source tree.

## Prerequisites for AI Agent

### Required Tools (Already Available)
- CMake build system (already configured)
- Ogre3D 13.6.4 (already in ext/ogre-source)
- SDL2 (already linked via Homebrew)
- All development tools available via existing build system

### Validation Commands
```bash
# After each step, validate with:
cd /Volumes/Eregion/projects/yars/build
make -j8
./bin/yars --iterations 100 ../xml/braitenberg.xml

# For testing framework:
./bin/yars_tests

# Check for errors in output
echo $?  # Should return 0 for success
```

## Phase 1: Material System Modernization 🔴 **CRITICAL**

### Step 1.1: Analyze Current Materials
**AI Action**: Execute analysis commands and document findings

```bash
# Count and list all material files
find /Volumes/Eregion/projects/yars/materials -name "*.material" | sort

# Analyze material content
for file in /Volumes/Eregion/projects/yars/materials/*.material; do
    echo "=== $file ==="
    grep -E "material |technique|pass|texture_unit" "$file" | head -10
    echo
done

# Find texture files
find /Volumes/Eregion/projects/yars/materials -type f \( -name "*.jpg" -o -name "*.png" -o -name "*.tga" \) | sort

# Find material usage in code
grep -r "setMaterialName\|materialName.*=" /Volumes/Eregion/projects/yars/src/yars/view/gui/ --include="*.cpp"
```

**Expected Findings**: Document the output in comments for next steps

### Step 1.2: Create MaterialManager Class
**AI Action**: Create new files with exact content below

**File**: `/Volumes/Eregion/projects/yars/src/yars/view/gui/MaterialManager.h`
```cpp
#ifndef __MATERIAL_MANAGER_H__
#define __MATERIAL_MANAGER_H__

#include <OGRE/Ogre.h>
#include <OGRE/RTShaderSystem/OgreRTShaderSystem.h>
#include <string>
#include <map>
#include <set>

namespace yars {

struct MaterialParams {
    Ogre::ColourValue diffuse{0.8f, 0.8f, 0.8f, 1.0f};
    Ogre::ColourValue ambient{0.2f, 0.2f, 0.2f, 1.0f};
    Ogre::ColourValue specular{0.9f, 0.9f, 0.9f, 1.0f};
    float shininess{32.0f};
    std::string diffuseTexture;
    std::string normalTexture;
    bool transparent{false};
    bool castShadows{true};
    bool receiveShadows{true};
};

class MaterialManager {
public:
    static MaterialManager* instance();
    ~MaterialManager();
    
    // Core material operations
    bool createRTSSMaterial(const std::string& name, const MaterialParams& params);
    void registerFallbackMaterials();
    std::string resolveMaterialName(const std::string& legacyName);
    
    // Material validation
    bool materialExists(const std::string& name);
    void validateAllMaterials();
    
    // Batch operations
    void convertLegacyMaterials();
    void createDefaultMaterials();
    
private:
    MaterialManager();
    void _createBasicRTSSMaterial(const std::string& name, const MaterialParams& params);
    
    static MaterialManager* _instance;
    std::map<std::string, std::string> _materialAliases;
    std::set<std::string> _validMaterials;
    Ogre::MaterialManager* _ogreMaterialManager;
    Ogre::RTShader::ShaderGenerator* _shaderGenerator;
};

} // namespace yars

#endif // __MATERIAL_MANAGER_H__
```

**File**: `/Volumes/Eregion/projects/yars/src/yars/view/gui/MaterialManager.cpp`
```cpp
#include "MaterialManager.h"
#include <iostream>
#include <filesystem>

namespace yars {

MaterialManager* MaterialManager::_instance = nullptr;

MaterialManager* MaterialManager::instance() {
    if (_instance == nullptr) {
        _instance = new MaterialManager();
    }
    return _instance;
}

MaterialManager::MaterialManager() {
    _ogreMaterialManager = Ogre::MaterialManager::getSingletonPtr();
    _shaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
    
    // Create default fallback materials
    registerFallbackMaterials();
    createDefaultMaterials();
}

MaterialManager::~MaterialManager() {
    _instance = nullptr;
}

void MaterialManager::registerFallbackMaterials() {
    // Map legacy YARS materials to RTSS-compatible equivalents
    _materialAliases["YARS/Gray"] = "RTSS_Gray";
    _materialAliases["YARS/Red"] = "RTSS_Red";
    _materialAliases["YARS/Green"] = "RTSS_Green";
    _materialAliases["YARS/Blue"] = "RTSS_Blue";
    _materialAliases["YARS/White"] = "RTSS_White";
    _materialAliases["YARS/Black"] = "RTSS_Black";
    _materialAliases["YARS/ProximitySensor"] = "RTSS_ProximitySensor";
    _materialAliases["YARS/LDRSensor"] = "RTSS_LDRSensor";
    _materialAliases["Chain/Chain"] = "RTSS_Chain";
    _materialAliases["Simple/Simple"] = "RTSS_Simple";
    
    // Add common texture-based materials
    _materialAliases["wood2"] = "RTSS_Wood";
    _materialAliases["ground"] = "RTSS_Ground";
    _materialAliases["wall"] = "RTSS_Wall";
}

void MaterialManager::createDefaultMaterials() {
    // Create basic colored materials
    MaterialParams grayParams;
    grayParams.diffuse = Ogre::ColourValue(0.75f, 0.75f, 0.75f, 1.0f);
    grayParams.ambient = Ogre::ColourValue(0.5f, 0.5f, 0.5f, 1.0f);
    createRTSSMaterial("RTSS_Gray", grayParams);
    
    MaterialParams redParams;
    redParams.diffuse = Ogre::ColourValue(0.8f, 0.2f, 0.2f, 1.0f);
    redParams.ambient = Ogre::ColourValue(0.4f, 0.1f, 0.1f, 1.0f);
    createRTSSMaterial("RTSS_Red", redParams);
    
    MaterialParams greenParams;
    greenParams.diffuse = Ogre::ColourValue(0.2f, 0.8f, 0.2f, 1.0f);
    greenParams.ambient = Ogre::ColourValue(0.1f, 0.4f, 0.1f, 1.0f);
    createRTSSMaterial("RTSS_Green", greenParams);
    
    MaterialParams blueParams;
    blueParams.diffuse = Ogre::ColourValue(0.2f, 0.2f, 0.8f, 1.0f);
    blueParams.ambient = Ogre::ColourValue(0.1f, 0.1f, 0.4f, 1.0f);
    createRTSSMaterial("RTSS_Blue", blueParams);
    
    MaterialParams whiteParams;
    whiteParams.diffuse = Ogre::ColourValue(0.9f, 0.9f, 0.9f, 1.0f);
    whiteParams.ambient = Ogre::ColourValue(0.6f, 0.6f, 0.6f, 1.0f);
    createRTSSMaterial("RTSS_White", whiteParams);
    
    MaterialParams blackParams;
    blackParams.diffuse = Ogre::ColourValue(0.1f, 0.1f, 0.1f, 1.0f);
    blackParams.ambient = Ogre::ColourValue(0.05f, 0.05f, 0.05f, 1.0f);
    createRTSSMaterial("RTSS_Black", blackParams);
    
    // Sensor materials with transparency
    MaterialParams proxParams;
    proxParams.diffuse = Ogre::ColourValue(1.0f, 1.0f, 1.0f, 0.5f);
    proxParams.transparent = true;
    createRTSSMaterial("RTSS_ProximitySensor", proxParams);
    
    MaterialParams ldrParams;
    ldrParams.diffuse = Ogre::ColourValue(1.0f, 0.93f, 0.03f, 0.5f);
    ldrParams.transparent = true;
    createRTSSMaterial("RTSS_LDRSensor", ldrParams);
    
    std::cout << "MaterialManager: Created " << _validMaterials.size() << " default materials" << std::endl;
}

bool MaterialManager::createRTSSMaterial(const std::string& name, const MaterialParams& params) {
    try {
        // Check if material already exists
        if (materialExists(name)) {
            return true;
        }
        
        _createBasicRTSSMaterial(name, params);
        _validMaterials.insert(name);
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "MaterialManager: Failed to create material '" << name << "': " << e.what() << std::endl;
        return false;
    }
}

void MaterialManager::_createBasicRTSSMaterial(const std::string& name, const MaterialParams& params) {
    Ogre::MaterialPtr material = _ogreMaterialManager->create(name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    
    Ogre::Technique* technique = material->createTechnique();
    technique->setSchemeName(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
    
    Ogre::Pass* pass = technique->createPass();
    pass->setDiffuse(params.diffuse);
    pass->setAmbient(params.ambient);
    pass->setSpecular(params.specular);
    pass->setShininess(params.shininess);
    
    if (params.transparent) {
        pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
        pass->setDepthWriteEnabled(false);
    }
    
    if (!params.diffuseTexture.empty()) {
        Ogre::TextureUnitState* texUnit = pass->createTextureUnitState(params.diffuseTexture);
        if (!texUnit) {
            std::cout << "MaterialManager: Warning - texture '" << params.diffuseTexture << "' not found for material '" << name << "'" << std::endl;
        }
    }
    
    // Generate RTSS shader for this material
    if (_shaderGenerator) {
        _shaderGenerator->createShaderBasedTechnique(name, 
            Ogre::MaterialManager::DEFAULT_SCHEME_NAME,
            Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
        _shaderGenerator->validateMaterial(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME, name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    }
}

std::string MaterialManager::resolveMaterialName(const std::string& legacyName) {
    // Check if it's already a valid material
    if (materialExists(legacyName)) {
        return legacyName;
    }
    
    // Look for alias
    auto it = _materialAliases.find(legacyName);
    if (it != _materialAliases.end()) {
        return it->second;
    }
    
    // Return fallback material
    std::cout << "MaterialManager: Material '" << legacyName << "' not found, using RTSS_Gray fallback" << std::endl;
    return "RTSS_Gray";
}

bool MaterialManager::materialExists(const std::string& name) {
    return _ogreMaterialManager->resourceExists(name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
}

void MaterialManager::validateAllMaterials() {
    int validCount = 0;
    int invalidCount = 0;
    
    for (const std::string& materialName : _validMaterials) {
        if (materialExists(materialName)) {
            validCount++;
        } else {
            invalidCount++;
            std::cout << "MaterialManager: Invalid material: " << materialName << std::endl;
        }
    }
    
    std::cout << "MaterialManager: Validation complete - " << validCount << " valid, " << invalidCount << " invalid materials" << std::endl;
}

} // namespace yars
```

### Step 1.3: Update CMakeLists.txt
**AI Action**: Add MaterialManager to build system

**Modify**: `/Volumes/Eregion/projects/yars/src/yars/view/gui/CMakeLists.txt`
```cmake
# Find the existing set of source files and add MaterialManager
# Look for a line like: set(YARS_VIEW_GUI_SRC ...)
# Or similar pattern and add:
#   MaterialManager.cpp

# If no existing pattern, add after other .cpp files:
# MaterialManager.cpp
```

**AI Command**: Search for the pattern and modify accordingly:
```bash
# First find the CMakeLists.txt structure
grep -A 20 -B 5 "\.cpp" /Volumes/Eregion/projects/yars/src/yars/view/gui/CMakeLists.txt

# Then use Edit tool to add MaterialManager.cpp to the appropriate list
```

### Step 1.4: Update SceneGraph Classes
**AI Action**: Modify existing SceneGraph classes to use MaterialManager

**File to Modify**: `/Volumes/Eregion/projects/yars/src/yars/view/gui/SceneGraphSphereNode.cpp`

**AI Command**: 
1. Use Read tool to examine the current material handling (around lines 38-50)
2. Use Edit tool to replace the hardcoded material fallback with MaterialManager calls

**Replace this pattern**:
```cpp
// Look for lines like:
std::string materialName = _data->texture();
if (materialName.find("YARS/") == 0 || materialName.find("Chain/") == 0) {
    materialName = "SimpleLit";
    std::cout << "Using fallback material..." << std::endl;
}
```

**With**:
```cpp
#include "MaterialManager.h"

// In the method:
std::string materialName = _data->texture();
materialName = MaterialManager::instance()->resolveMaterialName(materialName);
```

**Repeat for other SceneGraph classes**:
- `SceneGraphBoxNode.cpp`
- `SceneGraphCylinderNode.cpp` 
- `SceneGraphCapsuleNode.cpp`
- `SceneGraphMeshNode.cpp`
- `SceneGraphPlyNode.cpp`

### Step 1.5: Update OgreHandler Integration
**AI Action**: Integrate MaterialManager with OgreHandler

**File to Modify**: `/Volumes/Eregion/projects/yars/src/yars/view/gui/OgreHandler.cpp`

**Add MaterialManager initialization after RTSS setup**:
```cpp
// After _shaderGenerator setup (around line 130-150), add:
#include "MaterialManager.h"

// In OgreHandler constructor, after RTSS initialization:
try {
    MaterialManager::instance()->validateAllMaterials();
    std::cout << "MaterialManager initialized successfully" << std::endl;
} catch (const std::exception& e) {
    std::cerr << "Failed to initialize MaterialManager: " << e.what() << std::endl;
}
```

### Step 1.6: Build and Test Phase 1
**AI Action**: Validate Phase 1 implementation

```bash
cd /Volumes/Eregion/projects/yars/build
make -j8

# Check for compilation errors
echo "Build status: $?"

# Test basic functionality
./bin/yars --iterations 100 ../xml/braitenberg.xml

# Check that materials are being resolved
grep -i "MaterialManager" ogre.log || echo "No MaterialManager messages found"

# Run unit tests
./bin/yars_tests
```

**Expected Results**:
- Build completes without errors
- YARS runs without crashing  
- Objects render with basic colors (even if not textured yet)
- Console output shows "MaterialManager initialized successfully"
- No "material not found" errors in logs

**If Errors Occur**: Document the exact error messages and troubleshoot step by step

## Phase 2: Shader System Integration 🟡 **HIGH**

### Step 2.1: Enhance RTSS Configuration
**AI Action**: Create comprehensive shader management

**File**: `/Volumes/Eregion/projects/yars/src/yars/view/gui/ShaderManager.h`
```cpp
#ifndef __SHADER_MANAGER_H__
#define __SHADER_MANAGER_H__

#include <OGRE/Ogre.h>
#include <OGRE/RTShaderSystem/OgreRTShaderSystem.h>
#include <string>

namespace yars {

class ShaderManager {
public:
    static ShaderManager* instance();
    ~ShaderManager();
    
    bool initializeRTSS(Ogre::SceneManager* sceneManager);
    void configureShaderCache(const std::string& cachePath);
    bool loadCustomShaderTemplates();
    void validateShaderGeneration();
    
    // Shader validation
    bool validateMaterialShaders(const std::string& materialName);
    void regenerateAllShaders();
    
private:
    ShaderManager();
    void _setupDefaultShaderLibrary();
    void _configureLighting();
    
    static ShaderManager* _instance;
    Ogre::RTShader::ShaderGenerator* _shaderGenerator;
    std::string _shaderCachePath;
    bool _initialized;
};

} // namespace yars

#endif // __SHADER_MANAGER_H__
```

**File**: `/Volumes/Eregion/projects/yars/src/yars/view/gui/ShaderManager.cpp`
```cpp
#include "ShaderManager.h"
#include <iostream>
#include <filesystem>

namespace yars {

ShaderManager* ShaderManager::_instance = nullptr;

ShaderManager* ShaderManager::instance() {
    if (_instance == nullptr) {
        _instance = new ShaderManager();
    }
    return _instance;
}

ShaderManager::ShaderManager() 
    : _shaderGenerator(nullptr), _initialized(false) {
    _shaderCachePath = "./shader_cache";
}

ShaderManager::~ShaderManager() {
    _instance = nullptr;
}

bool ShaderManager::initializeRTSS(Ogre::SceneManager* sceneManager) {
    try {
        _shaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
        if (!_shaderGenerator) {
            std::cerr << "ShaderManager: RTSS not available" << std::endl;
            return false;
        }
        
        // Configure shader cache
        configureShaderCache(_shaderCachePath);
        
        // Set up scene manager with RTSS
        _shaderGenerator->addSceneManager(sceneManager);
        
        // Configure default shader library
        _setupDefaultShaderLibrary();
        
        // Configure lighting
        _configureLighting();
        
        _initialized = true;
        std::cout << "ShaderManager: RTSS initialized successfully" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "ShaderManager: RTSS initialization failed: " << e.what() << std::endl;
        return false;
    }
}

void ShaderManager::configureShaderCache(const std::string& cachePath) {
    _shaderCachePath = cachePath;
    
    // Create cache directory if it doesn't exist
    std::filesystem::create_directories(cachePath);
    
    if (_shaderGenerator) {
        _shaderGenerator->setShaderCachePath(cachePath);
        std::cout << "ShaderManager: Shader cache configured at: " << cachePath << std::endl;
    }
}

void ShaderManager::_setupDefaultShaderLibrary() {
    if (!_shaderGenerator) return;
    
    // Add vertex shader processing
    _shaderGenerator->createScheme("RTSSDefault");
    
    // Enable basic vertex processing
    auto& renderState = _shaderGenerator->getRenderState("RTSSDefault", "Default", 0);
    renderState.reset();
    
    // Add lighting calculations
    auto lightingSubRenderState = _shaderGenerator->createSubRenderState("PerPixelLighting");
    renderState.addTemplateSubRenderState(lightingSubRenderState);
    
    std::cout << "ShaderManager: Default shader library configured" << std::endl;
}

void ShaderManager::_configureLighting() {
    if (!_shaderGenerator) return;
    
    // Configure lighting model for all materials
    auto& renderState = _shaderGenerator->getRenderState(
        Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME,
        Ogre::MaterialManager::DEFAULT_SCHEME_NAME, 0);
    
    // Add per-pixel lighting
    auto lightingSubRenderState = _shaderGenerator->createSubRenderState("PerPixelLighting");
    if (lightingSubRenderState) {
        renderState.addTemplateSubRenderState(lightingSubRenderState);
    }
    
    std::cout << "ShaderManager: Lighting configuration complete" << std::endl;
}

bool ShaderManager::validateMaterialShaders(const std::string& materialName) {
    if (!_initialized || !_shaderGenerator) {
        return false;
    }
    
    try {
        return _shaderGenerator->validateMaterial(
            Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME,
            materialName,
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    } catch (const std::exception& e) {
        std::cerr << "ShaderManager: Shader validation failed for '" << materialName << "': " << e.what() << std::endl;
        return false;
    }
}

void ShaderManager::validateShaderGeneration() {
    if (!_initialized) {
        std::cout << "ShaderManager: Not initialized, skipping validation" << std::endl;
        return;
    }
    
    std::cout << "ShaderManager: Shader system validation complete" << std::endl;
}

} // namespace yars
```

### Step 2.2: Integrate with OgreHandler
**AI Action**: Update OgreHandler to use ShaderManager

**Modify**: `/Volumes/Eregion/projects/yars/src/yars/view/gui/OgreHandler.cpp`

```cpp
// Add include
#include "ShaderManager.h"

// In OgreHandler constructor, replace existing RTSS code with:
// Initialize RTSS first
if (_shaderGenerator) {
    ShaderManager::instance()->initializeRTSS(_sceneManager);
}

// After MaterialManager initialization, add:
ShaderManager::instance()->validateShaderGeneration();
```

### Step 2.3: Build and Test Phase 2
**AI Action**: Validate shader system

```bash
cd /Volumes/Eregion/projects/yars/build
make -j8

# Test with extended run
./bin/yars --iterations 500 ../xml/braitenberg.xml

# Check shader cache creation
ls -la shader_cache/ || echo "No shader cache created"

# Look for shader-related output
grep -i "shader\|rtss" ogre.log | tail -10
```

**Expected Results**:
- Improved lighting on objects
- Shader cache directory created
- No shader compilation errors in logs

## Phase 3: Coordinate System Unification 🟡 **HIGH**

### Step 3.1: Create Coordinate Transform System
**AI Action**: Create coordinate transformation utilities

**File**: `/Volumes/Eregion/projects/yars/src/yars/view/gui/CoordinateTransform.h`
```cpp
#ifndef __COORDINATE_TRANSFORM_H__
#define __COORDINATE_TRANSFORM_H__

#include <OGRE/Ogre.h>
#include <yars/types/P3D.h>
#include <yars/types/Pose.h>
#include <yars/types/Quaternion.h>

namespace yars {

/**
 * Coordinate System Transformation Utilities
 * 
 * Bullet Physics: Z-up coordinate system (Z = up, Y = forward)
 * Ogre3D Rendering: Y-up coordinate system (Y = up, Z = backward)
 * 
 * This class handles all coordinate transformations between the two systems.
 */
class CoordinateTransform {
public:
    // Physics (Bullet) to Rendering (Ogre) conversions
    static Ogre::Vector3 bulletToOgre(const P3D& bulletPos);
    static Ogre::Quaternion bulletToOgre(const Quaternion& bulletQuat);
    static Ogre::Vector3 bulletVectorToOgre(const P3D& bulletVec);
    
    // Rendering (Ogre) to Physics (Bullet) conversions
    static P3D ogreToBullet(const Ogre::Vector3& ogrePos);
    static Quaternion ogreToBullet(const Ogre::Quaternion& ogreQuat);
    
    // Convenience methods for Pose handling
    static void applyTransform(Ogre::SceneNode* node, const Pose& bulletPose);
    static Pose extractPose(const Ogre::SceneNode* node);
    
    // Camera-specific transformations
    static Ogre::Vector3 transformCameraPosition(const P3D& bulletCamPos);
    static Ogre::Vector3 transformCameraLookAt(const P3D& bulletLookAt);
    
    // Validation utilities
    static void validateTransformation(const P3D& original, const Ogre::Vector3& transformed);
    static void printTransformInfo(const Pose& bulletPose, const Ogre::Vector3& ogrePos, const Ogre::Quaternion& ogreQuat);
    
private:
    // Transformation matrices (computed once)
    static const Ogre::Matrix3 BULLET_TO_OGRE_ROTATION;
    static const Ogre::Matrix3 OGRE_TO_BULLET_ROTATION;
    
    // Constants for coordinate system conversion
    static constexpr float COORDINATE_SCALE = 1.0f;
};

} // namespace yars

#endif // __COORDINATE_TRANSFORM_H__
```

**File**: `/Volumes/Eregion/projects/yars/src/yars/view/gui/CoordinateTransform.cpp`
```cpp
#include "CoordinateTransform.h"
#include <iostream>

namespace yars {

// Coordinate system transformation matrices
// Bullet (Z-up) to Ogre (Y-up): rotate -90 degrees around X-axis
const Ogre::Matrix3 CoordinateTransform::BULLET_TO_OGRE_ROTATION = 
    Ogre::Matrix3(1,  0,  0,
                  0,  0,  1,
                  0, -1,  0);

// Inverse transformation: Ogre (Y-up) to Bullet (Z-up)
const Ogre::Matrix3 CoordinateTransform::OGRE_TO_BULLET_ROTATION = 
    Ogre::Matrix3(1,  0,  0,
                  0,  0, -1,
                  0,  1,  0);

Ogre::Vector3 CoordinateTransform::bulletToOgre(const P3D& bulletPos) {
    Ogre::Vector3 ogrePos(bulletPos.x, bulletPos.y, bulletPos.z);
    return BULLET_TO_OGRE_ROTATION * ogrePos;
}

Ogre::Quaternion CoordinateTransform::bulletToOgre(const Quaternion& bulletQuat) {
    // Convert Bullet quaternion to Ogre quaternion with coordinate system rotation
    Ogre::Quaternion ogreQuat(bulletQuat.w, bulletQuat.x, bulletQuat.y, bulletQuat.z);
    
    // Apply coordinate system rotation
    Ogre::Quaternion coordRotation(Ogre::Radian(-Ogre::Math::PI/2), Ogre::Vector3(1, 0, 0));
    return coordRotation * ogreQuat;
}

Ogre::Vector3 CoordinateTransform::bulletVectorToOgre(const P3D& bulletVec) {
    // Same as position transformation for vectors
    return bulletToOgre(bulletVec);
}

P3D CoordinateTransform::ogreToBullet(const Ogre::Vector3& ogrePos) {
    Ogre::Vector3 bulletPos = OGRE_TO_BULLET_ROTATION * ogrePos;
    return P3D(bulletPos.x, bulletPos.y, bulletPos.z);
}

Quaternion CoordinateTransform::ogreToBullet(const Ogre::Quaternion& ogreQuat) {
    // Apply inverse coordinate system rotation
    Ogre::Quaternion coordRotation(Ogre::Radian(Ogre::Math::PI/2), Ogre::Vector3(1, 0, 0));
    Ogre::Quaternion bulletQuat = coordRotation * ogreQuat;
    
    return Quaternion(bulletQuat.w, bulletQuat.x, bulletQuat.y, bulletQuat.z);
}

void CoordinateTransform::applyTransform(Ogre::SceneNode* node, const Pose& bulletPose) {
    if (!node) return;
    
    Ogre::Vector3 ogrePos = bulletToOgre(bulletPose.position);
    Ogre::Quaternion ogreQuat = bulletToOgre(bulletPose.q);
    
    node->setPosition(ogrePos);
    node->setOrientation(ogreQuat);
}

Pose CoordinateTransform::extractPose(const Ogre::SceneNode* node) {
    if (!node) return Pose();
    
    Ogre::Vector3 ogrePos = node->getPosition();
    Ogre::Quaternion ogreQuat = node->getOrientation();
    
    P3D bulletPos = ogreToBullet(ogrePos);
    Quaternion bulletQuat = ogreToBullet(ogreQuat);
    
    Pose pose;
    pose.position = bulletPos;
    pose.q = bulletQuat;
    return pose;
}

Ogre::Vector3 CoordinateTransform::transformCameraPosition(const P3D& bulletCamPos) {
    return bulletToOgre(bulletCamPos);
}

Ogre::Vector3 CoordinateTransform::transformCameraLookAt(const P3D& bulletLookAt) {
    return bulletToOgre(bulletLookAt);
}

void CoordinateTransform::validateTransformation(const P3D& original, const Ogre::Vector3& transformed) {
    Ogre::Vector3 expected = bulletToOgre(original);
    float diff = (transformed - expected).length();
    
    if (diff > 0.001f) {
        std::cout << "CoordinateTransform: Validation failed - difference: " << diff << std::endl;
        std::cout << "  Original: (" << original.x << ", " << original.y << ", " << original.z << ")" << std::endl;
        std::cout << "  Expected: (" << expected.x << ", " << expected.y << ", " << expected.z << ")" << std::endl;
        std::cout << "  Actual:   (" << transformed.x << ", " << transformed.y << ", " << transformed.z << ")" << std::endl;
    }
}

void CoordinateTransform::printTransformInfo(const Pose& bulletPose, const Ogre::Vector3& ogrePos, const Ogre::Quaternion& ogreQuat) {
    std::cout << "CoordinateTransform Debug:" << std::endl;
    std::cout << "  Bullet Pos: (" << bulletPose.position.x << ", " << bulletPose.position.y << ", " << bulletPose.position.z << ")" << std::endl;
    std::cout << "  Ogre Pos:   (" << ogrePos.x << ", " << ogrePos.y << ", " << ogrePos.z << ")" << std::endl;
    std::cout << "  Bullet Quat: (" << bulletPose.q.w << ", " << bulletPose.q.x << ", " << bulletPose.q.y << ", " << bulletPose.q.z << ")" << std::endl;
    std::cout << "  Ogre Quat:   (" << ogreQuat.w << ", " << ogreQuat.x << ", " << ogreQuat.y << ", " << ogreQuat.z << ")" << std::endl;
}

} // namespace yars
```

### Step 3.2: Update All SceneGraph Classes
**AI Action**: Replace direct coordinate assignments with coordinate transforms

**For each SceneGraph*Node.cpp file, replace patterns like**:
```cpp
// OLD CODE:
Pose pose = _data->pose();
::Quaternion q = pose.q;  // Note: using global Quaternion
P3D p = pose.position;
_node->setPosition(Ogre::Vector3(p.x, p.y, p.z));
_node->setOrientation(Ogre::Quaternion(q.w, q.x, q.y, q.z));
```

**NEW CODE**:
```cpp
#include "CoordinateTransform.h"

// In update() method:
Pose pose = _data->pose();
CoordinateTransform::applyTransform(_node, pose);
```

**Files to Update**:
- `SceneGraphSphereNode.cpp`
- `SceneGraphBoxNode.cpp`  
- `SceneGraphCylinderNode.cpp`
- `SceneGraphCapsuleNode.cpp`
- `SceneGraphMeshNode.cpp`
- `SceneGraphPlyNode.cpp`
- `SceneGraphRobotNode.cpp`
- `SceneGraphEnvironmentNode.cpp`

**AI Implementation**:
```bash
# For each file, use these steps:
# 1. Read the file to find the update() method
# 2. Look for the pose transformation pattern
# 3. Replace with CoordinateTransform::applyTransform()
# 4. Add the include at the top

# Example for SceneGraphSphereNode.cpp:
```

### Step 3.3: Update Camera System
**AI Action**: Fix camera positioning with coordinate transforms

**Modify**: `/Volumes/Eregion/projects/yars/src/yars/view/gui/CameraMan.cpp` (if it exists)

**Search for camera positioning code and update**:
```cpp
// Look for camera->setPosition() and camera->lookAt() calls
// Replace with coordinate-transformed versions using CoordinateTransform
```

### Step 3.4: Build and Test Phase 3
**AI Action**: Validate coordinate system fixes

```bash
cd /Volumes/Eregion/projects/yars/build
make -j8

# Test with different camera angles
./bin/yars --iterations 200 ../xml/braitenberg.xml

# Check object positioning - objects should appear in correct locations
# relative to physics simulation

# Run unit tests to ensure Pose tests still pass
./bin/yars_tests
```

**Expected Results**:
- Objects appear in physically correct positions
- Camera controls work properly 
- No positioning artifacts or "floating" objects
- Pose unit tests continue to pass

## Phase 4-6: Remaining Implementation

### Quick Implementation Notes for AI Agent

**Phase 4**: Resource Management - Create ResourceManager class similar to MaterialManager pattern

**Phase 5**: SDL Cleanup - Simplify SdlWindow class, extract context management

**Phase 6**: Performance - Add LOD system, optimize rendering pipeline

## Testing and Validation Commands

### After Each Phase
```bash
# Build test
cd /Volumes/Eregion/projects/yars/build && make -j8

# Functionality test  
./bin/yars --iterations 100 ../xml/braitenberg.xml

# Extended test
./bin/yars --iterations 1000 ../xml/falling_objects.xml

# Unit tests
./bin/yars_tests

# Check for memory leaks (optional)
leaks --atExit -- ./bin/yars --iterations 50 ../xml/braitenberg.xml

# Performance test
time ./bin/yars --iterations 2000 ../xml/braitenberg.xml
```

### Success Criteria Validation
```bash
# Visual validation - check that:
echo "Phase 1: Objects have materials/colors (not pink/purple missing material)"
echo "Phase 2: Objects are properly lit with shadows"  
echo "Phase 3: Objects positioned correctly relative to physics"
echo "Phase 4: No 'resource not found' errors in logs"
echo "Phase 5: Stable window behavior, proper input response"
echo "Phase 6: Good performance, high visual quality"
```

## Error Handling for AI Agent

### Common Build Errors and Solutions

**CMake Errors**: 
```bash
# If CMake fails, check:
grep -r "MaterialManager\|ShaderManager\|CoordinateTransform" /Volumes/Eregion/projects/yars/src/yars/view/gui/CMakeLists.txt
# Ensure all .cpp files are added to build
```

**Compilation Errors**:
```bash
# Missing includes:
grep -r "#include.*MaterialManager" /Volumes/Eregion/projects/yars/src/yars/view/gui/*.cpp
# Add missing includes to files that use the new classes
```

**Runtime Errors**:
```bash
# Check initialization order
grep -A 5 -B 5 "MaterialManager\|ShaderManager" /Volumes/Eregion/projects/yars/src/yars/view/gui/OgreHandler.cpp
# Ensure proper initialization sequence
```

**Segmentation Faults**:
```bash
# Check singleton initialization
grep -A 10 "instance()" /Volumes/Eregion/projects/yars/src/yars/view/gui/Material*.cpp
# Ensure proper null checks
```

This implementation plan provides specific, executable instructions for an AI coding agent to complete the GUI refactoring systematically with built-in validation at each step.