#ifndef __VULKAN_TRACE_LINE_OBJECT_H__
#define __VULKAN_TRACE_LINE_OBJECT_H__

#include <yars/view/vulkan/VulkanSceneNode.h>
#include <yars/configuration/data/DataTraceLine.h>
#include <yars/types/Colour.h>
#include <glm/glm.hpp>
#include <vector>
#include <deque>

namespace yars {

/**
 * VulkanTraceLineObject - Renders dynamic trace lines for sensor visualization
 *
 * Parallels SceneGraphTraceLineObject:
 * - Visualizes sensor traces (proximity, LDR, etc.)
 * - Dynamic line rendering with VK_PRIMITIVE_TOPOLOGY_LINE_STRIP
 * - Color coding by trace type
 * - Per-frame vertex buffer updates
 * - Trail history management
 *
 * Key Features:
 * - Dynamic vertex buffer (updates each frame)
 * - Circular buffer for trail points (max length)
 * - Color interpolation from start to final color
 * - Line width control
 * - Efficient GPU updates via staging buffer
 */
class VulkanTraceLineObject : public VulkanSceneNode {
public:
    VulkanTraceLineObject(DataTraceLine* trace, VulkanContext* context);
    ~VulkanTraceLineObject() override;

    void update() override;
    void reset() override;

    /**
     * Add a new point to the trace trail
     * Called each frame with current sensor position
     *
     * @param position World position of trace point
     */
    void addPoint(const P3D& position);

    /**
     * Get number of active trail points
     */
    size_t getPointCount() const { return _points.size(); }

private:
    /**
     * Update vertex buffer with current trail points
     * Converts P3D points to Vertex format with colors
     */
    void updateVertexBuffer();

    /**
     * Calculate interpolated color for point at given index
     * Start color → Final color based on position in trail
     *
     * @param index Point index in trail (0 = oldest, size-1 = newest)
     * @return RGB color as glm::vec3
     */
    glm::vec3 getColorForPoint(size_t index) const;

    DataTraceLine* _data;

    // Trail point history (circular buffer)
    std::deque<P3D> _points;
    size_t _maxPoints;  // Maximum trail length

    // Visual properties from DataTraceLine
    glm::vec3 _startColor;  // RGB color (converted from Colour)
    glm::vec3 _finalColor;  // RGB color (converted from Colour)
    float _lineWidth;
    float _trailLength;     // Length in world units
    float _trailTime;       // Length in time (seconds)

    // Dynamic rendering
    bool _needsUpdate;      // Flag to update vertex buffer
};

} // namespace yars

#endif // __VULKAN_TRACE_LINE_OBJECT_H__
