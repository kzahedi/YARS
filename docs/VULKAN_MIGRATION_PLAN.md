# YARS Vulkan Migration Plan

## Executive Summary
Replace Ogre3D/SDL with Vulkan + SDL2 for rendering. Keep SDL2 for windowing (already working), replace Ogre3D rendering with direct Vulkan implementation.

**Timeline:** 8-10 weeks
**Risk Level:** High (Vulkan is complex)
**Reward:** Modern, performant, maintainable rendering system

---

## Phase 1: Foundation (Week 1-2)

### 1.1 Setup & Dependencies
**Tasks:**
- Install Vulkan SDK (LunarG SDK or system package)
- Keep SDL2 for windowing
- Add VMA (Vulkan Memory Allocator) for memory management
- Add GLM for math (vector/matrix operations)
- Add stb_image for texture loading
- Add SPIRV-Cross for shader compilation

**Dependencies:**
```cmake
find_package(Vulkan REQUIRED)
find_package(SDL2 REQUIRED)
# VMA: header-only, add as subdirectory
# GLM: header-only
# stb: single-header libraries
```

**Pitfalls & Solutions:**
- **Pitfall:** Vulkan SDK not found on macOS
  - **Solution:** Use MoltenVK (Vulkan over Metal), comes with SDK
  - **Command:** `brew install molten-vk vulkan-headers`

- **Pitfall:** SDL2 doesn't support Vulkan surface creation on macOS
  - **Solution:** Use `SDL_Vulkan_CreateSurface()` API (available in SDL 2.0.6+)

- **Pitfall:** Version mismatches between SDK and runtime
  - **Solution:** Query available Vulkan version first, use 1.1 as minimum

### 1.2 Vulkan Initialization
**Architecture:**
```
VulkanContext (singleton)
├── Instance (VkInstance)
├── Physical Device Selection
├── Logical Device (VkDevice)
├── Queue Families (Graphics, Present, Transfer)
├── Surface (VkSurfaceKHR) - from SDL window
└── Swapchain (VkSwapchainKHR)
```

**Critical Pitfalls:**
- **Pitfall:** Validation layers not available
  - **Solution:** Check `vkEnumerateInstanceLayerProperties()` first
  - **Fallback:** Disable validation in release mode

- **Pitfall:** Physical device doesn't support required features
  - **Solution:** Check for: geometryShader, fillModeNonSolid, samplerAnisotropy
  - **Fallback:** Gracefully degrade (disable features)

- **Pitfall:** Queue family doesn't support present
  - **Solution:** Find queue that supports BOTH graphics AND present
  - **Code:**
    ```cpp
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
    ```

- **Pitfall:** Swapchain format not supported
  - **Solution:** Query available formats, prefer VK_FORMAT_B8G8R8A8_SRGB
  - **Fallback:** Use first available format

---

## Phase 2: Rendering Pipeline (Week 3-4)

### 2.1 Shader System
**Tasks:**
- Write GLSL shaders (vertex, fragment)
- Compile to SPIR-V using glslangValidator or shaderc
- Create shader modules
- Integrate into pipeline

**Shaders Needed:**
1. **Basic.vert / Basic.frag** - Solid color rendering
2. **Textured.vert / Textured.frag** - Textured objects
3. **Line.vert / Line.frag** - Sensor rays
4. **Text.vert / Text.frag** - Overlay text

**Pitfalls:**
- **Pitfall:** SPIR-V compilation fails at runtime
  - **Solution:** Pre-compile shaders at build time
  - **CMake:**
    ```cmake
    find_program(GLSLC glslc)
    add_custom_command(
      OUTPUT ${SPIRV_OUTPUT}
      COMMAND ${GLSLC} ${SHADER_SOURCE} -o ${SPIRV_OUTPUT}
      DEPENDS ${SHADER_SOURCE}
    )
    ```

- **Pitfall:** Shader descriptor bindings mismatch
  - **Solution:** Keep descriptor layouts in sync with shader code
  - **Use:** Descriptor set layout builder pattern

### 2.2 Graphics Pipeline
**Components:**
- Vertex input state (position, normal, texcoord)
- Input assembly (topology: triangles, lines)
- Viewport/scissor state
- Rasterization (culling, polygon mode)
- Depth/stencil testing
- Color blending
- Dynamic state (viewport, scissor)

