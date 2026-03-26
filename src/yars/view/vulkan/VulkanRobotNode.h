#ifndef __VULKAN_ROBOT_NODE_H__
#define __VULKAN_ROBOT_NODE_H__

#include <yars/configuration/data/DataRobot.h>
#include <yars/view/vulkan/VulkanContext.h>
#include <yars/view/vulkan/VulkanSceneNode.h>
#include <vector>

namespace yars {

/**
 * VulkanRobotNode - Manages all objects that make up a robot
 *
 * Parallels SceneGraphRobotNode:
 * - Reads objects from DataRobot (body parts)
 * - Creates geometry nodes for each object
 * - Manages object hierarchy
 * - Updates all robot parts each frame
 *
 * Robot Structure:
 * - DataRobot contains DataObjects (boxes, spheres, cylinders)
 * - Each object may have sensors attached
 * - Each robot has actuators (joints, muscles)
 *
 * Phase 1 Implementation:
 * - Robot body objects only (boxes, spheres, cylinders)
 * - Sensors visualization deferred to later phase
 * - Actuators visualization deferred to later phase
 */
class VulkanRobotNode {
public:
    /**
     * Create robot node from YARS data
     *
     * @param robot DataRobot containing all robot objects
     * @param context Vulkan rendering context
     */
    VulkanRobotNode(DataRobot* robot, VulkanContext* context);
    ~VulkanRobotNode();

    /**
     * Update all robot objects
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
     * @return Vector of scene nodes (body parts)
     */
    const std::vector<VulkanSceneNode*>& getObjects() const { return _objects; }

    /**
     * Get number of robot objects
     */
    size_t getObjectCount() const { return _objects.size(); }

    /**
     * Get robot name from data
     */
    std::string getName() const;

private:
    /**
     * Create all robot objects from data
     * Iterates DataRobot objects and creates appropriate nodes
     */
    void createObjects();

    /**
     * Create single object based on type
     * Factory method for type-based instantiation
     *
     * @param object DataObject to create node from
     * @return Created scene node or nullptr if unsupported type
     */
    VulkanSceneNode* createObject(DataObject* object);

    DataRobot* _data;
    VulkanContext* _context;

    // Robot body parts (boxes, spheres, cylinders)
    std::vector<VulkanSceneNode*> _objects;

    // TODO: Future phases
    // std::vector<VulkanSensorNode*> _sensors;
    // std::vector<VulkanActuatorNode*> _actuators;
};

} // namespace yars

#endif // __VULKAN_ROBOT_NODE_H__
