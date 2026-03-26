#ifndef __VULKAN_SCENE_NODE_H__
#define __VULKAN_SCENE_NODE_H__

#include <yars/view/vulkan/VulkanContext.h>
#include <yars/view/vulkan/VulkanBuffers.h>
#include <yars/view/vulkan/VulkanTexture.h>
#include <yars/types/Pose.h>
#include <yars/configuration/data/DataObject.h>
#include <glm/glm.hpp>
#include <string>

namespace yars {

/**
 * VulkanSceneNode - Base class for all renderable objects in Vulkan
 *
 * Parallels SceneGraphObjectNode architecture:
 * - Pure virtual update() and reset() methods
 * - Manages vertex/index buffers for geometry
 * - Tracks transformation matrix (model matrix)
 * - Holds reference to YARS DataObject for physics updates
 */
class VulkanSceneNode {
public:
    VulkanSceneNode(VulkanContext* context);
    virtual ~VulkanSceneNode();

    // Core interface (must be implemented by subclasses)
    virtual void update() = 0;  // Update from physics data
    virtual void reset() = 0;   // Reset to initial state

    // Rendering
    virtual void render();      // Bind buffers and submit draw call

    // Getters
    const glm::mat4& getModelMatrix() const { return _modelMatrix; }
    VulkanBuffer* getBuffer() { return _buffer; }
    bool hasGeometry() const { return _buffer != nullptr && _buffer->getIndexCount() > 0; }
    bool isVisible() const { return _visible; }
    VulkanTexture* getTexture() { return _texture; }
    const std::string& getTextureName() const { return _textureName; }

    // Setters
    void setTextureName(const std::string& name) { _textureName = name; }
    void setTexture(VulkanTexture* texture);

    // Descriptor sets for this node (one per frame in flight)
    const std::vector<VkDescriptorSet>& getDescriptorSets() const { return _descriptorSets; }
    bool hasDescriptorSets() const { return !_descriptorSets.empty(); }

protected:
    // Helper to set pose from DataObject
    void setPose(DataObject* data);
    void updateModelMatrix(const Pose& pose);

    VulkanContext* _context;
    VulkanBuffer* _buffer;
    glm::mat4 _modelMatrix;
    bool _visible;

    // Texture support
    std::string _textureName;  // Material name from XML (e.g., "YARS/Wall1", "Chain/Wheel")
    VulkanTexture* _texture;   // Loaded texture (not owned - managed by VulkanTextureManager)

    // Descriptor sets (one per frame in flight) for this node's rendering
    std::vector<VkDescriptorSet> _descriptorSets;
};

} // namespace yars

#endif // __VULKAN_SCENE_NODE_H__
