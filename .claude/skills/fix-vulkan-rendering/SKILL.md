# Fix Vulkan Multi-Object Rendering

## Critical Bug
**Status**: 🔴 BLOCKING

**Problem**: VulkanContext::drawFrame() only renders ONE object per frame, but YARS scenes have multiple objects (robots, environment, etc.). Currently displays blue window with no geometry.

## Root Cause Analysis

### Current Implementation
`src/yars/view/vulkan/VulkanRendering.cpp` lines 358-372:

```cpp
// Draw geometry (either from bound buffers or default triangle)
if (_activeVertexBuffer != VK_NULL_HANDLE && _activeIndexBuffer != VK_NULL_HANDLE) {
    // Bind vertex buffer
    VkBuffer vertexBuffers[] = {_activeVertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(_commandBuffers[_currentFrame], 0, 1, vertexBuffers, offsets);

    // Bind index buffer
    vkCmdBindIndexBuffer(_commandBuffers[_currentFrame], _activeIndexBuffer, 0, VK_INDEX_TYPE_UINT32);

    // Draw indexed geometry
    vkCmdDrawIndexed(_commandBuffers[_currentFrame], _activeIndexCount, 1, 0, 0, 0);
} else {
    // Fallback: draw hardcoded triangle (3 vertices from shader)
    vkCmdDraw(_commandBuffers[_currentFrame], 3, 1, 0, 0);
}
```

**Issue**: Only draws `_activeVertexBuffer`, but scene has many objects with their own buffers.

### What's Missing

VulkanHandler calls:
1. `_sceneGraph->update()` - Updates all scene nodes ✅
2. `_context->drawFrame()` - Renders... but only ONE object ❌

Scene graph contains:
- Multiple environment objects (boxes, spheres, cylinders, etc.)
- Multiple robot body parts
- Trace lines

Each has its own VulkanBuffer with vertex/index data, but they're never drawn!

## Solution Architecture

### Option A: Pass Scene Graph to VulkanContext (Recommended)

**Modify VulkanHandler::step()**:
```cpp
void VulkanHandler::step() {
    if (!_context || !_sceneGraph) {
        return;
    }

    try {
        // Update scene graph from physics data
        _sceneGraph->update();

        // Render all objects in scene graph
        _context->drawFrame(_sceneGraph);  // Pass scene graph!
    }
    catch (const std::exception& e) {
        // ... error handling
    }
}
```

**Modify VulkanContext::drawFrame()**:
```cpp
void VulkanContext::drawFrame(VulkanSceneGraph* sceneGraph) {
    // ... existing setup code ...

    vkCmdBeginRenderPass(_commandBuffers[_currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(_commandBuffers[_currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);

    // NEW: Render all scene objects
    renderSceneGraph(sceneGraph, _commandBuffers[_currentFrame], _currentFrame);

    vkCmdEndRenderPass(_commandBuffers[_currentFrame]);

    // ... existing present code ...
}

void VulkanContext::renderSceneGraph(VulkanSceneGraph* sceneGraph,
                                     VkCommandBuffer commandBuffer,
                                     uint32_t frameIndex) {
    // Render environment objects
    VulkanEnvironmentNode* env = sceneGraph->getEnvironment();
    if (env) {
        for (auto* object : env->getObjects()) {
            renderSceneNode(object, commandBuffer, frameIndex);
        }
    }

    // Render robots
    for (auto* robot : sceneGraph->getRobots()) {
        for (auto* part : robot->getObjects()) {
            renderSceneNode(part, commandBuffer, frameIndex);
        }
    }

    // Render trace lines
    for (auto* trace : sceneGraph->getTraceLines()) {
        renderSceneNode(trace, commandBuffer, frameIndex);
    }
}

void VulkanContext::renderSceneNode(VulkanSceneNode* node,
                                   VkCommandBuffer commandBuffer,
                                   uint32_t frameIndex) {
    if (!node || !node->hasGeometry()) {
        return;
    }

    VulkanBuffer* buffer = node->getBuffer();
    if (!buffer) {
        return;
    }

    // Get MVP matrix from node
    glm::mat4 model = node->getModelMatrix();
    glm::mat4 view = _camera->getViewMatrix();
    glm::mat4 proj = _camera->getProjectionMatrix();

    // Update uniforms for this object
    updateUniformBuffer(frameIndex, model, view, proj);

    // Bind descriptor sets
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                           _pipelineLayout, 0, 1, &_descriptorSets[frameIndex], 0, nullptr);

    // Bind vertex buffer
    VkBuffer vertexBuffers[] = {buffer->getVertexBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    // Bind index buffer
    vkCmdBindIndexBuffer(commandBuffer, buffer->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

    // Draw
    vkCmdDrawIndexed(commandBuffer, buffer->getIndexCount(), 1, 0, 0, 0);
}
```

