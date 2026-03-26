#include "VulkanBuffers.h"
#include "VulkanContext.h"
#include <cstring>
#include <stdexcept>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace yars {

// ============================================================================
// Vertex
// ============================================================================

VkVertexInputBindingDescription Vertex::getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription{};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    return bindingDescription;
}

std::vector<VkVertexInputAttributeDescription> Vertex::getAttributeDescriptions() {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(4);

    // Position
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, position);

    // Normal
    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, normal);

    // Color
    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, color);

    // TexCoord
    attributeDescriptions[3].binding = 0;
    attributeDescriptions[3].location = 3;
    attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[3].offset = offsetof(Vertex, texCoord);

    return attributeDescriptions;
}

// ============================================================================
// VulkanBuffer
// ============================================================================

VulkanBuffer::VulkanBuffer(VulkanContext* context)
    : _context(context) {
}

VulkanBuffer::~VulkanBuffer() {
    cleanup();
}

uint32_t VulkanBuffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(_context->getPhysicalDevice(), &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("Failed to find suitable memory type!");
}

bool VulkanBuffer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                               VkMemoryPropertyFlags properties,
                               VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(_context->getDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        return false;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(_context->getDevice(), buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(_context->getDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        vkDestroyBuffer(_context->getDevice(), buffer, nullptr);
        return false;
    }

    vkBindBufferMemory(_context->getDevice(), buffer, bufferMemory, 0);
    return true;
}

void VulkanBuffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = _context->getCommandPool();
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(_context->getDevice(), &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(_context->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(_context->getGraphicsQueue());

    vkFreeCommandBuffers(_context->getDevice(), _context->getCommandPool(), 1, &commandBuffer);
}

bool VulkanBuffer::createVertexBuffer(const std::vector<Vertex>& vertices) {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    // Create staging buffer
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    if (!createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     stagingBuffer, stagingBufferMemory)) {
        return false;
    }

    // Copy data to staging buffer
    void* data;
    vkMapMemory(_context->getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(_context->getDevice(), stagingBufferMemory);

    // Create vertex buffer
    if (!createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                     _vertexBuffer, _vertexBufferMemory)) {
        vkDestroyBuffer(_context->getDevice(), stagingBuffer, nullptr);
        vkFreeMemory(_context->getDevice(), stagingBufferMemory, nullptr);
        return false;
    }

    // Copy from staging to vertex buffer
    copyBuffer(stagingBuffer, _vertexBuffer, bufferSize);

    // Cleanup staging buffer
    vkDestroyBuffer(_context->getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(_context->getDevice(), stagingBufferMemory, nullptr);

    return true;
}

bool VulkanBuffer::createIndexBuffer(const std::vector<uint32_t>& indices) {
    _indexCount = static_cast<uint32_t>(indices.size());
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    // Create staging buffer
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    if (!createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     stagingBuffer, stagingBufferMemory)) {
        return false;
    }

    // Copy data to staging buffer
    void* data;
    vkMapMemory(_context->getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(_context->getDevice(), stagingBufferMemory);

    // Create index buffer
    if (!createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                     _indexBuffer, _indexBufferMemory)) {
        vkDestroyBuffer(_context->getDevice(), stagingBuffer, nullptr);
        vkFreeMemory(_context->getDevice(), stagingBufferMemory, nullptr);
        return false;
    }

    // Copy from staging to index buffer
    copyBuffer(stagingBuffer, _indexBuffer, bufferSize);

    // Cleanup staging buffer
    vkDestroyBuffer(_context->getDevice(), stagingBuffer, nullptr);
    vkFreeMemory(_context->getDevice(), stagingBufferMemory, nullptr);

    return true;
}

void VulkanBuffer::cleanup() {
    if (_indexBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(_context->getDevice(), _indexBuffer, nullptr);
        _indexBuffer = VK_NULL_HANDLE;
    }
    if (_indexBufferMemory != VK_NULL_HANDLE) {
        vkFreeMemory(_context->getDevice(), _indexBufferMemory, nullptr);
        _indexBufferMemory = VK_NULL_HANDLE;
    }
    if (_vertexBuffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(_context->getDevice(), _vertexBuffer, nullptr);
        _vertexBuffer = VK_NULL_HANDLE;
    }
    if (_vertexBufferMemory != VK_NULL_HANDLE) {
        vkFreeMemory(_context->getDevice(), _vertexBufferMemory, nullptr);
        _vertexBufferMemory = VK_NULL_HANDLE;
    }
}

// ============================================================================
// GeometryGenerator
// ============================================================================

void GeometryGenerator::generateCube(float size, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
    float halfSize = size * 0.5f;

    // Define 8 corner vertices
    glm::vec3 corners[8] = {
        {-halfSize, -halfSize, -halfSize},  // 0
        { halfSize, -halfSize, -halfSize},  // 1
        { halfSize,  halfSize, -halfSize},  // 2
        {-halfSize,  halfSize, -halfSize},  // 3
        {-halfSize, -halfSize,  halfSize},  // 4
        { halfSize, -halfSize,  halfSize},  // 5
        { halfSize,  halfSize,  halfSize},  // 6
        {-halfSize,  halfSize,  halfSize}   // 7
    };

    vertices.clear();
    indices.clear();

    // Define 6 faces, each with 4 vertices
    struct Face {
        int v[4];
        glm::vec3 normal;
    };

    Face faces[6] = {
        {{0, 1, 2, 3}, {0, 0, -1}},  // Front
        {{5, 4, 7, 6}, {0, 0,  1}},  // Back
        {{4, 0, 3, 7}, {-1, 0, 0}},  // Left
        {{1, 5, 6, 2}, { 1, 0, 0}},  // Right
        {{3, 2, 6, 7}, {0,  1, 0}},  // Top
        {{4, 5, 1, 0}, {0, -1, 0}}   // Bottom
    };

    for (int f = 0; f < 6; f++) {
        uint32_t baseIndex = static_cast<uint32_t>(vertices.size());

        // Create 4 vertices for this face
        for (int v = 0; v < 4; v++) {
            Vertex vertex{};
            vertex.position = corners[faces[f].v[v]];
            vertex.normal = faces[f].normal;
            vertex.color = glm::vec3(0.7f, 0.7f, 0.7f);
            vertex.texCoord = glm::vec2((v & 1) ? 1.0f : 0.0f, (v & 2) ? 1.0f : 0.0f);
            vertices.push_back(vertex);
        }

        // Create 2 triangles for this face
        indices.push_back(baseIndex + 0);
        indices.push_back(baseIndex + 1);
        indices.push_back(baseIndex + 2);

        indices.push_back(baseIndex + 0);
        indices.push_back(baseIndex + 2);
        indices.push_back(baseIndex + 3);
    }
}

void GeometryGenerator::generateSphere(float radius, uint32_t segments, uint32_t rings,
                                      std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
    vertices.clear();
    indices.clear();

    // Generate vertices
    for (uint32_t ring = 0; ring <= rings; ring++) {
        float phi = M_PI * ring / rings;
        float sinPhi = sin(phi);
        float cosPhi = cos(phi);

        for (uint32_t segment = 0; segment <= segments; segment++) {
            float theta = 2.0f * M_PI * segment / segments;
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);

            Vertex vertex{};
            vertex.normal = glm::vec3(cosTheta * sinPhi, cosPhi, sinTheta * sinPhi);
            vertex.position = radius * vertex.normal;
            vertex.color = glm::vec3(0.7f, 0.7f, 0.7f);
            vertex.texCoord = glm::vec2((float)segment / segments, (float)ring / rings);

            vertices.push_back(vertex);
        }
    }

    // Generate indices
    for (uint32_t ring = 0; ring < rings; ring++) {
        for (uint32_t segment = 0; segment < segments; segment++) {
            uint32_t current = ring * (segments + 1) + segment;
            uint32_t next = current + segments + 1;

            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }
}

void GeometryGenerator::generateCylinder(float radius, float height, uint32_t segments,
                                        std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
    vertices.clear();
    indices.clear();

    float halfHeight = height * 0.5f;

    // Top and bottom centers
    uint32_t topCenterIdx = 0;
    uint32_t bottomCenterIdx = segments + 1;

    // Top center
    vertices.push_back({{0, halfHeight, 0}, {0, 1, 0}, {0.7f, 0.7f, 0.7f}, {0.5f, 0.5f}});

    // Top ring
    for (uint32_t i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        vertices.push_back({{x, halfHeight, z}, {0, 1, 0}, {0.7f, 0.7f, 0.7f},
                           {(float)i / segments, 1.0f}});
    }

    // Bottom center
    vertices.push_back({{0, -halfHeight, 0}, {0, -1, 0}, {0.7f, 0.7f, 0.7f}, {0.5f, 0.5f}});

    // Bottom ring
    for (uint32_t i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);

        vertices.push_back({{x, -halfHeight, z}, {0, -1, 0}, {0.7f, 0.7f, 0.7f},
                           {(float)i / segments, 0.0f}});
    }

    // Sides
    uint32_t sideTopStart = static_cast<uint32_t>(vertices.size());
    for (uint32_t i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        glm::vec3 normal = glm::normalize(glm::vec3(x, 0, z));

        vertices.push_back({{x, halfHeight, z}, normal, {0.7f, 0.7f, 0.7f},
                           {(float)i / segments, 1.0f}});
    }

    uint32_t sideBottomStart = static_cast<uint32_t>(vertices.size());
    for (uint32_t i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * i / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        glm::vec3 normal = glm::normalize(glm::vec3(x, 0, z));

        vertices.push_back({{x, -halfHeight, z}, normal, {0.7f, 0.7f, 0.7f},
                           {(float)i / segments, 0.0f}});
    }

    // Top cap indices
    for (uint32_t i = 0; i < segments; i++) {
        indices.push_back(topCenterIdx);
        indices.push_back(topCenterIdx + i + 1);
        indices.push_back(topCenterIdx + i + 2);
    }

    // Bottom cap indices
    for (uint32_t i = 0; i < segments; i++) {
        indices.push_back(bottomCenterIdx);
        indices.push_back(bottomCenterIdx + i + 2);
        indices.push_back(bottomCenterIdx + i + 1);
    }

    // Side indices
    for (uint32_t i = 0; i < segments; i++) {
        indices.push_back(sideTopStart + i);
        indices.push_back(sideBottomStart + i);
        indices.push_back(sideTopStart + i + 1);

        indices.push_back(sideTopStart + i + 1);
        indices.push_back(sideBottomStart + i);
        indices.push_back(sideBottomStart + i + 1);
    }
}

