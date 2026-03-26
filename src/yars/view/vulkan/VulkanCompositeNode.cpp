#include "VulkanCompositeNode.h"
#include "VulkanBoxNode.h"
#include "VulkanSphereNode.h"
#include "VulkanCylinderNode.h"
#include "VulkanCapsuleNode.h"
#include "VulkanPlyNode.h"
#include <yars/configuration/data/DataObject.h>
#include <yars/configuration/data/DataBox.h>
#include <yars/configuration/data/DataSphere.h>
#include <yars/configuration/data/DataCylinder.h>
#include <yars/configuration/data/DataCapsule.h>
#include <yars/configuration/data/DataPly.h>
#include <iostream>

namespace yars {

VulkanCompositeNode::VulkanCompositeNode(DataComposite* composite, VulkanContext* context)
    : VulkanSceneNode(context)
    , _data(composite)
    , _context(context)
{
    std::cout << "VulkanCompositeNode: Creating composite object '" << composite->name() << "'" << std::endl;
    createChildren();
    update();  // Set initial pose
    std::cout << "VulkanCompositeNode: Created " << _children.size()
              << " child objects for composite '" << composite->name() << "'" << std::endl;
}

VulkanCompositeNode::~VulkanCompositeNode() {
    // Clean up all child objects
    for (auto* child : _children) {
        delete child;
    }
    _children.clear();
}

void VulkanCompositeNode::createChildren() {
    // Iterate through all geometry objects in the composite
    // Pattern from DataComposite: g_begin() / g_end()
    for (auto it = _data->g_begin(); it != _data->g_end(); ++it) {
        DataObject* obj = *it;
        VulkanSceneNode* child = createChildObject(obj);
        if (child) {
            _children.push_back(child);
        }
    }
}

VulkanSceneNode* VulkanCompositeNode::createChildObject(DataObject* object) {
    if (!object) {
        return nullptr;
    }

    // Factory pattern based on object type
    // Same logic as VulkanEnvironmentNode and VulkanRobotNode
    switch (object->type()) {
        case DATA_OBJECT_BOX: {
            DataBox* box = dynamic_cast<DataBox*>(object);
            if (box) {
                std::cout << "VulkanCompositeNode: Creating box child '" << box->name() << "'" << std::endl;
                return new VulkanBoxNode(box, _context);
            }
            break;
        }

        case DATA_OBJECT_SPHERE: {
            DataSphere* sphere = dynamic_cast<DataSphere*>(object);
            if (sphere) {
                std::cout << "VulkanCompositeNode: Creating sphere child '" << sphere->name() << "'" << std::endl;
                return new VulkanSphereNode(sphere, _context);
            }
            break;
        }

        case DATA_OBJECT_CYLINDER: {
            DataCylinder* cylinder = dynamic_cast<DataCylinder*>(object);
            if (cylinder) {
                std::cout << "VulkanCompositeNode: Creating cylinder child '" << cylinder->name() << "'" << std::endl;
                return new VulkanCylinderNode(cylinder, _context);
            }
            break;
        }

        case DATA_OBJECT_CAPPED_CYLINDER: {
            DataCapsule* capsule = dynamic_cast<DataCapsule*>(object);
            if (capsule) {
                std::cout << "VulkanCompositeNode: Creating capsule child '" << capsule->name() << "'" << std::endl;
                return new VulkanCapsuleNode(capsule, _context);
            }
            break;
        }

        case DATA_OBJECT_PLY: {
            DataPly* ply = dynamic_cast<DataPly*>(object);
            if (ply) {
                std::cout << "VulkanCompositeNode: Creating PLY mesh child '" << ply->name() << "'" << std::endl;
                return new VulkanPlyNode(ply, _context);
            }
            break;
        }

        case DATA_OBJECT_COMPOSITE: {
            // Nested composites are supported
            DataComposite* nestedComposite = dynamic_cast<DataComposite*>(object);
            if (nestedComposite) {
                std::cout << "VulkanCompositeNode: Creating nested composite child '"
                          << nestedComposite->name() << "'" << std::endl;
                return new VulkanCompositeNode(nestedComposite, _context);
            }
            break;
        }

        default:
            std::cerr << "VulkanCompositeNode: Unknown child object type " << object->type() << std::endl;
            break;
    }

    return nullptr;
}

void VulkanCompositeNode::update() {
    // Update parent transformation from physics data
    setPose(_data);

    // Update all child objects
    // Each child has its own transformation relative to the composite
    for (auto* child : _children) {
        if (child) {
            child->update();
        }
    }
}

void VulkanCompositeNode::reset() {
    // Reset parent transformation
    setPose(_data);

    // Reset all child objects
    for (auto* child : _children) {
        if (child) {
            child->reset();
        }
    }
}

} // namespace yars