**Pitfalls:**
- **Pitfall:** Pipeline creation is SLOW (can take 100ms+)
  - **Solution:** Create all pipelines at startup, cache them
  - **Solution:** Use pipeline cache (VkPipelineCache)

- **Pitfall:** Dynamic state not specified
  - **Solution:** Explicitly set VK_DYNAMIC_STATE_VIEWPORT and SCISSOR

- **Pitfall:** Depth testing disabled by accident
  - **Solution:** Always enable for 3D rendering:
    ```cpp
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    ```

### 2.3 Render Pass
**Structure:**
```
RenderPass
├── Attachment 0: Color (swapchain image)
├── Attachment 1: Depth/Stencil
└── Subpass 0: Graphics (color + depth)
```

**Pitfalls:**
- **Pitfall:** Incorrect image layout transitions
  - **Solution:** Use proper initial/final layouts:
    - Color: UNDEFINED → COLOR_ATTACHMENT_OPTIMAL → PRESENT_SRC_KHR
    - Depth: UNDEFINED → DEPTH_STENCIL_ATTACHMENT_OPTIMAL

- **Pitfall:** Load/store ops wrong (performance)
  - **Solution:**
    - loadOp = CLEAR (clear at start)
    - storeOp = STORE (keep for swapchain)

---

## Phase 3: Geometry & Memory (Week 5-6)

### 3.1 Vertex Buffer Management
**Architecture:**
```
GeometryBuffer
├── Staging Buffer (HOST_VISIBLE, HOST_COHERENT)
├── Device Buffer (DEVICE_LOCAL)
└── Transfer Queue (copy staging → device)
```

**Geometry Types:**
- Sphere (tesselated icosphere)
- Cylinder (capped)
- Box (8 vertices, 36 indices)
- Plane (ground)
- Line (sensor rays)

**Pitfalls:**
- **Pitfall:** Memory alignment requirements
  - **Solution:** Use VMA for automatic alignment
  - **Alternative:** Manual alignment with `VkMemoryRequirements`

- **Pitfall:** Staging buffer too small
  - **Solution:** Create per-geometry or use ring buffer

- **Pitfall:** Synchronization issues (reading while GPU writes)
  - **Solution:** Use fences/semaphores:
    ```cpp
    vkQueueSubmit(transferQueue, &submitInfo, fence);
    vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
    ```

### 3.2 Uniform Buffers (MVP matrices, lights)
**Layout:**
```cpp
struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    glm::vec4 lightDir;
    glm::vec4 lightColor;
};
```

**Pitfalls:**
- **Pitfall:** Buffer not aligned to minUniformBufferOffsetAlignment
  - **Solution:** Query `physicalDeviceProperties.limits.minUniformBufferOffsetAlignment`
  - **Pad:** struct to alignment boundary

- **Pitfall:** Updating UBO while GPU is reading
  - **Solution:** Use multiple UBOs (double/triple buffering)
  - **Pattern:** One UBO per swapchain image

---

## Phase 4: Textures & Materials (Week 7)

### 4.1 Texture Loading
**Pipeline:**
```
Image File → stb_image → Staging Buffer → VkImage → VkImageView → VkSampler
```

**Pitfalls:**
- **Pitfall:** Image layout transitions complex
  - **Solution:** Use transition helper function:
    ```cpp
    transitionImageLayout(image,
      VK_IMAGE_LAYOUT_UNDEFINED,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    // Copy buffer to image
    transitionImageLayout(image,
      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    ```

- **Pitfall:** Mipmaps not generated
  - **Solution:** Check blitting support, generate mips on GPU:
    ```cpp
    vkCmdBlitImage(...); // iteratively half dimensions
    ```

- **Pitfall:** Sampler state wrong (texture looks wrong)
  - **Solution:** Set proper filtering:
    - minFilter/magFilter = LINEAR
    - mipmapMode = LINEAR
    - addressMode = REPEAT

### 4.2 Material System
**Design:**
```cpp
struct Material {
    glm::vec4 diffuseColor;
    VkDescriptorSet descriptorSet;  // Texture + sampler
    uint32_t pipelineIndex;         // Which pipeline to use
};
```

