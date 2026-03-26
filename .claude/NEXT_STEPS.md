# Next Steps - Vulkan Texture Support

## Current Status (Nov 5, 2024)

### ✅ What Works
- **Basic Rendering**: `vulkan_geometry_test` shows rotating cube - CONFIRMED WORKING BY USER
- **Build System**: All code compiles and links
- **Multi-Object Rendering**: Code implemented (not yet tested with YARS)
- **Frame Export**: Code implemented (not yet tested)

### ❌ Known Issues
1. **YARS Controller Loading**: YARS cannot find controllers even though they're built
   - Error: "Controller 'Braitenberg2b' not found"
   - This is blocking full YARS testing

2. **No Texture Support**: Current shaders don't sample textures
   - Geometry test shows solid colors only
   - Need to add texture sampling to shaders

## Recommended Approach

### Step 1: Add Texture Support to Geometry Test

**Goal**: Verify texture loading and rendering works in isolation

**Tasks:**
1. Create textured shader (mesh_textured.vert/frag)
2. Update pipeline to bind textures
3. Load a test texture in geometry test
4. Verify textured cube renders

**Files to Modify:**
- `shaders/mesh_textured.vert` (NEW)
- `shaders/mesh_textured.frag` (NEW)
- `VulkanContext.cpp` - Add texture descriptor support
- `VulkanGeometryTest.cpp` - Load and bind a texture

**Test Command:**
```bash
cd /Volumes/Eregion/projects/yars/build/bin
./vulkan_geometry_test
# Should see: Cube with wood2.jpg texture (or similar)
```

### Step 2: Fix YARS Controller Loading

**Goal**: Get YARS to find and load controller libraries

**Investigation Needed:**
- Where does YARS look for controller libraries?
- What naming convention does it expect?
- Does it need DYLD_LIBRARY_PATH set?

**Test Once Fixed:**
```bash
cd /Volumes/Eregion/projects/yars/build/bin
./yars --vulkan ../../xml/braitenberg.xml
# Should: Load controller and show simulation
```

### Step 3: Test Multi-Object Rendering with YARS

**Goal**: Verify the multi-object rendering fix works with real scenes

**Only proceed after Steps 1 and 2 are complete.**

**Test:**
```bash
cd /Volumes/Eregion/projects/yars/build/bin
./yars --vulkan ../../xml/braitenberg.xml
# Expected: See robot AND environment (not just robot or just environment)
```

### Step 4: Test Frame Export

**Goal**: Verify frame export produces correct images

**Test:**
```bash
cd /Volumes/Eregion/projects/yars/build/bin
./yars --vulkan ../../xml/braitenberg.xml --iterations 10 &
YARS_PID=$!
sleep 5
# Export frame somehow - need to add CLI option or trigger
kill $YARS_PID
open exported_frame.png
```

## What NOT to Do

❌ Don't try to test everything at once
❌ Don't skip the simple geometry test texture addition
❌ Don't declare YARS rendering works without seeing it
❌ Don't debug controller loading while also implementing textures

## Decision Point

**User's suggestion**: "maybe we should try to apply the braitenberg textures in the vulkan geometry example first"

**This is the right approach.** Let's:
1. Add texture support to geometry test
2. Verify it works visually
3. Then tackle YARS integration issues

---

## Implementation Plan: Texture Support

### Phase A: Shader Modifications

1. **Create mesh_textured.frag**:
```glsl
#version 450

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec3 fragColor;
layout(location = 2) in vec2 fragTexCoord;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main() {
    // Sample texture
    vec4 texColor = texture(texSampler, fragTexCoord);

    // Simple lighting
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float diffuse = max(dot(normalize(fragNormal), lightDir), 0.4);

    // Combine texture with lighting
    vec3 finalColor = texColor.rgb * diffuse;
    outColor = vec4(finalColor, texColor.a);
}
```

2. **Keep mesh_textured.vert same as mesh.vert**

3. **Update VulkanContext**:
   - Add texture sampler to descriptor set
   - Bind texture in command buffer
   - Load texture using VulkanTextureManager

4. **Update VulkanGeometryTest**:
   - Load wood2.jpg or similar
   - Pass texture to context for binding

### Phase B: Testing Protocol

1. **Build and run**:
```bash
cd /Volumes/Eregion/projects/yars/build
cmake --build . --parallel 8
cd bin
./vulkan_geometry_test
```

2. **Visual Verification**:
- [ ] Window shows cube
- [ ] Cube has wood texture (not solid color)
- [ ] Texture moves with cube rotation
- [ ] No crashes, clean exit

3. **Frame Export Test**:
```bash
# After geometry test works
./vulkan_frame_export_test
open vulkan_export.png
# Verify: PNG shows textured cube
```

---

*Priority: Implement Phase A first, test, then move to YARS integration*
