#ifndef __VULKAN_COMPOSITE_NODE_H__
#define __VULKAN_COMPOSITE_NODE_H__

#include <yars/view/vulkan/VulkanSceneNode.h>
#include <yars/configuration/data/DataComposite.h>
#include <vector>

namespace yars {

class VulkanContext;

/**
 * VulkanCompositeNode - Renders a composite object (multiple sub-objects) with Vulkan
 *
 * Composite objects are containers for multiple geometric primitives
 * that are treated as a single rigid body. Each sub-object has its own
 * geometry but shares the parent's transformation.
 *
 * Parallels SceneGraphCompositeNode:
 * - Contains multiple child VulkanSceneNode objects
 * - Updates transformation from physics data (applies to all children)
 * - Each child maintains its own local offset from parent
 */
class VulkanCompositeNode : public VulkanSceneNode {
public:
    VulkanCompositeNode(DataComposite* composite, VulkanContext* context);
    ~VulkanCompositeNode() override;

    void update() override;
    void reset() override;

    size_t getChildCount() const { return _children.size(); }

private:
    void createChildren();
    VulkanSceneNode* createChildObject(DataObject* object);

    DataComposite* _data;
    VulkanContext* _context;
    std::vector<VulkanSceneNode*> _children;
};

} // namespace yars

#endif // __VULKAN_COMPOSITE_NODE_H__
