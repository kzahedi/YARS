## ADDED Requirements

### Requirement: Vulkan test executables build successfully
The Vulkan renderer test executables in `src/yars/view/vulkan/` SHALL build without errors.

#### Scenario: YarsVulkan library compiles
- **WHEN** `cmake && make YarsVulkan` is run
- **THEN** `libYarsVulkan.a` is produced with no compiler errors

#### Scenario: Vulkan geometry test builds
- **WHEN** `make vulkan_geometry_test` is run
- **THEN** the executable is produced without linker errors

### Requirement: Vulkan smoke test runs without crash
The `vulkan_geometry_test` executable SHALL start and render at least one frame without crashing.

#### Scenario: Vulkan geometry test executes
- **WHEN** `vulkan_geometry_test` is run
- **THEN** it does not crash (exit code 0 or clean exit); any rendering output is documented

### Requirement: Vulkan build status is documented
A status document SHALL capture the current build and runtime state of the Vulkan renderer.

#### Scenario: Status document exists after verification
- **WHEN** Vulkan build and smoke tests are complete
- **THEN** `docs/vulkan-status.md` exists documenting: what builds, what runs, known issues, what remains for full integration

### Requirement: --nogui disables Ogre rendering in main YARS binary
The main YARS binary SHALL run without initializing Ogre3D when `--nogui` is passed.

#### Scenario: YARS runs headlessly without Ogre errors
- **WHEN** `yars --nogui --iterations 100 --xml braitenberg_nocontroller.xml`
- **THEN** no Ogre initialization occurs, exit code is 0, no display required
