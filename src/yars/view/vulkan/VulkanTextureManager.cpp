#include "VulkanTextureManager.h"
#include "VulkanContext.h"
#include <iostream>
#include <filesystem>

namespace yars {

VulkanTextureManager* VulkanTextureManager::_instance = nullptr;

VulkanTextureManager* VulkanTextureManager::instance() {
    if (_instance == nullptr) {
        _instance = new VulkanTextureManager();
    }
    return _instance;
}

void VulkanTextureManager::destroy() {
    if (_instance != nullptr) {
        delete _instance;
        _instance = nullptr;
    }
}

VulkanTextureManager::~VulkanTextureManager() {
    clearAll();
}

void VulkanTextureManager::initialize(VulkanContext* context) {
    _context = context;

    // Add default YARS texture search paths
    addSearchPath("./textures");
    addSearchPath("../textures");
    addSearchPath("../../textures");
    addSearchPath("./resources/textures");
    addSearchPath("../resources/textures");

    std::cout << "VulkanTextureManager: Initialized with " << _searchPaths.size() << " search paths" << std::endl;
}

void VulkanTextureManager::addSearchPath(const std::string& path) {
    // Check if path exists before adding
    if (std::filesystem::exists(path)) {
        _searchPaths.push_back(path);
        std::cout << "VulkanTextureManager: Added texture search path: " << path << std::endl;
    }
}

VulkanTexture* VulkanTextureManager::getTexture(const std::string& filename) {
    if (filename.empty()) {
        return nullptr;
    }

    // Check cache first
    auto it = _cache.find(filename);
    if (it != _cache.end()) {
        return it->second.get();
    }

    // Not in cache - try to load
    std::string fullPath = findTextureFile(filename);
    if (fullPath.empty()) {
        std::cerr << "VulkanTextureManager: Texture file not found: " << filename << std::endl;
        return nullptr;
    }

    return loadTexture(filename, fullPath);
}

bool VulkanTextureManager::preloadTexture(const std::string& filename) {
    VulkanTexture* tex = getTexture(filename);
    return (tex != nullptr && tex->isValid());
}

void VulkanTextureManager::clearAll() {
    std::cout << "VulkanTextureManager: Clearing " << _cache.size() << " cached textures" << std::endl;
    _cache.clear();
}

std::string VulkanTextureManager::findTextureFile(const std::string& filename) {
    // Try absolute path first
    if (std::filesystem::exists(filename)) {
        return filename;
    }

    // Try search paths
    for (const std::string& searchPath : _searchPaths) {
        std::string fullPath = searchPath + "/" + filename;
        if (std::filesystem::exists(fullPath)) {
            return fullPath;
        }
    }

    return "";
}

VulkanTexture* VulkanTextureManager::loadTexture(const std::string& filename, const std::string& fullPath) {
    if (!_context) {
        std::cerr << "VulkanTextureManager: Context not initialized" << std::endl;
        return nullptr;
    }

    std::cout << "VulkanTextureManager: Loading texture '" << filename << "' from: " << fullPath << std::endl;

    // Create new texture
    auto texture = std::make_unique<VulkanTexture>(_context);

    // Load from file
    if (!texture->loadFromFile(fullPath)) {
        std::cerr << "VulkanTextureManager: Failed to load texture: " << fullPath << std::endl;
        return nullptr;
    }

    // Add to cache
    VulkanTexture* texturePtr = texture.get();
    _cache[filename] = std::move(texture);

    std::cout << "VulkanTextureManager: Successfully loaded and cached texture: " << filename << std::endl;
    return texturePtr;
}

std::string VulkanTextureManager::materialToTexture(const std::string& materialName) {
    // Map YARS/Ogre material names to texture filenames
    // Based on materials/YARS.material and materials/Chain.material

    if (materialName.empty()) return "";

    // YARS materials
    if (materialName == "YARS/Wall1" || materialName == "YARS/Wall2" || materialName == "YARS/Wall3") {
        return "wall.jpg";
    }
    if (materialName == "YARS/DryGround" || materialName == "YARS/DryGroundSmall") {
        return "ground.jpg";
    }
    if (materialName == "YARS/Green") {
        return "GreenSkin.jpg";
    }
    if (materialName == "YARS/Blue") {
        // Blue is typically a solid color, but could map to a blue texture
        return ""; // No texture for solid colors
    }

    // Chain materials (robot parts)
    if (materialName == "Chain/Wheel") {
        return "wheel.jpg";
    }
    if (materialName == "Chain/Circuit/Body/Green") {
        return "CBT.jpg";  // Circuit board texture
    }
    if (materialName == "Chain/Circuit/Body/Blue") {
        return "CBT_blue.jpg";
    }
    if (materialName == "Chain/Circuit/Body/Orange") {
        return "CBT_orange.jpg";
    }
    if (materialName == "Chain/Circuit/Body/Purple") {
        return "CBT_purple.jpg";
    }
    if (materialName == "Chain/Circuit/Body/Yellow") {
        return "CBT_yellow.jpg";
    }
    if (materialName == "Chain/Circuit/Top/Green" ||
        materialName == "Chain/Circuit/Top/Blue" ||
        materialName == "Chain/Circuit/Top/Orange" ||
        materialName == "Chain/Circuit/Top/Purple" ||
        materialName == "Chain/Circuit/Top/Yellow") {
        // Top materials are either solid colors or animated textures
        // For simplicity, use the base CBT texture
        if (materialName.find("Blue") != std::string::npos) return "CBT_blue.jpg";
        if (materialName.find("Orange") != std::string::npos) return "CBT_orange.jpg";
        if (materialName.find("Purple") != std::string::npos) return "CBT_purple.jpg";
        if (materialName.find("Yellow") != std::string::npos) return "CBT_yellow.jpg";
        return "CBT.jpg";  // Green
    }
    if (materialName == "Chain/DryGround") {
        return "ground.jpg";
    }

    // Unknown material
    return "";
}

VulkanTexture* VulkanTextureManager::getTextureByMaterial(const std::string& materialName) {
    std::string textureFile = materialToTexture(materialName);
    if (textureFile.empty()) {
        return nullptr;
    }
    return getTexture(textureFile);
}

} // namespace yars
