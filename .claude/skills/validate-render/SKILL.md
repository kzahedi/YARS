---
description: Validates YARS rendering by exporting first frame as PNG and comparing with reference screenshot
version: 1.0.0
allowed-tools: [Bash, Read]
---

# Validate YARS Rendering

This skill ensures that rendering and texture functionality works correctly by:

1. Running YARS with image export enabled to capture the first frame
2. Comparing the exported frame with the reference screenshot.png
3. Reporting any visual differences that might indicate rendering issues

## Usage

Automatically invoked when:
- User asks to check visualization or rendering
- After graphics/texture changes
- When validating material or shader modifications
- To verify Ogre3D rendering is working correctly

## Validation Steps

```bash
# Navigate to build directory
cd build

# Run YARS with frame capture (adjust XML config as needed for export)
timeout 60s ./bin/yars --xml ../xml/braitenberg.xml --iterations 10

# Compare exported frame with reference
# (Exact comparison command depends on image export location and format)
```

## Success Criteria

- ✅ YARS renders without OpenGL errors
- ✅ Frame is successfully exported as PNG
- ✅ Exported image matches reference screenshot (or differences are expected)

## Notes

- Always set timeout when running YARS to prevent hanging
- Use YARS image export functionality (frame capture feature)
- Visual comparison may need tolerance for minor rendering differences
- Reference screenshot.png should be in project root or specified location
