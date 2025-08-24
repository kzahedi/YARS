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
    bool useVertexColors{false};
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
    void createRTSSForLegacyMaterials();
    
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