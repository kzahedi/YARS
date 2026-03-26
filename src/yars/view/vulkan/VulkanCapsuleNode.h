#ifndef __VULKAN_CAPSULE_NODE_H__
#define __VULKAN_CAPSULE_NODE_H__

#include <yars/view/vulkan/VulkanSceneNode.h>
#include <yars/configuration/data/DataCapsule.h>

namespace yars {

/**
 * VulkanCapsuleNode - Renders a 3D capsule (capped cylinder) with Vulkan
 *
 * A capsule is a cylinder with hemispherical caps on both ends.
 * Commonly used for robot joints, actuators, and collision shapes.
 *
 * Parallels SceneGraphCapsuleNode:
 * - Creates geometry from DataCapsule radius and height
 * - Updates transformation from physics data
 * - Manages vertex/index buffers for capsule geometry
 * - Supports texture mapping
 */
class VulkanCapsuleNode : public VulkanSceneNode {
public:
    VulkanCapsuleNode(DataCapsule* capsule, VulkanContext* context);
    ~VulkanCapsuleNode() override = default;

    void update() override;
    void reset() override { update(); }

private:
    void createGeometry();

    DataCapsule* _data;
    static constexpr uint32_t SEGMENTS = 32;  // Radial segments
    static constexpr uint32_t RINGS = 16;     // Hemisphere rings
};

} // namespace yars

#endif // __VULKAN_CAPSULE_NODE_H__
