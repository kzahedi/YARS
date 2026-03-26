#ifndef __VULKAN_SPHERE_NODE_H__
#define __VULKAN_SPHERE_NODE_H__

#include <yars/view/vulkan/VulkanSceneNode.h>
#include <yars/configuration/data/DataSphere.h>

namespace yars {

/**
 * VulkanSphereNode - Renders a 3D sphere with Vulkan
 *
 * Parallels SceneGraphSphereNode:
 * - Creates geometry from DataSphere radius
 * - Updates transformation from physics data
 * - Manages vertex/index buffers for sphere geometry
 * - Supports texture mapping
 */
class VulkanSphereNode : public VulkanSceneNode {
public:
    VulkanSphereNode(DataSphere* sphere, VulkanContext* context);
    ~VulkanSphereNode() override = default;

    void update() override;
    void reset() override { update(); }

private:
    void createGeometry();
    void loadTexture();

    DataSphere* _data;
    static constexpr uint32_t SEGMENTS = 32;  // Longitude segments
    static constexpr uint32_t RINGS = 16;     // Latitude rings
};

} // namespace yars

#endif // __VULKAN_SPHERE_NODE_H__
