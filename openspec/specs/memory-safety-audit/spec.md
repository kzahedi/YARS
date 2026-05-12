## ADDED Requirements

### Requirement: Sanitizer-enabled build is available
The build system SHALL provide an opt-in sanitizer build that links AddressSanitizer, UndefinedBehaviorSanitizer, and LeakSanitizer.

#### Scenario: Sanitizer build compiles cleanly
- **WHEN** `cmake -DYARS_ENABLE_SANITIZERS=ON ..` is run in a fresh `build-asan/` directory followed by `make -j`
- **THEN** the YARS binary is produced with no compile or link errors

#### Scenario: Sanitizer binary runs a baseline simulation
- **WHEN** the sanitizer-built `yars --iterations 100 --nogui --xml xml/braitenberg_nocontroller.xml` is run
- **THEN** the simulation exits with code 0 and no YARS-attributable sanitizer reports are emitted

### Requirement: Simulation corpus is sanitizer-clean
The XML configurations in scope of the `simulation-audit` capability SHALL produce no YARS-attributable AddressSanitizer reports when run for 500 iterations under the sanitizer build.

#### Scenario: Standalone config is ASan-clean
- **WHEN** any standalone config from the simulation-audit scope is run with `--iterations 500 --nogui` under the sanitizer build
- **THEN** ASan reports zero leaks, no use-after-free, and no double-free attributable to YARS source code

#### Scenario: Controller-based config is ASan-clean
- **WHEN** any controller-based config from the simulation-audit scope is run with `--iterations 500 --nogui --lib <contrib_lib>` under the sanitizer build
- **THEN** ASan reports zero leaks, no use-after-free, and no double-free attributable to YARS source code

### Requirement: Valgrind subset is leak-clean
A representative subset of the simulation corpus (`braitenberg_logging.xml`, `falling_objects.xml`, `braitenberg_zoo.xml`) SHALL produce no "definitely lost" valgrind leaks attributable to YARS source code when run for 100 iterations headless.

#### Scenario: Subset config is valgrind-clean
- **WHEN** `valgrind --leak-check=full yars --iterations 100 --nogui --xml <subset_config>` is run
- **THEN** "definitely lost" bytes attributable to YARS source code are zero

### Requirement: Findings are documented
A status document SHALL record the sanitizer-clean state of each config and any third-party suppressions in use.

#### Scenario: Status document exists
- **WHEN** the memory-safety audit is complete
- **THEN** `docs/planning/memory-safety-audit-status.md` lists each in-scope config with pass/fail/suppressed status and links to the relevant log files
