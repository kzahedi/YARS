#ifndef __VULKAN_BOX_NODE_H__
#define __VULKAN_BOX_NODE_H__

#include <yars/view/vulkan/VulkanSceneNode.h>
#include <yars/configuration/data/DataBox.h>

namespace yars {

/**
 * VulkanBoxNode - Renders a 3D box (cuboid) with Vulkan
 *
 * Parallels SceneGraphBoxNode:
 * - Creates geometry from DataBox dimensions
 * - Updates transformation from physics data
 * - Manages vertex/index buffers for box geometry
 */
class VulkanBoxNode : public VulkanSceneNode {
public:
    VulkanBoxNode(DataBox* box, VulkanContext* context);
    ~VulkanBoxNode() override = default;

    void update() override;
    void reset() override { update(); }

private:
    void createGeometry();
    void loadTexture();

    DataBox* _data;
};

} // namespace yars

#endif // __VULKAN_BOX_NODE_H__