void GeometryGenerator::generatePlane(float width, float depth, uint32_t widthSegments, uint32_t depthSegments,
                                     std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
    vertices.clear();
    indices.clear();

    float halfWidth = width * 0.5f;
    float halfDepth = depth * 0.5f;

    // Generate vertices
    for (uint32_t z = 0; z <= depthSegments; z++) {
        for (uint32_t x = 0; x <= widthSegments; x++) {
            float xPos = -halfWidth + (width * x / widthSegments);
            float zPos = -halfDepth + (depth * z / depthSegments);

            Vertex vertex{};
            vertex.position = glm::vec3(xPos, 0, zPos);
            vertex.normal = glm::vec3(0, 1, 0);
            vertex.color = glm::vec3(0.7f, 0.7f, 0.7f);
            vertex.texCoord = glm::vec2((float)x / widthSegments, (float)z / depthSegments);

            vertices.push_back(vertex);
        }
    }

    // Generate indices
    for (uint32_t z = 0; z < depthSegments; z++) {
        for (uint32_t x = 0; x < widthSegments; x++) {
            uint32_t topLeft = z * (widthSegments + 1) + x;
            uint32_t topRight = topLeft + 1;
            uint32_t bottomLeft = (z + 1) * (widthSegments + 1) + x;
            uint32_t bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
}

} // namespace yars
