---
description: Validates YARS build by running braitenberg test and comparing logfile output with reference
version: 1.0.0
allowed-tools: [Bash, Read, Glob]
---

# Validate YARS Build

This skill ensures that code changes haven't broken YARS simulation functionality by:

1. Compiling YARS in the ./build directory
2. Running the braitenberg_logging.xml test with 1000 iterations and 60s timeout
3. Comparing the generated logfile with the reference logfile
4. Reporting success only if compilation succeeds, simulation runs without errors, and logfiles match

## Usage

Automatically invoked when:
- User asks to validate changes
- After significant code modifications
- Before committing changes
- When testing build integrity

## Validation Steps

```bash
# Navigate to build directory
cd build

# Compile YARS
cmake .. && make -j4

# Run braitenberg test with timeout
timeout 60s ./bin/yars --iterations 1000 --xml ../xml/braitenberg_logging.xml

# Compare logfiles
diff braitenberg-*.csv ../reference_logfile.csv
```

## Success Criteria

- ✅ Compilation completes without errors
- ✅ YARS runs without crashes or timeouts
- ✅ Generated logfile matches reference exactly (diff shows no differences)

## Notes

- Always run from project root, using ./build subdirectory
- Timeout prevents infinite loops if GUI doesn't close properly
- Any difference in logfile output indicates behavior regression
