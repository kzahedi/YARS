#include "VulkanCylinderNode.h"
#include "VulkanBuffers.h"
#include "VulkanTextureManager.h"

namespace yars {

VulkanCylinderNode::VulkanCylinderNode(DataCylinder* cylinder, VulkanContext* context)
    : VulkanSceneNode(context)
    , _data(cylinder)
{
    createGeometry();
    loadTexture();  // Load texture based on material
    update();  // Set initial pose
}

void VulkanCylinderNode::createGeometry() {
    // Get dimensions from DataCylinder
    RHDimension d = _data->dimension();
    float radius = static_cast<float>(d.radius);
    float height = static_cast<float>(d.height);

    // Use GeometryGenerator to create cylinder geometry
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    GeometryGenerator::generateCylinder(radius, height, SEGMENTS, vertices, indices);

    // Create vertex and index buffers
    if (!_buffer->createVertexBuffer(vertices)) {
        std::cerr << "VulkanCylinderNode: Failed to create vertex buffer for cylinder '"
                  << _data->name() << "'" << std::endl;
        return;
    }

    if (!_buffer->createIndexBuffer(indices)) {
        std::cerr << "VulkanCylinderNode: Failed to create index buffer for cylinder '"
                  << _data->name() << "'" << std::endl;
        return;
    }
}

void VulkanCylinderNode::update() {
    // Update transformation matrix from physics data
    static int updateCount = 0;
    if (updateCount < 3) {
        Pose p = _data->pose();
        std::cout << "DEBUG VulkanCylinderNode::update() called, pose=("
                  << p.position.x << ", " << p.position.y << ", " << p.position.z << ")" << std::endl;
        updateCount++;
    }
    setPose(_data);
}

void VulkanCylinderNode::loadTexture() {
    // Cylinders are typically robot bodies in braitenberg
    std::string objectName = _data->name();
    std::string materialName;

    // Check if this is a robot body
    if (objectName.find("body") != std::string::npos || objectName.find("main") != std::string::npos) {
        materialName = "Chain/Circuit/Body/Green";
    } else {
        // Default for cylinders
        materialName = "Chain/Circuit/Body/Green";
    }

    // Load texture via TextureManager
    VulkanTextureManager* texManager = VulkanTextureManager::instance();
    if (texManager) {
        VulkanTexture* texture = texManager->getTextureByMaterial(materialName);
        if (texture && texture->isValid()) {
            setTexture(texture);
            setTextureName(materialName);
            std::cout << "VulkanCylinderNode: Loaded texture for '" << objectName
                      << "' using material '" << materialName << "'" << std::endl;
        }
    }
}

} // namespace yars
