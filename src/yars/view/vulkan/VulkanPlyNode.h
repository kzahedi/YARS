#ifndef __VULKAN_PLY_NODE_H__
#define __VULKAN_PLY_NODE_H__

#include <yars/view/vulkan/VulkanSceneNode.h>
#include <yars/configuration/data/DataPly.h>

namespace yars {

/**
 * VulkanPlyNode - Renders a custom PLY mesh with Vulkan
 *
 * PLY (Polygon File Format) meshes allow custom 3D models.
 * Used for complex robot designs and environment objects.
 *
 * Parallels SceneGraphPlyNode:
 * - Loads geometry from DataPly vertices, normals, and indices
 * - Updates transformation from physics data
 * - Manages vertex/index buffers for custom mesh geometry
 * - Supports texture mapping
 */
class VulkanPlyNode : public VulkanSceneNode {
public:
    VulkanPlyNode(DataPly* ply, VulkanContext* context);
    ~VulkanPlyNode() override = default;

    void update() override;
    void reset() override { update(); }

private:
    void createGeometry();

    DataPly* _data;
};

} // namespace yars

#endif // __VULKAN_PLY_NODE_H__
