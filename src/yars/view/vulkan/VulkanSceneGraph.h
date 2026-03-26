#ifndef __VULKAN_SCENE_GRAPH_H__
#define __VULKAN_SCENE_GRAPH_H__

#include <yars/view/vulkan/VulkanContext.h>
#include <vector>

namespace yars {

// Forward declarations for scene node types
class VulkanSceneNode;
class VulkanRobotNode;
class VulkanEnvironmentNode;
class VulkanTraceLineObject;

/**
 * VulkanSceneGraph - Manages hierarchical scene structure for Vulkan rendering
 *
 * Parallels SceneGraph architecture:
 * - Maintains collection of robot nodes, environment nodes, and trace objects
 * - Coordinates update() calls for all scene nodes
 * - Manages scene reset and cleanup
 */
class VulkanSceneGraph {
public:
    VulkanSceneGraph(VulkanContext* context);
    ~VulkanSceneGraph();

    // Core interface (matches SceneGraph)
    void update();  // Update all scene nodes from physics data
    void reset();   // Reset all scene nodes to initial state

    // Rendering access
    VulkanEnvironmentNode* getEnvironment() const { return _environment; }
    const std::vector<VulkanRobotNode*>& getRobots() const { return _robots; }
    const std::vector<VulkanTraceLineObject*>& getTraceLines() const { return _traceLines; }

private:
    void createRobotNodes();
    void createEnvironmentNode();
    void createTraces();

    VulkanContext* _context;
    std::vector<VulkanRobotNode*> _robots;
    VulkanEnvironmentNode* _environment;
    std::vector<VulkanTraceLineObject*> _traceLines;
};

} // namespace yars

#endif // __VULKAN_SCENE_GRAPH_H__