### Required Changes to Scene Graph Classes

**VulkanSceneGraph.h**:
```cpp
public:
    // Add getters for rendering
    VulkanEnvironmentNode* getEnvironment() const { return _environment; }
    const std::vector<VulkanRobotNode*>& getRobots() const { return _robots; }
    const std::vector<VulkanTraceLineObject*>& getTraceLines() const { return _traceLines; }
```

**VulkanEnvironmentNode.h**:
```cpp
public:
    const std::vector<VulkanSceneNode*>& getObjects() const { return _objects; }
```

**VulkanRobotNode.h**:
```cpp
public:
    const std::vector<VulkanSceneNode*>& getObjects() const { return _objects; }
```

**VulkanSceneNode.h**:
```cpp
public:
    bool hasGeometry() const { return _buffer && _buffer->getIndexCount() > 0; }
    VulkanBuffer* getBuffer() const { return _buffer; }
    glm::mat4 getModelMatrix() const { return _modelMatrix; }
```

## Implementation Checklist

### Phase 1: Add Scene Graph Access
- [ ] Add getters to VulkanSceneGraph
- [ ] Add getters to VulkanEnvironmentNode
- [ ] Add getters to VulkanRobotNode
- [ ] Add methods to VulkanSceneNode (hasGeometry, getBuffer, getModelMatrix)

### Phase 2: Modify Rendering Pipeline
- [ ] Update VulkanContext::drawFrame() signature to accept VulkanSceneGraph*
- [ ] Implement renderSceneGraph() method
- [ ] Implement renderSceneNode() method
- [ ] Update VulkanHandler::step() to pass scene graph

### Phase 3: Test and Validate
- [ ] Build successfully
- [ ] Run with --vulkan flag
- [ ] Verify objects are visible (not blue window)
- [ ] Check console output for errors
- [ ] Test with multiple scenes (braitenberg.xml, hexapod.xml, etc.)

## Testing

```bash
cd /Volumes/Eregion/projects/yars/build
make -j8

# Test with braitenberg (simple scene)
./bin/yars --vulkan --iterations 100 ../xml/braitenberg.xml

# Should see:
# - Robot visible (boxes, spheres, cylinders)
# - Environment objects (floor, walls)
# - Smooth animation
```

## Success Criteria

✅ Window shows rendered objects (not just blue)
✅ Multiple objects visible simultaneously
✅ Objects update position each frame
✅ No Vulkan validation errors
✅ Performance is acceptable (>30 FPS)

## Files to Modify

1. `src/yars/view/vulkan/VulkanContext.h` - Add scene graph rendering methods
2. `src/yars/view/vulkan/VulkanRendering.cpp` - Implement multi-object rendering
3. `src/yars/view/vulkan/VulkanHandler.cpp` - Pass scene graph to drawFrame()
4. `src/yars/view/vulkan/VulkanSceneGraph.h` - Add getter methods
5. `src/yars/view/vulkan/VulkanEnvironmentNode.h` - Add getter methods
6. `src/yars/view/vulkan/VulkanRobotNode.h` - Add getter methods
7. `src/yars/view/vulkan/VulkanSceneNode.h` - Add rendering helper methods

## After This Fix

With multi-object rendering working:
1. Implement frame export (see `implement-frame-export` skill)
2. Create reference screenshots
3. Set up visual regression testing
4. Move to Phase 2: Enhanced lighting and materials

## Priority

**🔴 CRITICAL** - This blocks all visual testing and validation of the Vulkan renderer. Must be fixed before continuing with Phase 2-5 features.
