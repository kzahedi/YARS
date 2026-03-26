#ifndef __VULKAN_BUFFERS_H__
#define __VULKAN_BUFFERS_H__

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <vector>

namespace yars {

// Forward declaration
class VulkanContext;

/**
 * Vertex structure for 3D geometry
 */
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 texCoord;

    // Vertex input binding description
    static VkVertexInputBindingDescription getBindingDescription();

    // Vertex input attribute descriptions
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
};

/**
 * Buffer management helper class
 */
class VulkanBuffer {
public:
    VulkanBuffer(VulkanContext* context);
    ~VulkanBuffer();

    // Buffer creation
    bool createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
                     VkMemoryPropertyFlags properties,
                     VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    // Buffer operations
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    // Vertex buffer
    bool createVertexBuffer(const std::vector<Vertex>& vertices);
    VkBuffer getVertexBuffer() const { return _vertexBuffer; }

    // Index buffer
    bool createIndexBuffer(const std::vector<uint32_t>& indices);
    VkBuffer getIndexBuffer() const { return _indexBuffer; }
    uint32_t getIndexCount() const { return _indexCount; }

    // Cleanup
    void cleanup();

private:
    VulkanContext* _context;

    VkBuffer _vertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory _vertexBufferMemory = VK_NULL_HANDLE;

    VkBuffer _indexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory _indexBufferMemory = VK_NULL_HANDLE;
    uint32_t _indexCount = 0;

    // Helper to find suitable memory type
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};

/**
 * Geometry generators for common shapes
 */
class GeometryGenerator {
public:
    // Basic shapes
    static void generateCube(float size, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
    static void generateSphere(float radius, uint32_t segments, uint32_t rings,
                              std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
    static void generateCylinder(float radius, float height, uint32_t segments,
                                std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
    static void generatePlane(float width, float depth, uint32_t widthSegments, uint32_t depthSegments,
                             std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
};

} // namespace yars

#endif // __VULKAN_BUFFERS_H__
