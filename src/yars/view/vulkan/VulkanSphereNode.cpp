#include "VulkanSphereNode.h"
#include "VulkanBuffers.h"
#include "VulkanTextureManager.h"

namespace yars {

VulkanSphereNode::VulkanSphereNode(DataSphere* sphere, VulkanContext* context)
    : VulkanSceneNode(context)
    , _data(sphere)
{
    createGeometry();
    loadTexture();  // Load texture based on material
    update();  // Set initial pose
}

void VulkanSphereNode::createGeometry() {
    // Get radius from DataSphere
    float radius = static_cast<float>(_data->radius());

    // Use GeometryGenerator to create sphere geometry
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    GeometryGenerator::generateSphere(radius, SEGMENTS, RINGS, vertices, indices);

    // Create vertex and index buffers
    if (!_buffer->createVertexBuffer(vertices)) {
        std::cerr << "VulkanSphereNode: Failed to create vertex buffer for sphere '"
                  << _data->name() << "'" << std::endl;
        return;
    }

    if (!_buffer->createIndexBuffer(indices)) {
        std::cerr << "VulkanSphereNode: Failed to create index buffer for sphere '"
                  << _data->name() << "'" << std::endl;
        return;
    }
}

void VulkanSphereNode::update() {
    // Update transformation matrix from physics data
    setPose(_data);
}

void VulkanSphereNode::loadTexture() {
    // Spheres are typically wheels in braitenberg
    std::string objectName = _data->name();
    std::string materialName;

    // Check if this is a wheel
    if (objectName.find("wheel") != std::string::npos) {
        materialName = "Chain/Wheel";
    } else {
        // Default for spheres
        materialName = "Chain/Wheel";
    }

    // Load texture via TextureManager
    VulkanTextureManager* texManager = VulkanTextureManager::instance();
    if (texManager) {
        VulkanTexture* texture = texManager->getTextureByMaterial(materialName);
        if (texture && texture->isValid()) {
            setTexture(texture);
            setTextureName(materialName);
            std::cout << "VulkanSphereNode: Loaded texture for '" << objectName
                      << "' using material '" << materialName << "'" << std::endl;
        }
    }
}

} // namespace yars
