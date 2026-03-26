# Vulkan Visual Validation Protocol

## ⚠️ CRITICAL REQUIREMENT ⚠️

**NEVER declare Vulkan rendering is "working" or "fixed" without completing ALL steps in this protocol.**

**NEVER tell the user to run a command without first running it yourself and verifying it works.**

**NEVER skip visual validation. Code that compiles ≠ Code that works.**

## Purpose

This skill defines the MANDATORY testing protocol for all Vulkan rendering work in YARS. Following this protocol prevents:
- Declaring success without actual visual verification
- Recommending broken tests to users
- Segfaults and crashes when users follow your instructions
- Wasted time debugging issues that weren't caught during development

## Mandatory Testing Protocol

### Step 1: Verify Build Environment

Before ANY testing:

```bash
cd /Volumes/Eregion/projects/yars/build

# Check shaders are compiled
ls -la bin/shaders/*.spv
# Must see: mesh_vert.spv, mesh_frag.spv, and others

# Check executable exists
ls -la bin/vulkan_frame_export_test  # or other test
ls -la bin/yars

# Verify we're on the correct branch
git branch --show-current
# Should show: feature/vulkan-renderer
```

**If ANY of these fail, STOP and fix before proceeding.**

### Step 2: Test Simple Geometry First

Start with the simplest possible test that can verify basic rendering:

```bash
cd /Volumes/Eregion/projects/yars/build/bin

# Run simple geometry test (no scene graph, just one shape)
./vulkan_geometry_test

# Expected: Window appears, shows a rotating cube, runs for 5 seconds, closes
# If test hangs or crashes: rendering is BROKEN, do not proceed
```

**Visual Checklist**:
- [ ] Window opens (not black, not frozen)
- [ ] Cube is visible (not just blue background)
- [ ] Cube rotates smoothly
- [ ] Test exits cleanly after 5 seconds
- [ ] No segfaults, no hangs

**If geometry test fails, fix it before testing anything more complex.**

### Step 3: Test Scene Graph Integration

Only after Step 2 passes:

```bash
cd /Volumes/Eregion/projects/yars/build/bin

# Run handler test (includes scene graph)
timeout 10 ./vulkan_handler_test &
PID=$!

# Wait 2 seconds then check if still running
sleep 2
if kill -0 $PID 2>/dev/null; then
    echo "Test is running"
    kill $PID 2>/dev/null
else
    echo "Test crashed or exited early - FAILURE"
fi
```

**If this test hangs or crashes, the scene graph integration is broken.**

### Step 4: Test Frame Export

Only after Steps 2 and 3 pass:

```bash
cd /Volumes/Eregion/projects/yars/build/bin

# Run frame export test with timeout
timeout 10 ./vulkan_frame_export_test

# Verify PNG was created
if [ -f vulkan_export.png ]; then
    echo "✅ Frame export succeeded"
    file vulkan_export.png  # Should show PNG image data

    # Check file size (should be > 1KB for 800x600 image)
    SIZE=$(stat -f%z vulkan_export.png 2>/dev/null || stat -c%s vulkan_export.png 2>/dev/null)
    if [ "$SIZE" -gt 1000 ]; then
        echo "✅ PNG file size looks reasonable: $SIZE bytes"
    else
        echo "❌ PNG file is too small: $SIZE bytes - likely corrupted"
    fi
else
    echo "❌ Frame export FAILED - no PNG created"
fi
```

**Visual Validation Required**:
```bash
open vulkan_export.png  # macOS
# or
xdg-open vulkan_export.png  # Linux
```

**Visual Checklist**:
- [ ] Image opens without errors
- [ ] Image is NOT entirely blue
- [ ] Image contains visible geometry (cube, shapes, etc.)
- [ ] Image is not corrupted (no artifacts, valid colors)

**If you cannot see the exported image yourself, you MUST ask the user to verify it.**

### Step 5: Test Full YARS Integration

Only after Steps 2-4 pass:

```bash
cd /Volumes/Eregion/projects/yars/build/bin

# Test YARS with Vulkan renderer (5 second timeout)
timeout 5 ./yars --vulkan ../../xml/braitenberg_controller_go_gui.xml &
PID=$!

sleep 2

if kill -0 $PID 2>/dev/null; then
    echo "✅ YARS with Vulkan is running"
    kill $PID
    wait $PID 2>/dev/null
else
    echo "❌ YARS crashed or exited early"
fi
```

