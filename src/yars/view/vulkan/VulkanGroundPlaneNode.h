#ifndef __VULKAN_GROUND_PLANE_NODE_H__
#define __VULKAN_GROUND_PLANE_NODE_H__

#include <yars/view/vulkan/VulkanSceneNode.h>

namespace yars {

/**
 * VulkanGroundPlaneNode - Renders a textured ground plane
 *
 * Simple static ground plane that doesn't update from physics
 */
class VulkanGroundPlaneNode : public VulkanSceneNode {
public:
    VulkanGroundPlaneNode(VulkanContext* context);
    ~VulkanGroundPlaneNode() override = default;

    void update() override {} // Ground is static, no updates needed
    void reset() override {}  // Ground doesn't reset

    // Set ground plane transformation
    void setModelMatrix(const glm::mat4& matrix) { _modelMatrix = matrix; }
};

} // namespace yars

#endif // __VULKAN_GROUND_PLANE_NODE_H__
