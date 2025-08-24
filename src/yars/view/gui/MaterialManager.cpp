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
    _materialAliases["Chain/Wheel"] = "RTSS_Wheel";
    _materialAliases["Chain/DryGround"] = "RTSS_Ground";
    _materialAliases["Simple/Simple"] = "RTSS_Simple";
    _materialAliases["SimpleLit"] = "RTSS_Gray";
    _materialAliases["SimpleWhite"] = "RTSS_White";
    
    // Add common texture-based materials
    _materialAliases["wood2"] = "RTSS_Wood";
    _materialAliases["ground"] = "RTSS_Ground";
    _materialAliases["wall"] = "RTSS_Wall";
    
    // Add YARS ground and environment materials
    _materialAliases["YARS/DryGround"] = "RTSS_Ground";
    _materialAliases["YARS/DryGroundSmall"] = "RTSS_Ground"; 
    _materialAliases["YARS/Wall1"] = "RTSS_Wall";
    _materialAliases["YARS/Wall2"] = "RTSS_Wall";
    _materialAliases["YARS/Wall3"] = "RTSS_Wall";
    
    // Add YARS trace and effect materials
    _materialAliases["YARS/TraceLine"] = "RTSS_TraceLine";
    _materialAliases["YARS/Flare"] = "RTSS_Flare";
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
    
    // Texture-based materials (will try to load textures)
    MaterialParams woodParams;
    woodParams.diffuse = Ogre::ColourValue(0.6f, 0.4f, 0.2f, 1.0f);
    woodParams.diffuseTexture = "wood2.jpg";
    createRTSSMaterial("RTSS_Wood", woodParams);
    
    MaterialParams groundParams;
    groundParams.diffuse = Ogre::ColourValue(0.5f, 0.4f, 0.3f, 1.0f);
    groundParams.diffuseTexture = "ground.jpg";
    createRTSSMaterial("RTSS_Ground", groundParams);
    
    MaterialParams wallParams;
    wallParams.diffuse = Ogre::ColourValue(0.7f, 0.7f, 0.6f, 1.0f);
    wallParams.diffuseTexture = "wall.jpg";
    createRTSSMaterial("RTSS_Wall", wallParams);
    
    MaterialParams chainParams;
    chainParams.diffuse = Ogre::ColourValue(0.4f, 0.4f, 0.4f, 1.0f);
    chainParams.specular = Ogre::ColourValue(0.8f, 0.8f, 0.8f, 1.0f);
    chainParams.shininess = 64.0f;
    createRTSSMaterial("RTSS_Chain", chainParams);
    
    MaterialParams wheelParams;
    wheelParams.diffuse = Ogre::ColourValue(0.3f, 0.3f, 0.3f, 1.0f);
    wheelParams.diffuseTexture = "wheel.jpg";
    createRTSSMaterial("RTSS_Wheel", wheelParams);
    
    MaterialParams simpleParams;
    simpleParams.diffuse = Ogre::ColourValue(0.8f, 0.8f, 0.8f, 1.0f);
    simpleParams.ambient = Ogre::ColourValue(0.3f, 0.3f, 0.3f, 1.0f);
    createRTSSMaterial("RTSS_Simple", simpleParams);
    
    // Special materials for traces and effects
    MaterialParams traceParams;
    traceParams.diffuse = Ogre::ColourValue(1.0f, 1.0f, 1.0f, 1.0f);  // Will use vertex colors
    traceParams.ambient = Ogre::ColourValue(1.0f, 1.0f, 1.0f, 1.0f);
    traceParams.transparent = true;
    traceParams.castShadows = false;
    traceParams.useVertexColors = true;
    createRTSSMaterial("RTSS_TraceLine", traceParams);
    
    MaterialParams flareParams;
    flareParams.diffuse = Ogre::ColourValue(1.0f, 1.0f, 1.0f, 1.0f);
    flareParams.ambient = Ogre::ColourValue(0.0f, 0.0f, 0.0f, 1.0f);
    flareParams.transparent = true;
    flareParams.castShadows = false;
    flareParams.diffuseTexture = "flare.jpg";
    createRTSSMaterial("RTSS_Flare", flareParams);
    
    std::cout << "MaterialManager: Created " << _validMaterials.size() << " default materials" << std::endl;
    
    // Create RTSS techniques for problematic legacy materials
    createRTSSForLegacyMaterials();
}

