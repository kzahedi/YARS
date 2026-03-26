# Implement Frame Export Skill

## Purpose
Add frame capture and export functionality to the Vulkan renderer for testing and validation.

## Implementation Steps

### Step 1: Add Frame Export to VulkanContext

**File**: `src/yars/view/vulkan/VulkanContext.h`

Add methods:
```cpp
// Frame capture
bool exportFrame(const std::string& filename);
void* getFramebufferData(uint32_t imageIndex, size_t& dataSize);
```

### Step 2: Implement Frame Capture

**File**: `src/yars/view/vulkan/VulkanContext.cpp`

After frame rendering in `drawFrame()`:
```cpp
bool VulkanContext::exportFrame(const std::string& filename) {
    // 1. Get current swapchain image
    // 2. Create staging buffer
    // 3. Copy image to staging buffer
    // 4. Map buffer memory
    // 5. Use stb_image_write to save as PNG
    // 6. Cleanup

    return true;
}
```

### Step 3: Add Command Line Options

**File**: `src/yars/defines/program_options.h`

Add:
```cpp
#define YARS_STRING_EXPORT_FRAME     "export-frame"
#define YARS_STRING_EXPORT_FILE      "export-file"
```

**File**: `src/yars/configuration/container/ProgramOptionsConfiguration.cpp`

Add options:
```cpp
("export-frame", po::value<int>(), "Export frame at specified iteration")
("export-file", po::value<string>(), "Filename for exported frame")
```

### Step 4: Hook Into Main Loop

**File**: `src/yars/view/YarsViewModel.cpp`

In `visualiseScene()`:
```cpp
void YarsViewModel::visualiseScene() {
    // ... existing code ...

    _vulkanHandler->step();

    // Check if we should export this frame
    if (__YARS_GET_EXPORT_FRAME == _currentIteration) {
        std::string filename = __YARS_GET_EXPORT_FILE;
        if (filename.empty()) filename = "export_frame.png";
        _vulkanHandler->exportFrame(filename);
    }
}
```

### Step 5: Use STB Image Write

**Dependencies**: Already have `ext/stb/stb_image.h` for loading

Add `stb_image_write.h`:
```cpp
// In one .cpp file (e.g., VulkanContext.cpp)
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
```

### Step 6: Image Format Conversion

Vulkan framebuffer format may need conversion:
```cpp
// Convert BGRA to RGBA if needed
// Flip Y-axis (Vulkan has origin at top-left, PNG at bottom-left)
```

## Testing

After implementation:
```bash
cd /Volumes/Eregion/projects/yars/build

# Export frame 5
./bin/yars --vulkan --iterations 10 --export-frame 5 --export-file test.png ../xml/braitenberg.xml

# Verify PNG was created
file test.png  # Should show: PNG image data

# View the image
open test.png  # macOS
```

## Expected Behavior

1. YARS runs normally with Vulkan renderer
2. At specified iteration, frame is captured
3. PNG file is written to disk
4. Simulation continues
5. File contains actual rendered frame (not blank/blue)

## Validation

Compare exported frame with expected output:
```bash
# Visual inspection
open test.png

# Automated comparison (when reference exists)
compare test.png screenshot.png -metric AE diff.png
# AE = Absolute Error count
# Low value (<1000 pixels) = good match
# High value = rendering difference
```

## Common Pitfalls

### Image is upside down
**Solution**: Flip Y coordinates when copying framebuffer

### Image is wrong format/colors
**Solution**: Check Vulkan surface format, convert BGRA→RGBA

### Black/empty image
**Solution**: Ensure frame is fully rendered before capture
- Wait for queue idle: `vkQueueWaitIdle(_graphicsQueue)`
- Ensure rendering actually draws geometry

### Memory leak
**Solution**: Properly destroy staging buffers and free mapped memory

## Success Criteria

✅ PNG file is created
✅ Image shows rendered scene
✅ Image has correct dimensions (window size)
✅ Colors are correct
✅ No memory leaks
✅ No performance impact on normal rendering

## Files Modified

- `src/yars/view/vulkan/VulkanContext.h`
- `src/yars/view/vulkan/VulkanContext.cpp`
- `src/yars/view/vulkan/VulkanHandler.h`
- `src/yars/view/vulkan/VulkanHandler.cpp`
- `src/yars/view/YarsViewModel.cpp`
- `src/yars/defines/program_options.h`
- `src/yars/configuration/container/ProgramOptionsConfiguration.cpp`

## References

- [stb_image_write.h](https://github.com/nothings/stb/blob/master/stb_image_write.h)
- [Vulkan Tutorial - Images](https://vulkan-tutorial.com/Texture_mapping/Images)
- Existing YARS image export (if any - check ConsoleView for screenshot code)