**Migration Path:**
- Map YARS material names → Vulkan materials
- Reuse existing MaterialManager infrastructure
- Replace Ogre calls with Vulkan equivalents

---

## Phase 5: Frame Capture (Week 8)

### 5.1 Offscreen Rendering
**Approach:**
```
Create separate framebuffer (not swapchain)
Render to VkImage (not surface)
Copy VkImage → host-visible buffer
Read buffer → PNG file (stb_image_write)
```

**Pitfalls:**
- **Pitfall:** Format conversion (BGR → RGB, SRGB → Linear)
  - **Solution:** Create image with VK_FORMAT_R8G8B8A8_UNORM
  - **Handle:** Format conversion during copy if needed

- **Pitfall:** Slow readback (blocks GPU)
  - **Solution:** Use separate command buffer for copy
  - **Solution:** Double-buffer: capture frame N while rendering N+1

---

## Phase 6: Integration (Week 9-10)

### 6.1 Replace Ogre Classes
**Mapping:**
```
Ogre::Root              → VulkanContext
Ogre::SceneManager      → VulkanRenderer
Ogre::SceneNode         → Keep existing (just transforms)
Ogre::Entity            → VulkanMesh
Ogre::Camera            → VulkanCamera
Ogre::RenderWindow      → SDL_Window + VkSurfaceKHR
Ogre::Material          → VulkanMaterial
Ogre::Texture           → VkImage + VkImageView
```

### 6.2 File Changes
**Core Files to Modify:**
- `src/yars/view/gui/OgreHandler.h/cpp` → `VulkanHandler.h/cpp`
- `src/yars/view/gui/SdlWindow.h/cpp` → Update to use Vulkan
- `src/yars/view/gui/MaterialManager.h/cpp` → Vulkan materials
- `src/yars/view/gui/SceneGraph*.cpp` → Vulkan rendering calls

**Strategy:**
1. Create parallel Vulkan implementation
2. Add `#ifdef USE_VULKAN` / `#ifdef USE_OGRE` guards
3. Test both side-by-side
4. Remove Ogre code once Vulkan stable

---

## Critical Success Factors

### Performance Targets
- **Startup time:** < 2 seconds (pipeline creation)
- **Frame time:** < 16ms (60 FPS)
- **Memory usage:** < 500MB (vs Ogre's ~1GB)

### Debugging Strategy
1. **Enable Validation Layers** (critical!)
   - Catches 90% of Vulkan errors
   - Use VK_LAYER_KHRONOS_validation

2. **Use RenderDoc** for GPU debugging
   - Capture frames
   - Inspect pipeline state
   - Debug shaders

3. **Verbose Logging** at each stage
   - Log all Vulkan calls in debug mode
   - Check VkResult for every function

### Testing Plan
```
Week 8: Clear screen (blue)
Week 9: Render single sphere
Week 10: Render full scene (match screenshot.png)
Week 11: Frame capture working
Week 12: Performance optimization
```

---

## Emergency Rollback Plan

If Vulkan proves too difficult:
1. Keep Ogre code in `#ifdef` blocks
2. Switch back with CMake flag
3. Vulkan implementation becomes "future work"

**Indicators to rollback:**
- Can't get basic rendering working by Week 6
- Persistent crashes/validation errors
- Performance worse than Ogre

---

## Resources

**Essential Reading:**
1. Vulkan Tutorial (vulkan-tutorial.com) - START HERE
2. Vulkan SDK Documentation
3. Vulkan Specification (for reference)
4. VMA documentation (memory management)

**Example Code:**
- Sascha Willems' Vulkan Examples (github.com/SaschaWillems/Vulkan)
- Vulkan-Hpp for modern C++ (optional, adds overhead)

**Tools:**
- RenderDoc (GPU debugging)
- Vulkan Configurator (layer management)
- glslangValidator (shader compilation)

---

## Next Steps

1. **Verify Vulkan availability:** `vulkaninfo | head -20`
2. **Create new branch:** `git checkout -b feature/vulkan-renderer`
3. **Start with minimal window:** SDL + Vulkan surface
4. **Clear screen blue:** First validation of setup
5. **Iterate:** Add features incrementally

**This is a marathon, not a sprint. Build slowly, test thoroughly, document everything.**
