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
    : _shaderGenerator(nullptr), _initialized(false), _shadowsEnabled(false), _sceneManager(nullptr) {
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
        
        // Register core sub-render state factories for shader generation
        // Check if FFPTransform factory already exists before adding
        if (!_shaderGenerator->getSubRenderStateFactory("FFP_Transform")) {
            _shaderGenerator->addSubRenderStateFactory(new Ogre::RTShader::FFPTransformFactory());
        }

        // Check if FFPTexturing factory already exists before adding
        if (!_shaderGenerator->getSubRenderStateFactory("FFP_Texturing")) {
            _shaderGenerator->addSubRenderStateFactory(new Ogre::RTShader::FFPTexturingFactory());
        }

        // Set target shader language
        _shaderGenerator->setTargetLanguage("glsl");
        
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
    }
}

void ShaderManager::_setupDefaultShaderLibrary() {
    if (!_shaderGenerator) {
        return;
    }

    // Use the single-argument overload to get the GLOBAL scheme render state.
    // The 4-argument overload is for per-material render states; passing an empty
    // material name always returns nullptr and silently skips all configuration.
    auto* renderState = _shaderGenerator->getRenderState(
        Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

    if (renderState) {
        // Per-pixel lighting: the RTSS automatically includes FFP_Transform and
        // FFP_Texturing; we only need to override the lighting to per-pixel.
        auto lightingSubRenderState = _shaderGenerator->createSubRenderState("SGX_PerPixelLighting");
        if (lightingSubRenderState) {
            renderState->addTemplateSubRenderState(lightingSubRenderState);
        } else {
            auto ffpLighting = _shaderGenerator->createSubRenderState("FFP_Lighting");
            if (ffpLighting) {
                renderState->addTemplateSubRenderState(ffpLighting);
            }
        }
    }
}

void ShaderManager::_configureLightingSystem() {
    if (!_shaderGenerator || !_sceneManager) return;

    // Set ambient light for the scene
    _sceneManager->setAmbientLight(Ogre::ColourValue(0.3f, 0.3f, 0.3f));
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

    } catch (const std::exception& e) {
        std::cerr << "ShaderManager: Warning - could not create default lights: " << e.what() << std::endl;
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

    } catch (const std::exception& e) {
        std::cerr << "ShaderManager: Warning - shadow mapping setup failed: " << e.what() << std::endl;
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

    } catch (const std::exception& e) {
        std::cerr << "ShaderManager: Failed to add directional light: " << e.what() << std::endl;
    }
}

void ShaderManager::addAmbientLight(const Ogre::ColourValue& color) {
    if (!_sceneManager) return;
    
    _sceneManager->setAmbientLight(color);
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
        return;
    }

    try {
        // Note: invalidateAllSchemes() not available in this Ogre version
    } catch (const std::exception& e) {
        std::cerr << "ShaderManager: Failed to regenerate shaders: " << e.what() << std::endl;
    }
}

void ShaderManager::validateShaderGeneration() {
}

bool ShaderManager::loadCustomShaderTemplates() {
    // This method could be used to load custom shader templates from files
    // For now, we rely on the built-in RTSS sub-render states
    return true;
}

} // namespace yars