void MaterialManager::createRTSSForLegacyMaterials() {
    if (!_shaderGenerator) {
        std::cout << "MaterialManager: No shader generator - skipping legacy material RTSS creation" << std::endl;
        return;
    }
    
    std::cout << "MaterialManager: Creating RTSS techniques for legacy materials..." << std::endl;
    
    // List of legacy materials that need RTSS techniques
    std::vector<std::string> legacyMaterials = {
        "BaseWhite",
        "BaseWhiteNoLighting", 
        "YARS/FlyingLightMaterial"
    };
    
    // Also try some alternative YARS materials to test what actually loads
    std::vector<std::string> testYARSMaterials = {
        "YARS/Gray",
        "YARS/Red", 
        "YARS/TraceLine"
    };
    
    std::cout << "MaterialManager: Testing some YARS materials..." << std::endl;
    
    // Check if YARS resource group exists before accessing it
    bool hasYARSResourceGroup = Ogre::ResourceGroupManager::getSingleton().resourceGroupExists("YARS");
    
    for (const std::string& testName : testYARSMaterials) {
        Ogre::MaterialPtr testMat = _ogreMaterialManager->getByName(testName);
        if (!testMat && hasYARSResourceGroup) {
            testMat = _ogreMaterialManager->getByName(testName, "YARS");
        }
        std::cout << "  Test material '" << testName << "': " << (testMat ? "found" : "not found") << std::endl;
    }
    
    for (const std::string& materialName : legacyMaterials) {
        try {
            Ogre::MaterialPtr material = _ogreMaterialManager->getByName(materialName);
            
            // Also try searching in specific resource groups if not found
            if (!material && materialName.find("YARS/") != std::string::npos && hasYARSResourceGroup) {
                material = _ogreMaterialManager->getByName(materialName, "YARS");
                std::cout << "MaterialManager: Searched for '" << materialName << "' in YARS group: " << (material ? "found" : "not found") << std::endl;
            }
            
            if (material) {
                // Check if it already has an RTSS technique
                bool hasRTSSScheme = false;
                for (unsigned short i = 0; i < material->getNumTechniques(); ++i) {
                    if (material->getTechnique(i)->getSchemeName() == Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME) {
                        hasRTSSScheme = true;
                        break;
                    }
                }
                
                if (!hasRTSSScheme) {
                    bool success = _shaderGenerator->createShaderBasedTechnique(*material,
                        Ogre::MaterialManager::DEFAULT_SCHEME_NAME,
                        Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME,
                        true);
                    
                    if (success) {
                        std::cout << "MaterialManager: Created RTSS technique for legacy material '" << materialName << "'" << std::endl;
                    } else {
                        std::cout << "MaterialManager: WARNING - Failed to create RTSS technique for '" << materialName << "'" << std::endl;
                    }
                }
            } else {
                std::cout << "MaterialManager: Legacy material '" << materialName << "' not found" << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << "MaterialManager: Error processing legacy material '" << materialName << "': " << e.what() << std::endl;
        }
    }
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
    // Use DEFAULT scheme - RTSS technique will be created on-demand by SGTechniqueResolverListener
    technique->setSchemeName(Ogre::MaterialManager::DEFAULT_SCHEME_NAME);
    
    Ogre::Pass* pass = technique->createPass();
    
    if (params.useVertexColors) {
        // Enable vertex color usage
        pass->setVertexColourTracking(Ogre::TVC_DIFFUSE | Ogre::TVC_AMBIENT);
        pass->setDiffuse(1.0f, 1.0f, 1.0f, 1.0f);  // White base color for vertex colors
        pass->setAmbient(1.0f, 1.0f, 1.0f);  // RGB only
        pass->setLightingEnabled(false);  // Vertex color materials typically don't use lighting
    } else {
        pass->setDiffuse(params.diffuse);
        pass->setAmbient(params.ambient);
        // Ensure lighting is enabled for RTSS
        pass->setLightingEnabled(true);
    }
    
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
    
    // Material created with DEFAULT scheme - RTSS technique will be generated on-demand during rendering
    std::cout << "MaterialManager: Created material '" << name << "' with DEFAULT scheme (RTSS on-demand)" << std::endl;
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
    
    // CRITICAL: Force RTSS to finalize all shader generation after all materials are created
    if (_shaderGenerator) {
        std::cout << "MaterialManager: Finalizing RTSS shader generation..." << std::endl;
        try {
            // Force the RTSS to finalize all pending shader compilations
            _shaderGenerator->flushShaderCache();
            
            // Re-validate and compile all materials with RTSS scheme
            for (const std::string& materialName : _validMaterials) {
                Ogre::MaterialPtr material = _ogreMaterialManager->getByName(materialName);
                if (material) {
                    // Force material compilation to generate actual shader programs
                    material->touch();
                    material->compile();
                    _shaderGenerator->validateMaterial(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME, materialName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
                }
            }
            
            std::cout << "MaterialManager: RTSS finalization completed successfully" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "MaterialManager: RTSS finalization error: " << e.what() << std::endl;
        }
    }
    
    std::cout << "MaterialManager: Validation complete - " << validCount << " valid, " << invalidCount << " invalid materials" << std::endl;
}

void MaterialManager::convertLegacyMaterials() {
    // This method could be used to convert existing .material files to RTSS format
    // For now, we rely on the alias system
    std::cout << "MaterialManager: Legacy material conversion not implemented - using alias system" << std::endl;
}

} // namespace yars