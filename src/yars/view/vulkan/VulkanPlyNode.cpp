#include "VulkanPlyNode.h"
#include "VulkanBuffers.h"
#include <iostream>

namespace yars {

VulkanPlyNode::VulkanPlyNode(DataPly* ply, VulkanContext* context)
    : VulkanSceneNode(context)
    , _data(ply)
{
    createGeometry();
    update();  // Set initial pose
}

void VulkanPlyNode::createGeometry() {
    // Get mesh data from DataPly
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // Check if we have valid mesh data
    if (_data->v_size() == 0) {
        std::cerr << "VulkanPlyNode: No vertices in PLY mesh '" << _data->name() << "'" << std::endl;
        return;
    }

    // Lock the DataPly for thread-safe access
    _data->lock();

    // Build vertex buffer from DataPly vertices and normals
    int vertexCount = _data->v_size();
    int normalCount = _data->v_size();  // Assume normals match vertices
    bool hasNormals = (normalCount > 0);

    vertices.reserve(vertexCount);

    for (int i = 0; i < vertexCount; i++) {
        Vertex vertex{};

        // Get position from DataPly
        P3D pos = _data->vertex(i);
        vertex.position = glm::vec3(static_cast<float>(pos.x),
                                    static_cast<float>(pos.y),
                                    static_cast<float>(pos.z));

        // Get normal from DataPly if available
        if (hasNormals && i < normalCount) {
            P3D norm = _data->normal(i);
            vertex.normal = glm::vec3(static_cast<float>(norm.x),
                                     static_cast<float>(norm.y),
                                     static_cast<float>(norm.z));
            // Normalize the normal vector
            float length = glm::length(vertex.normal);
            if (length > 0.0001f) {
                vertex.normal /= length;
            } else {
                // Default normal if invalid
                vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
            }
        } else {
            // Default normal if not provided
            vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }

        // Default color (will be modulated by texture if present)
        vertex.color = glm::vec3(0.7f, 0.7f, 0.7f);

        // Basic UV coordinates (can be improved with actual UV data if PLY format supports it)
        // For now, use a simple planar projection
        vertex.texCoord = glm::vec2(0.5f + vertex.position.x * 0.5f,
                                     0.5f + vertex.position.z * 0.5f);

        vertices.push_back(vertex);
    }

    // Build index buffer from DataPly indices
    // DataPly stores indices as int, convert to uint32_t
    for (auto it = _data->i_begin(); it != _data->i_end(); ++it) {
        int idx = *it;
        if (idx >= 0 && idx < vertexCount) {
            indices.push_back(static_cast<uint32_t>(idx));
        } else {
            std::cerr << "VulkanPlyNode: Invalid index " << idx
                      << " in PLY mesh '" << _data->name() << "'" << std::endl;
        }
    }

    _data->unlock();

    // Verify we have valid geometry
    if (vertices.empty()) {
        std::cerr << "VulkanPlyNode: No valid vertices for PLY mesh '"
                  << _data->name() << "'" << std::endl;
        return;
    }

    if (indices.empty()) {
        std::cerr << "VulkanPlyNode: No valid indices for PLY mesh '"
                  << _data->name() << "'" << std::endl;
        return;
    }

    // Validate triangle count (indices should be multiple of 3)
    if (indices.size() % 3 != 0) {
        std::cerr << "VulkanPlyNode: Warning - index count is not a multiple of 3 for PLY mesh '"
                  << _data->name() << "' (count=" << indices.size() << ")" << std::endl;
    }

    std::cout << "VulkanPlyNode: Creating PLY mesh '" << _data->name()
              << "' with " << vertices.size() << " vertices, "
              << indices.size() / 3 << " triangles" << std::endl;

    // Create vertex and index buffers
    if (!_buffer->createVertexBuffer(vertices)) {
        std::cerr << "VulkanPlyNode: Failed to create vertex buffer for PLY mesh '"
                  << _data->name() << "'" << std::endl;
        return;
    }

    if (!_buffer->createIndexBuffer(indices)) {
        std::cerr << "VulkanPlyNode: Failed to create index buffer for PLY mesh '"
                  << _data->name() << "'" << std::endl;
        return;
    }
}

void VulkanPlyNode::update() {
    // Update transformation matrix from physics data
    setPose(_data);
}

} // namespace yars
