#include "VulkanRobotNode.h"
#include "VulkanBoxNode.h"
#include "VulkanSphereNode.h"
#include "VulkanCylinderNode.h"
#include "VulkanCapsuleNode.h"
#include "VulkanPlyNode.h"
#include "VulkanCompositeNode.h"
#include <yars/configuration/data/DataObject.h>
#include <yars/configuration/data/DataBox.h>
#include <yars/configuration/data/DataSphere.h>
#include <yars/configuration/data/DataCylinder.h>
#include <yars/configuration/data/DataCapsule.h>
#include <yars/configuration/data/DataPly.h>
#include <yars/configuration/data/DataComposite.h>
#include <iostream>

namespace yars {

VulkanRobotNode::VulkanRobotNode(DataRobot* robot, VulkanContext* context)
    : _data(robot)
    , _context(context)
{
    std::cout << "VulkanRobotNode: Creating robot '" << robot->name() << "'" << std::endl;
    createObjects();
    std::cout << "VulkanRobotNode: Created " << _objects.size() << " objects for robot '"
              << robot->name() << "'" << std::endl;
}

VulkanRobotNode::~VulkanRobotNode() {
    // Clean up all objects
    for (auto* obj : _objects) {
        delete obj;
    }
    _objects.clear();
}

void VulkanRobotNode::createObjects() {
    // Iterate all objects in robot (body parts)
    // Pattern from SceneGraphRobotNode.cpp:13
    for (DataObjects::iterator g = _data->g_begin(); g != _data->g_end(); g++) {
        VulkanSceneNode* node = createObject(*g);
        if (node) {
            _objects.push_back(node);
        }
    }
}

VulkanSceneNode* VulkanRobotNode::createObject(DataObject* object) {
    if (!object) {
        return nullptr;
    }

    // Factory pattern based on object type
    // Type constants from DataObject.h
    switch (object->type()) {
        case DATA_OBJECT_BOX: {
            DataBox* box = dynamic_cast<DataBox*>(object);
            if (box) {
                std::cout << "VulkanRobotNode: Creating box '" << box->name() << "'" << std::endl;
                return new VulkanBoxNode(box, _context);
            }
            break;
        }

        case DATA_OBJECT_SPHERE: {
            DataSphere* sphere = dynamic_cast<DataSphere*>(object);
            if (sphere) {
                std::cout << "VulkanRobotNode: Creating sphere '" << sphere->name() << "'" << std::endl;
                return new VulkanSphereNode(sphere, _context);
            }
            break;
        }

        case DATA_OBJECT_CYLINDER: {
            DataCylinder* cylinder = dynamic_cast<DataCylinder*>(object);
            if (cylinder) {
                std::cout << "VulkanRobotNode: Creating cylinder '" << cylinder->name() << "'" << std::endl;
                return new VulkanCylinderNode(cylinder, _context);
            }
            break;
        }

        case DATA_OBJECT_CAPPED_CYLINDER: {
            DataCapsule* capsule = dynamic_cast<DataCapsule*>(object);
            if (capsule) {
                std::cout << "VulkanRobotNode: Creating capsule '" << capsule->name() << "'" << std::endl;
                return new VulkanCapsuleNode(capsule, _context);
            }
            break;
        }

        case DATA_OBJECT_PLY: {
            DataPly* ply = dynamic_cast<DataPly*>(object);
            if (ply) {
                std::cout << "VulkanRobotNode: Creating PLY mesh '" << ply->name() << "'" << std::endl;
                return new VulkanPlyNode(ply, _context);
            }
            break;
        }

        case DATA_OBJECT_COMPOSITE: {
            DataComposite* composite = dynamic_cast<DataComposite*>(object);
            if (composite) {
                std::cout << "VulkanRobotNode: Creating composite object '" << composite->name() << "'" << std::endl;
                return new VulkanCompositeNode(composite, _context);
            }
            break;
        }

        default:
            std::cerr << "VulkanRobotNode: Unknown object type " << object->type() << std::endl;
            break;
    }

    return nullptr;
}

void VulkanRobotNode::update() {
    // Update all robot objects to sync with physics
    for (auto* obj : _objects) {
        if (obj) {
            obj->update();
        }
    }
}

void VulkanRobotNode::reset() {
    // Reset all objects to initial state
    for (auto* obj : _objects) {
        if (obj) {
            obj->reset();
        }
    }
}

std::string VulkanRobotNode::getName() const {
    return _data->name();
}

} // namespace yars
