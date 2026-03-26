#ifndef __VULKAN_ENVIRONMENT_NODE_H__
#define __VULKAN_ENVIRONMENT_NODE_H__

#include <yars/configuration/data/DataEnvironment.h>
#include <yars/view/vulkan/VulkanContext.h>
#include <yars/view/vulkan/VulkanSceneNode.h>
#include <vector>
#include <memory>

namespace yars {

/**
 * VulkanEnvironmentNode - Manages all environment objects in Vulkan scene
 *
 * Parallels SceneGraphEnvironmentNode:
 * - Reads objects from DataEnvironment
 * - Creates appropriate geometry nodes (Box, Sphere, Cylinder)
 * - Manages object lifecycle
 * - Updates all objects each frame
 *
 * Key Responsibilities:
 * - Object factory pattern (type-based node creation)
 * - Container for all environment scene nodes
 * - Batch update/reset operations
 * - Resource management
 */
class VulkanEnvironmentNode {
public:
    /**
     * Create environment node from YARS data
     *
     * @param env DataEnvironment containing all environment objects
     * @param context Vulkan rendering context
     */
    VulkanEnvironmentNode(DataEnvironment* env, VulkanContext* context);
    ~VulkanEnvironmentNode();

    /**
     * Update all environment objects
     * Called each frame to sync with physics
     */
    void update();

    /**
     * Reset all objects to initial state
     */
    void reset();

    /**
     * Get all scene nodes for rendering
     *
     * @return Vector of scene nodes
     */
    const std::vector<VulkanSceneNode*>& getObjects() const { return _objects; }

    /**
     * Get number of environment objects
     */
    size_t getObjectCount() const { return _objects.size(); }

    /**
     * Get ground plane node (may be nullptr if no ground)
     */
    VulkanSceneNode* getGroundPlane() const { return _groundPlane; }

private:
    /**
     * Create all environment objects from data
     * Iterates DataEnvironment and creates appropriate nodes
     */
    void createObjects();

    /**
     * Create ground plane if specified in environment
     */
    void createGroundPlane();

    /**
     * Create single object based on type
     * Factory method for type-based instantiation
     *
     * @param object DataObject to create node from
     * @return Created scene node or nullptr if unsupported type
     */
    VulkanSceneNode* createObject(DataObject* object);

    DataEnvironment* _data;
    VulkanContext* _context;

    // All environment objects (boxes, spheres, cylinders, etc.)
    std::vector<VulkanSceneNode*> _objects;

    // Ground plane (may be nullptr)
    VulkanSceneNode* _groundPlane;
};

} // namespace yars

#endif // __VULKAN_ENVIRONMENT_NODE_H__
