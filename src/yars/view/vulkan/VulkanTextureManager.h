#ifndef __VULKAN_TEXTURE_MANAGER_H__
#define __VULKAN_TEXTURE_MANAGER_H__

#include <yars/view/vulkan/VulkanTexture.h>
#include <string>
#include <map>
#include <memory>

namespace yars {

class VulkanContext;

/**
 * VulkanTextureManager - Manages texture loading and caching
 *
 * Singleton pattern for centralized texture management:
 * - Caches loaded textures to avoid duplicate loading
 * - Manages texture resource lifecycle
 * - Provides texture search paths for YARS resource directories
 *
 * Usage:
 *   VulkanTextureManager* mgr = VulkanTextureManager::instance();
 *   VulkanTexture* tex = mgr->getTexture("wood2.jpg");
 *   if (tex && tex->isValid()) {
 *       // Use texture for rendering
 *   }
 */
class VulkanTextureManager {
public:
    static VulkanTextureManager* instance();
    static void destroy();

    /**
     * Initialize texture manager with Vulkan context
     * Must be called before loading any textures
     */
    void initialize(VulkanContext* context);

    /**
     * Get texture by name (filename)
     * Returns cached texture if already loaded, otherwise loads from disk
     *
     * @param filename Texture filename (e.g., "wood2.jpg", "ground.jpg")
     * @return Pointer to texture, or nullptr if not found
     */
    VulkanTexture* getTexture(const std::string& filename);

    /**
     * Get texture by YARS material name
     * Maps Ogre material names (e.g., "YARS/Wall1", "Chain/Wheel") to texture filenames
     *
     * @param materialName Material name from XML (e.g., "YARS/Wall1", "Chain/Wheel")
     * @return Pointer to texture, or nullptr if material unknown or texture not found
     */
    VulkanTexture* getTextureByMaterial(const std::string& materialName);

    /**
     * Map material name to texture filename
     * Static helper for material system integration
     *
     * @param materialName Material name from XML
     * @return Texture filename, or empty string if unknown material
     */
    static std::string materialToTexture(const std::string& materialName);

    /**
     * Preload a texture (e.g., during initialization)
     *
     * @param filename Texture filename
     * @return true if successfully loaded
     */
    bool preloadTexture(const std::string& filename);

    /**
     * Add search path for texture files
     * YARS typically uses: ./textures, ../textures, etc.
     *
     * @param path Directory path to search for textures
     */
    void addSearchPath(const std::string& path);

    /**
     * Clear all cached textures (cleanup)
     */
    void clearAll();

    /**
     * Get statistics
     */
    size_t getCachedTextureCount() const { return _cache.size(); }
    size_t getSearchPathCount() const { return _searchPaths.size(); }

private:
    VulkanTextureManager() = default;
    ~VulkanTextureManager();
    VulkanTextureManager(const VulkanTextureManager&) = delete;
    VulkanTextureManager& operator=(const VulkanTextureManager&) = delete;

    /**
     * Find texture file in search paths
     *
     * @param filename Texture filename
     * @return Full path to texture file, or empty string if not found
     */
    std::string findTextureFile(const std::string& filename);

    /**
     * Load texture from file and add to cache
     *
     * @param filename Texture filename
     * @param fullPath Full path to texture file
     * @return Pointer to loaded texture, or nullptr on failure
     */
    VulkanTexture* loadTexture(const std::string& filename, const std::string& fullPath);

    static VulkanTextureManager* _instance;

    VulkanContext* _context;
    std::map<std::string, std::unique_ptr<VulkanTexture>> _cache;
    std::vector<std::string> _searchPaths;
};

} // namespace yars

#endif // __VULKAN_TEXTURE_MANAGER_H__
