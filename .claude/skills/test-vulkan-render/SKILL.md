# Test Vulkan Renderer Skill

## Purpose
Test the YARS Vulkan renderer by running simulations and validating visual output through frame export and comparison.

## Usage
Use this skill whenever you make changes to the Vulkan rendering pipeline to verify that objects are actually being rendered correctly.

## Testing Protocol

### Step 1: Run YARS with Vulkan Renderer
```bash
cd /Volumes/Eregion/projects/yars/build

# Run a short simulation with the Vulkan renderer
./bin/yars --vulkan --iterations 10 ../xml/braitenberg.xml
```

### Step 2: Visual Inspection
- Window should appear with rendered objects (robot, environment)
- Should NOT be just a blue/black/empty window
- Objects should be visible and have proper geometry

### Step 3: Export Frame for Comparison
Currently YARS doesn't have `--export-frame` flag implemented for Vulkan.

**TODO**: Implement frame export functionality:
1. Add frame capture to VulkanContext
2. Save framebuffer to PNG after rendering
3. Compare with reference screenshot.png

### Step 4: Compare with Reference
When frame export is implemented:
```bash
# Export a frame
./bin/yars --vulkan --iterations 10 --export-frame 5 --export-file test_frame.png ../xml/braitenberg.xml

# Compare with reference
compare test_frame.png screenshot.png -compose src diff_output.png

# Check if images are similar
# Small differences are OK (antialiasing, minor shader differences)
# Large differences indicate rendering bug
```

## Expected Output

### Successful Render
```
VulkanHandler: Initializing Vulkan context...
VulkanSceneGraph: Creating robot nodes...
VulkanSceneGraph: Created 1 robot(s)
VulkanSceneGraph: Creating environment node...
VulkanEnvironmentNode: Created with X objects
```

Objects should be visible in the window.

### Failed Render (Current Bug)
```
# Terminal output looks OK but window shows:
- Just blue/black color
- No geometry visible
- Physics runs but nothing renders
```

This indicates `drawFrame()` is not iterating through scene nodes.

## Common Issues

### Issue: Blue Window, No Objects
**Cause**: `VulkanContext::drawFrame()` only renders one object per frame
**Solution**: Implement scene graph iteration in rendering loop
**See**: vulkan.md "Known Issues" section

### Issue: Objects Appear But Wrong Colors/Textures
**Cause**: Material system not fully implemented
**Solution**: Phase 3 - implement VulkanMaterial system

### Issue: Objects in Wrong Positions
**Cause**: Coordinate system transformation issue
**Solution**: Check VulkanSceneNode::setPose() and matrix calculations

## Files to Check

When debugging rendering issues:
- `src/yars/view/vulkan/VulkanRendering.cpp` - drawFrame() implementation
- `src/yars/view/vulkan/VulkanHandler.cpp` - step() calls drawFrame()
- `src/yars/view/vulkan/VulkanSceneGraph.cpp` - scene structure
- `src/yars/view/vulkan/VulkanSceneNode.cpp` - individual object rendering

## Success Criteria

✅ Window appears and stays open
✅ Objects are visible (robot, environment)
✅ Objects are in correct positions
✅ Simulation runs smoothly (no crashes)
✅ No Vulkan validation errors

## Next Steps After Rendering Works

1. Implement frame export functionality
2. Create reference screenshots for different scenes
3. Add automated visual regression testing
4. Implement proper material and texture support
