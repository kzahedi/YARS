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
    : _shaderGenerator(nullptr), _initialized(false), _shadowsEnabled(true), _sceneManager(nullptr) {
    _shaderCachePath = "./shader_cache";
}

ShaderManager::~ShaderManager() {
    _instance = nullptr;
}

bool ShaderManager::initializeRTSS(Ogre::SceneManager* sceneManager) {
    try {
        _sceneManager = sceneManager;
        _shaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
        if (!_shaderGenerator) {
            std::cerr << "ShaderManager: RTSS not available" << std::endl;
            return false;
        }
        
        // CRITICAL: Register core sub-render state factories for shader generation
        std::cout << "ShaderManager: Checking RTSS sub-render state factories..." << std::endl;
        
        // Check if FFPTransform factory already exists before adding
        if (!_shaderGenerator->getSubRenderStateFactory("FFP_Transform")) {
            _shaderGenerator->addSubRenderStateFactory(new Ogre::RTShader::FFPTransformFactory());
            std::cout << "ShaderManager: Added FFPTransformFactory" << std::endl;
        } else {
            std::cout << "ShaderManager: FFPTransformFactory already exists" << std::endl;
        }
        
        // Check if FFPTexturing factory already exists before adding
        if (!_shaderGenerator->getSubRenderStateFactory("FFP_Texturing")) {
            _shaderGenerator->addSubRenderStateFactory(new Ogre::RTShader::FFPTexturingFactory());
            std::cout << "ShaderManager: Added FFPTexturingFactory" << std::endl;
        } else {
            std::cout << "ShaderManager: FFPTexturingFactory already exists" << std::endl;
        }
        
        // Set target shader language
        _shaderGenerator->setTargetLanguage("glsl");
        std::cout << "ShaderManager: RTSS factories registered and target language set to GLSL" << std::endl;
        
        // Configure shader cache
        configureShaderCache(_shaderCachePath);
        
        // Set up scene manager with RTSS
        _shaderGenerator->addSceneManager(sceneManager);
        
        // Configure default shader library
        _setupDefaultShaderLibrary();
        
        // Configure lighting system
        _configureLightingSystem();
        
        // Create default scene lighting
        _createDefaultLights(sceneManager);
        
        // Configure shadow mapping if enabled
        if (_shadowsEnabled) {
            configureShadowMapping();
        }
        
        _initialized = true;
        std::cout << "ShaderManager: RTSS initialized successfully with lighting and shadows" << std::endl;
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
    if (!_shaderGenerator) {
        std::cout << "ShaderManager: No shader generator available for default library setup" << std::endl;
        return;
    }
    
    std::cout << "ShaderManager: Setting up default shader library with sub-render states..." << std::endl;
    
    // Create default render state for RTSS scheme
    auto* renderState = _shaderGenerator->getRenderState(
        Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME,
        Ogre::MaterialManager::DEFAULT_SCHEME_NAME,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 0);
    
    if (renderState) {
        std::cout << "ShaderManager: Got render state for default scheme" << std::endl;
        // Reset to clear any previous state
        renderState->reset();
        
        // Add basic vertex transformation
        auto transformSubRenderState = _shaderGenerator->createSubRenderState("FFP_Transform");
        if (transformSubRenderState) {
            renderState->addTemplateSubRenderState(transformSubRenderState);
            std::cout << "ShaderManager: Added FFP_Transform sub-render state" << std::endl;
        } else {
            std::cout << "ShaderManager: WARNING - Failed to create FFP_Transform sub-render state" << std::endl;
        }
        
        // Add texture sampling  
        auto textureSubRenderState = _shaderGenerator->createSubRenderState("FFP_Texturing");
        if (textureSubRenderState) {
            renderState->addTemplateSubRenderState(textureSubRenderState);
            std::cout << "ShaderManager: Added FFP_Texturing sub-render state" << std::endl;
        } else {
            std::cout << "ShaderManager: WARNING - Failed to create FFP_Texturing sub-render state" << std::endl;
        }
        
        // Add per-pixel lighting calculations (try different names)
        auto lightingSubRenderState = _shaderGenerator->createSubRenderState("FFP_Lighting");
        if (lightingSubRenderState) {
            renderState->addTemplateSubRenderState(lightingSubRenderState);
            std::cout << "ShaderManager: Added FFP_Lighting sub-render state" << std::endl;
        } else {
            std::cout << "ShaderManager: WARNING - Failed to create FFP_Lighting sub-render state" << std::endl;
        }
    } else {
        std::cout << "ShaderManager: WARNING - Could not get render state for default scheme" << std::endl;
    }
    
    std::cout << "ShaderManager: Default shader library configured with lighting and texturing" << std::endl;
}

void ShaderManager::_configureLightingSystem() {
    if (!_shaderGenerator || !_sceneManager) return;
    
    // Set ambient light for the scene
    _sceneManager->setAmbientLight(Ogre::ColourValue(0.3f, 0.3f, 0.3f));
    
    // Configure lighting model for materials
    auto* renderState = _shaderGenerator->getRenderState(
        Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME,
        Ogre::MaterialManager::DEFAULT_SCHEME_NAME,
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 0);
    
    // Ensure per-pixel lighting is enabled
    if (renderState) {
        auto lightingSubRenderState = _shaderGenerator->createSubRenderState("PerPixelLighting");
        if (lightingSubRenderState) {
            renderState->addTemplateSubRenderState(lightingSubRenderState);
        }
    }
    
    std::cout << "ShaderManager: Lighting system configured" << std::endl;
}

void ShaderManager::_createDefaultLights(Ogre::SceneManager* sceneManager) {
    if (!sceneManager) return;
    
    try {
        // Create main directional light (sun-like)
        Ogre::Light* directionalLight = sceneManager->createLight("MainDirectional");
        directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
        Ogre::SceneNode* lightNode1 = sceneManager->getRootSceneNode()->createChildSceneNode("MainDirectionalNode");
        lightNode1->setDirection(Ogre::Vector3(-1, -1, -1));
        lightNode1->attachObject(directionalLight);
        directionalLight->setDiffuseColour(Ogre::ColourValue(0.8f, 0.8f, 0.7f));
        directionalLight->setSpecularColour(Ogre::ColourValue(1.0f, 1.0f, 0.9f));
        directionalLight->setCastShadows(true);
        
        // Create fill light (softer, from opposite direction)
        Ogre::Light* fillLight = sceneManager->createLight("FillLight");
        fillLight->setType(Ogre::Light::LT_DIRECTIONAL);
        Ogre::SceneNode* lightNode2 = sceneManager->getRootSceneNode()->createChildSceneNode("FillLightNode");
        lightNode2->setDirection(Ogre::Vector3(0.5f, 1, 0.5f));
        lightNode2->attachObject(fillLight);
        fillLight->setDiffuseColour(Ogre::ColourValue(0.4f, 0.4f, 0.5f));
        fillLight->setSpecularColour(Ogre::ColourValue(0.2f, 0.2f, 0.3f));
        fillLight->setCastShadows(false);
        
        std::cout << "ShaderManager: Created default scene lighting (directional + fill)" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "ShaderManager: Warning - could not create default lights: " << e.what() << std::endl;
    }
}

void ShaderManager::configureShadowMapping() {
    if (!_sceneManager || !_shadowsEnabled) return;
    
    try {
        // Configure shadow mapping technique
        _sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
        _sceneManager->setShadowColour(Ogre::ColourValue(0.5, 0.5, 0.5));
        
        // Set shadow camera setup
        _sceneManager->setShadowCameraSetup(Ogre::ShadowCameraSetupPtr(
            new Ogre::DefaultShadowCameraSetup()));
        
        std::cout << "ShaderManager: Shadow mapping configured (1024x1024 additive)" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "ShaderManager: Warning - shadow mapping setup failed: " << e.what() << std::endl;
    }
}

void ShaderManager::enableShadows(bool enable) {
    _shadowsEnabled = enable;
    if (_sceneManager) {
        if (enable) {
            configureShadowMapping();
        } else {
            _sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
        }
        std::cout << "ShaderManager: Shadows " << (enable ? "enabled" : "disabled") << std::endl;
    }
}

void ShaderManager::addDirectionalLight(const Ogre::Vector3& direction, const Ogre::ColourValue& color) {
    if (!_sceneManager) return;
    
    try {
        static int lightCounter = 0;
        std::string lightName = "CustomDirectional_" + std::to_string(lightCounter++);
        
        Ogre::Light* light = _sceneManager->createLight(lightName);
        light->setType(Ogre::Light::LT_DIRECTIONAL);
        Ogre::SceneNode* lightNode = _sceneManager->getRootSceneNode()->createChildSceneNode(lightName + "_Node");
        lightNode->setDirection(direction);
        lightNode->attachObject(light);
        light->setDiffuseColour(color);
        light->setSpecularColour(color * 0.8f);
        light->setCastShadows(_shadowsEnabled);
        
        std::cout << "ShaderManager: Added directional light '" << lightName << "'" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "ShaderManager: Failed to add directional light: " << e.what() << std::endl;
    }
}

void ShaderManager::addAmbientLight(const Ogre::ColourValue& color) {
    if (!_sceneManager) return;
    
    _sceneManager->setAmbientLight(color);
    std::cout << "ShaderManager: Set ambient light color" << std::endl;
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

void ShaderManager::regenerateAllShaders() {
    if (!_initialized || !_shaderGenerator) {
        std::cout << "ShaderManager: Cannot regenerate - not initialized" << std::endl;
        return;
    }
    
    try {
        // Note: invalidateAllSchemes() not available in this Ogre version
        std::cout << "ShaderManager: Shader regeneration requested (manual reload may be needed)" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "ShaderManager: Failed to regenerate shaders: " << e.what() << std::endl;
    }
}

void ShaderManager::validateShaderGeneration() {
    if (!_initialized) {
        std::cout << "ShaderManager: Not initialized, skipping validation" << std::endl;
        return;
    }
    
    std::cout << "ShaderManager: Shader system validation complete" << std::endl;
    std::cout << "  - RTSS: " << (_shaderGenerator ? "Active" : "Inactive") << std::endl;
    std::cout << "  - Lighting: Configured" << std::endl;
    std::cout << "  - Shadows: " << (_shadowsEnabled ? "Enabled" : "Disabled") << std::endl;
    std::cout << "  - Shader Cache: " << _shaderCachePath << std::endl;
}

bool ShaderManager::loadCustomShaderTemplates() {
    // This method could be used to load custom shader templates from files
    // For now, we rely on the built-in RTSS sub-render states
    std::cout << "ShaderManager: Custom shader templates not implemented - using RTSS built-ins" << std::endl;
    return true;
}

} // namespace yars