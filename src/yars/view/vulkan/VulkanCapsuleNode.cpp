#include "VulkanCapsuleNode.h"
#include "VulkanBuffers.h"
#include <cmath>

namespace yars {

VulkanCapsuleNode::VulkanCapsuleNode(DataCapsule* capsule, VulkanContext* context)
    : VulkanSceneNode(context)
    , _data(capsule)
{
    createGeometry();
    update();  // Set initial pose
}

void VulkanCapsuleNode::createGeometry() {
    // Get dimensions from DataCapsule (inherits from DataCylinder)
    RHDimension d = _data->dimension();
    float radius = static_cast<float>(d.radius);
    float height = static_cast<float>(d.height);

    // Capsule is a cylinder with hemispherical caps
    // Total height includes the two hemisphere caps (each with radius)
    float cylinderHeight = height - (2.0f * radius);
    if (cylinderHeight < 0.0f) cylinderHeight = 0.0f;  // Degenerate case: sphere

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    float halfCylinderHeight = cylinderHeight * 0.5f;

    // ========== Top Hemisphere ==========
    // Generate top half of sphere (phi from 0 to PI/2)
    uint32_t topHemisphereStartIdx = 0;
    for (uint32_t ring = 0; ring <= RINGS / 2; ring++) {
        float phi = M_PI * ring / RINGS;  // 0 to PI/2
        float sinPhi = sin(phi);
        float cosPhi = cos(phi);

        for (uint32_t segment = 0; segment <= SEGMENTS; segment++) {
            float theta = 2.0f * M_PI * segment / SEGMENTS;
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);

            Vertex vertex{};
            vertex.normal = glm::vec3(cosTheta * sinPhi, cosPhi, sinTheta * sinPhi);
            vertex.position = radius * vertex.normal;
            vertex.position.y += halfCylinderHeight;  // Offset to top of cylinder
            vertex.color = glm::vec3(0.7f, 0.7f, 0.7f);
            vertex.texCoord = glm::vec2((float)segment / SEGMENTS, (float)ring / (RINGS / 2));

            vertices.push_back(vertex);
        }
    }

    // Top hemisphere indices
    for (uint32_t ring = 0; ring < RINGS / 2; ring++) {
        for (uint32_t segment = 0; segment < SEGMENTS; segment++) {
            uint32_t current = topHemisphereStartIdx + ring * (SEGMENTS + 1) + segment;
            uint32_t next = current + SEGMENTS + 1;

            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }

    // ========== Cylinder Body ==========
    uint32_t cylinderTopStartIdx = static_cast<uint32_t>(vertices.size());

    // Top ring
    for (uint32_t i = 0; i <= SEGMENTS; i++) {
        float angle = 2.0f * M_PI * i / SEGMENTS;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        glm::vec3 normal = glm::normalize(glm::vec3(x, 0, z));

        Vertex vertex{};
        vertex.position = glm::vec3(x, halfCylinderHeight, z);
        vertex.normal = normal;
        vertex.color = glm::vec3(0.7f, 0.7f, 0.7f);
        vertex.texCoord = glm::vec2((float)i / SEGMENTS, 1.0f);
        vertices.push_back(vertex);
    }

    uint32_t cylinderBottomStartIdx = static_cast<uint32_t>(vertices.size());

    // Bottom ring
    for (uint32_t i = 0; i <= SEGMENTS; i++) {
        float angle = 2.0f * M_PI * i / SEGMENTS;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        glm::vec3 normal = glm::normalize(glm::vec3(x, 0, z));

        Vertex vertex{};
        vertex.position = glm::vec3(x, -halfCylinderHeight, z);
        vertex.normal = normal;
        vertex.color = glm::vec3(0.7f, 0.7f, 0.7f);
        vertex.texCoord = glm::vec2((float)i / SEGMENTS, 0.5f);
        vertices.push_back(vertex);
    }

    // Cylinder side indices
    for (uint32_t i = 0; i < SEGMENTS; i++) {
        indices.push_back(cylinderTopStartIdx + i);
        indices.push_back(cylinderBottomStartIdx + i);
        indices.push_back(cylinderTopStartIdx + i + 1);

        indices.push_back(cylinderTopStartIdx + i + 1);
        indices.push_back(cylinderBottomStartIdx + i);
        indices.push_back(cylinderBottomStartIdx + i + 1);
    }

    // ========== Bottom Hemisphere ==========
    // Generate bottom half of sphere (phi from PI/2 to PI)
    uint32_t bottomHemisphereStartIdx = static_cast<uint32_t>(vertices.size());
    for (uint32_t ring = 0; ring <= RINGS / 2; ring++) {
        float phi = M_PI * (0.5f + ring / (float)RINGS);  // PI/2 to PI
        float sinPhi = sin(phi);
        float cosPhi = cos(phi);

        for (uint32_t segment = 0; segment <= SEGMENTS; segment++) {
            float theta = 2.0f * M_PI * segment / SEGMENTS;
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);

            Vertex vertex{};
            vertex.normal = glm::vec3(cosTheta * sinPhi, cosPhi, sinTheta * sinPhi);
            vertex.position = radius * vertex.normal;
            vertex.position.y -= halfCylinderHeight;  // Offset to bottom of cylinder
            vertex.color = glm::vec3(0.7f, 0.7f, 0.7f);
            vertex.texCoord = glm::vec2((float)segment / SEGMENTS, 0.5f + (float)ring / (RINGS / 2) * 0.5f);

            vertices.push_back(vertex);
        }
    }

    // Bottom hemisphere indices
    for (uint32_t ring = 0; ring < RINGS / 2; ring++) {
        for (uint32_t segment = 0; segment < SEGMENTS; segment++) {
            uint32_t current = bottomHemisphereStartIdx + ring * (SEGMENTS + 1) + segment;
            uint32_t next = current + SEGMENTS + 1;

            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }

    // Create vertex and index buffers
    if (!_buffer->createVertexBuffer(vertices)) {
        std::cerr << "VulkanCapsuleNode: Failed to create vertex buffer for capsule '"
                  << _data->name() << "'" << std::endl;
        return;
    }

    if (!_buffer->createIndexBuffer(indices)) {
        std::cerr << "VulkanCapsuleNode: Failed to create index buffer for capsule '"
                  << _data->name() << "'" << std::endl;
        return;
    }
}

void VulkanCapsuleNode::update() {
    // Update transformation matrix from physics data
    setPose(_data);
}

} // namespace yars
