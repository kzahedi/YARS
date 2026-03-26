#ifndef __VULKAN_CYLINDER_NODE_H__
#define __VULKAN_CYLINDER_NODE_H__

#include <yars/view/vulkan/VulkanSceneNode.h>
#include <yars/configuration/data/DataCylinder.h>

namespace yars {

/**
 * VulkanCylinderNode - Renders a 3D cylinder with Vulkan
 *
 * Parallels SceneGraphCylinderNode:
 * - Creates geometry from DataCylinder radius and height
 * - Updates transformation from physics data
 * - Manages vertex/index buffers for cylinder geometry
 * - Supports texture mapping
 */
class VulkanCylinderNode : public VulkanSceneNode {
public:
    VulkanCylinderNode(DataCylinder* cylinder, VulkanContext* context);
    ~VulkanCylinderNode() override = default;

    void update() override;
    void reset() override { update(); }

private:
    void createGeometry();
    void loadTexture();

    DataCylinder* _data;
    static constexpr uint32_t SEGMENTS = 32;  // Radial segments
};

} // namespace yars

#endif // __VULKAN_CYLINDER_NODE_H__
