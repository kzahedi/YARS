---
description: Performs a complete clean rebuild of YARS from scratch
version: 1.0.0
allowed-tools: [Bash]
---

# Clean Rebuild

This skill performs a complete clean rebuild of YARS when build state is questionable or after major changes.

## Usage

Automatically invoked when:
- User asks for a clean build or fresh build
- Build errors suggest stale build artifacts
- After major CMake configuration changes
- When switching between build configurations

## Steps

```bash
# Remove entire build directory
rm -rf build

# Create fresh build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build with parallel jobs
make -j4
```

## Success Criteria

- ✅ Build directory successfully removed and recreated
- ✅ CMake configuration completes without errors
- ✅ Compilation completes without errors
- ✅ All binaries successfully created

## Notes

- Always executed from project root
- Removes ALL build artifacts including CMake cache
- Use this when incremental builds fail or behave oddly
- Takes longer than incremental build but guarantees clean state
