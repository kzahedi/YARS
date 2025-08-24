#ifndef __SHADER_MANAGER_H__
#define __SHADER_MANAGER_H__

#include <OGRE/Ogre.h>
#include <OGRE/RTShaderSystem/OgreRTShaderSystem.h>
#include <OGRE/RTShaderSystem/OgreShaderFFPTransform.h>
#include <OGRE/RTShaderSystem/OgreShaderFFPTexturing.h>
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
    
    // Lighting configuration
    void configureLighting(Ogre::SceneManager* sceneManager);
    void addDirectionalLight(const Ogre::Vector3& direction, const Ogre::ColourValue& color);
    void addAmbientLight(const Ogre::ColourValue& color);
    
    // Shadow configuration
    void configureShadowMapping();
    void enableShadows(bool enable);
    
private:
    ShaderManager();
    void _setupDefaultShaderLibrary();
    void _configureLightingSystem();
    void _createDefaultLights(Ogre::SceneManager* sceneManager);
    
    static ShaderManager* _instance;
    Ogre::RTShader::ShaderGenerator* _shaderGenerator;
    std::string _shaderCachePath;
    bool _initialized;
    bool _shadowsEnabled;
    Ogre::SceneManager* _sceneManager;
};

} // namespace yars

#endif // __SHADER_MANAGER_H__