**Visual Requirements**:
- Window must appear
- Must show environment (floor, walls)
- Must show robot geometry
- No crashes, no hangs

### Step 6: Document What You Actually Saw

After testing, you MUST document in your response:

```markdown
## Test Results - [DATE]

### Geometry Test
- Status: PASS/FAIL
- Window appeared: YES/NO
- Geometry visible: YES/NO
- Notes: [what you actually observed]

### Frame Export Test
- Status: PASS/FAIL
- PNG created: YES/NO
- PNG file size: [size]
- Visual inspection: [describe what's in the image]
- Screenshot: [if you can see it, describe it]

### YARS Integration Test
- Status: PASS/FAIL
- Window appeared: YES/NO
- Objects visible: YES/NO
- Notes: [what you actually observed]
```

## When Tests Fail

### If Step 2 Fails (Geometry Test)
**Do NOT proceed to Steps 3-5. Fix the basic rendering first.**

Common causes:
- Shader compilation issues
- Buffer creation problems
- Render pass setup errors
- Command buffer recording bugs

### If Step 3 Fails (Scene Graph Test)
**The scene graph integration is broken. Do NOT declare multi-object rendering is working.**

Common causes:
- Scene graph not passing to drawFrame()
- Iterator bugs in rendering loop
- Null pointer dereferences
- Missing geometry buffers

### If Step 4 Fails (Frame Export Test)
**Frame export is NOT working. Do NOT tell user to use it.**

Common causes:
- Image layout transitions incorrect
- Buffer copy timing issues
- Format conversion bugs
- File I/O errors

### If Step 5 Fails (YARS Integration)
**The Vulkan renderer is NOT ready for use.**

Common causes:
- Missing data files
- Configuration parsing issues
- Scene creation failures
- Resource loading problems

## Mandatory Actions Before Declaring Success

You MUST complete this checklist before saying rendering is "working" or "fixed":

- [ ] Ran Step 2 (geometry test) - confirmed geometry is visible
- [ ] Ran Step 3 (scene graph test) - confirmed no crashes
- [ ] Ran Step 4 (frame export test) - confirmed PNG is created and valid
- [ ] Ran Step 5 (YARS integration test) - confirmed YARS runs with Vulkan
- [ ] Documented actual test results (not assumptions)
- [ ] If unable to verify visually, explicitly asked user to verify
- [ ] Provided exact commands that work (tested them first)
- [ ] Noted any limitations or known issues

## Red Flags - When You're About to Make a Mistake

**STOP if you find yourself:**
- Saying "the rendering is fixed" without running tests
- Telling user to run a command you haven't tested
- Assuming code works because it compiles
- Skipping visual validation "to save time"
- Declaring success based on build output only
- Using phrases like "should work" or "probably works"

**Instead:**
- Run the actual tests
- Verify visual output yourself
- Document what you see
- Be honest about what you can/cannot verify
- Ask user to confirm if you cannot test visually

## Example Bad Behavior (DO NOT DO THIS)

```
❌ "All critical blocking issues have been resolved! The Vulkan renderer
   can now render multiple objects correctly."
```

**Why this is bad:** Claims success without any test results or visual verification.

## Example Good Behavior

```
✅ "I've implemented the multi-object rendering fix. Let me test it:

Test Results:
- Built successfully: YES
- Geometry test: Attempted to run, but test hangs - NEEDS FIX
- Frame export test: Cannot run until geometry test passes

Status: NOT WORKING YET - geometry test hangs, need to debug before proceeding."
```

**Why this is good:** Honest about test results, doesn't claim success prematurely.

## Integration with Development Workflow

After implementing ANY Vulkan rendering change:

1. Build the code
2. Run this validation protocol (ALL steps that apply)
3. Document results
4. If any step fails: FIX IT before moving on
5. Only claim success after visual verification

## Remember

**"It compiles" ≠ "It works"**

**"Tests exist" ≠ "Tests pass"**

**"I implemented it" ≠ "It renders correctly"**

**Visual validation is MANDATORY for graphics code.**
