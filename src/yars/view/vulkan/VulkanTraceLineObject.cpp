#include "VulkanTraceLineObject.h"
#include "VulkanBuffers.h"
#include <iostream>
#include <algorithm>

namespace yars {

VulkanTraceLineObject::VulkanTraceLineObject(DataTraceLine* trace, VulkanContext* context)
    : VulkanSceneNode(context)
    , _data(trace)
    , _maxPoints(100)  // Default max trail length
    , _needsUpdate(false)
{
    // Read visual properties from DataTraceLine
    Colour startCol = _data->color();
    Colour finalCol = _data->finalColour();

    // Store as glm::vec3 for easier interpolation
    _startColor = glm::vec3(
        static_cast<float>(startCol.red()),
        static_cast<float>(startCol.green()),
        static_cast<float>(startCol.blue())
    );
    _finalColor = glm::vec3(
        static_cast<float>(finalCol.red()),
        static_cast<float>(finalCol.green()),
        static_cast<float>(finalCol.blue())
    );

    _lineWidth = static_cast<float>(_data->lineWidth());
    _trailLength = static_cast<float>(_data->length());
    _trailTime = static_cast<float>(_data->time());

    // Calculate max points based on trail length/time
    // Assuming ~60 FPS, max points = time * 60
    if (_trailTime > 0.0f) {
        _maxPoints = static_cast<size_t>(_trailTime * 60.0f);
        _maxPoints = std::max(_maxPoints, size_t(10));   // At least 10 points
        _maxPoints = std::min(_maxPoints, size_t(1000)); // At most 1000 points
    }

    // Get initial position
    P3D initialPos = _data->position();
    addPoint(initialPos);

    std::cout << "VulkanTraceLineObject: Created trace with max " << _maxPoints << " points" << std::endl;
}

VulkanTraceLineObject::~VulkanTraceLineObject() {
    // Cleanup handled by base class VulkanSceneNode
}

void VulkanTraceLineObject::addPoint(const P3D& position) {
    // Add new point to trail
    _points.push_back(position);

    // Enforce maximum trail length (circular buffer behavior)
    if (_points.size() > _maxPoints) {
        _points.pop_front();
    }

    _needsUpdate = true;
}

void VulkanTraceLineObject::update() {
    // Get current position from data
    P3D currentPos = _data->position();

    // Only add point if it's different from last point (avoid duplicates)
    if (_points.empty() ||
        std::abs(currentPos.x - _points.back().x) > 0.001 ||
        std::abs(currentPos.y - _points.back().y) > 0.001 ||
        std::abs(currentPos.z - _points.back().z) > 0.001) {
        addPoint(currentPos);
    }

    // Update vertex buffer if needed
    if (_needsUpdate && _points.size() >= 2) {
        updateVertexBuffer();
        _needsUpdate = false;
    }
}

void VulkanTraceLineObject::reset() {
    // Clear all trail points
    _points.clear();

    // Add initial position
    P3D initialPos = _data->position();
    addPoint(initialPos);

    std::cout << "VulkanTraceLineObject: Reset trace" << std::endl;
}

void VulkanTraceLineObject::updateVertexBuffer() {
    if (_points.size() < 2) {
        return;  // Need at least 2 points for a line
    }

    // Create vertex data from trail points
    std::vector<Vertex> vertices;
    vertices.reserve(_points.size());

    for (size_t i = 0; i < _points.size(); ++i) {
        const P3D& point = _points[i];

        Vertex vertex;
        vertex.position = glm::vec3(
            static_cast<float>(point.x),
            static_cast<float>(point.y),
            static_cast<float>(point.z)
        );

        // Normal not used for lines, but set to up vector
        vertex.normal = glm::vec3(0.0f, 0.0f, 1.0f);

        // Color interpolation from start to final
        vertex.color = getColorForPoint(i);

        // Texture coords not used for lines
        vertex.texCoord = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    // Create vertex buffer (no index buffer for line strip)
    if (!_buffer->createVertexBuffer(vertices)) {
        std::cerr << "VulkanTraceLineObject: Failed to create vertex buffer for trace" << std::endl;
        return;
    }

    // Note: For line rendering, we use VK_PRIMITIVE_TOPOLOGY_LINE_STRIP
    // This is set in the graphics pipeline, not here
    // We don't need an index buffer for line strips
}

glm::vec3 VulkanTraceLineObject::getColorForPoint(size_t index) const {
    if (_points.empty()) {
        return glm::vec3(1.0f, 1.0f, 1.0f);  // White default
    }

    // Calculate interpolation factor (0.0 = oldest, 1.0 = newest)
    float t = static_cast<float>(index) / static_cast<float>(_points.size() - 1);

    // Linear interpolation between start and final colors
    return _startColor + t * (_finalColor - _startColor);
}

} // namespace yars
