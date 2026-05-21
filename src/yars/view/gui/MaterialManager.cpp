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
    
    // Create RTSS techniques for problematic legacy materials
    createRTSSForLegacyMaterials();
}

void MaterialManager::createRTSSForLegacyMaterials() {
    if (!_shaderGenerator) {
        return;
    }

    // Create RTSS techniques for ALL loaded materials that lack one,
    // and copy blend/depth settings from the base technique so that
    // alpha-blended materials (e.g. font overlays) stay transparent.
    auto _createRTSS = [&](Ogre::MaterialPtr material) {
        if (!material) return;
        for (unsigned short i = 0; i < material->getNumTechniques(); ++i) {
            if (material->getTechnique(i)->getSchemeName() ==
                Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME)
                return; // already has RTSS technique
        }
        bool ok = _shaderGenerator->createShaderBasedTechnique(
            *material,
            Ogre::MaterialManager::DEFAULT_SCHEME_NAME,
            Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME,
            true);
        if (!ok) return;
        _shaderGenerator->validateMaterial(
            Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME,
            material->getName(), material->getGroup());

        // Copy blend/depth settings from base technique (index 0) to RTSS
        // technique. Base tech scheme is "" not "Default", use index 0.
        Ogre::Technique *baseTech = material->getTechnique(0);
        Ogre::Technique *rtssTech = material->getTechnique(
            Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
        if (baseTech && rtssTech && baseTech != rtssTech) {
            for (unsigned short p = 0;
                 p < rtssTech->getNumPasses() && p < baseTech->getNumPasses(); ++p) {
                Ogre::Pass *rp = rtssTech->getPass(p);
                Ogre::Pass *bp = baseTech->getPass(p);
                if (bp->getSourceBlendFactor() != Ogre::SBF_ONE ||
                    bp->getDestBlendFactor()   != Ogre::SBF_ZERO) {
                    rp->setSceneBlending(bp->getSourceBlendFactor(),
                                         bp->getDestBlendFactor());
                }
                rp->setDepthCheckEnabled(bp->getDepthCheckEnabled());
                rp->setDepthWriteEnabled(bp->getDepthWriteEnabled());
            }
        }

        // Set the YARS-owned GLSL shadow caster on every RTSS-generated
        // technique. Without this, the caster pass falls back to the
        // fixed-function Ogre/TextureShadowCaster which fails to link
        // on GL3+ core ("technique has no Vertex Shader"). Skip the
        // caster material itself.
        if (rtssTech && material->getName() != "YARS/TextureShadowCaster") {
            if (rtssTech->getShadowCasterMaterial().get() == nullptr) {
                rtssTech->setShadowCasterMaterial("YARS/TextureShadowCaster");
            }
        }
    };

    // Cover built-in Ogre materials
    for (const std::string& name : {"BaseWhite", "BaseWhiteNoLighting"})
        _createRTSS(_ogreMaterialManager->getByName(name));

    // Cover every material in every loaded resource group
    Ogre::ResourceManager::ResourceMapIterator it = _ogreMaterialManager->getResourceIterator();
    while (it.hasMoreElements()) {
        Ogre::ResourcePtr res = it.getNext();
        Ogre::MaterialPtr mat = std::static_pointer_cast<Ogre::Material>(res);
        _createRTSS(mat);
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
    // Create in YARS group so texture lookups (ground.jpg, wheel.jpg, etc.) resolve correctly
    const std::string resGroup = Ogre::ResourceGroupManager::getSingleton().resourceGroupExists("YARS")
        ? "YARS" : Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;
    Ogre::MaterialPtr material = _ogreMaterialManager->create(name, resGroup);
    
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
        }
    }

    // Wire shadow caster: when SHADOWTYPE_TEXTURE_MODULATIVE is active,
    // Ogre substitutes this material in the caster pass. Caster has explicit
    // GLSL programs so it bypasses RTSS. Skip if this material itself is the
    // caster (avoid recursion) or castShadows was disabled by the caller.
    if (params.castShadows && name != "YARS/TextureShadowCaster") {
        technique->setShadowCasterMaterial("YARS/TextureShadowCaster");
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
    return "RTSS_Gray";
}

bool MaterialManager::materialExists(const std::string& name) {
    // Check default group first, then YARS group (materials from .material files)
    if (_ogreMaterialManager->resourceExists(name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME))
        return true;
    if (Ogre::ResourceGroupManager::getSingleton().resourceGroupExists("YARS") &&
        _ogreMaterialManager->resourceExists(name, "YARS"))
        return true;
    return false;
}

void MaterialManager::validateAllMaterials() {
    // CRITICAL: Force RTSS to finalize all shader generation after all materials are created
    if (_shaderGenerator) {
        try {
            // Force the RTSS to finalize all pending shader compilations
            _shaderGenerator->flushShaderCache();
            
            // Register and compile all RTSS materials
            for (const std::string& materialName : _validMaterials) {
                // Try default group first, then YARS group
                Ogre::MaterialPtr material = _ogreMaterialManager->getByName(materialName);
                if (!material)
                    material = _ogreMaterialManager->getByName(materialName, "YARS");
                if (material) {
                    // Register with RTSS (createShaderBasedTechnique must precede validateMaterial)
                    bool hasRTSS = false;
                    for (unsigned short i = 0; i < material->getNumTechniques(); ++i) {
                        if (material->getTechnique(i)->getSchemeName() ==
                            Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME) {
                            hasRTSS = true;
                            break;
                        }
                    }
                    if (!hasRTSS) {
                        _shaderGenerator->createShaderBasedTechnique(
                            *material,
                            Ogre::MaterialManager::DEFAULT_SCHEME_NAME,
                            Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME,
                            true);
                    }
                    // Now compile the RTSS shaders
                    const std::string group = material->getGroup();
                    _shaderGenerator->validateMaterial(
                        Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME, materialName, group);
                }
            }
            
        } catch (const std::exception& e) {
            std::cerr << "MaterialManager: RTSS finalization error: " << e.what() << std::endl;
        }
    }
    
}

void MaterialManager::convertLegacyMaterials() {
    // This method could be used to convert existing .material files to RTSS format
    // For now, we rely on the alias system
}

} // namespace yars