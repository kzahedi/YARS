#include "VulkanSceneNode.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <iostream>

namespace yars {

VulkanSceneNode::VulkanSceneNode(VulkanContext* context)
    : _context(context)
    , _buffer(nullptr)
    , _modelMatrix(1.0f)
    , _visible(true)
    , _texture(nullptr)
{
    _buffer = new VulkanBuffer(context);
}

VulkanSceneNode::~VulkanSceneNode() {
    if (_buffer) {
        _buffer->cleanup();
        delete _buffer;
        _buffer = nullptr;
    }
}

void VulkanSceneNode::render() {
    if (!_visible || !_buffer) {
        return;
    }

    // Buffers are bound and rendered via setActiveGeometry in VulkanContext
    // Each node can call this if needed, or the scene graph can manage it
    if (_buffer->getIndexCount() > 0) {
        _context->setActiveGeometry(
            _buffer->getVertexBuffer(),
            _buffer->getIndexBuffer(),
            _buffer->getIndexCount()
        );
    }
}

void VulkanSceneNode::setPose(DataObject* data) {
    if (!data) return;

    Pose pose = data->pose();
    updateModelMatrix(pose);
}

void VulkanSceneNode::updateModelMatrix(const Pose& pose) {
    // Get Bullet pose
    P3D p = pose.position;
    ::Quaternion q = pose.q;

    // DEBUG: Log Bullet input
    static int debugCount = 0;
    if (debugCount < 10) {
        std::cout << "=== DEBUG updateModelMatrix #" << debugCount << " ===" << std::endl;
        std::cout << "  Bullet pos: (" << p.x << ", " << p.y << ", " << p.z << ")" << std::endl;
        std::cout << "  Bullet quat: (" << q.w << ", " << q.x << ", " << q.y << ", " << q.z << ")" << std::endl;
        debugCount++;
    }

    // Transform position: Bullet(X,Y,Z) → Vulkan(X, Z, Y)
    // Bullet: X=right, Y=forward, Z=up
    // Vulkan: X=right, Y=up, Z=forward/backward
    glm::vec3 vulkanPos(p.x, p.z, p.y);

    // Transform quaternion: swap Y and Z axis components
    // Bullet rotation axis (x,y,z) → Vulkan (x,z,y)
    glm::quat vulkanQuat(q.w, q.x, q.z, q.y);

    // Build transformation matrix in Vulkan coordinate space
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), vulkanPos);
    glm::mat4 rotation = glm::mat4_cast(vulkanQuat);
    _modelMatrix = translation * rotation;

    // DEBUG: Log Vulkan output
    static int debugCount2 = 0;
    if (debugCount2 < 10) {
        std::cout << "  Vulkan quat: (" << vulkanQuat.w << ", " << vulkanQuat.x << ", "
                  << vulkanQuat.y << ", " << vulkanQuat.z << ")" << std::endl;
        glm::vec4 origin = _modelMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        glm::vec4 xAxis = _modelMatrix * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
        glm::vec4 yAxis = _modelMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
        glm::vec4 zAxis = _modelMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
        std::cout << "  Vulkan pos: (" << origin.x << ", " << origin.y << ", " << origin.z << ")" << std::endl;
        std::cout << "  Local X->Vulkan: (" << xAxis.x << ", " << xAxis.y << ", " << xAxis.z << ")" << std::endl;
        std::cout << "  Local Y->Vulkan: (" << yAxis.x << ", " << yAxis.y << ", " << yAxis.z << ")" << std::endl;
        std::cout << "  Local Z->Vulkan: (" << zAxis.x << ", " << zAxis.y << ", " << zAxis.z << ")" << std::endl;
        std::cout << "======================================" << std::endl;
        debugCount2++;
    }
}

void VulkanSceneNode::setTexture(VulkanTexture* texture) {
    _texture = texture;

    // Allocate descriptor sets for this node if texture is valid
    if (_texture && _texture->isValid()) {
        if (!_context->allocateNodeDescriptorSets(_texture, _descriptorSets)) {
            std::cerr << "Failed to allocate descriptor sets for node texture" << std::endl;
            _descriptorSets.clear();
        }
    }
}

} // namespace yars
