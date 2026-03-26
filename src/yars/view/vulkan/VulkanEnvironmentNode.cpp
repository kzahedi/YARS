#include "VulkanEnvironmentNode.h"
#include "VulkanBoxNode.h"
#include "VulkanSphereNode.h"
#include "VulkanCylinderNode.h"
#include "VulkanCapsuleNode.h"
#include "VulkanPlyNode.h"
#include "VulkanCompositeNode.h"
#include "VulkanGroundPlaneNode.h"
#include "VulkanTextureManager.h"
#include "VulkanBuffers.h"
#include <yars/configuration/data/DataObject.h>
#include <yars/configuration/data/DataBox.h>
#include <yars/configuration/data/DataSphere.h>
#include <yars/configuration/data/DataCylinder.h>
#include <yars/configuration/data/DataCapsule.h>
#include <yars/configuration/data/DataPly.h>
#include <yars/configuration/data/DataComposite.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace yars {

VulkanEnvironmentNode::VulkanEnvironmentNode(DataEnvironment* env, VulkanContext* context)
    : _data(env)
    , _context(context)
    , _groundPlane(nullptr)
{
    std::cout << "VulkanEnvironmentNode: Creating environment" << std::endl;
    createGroundPlane();
    createObjects();
    std::cout << "VulkanEnvironmentNode: Created " << _objects.size() << " objects" << std::endl;
}

VulkanEnvironmentNode::~VulkanEnvironmentNode() {
    // Clean up all objects
    for (auto* obj : _objects) {
        delete obj;
    }
    _objects.clear();

    // Clean up ground plane
    if (_groundPlane) {
        delete _groundPlane;
        _groundPlane = nullptr;
    }
}

void VulkanEnvironmentNode::createObjects() {
    // Iterate all objects in environment
    // Pattern from SceneGraphEnvironmentNode.cpp:92-97
    for (DataObjects::iterator g = _data->g_begin(); g != _data->g_end(); g++) {
        VulkanSceneNode* node = createObject(*g);
        if (node) {
            _objects.push_back(node);
        }
    }
}

VulkanSceneNode* VulkanEnvironmentNode::createObject(DataObject* object) {
    if (!object) {
        return nullptr;
    }

    // Factory pattern based on object type
    // Type constants from DataObject.h
    switch (object->type()) {
        case DATA_OBJECT_BOX: {
            DataBox* box = dynamic_cast<DataBox*>(object);
            if (box) {
                std::cout << "VulkanEnvironmentNode: Creating box '" << box->name() << "'" << std::endl;
                return new VulkanBoxNode(box, _context);
            }
            break;
        }

        case DATA_OBJECT_SPHERE: {
            DataSphere* sphere = dynamic_cast<DataSphere*>(object);
            if (sphere) {
                std::cout << "VulkanEnvironmentNode: Creating sphere '" << sphere->name() << "'" << std::endl;
                return new VulkanSphereNode(sphere, _context);
            }
            break;
        }

        case DATA_OBJECT_CYLINDER: {
            DataCylinder* cylinder = dynamic_cast<DataCylinder*>(object);
            if (cylinder) {
                std::cout << "VulkanEnvironmentNode: Creating cylinder '" << cylinder->name() << "'" << std::endl;
                return new VulkanCylinderNode(cylinder, _context);
            }
            break;
        }

        case DATA_OBJECT_CAPPED_CYLINDER: {
            DataCapsule* capsule = dynamic_cast<DataCapsule*>(object);
            if (capsule) {
                std::cout << "VulkanEnvironmentNode: Creating capsule '" << capsule->name() << "'" << std::endl;
                return new VulkanCapsuleNode(capsule, _context);
            }
            break;
        }

        case DATA_OBJECT_PLY: {
            DataPly* ply = dynamic_cast<DataPly*>(object);
            if (ply) {
                std::cout << "VulkanEnvironmentNode: Creating PLY mesh '" << ply->name() << "'" << std::endl;
                return new VulkanPlyNode(ply, _context);
            }
            break;
        }

        case DATA_OBJECT_COMPOSITE: {
            DataComposite* composite = dynamic_cast<DataComposite*>(object);
            if (composite) {
                std::cout << "VulkanEnvironmentNode: Creating composite object '" << composite->name() << "'" << std::endl;
                return new VulkanCompositeNode(composite, _context);
            }
            break;
        }

        default:
            std::cerr << "VulkanEnvironmentNode: Unknown object type " << object->type() << std::endl;
            break;
    }

    return nullptr;
}

void VulkanEnvironmentNode::createGroundPlane() {
    // Check if ground is specified in XML
    if (!_data->groundGiven()) {
        std::cout << "VulkanEnvironmentNode: No ground plane specified" << std::endl;
        return;
    }

    std::cout << "VulkanEnvironmentNode: Creating ground plane" << std::endl;

    // Create a large ground plane (50x50 meters should cover most scenes)
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    GeometryGenerator::generatePlane(50.0f, 50.0f, 10, 10, vertices, indices);

    // Create a new VulkanGroundPlaneNode for the ground
    _groundPlane = new VulkanGroundPlaneNode(_context);

    // Create buffers for the ground plane geometry
    if (!_groundPlane->getBuffer()->createVertexBuffer(vertices)) {
        std::cerr << "VulkanEnvironmentNode: Failed to create vertex buffer for ground plane" << std::endl;
        delete _groundPlane;
        _groundPlane = nullptr;
        return;
    }

    if (!_groundPlane->getBuffer()->createIndexBuffer(indices)) {
        std::cerr << "VulkanEnvironmentNode: Failed to create index buffer for ground plane" << std::endl;
        delete _groundPlane;
        _groundPlane = nullptr;
        return;
    }

    // Set ground plane transformation (centered at origin, at z=0)
    glm::mat4 groundTransform = glm::mat4(1.0f);
    groundTransform = glm::translate(groundTransform, glm::vec3(0.0f, 0.0f, -0.01f)); // Slightly below objects
    static_cast<VulkanGroundPlaneNode*>(_groundPlane)->setModelMatrix(groundTransform);

    // Load ground texture
    std::string textureName = _data->texture();
    if (!textureName.empty()) {
        VulkanTextureManager* texManager = VulkanTextureManager::instance();
        if (texManager) {
            VulkanTexture* texture = texManager->getTextureByMaterial(textureName);
            if (texture && texture->isValid()) {
                _groundPlane->setTexture(texture);
                _groundPlane->setTextureName(textureName);
                std::cout << "VulkanEnvironmentNode: Loaded ground texture '" << textureName << "'" << std::endl;
            } else {
                std::cout << "VulkanEnvironmentNode: Ground texture '" << textureName << "' not found" << std::endl;
            }
        }
    }

    std::cout << "VulkanEnvironmentNode: Ground plane created successfully" << std::endl;
}

void VulkanEnvironmentNode::update() {
    // Update all objects to sync with physics
    for (auto* obj : _objects) {
        obj->update();
    }
}

void VulkanEnvironmentNode::reset() {
    // Reset all objects to initial state
    for (auto* obj : _objects) {
        obj->reset();
    }
}

} // namespace yars
