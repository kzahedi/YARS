## MODIFIED Requirements

### Requirement: YARS builds from a clean Linux checkout
A fresh clone of YARS on Ubuntu 22.04 LTS with the documented apt-get dependencies SHALL configure AND fully build (including the GUI target) without source modifications.

#### Scenario: Clean Linux build succeeds
- **WHEN** a fresh YARS clone is configured with `cmake -S . -B build` and built with `cmake --build build -j` on Ubuntu 22.04 with the documented dependencies installed
- **THEN** the build completes with no compile or link errors and produces `build/bin/yars`

#### Scenario: GUI target compiles on Linux
- **WHEN** `YarsGUI` is built on Linux as part of the standard build
- **THEN** `OgreHandler.h` and its dependents compile without missing-header errors

#### Scenario: Path macros are valid C strings on Linux
- **WHEN** `YARS_INSTALL_PATH` (and any sibling path macros from `cmake/SetDefinitions.cmake`) are used in C++ source on Linux
- **THEN** the compiler treats each as a `const char[]` literal — no `character constant too long for its type` warning is emitted
