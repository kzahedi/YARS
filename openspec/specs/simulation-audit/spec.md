## ADDED Requirements

### Requirement: All self-contained simulations run without crash
Each XML configuration that does not require an external runtime process SHALL run for at least 500 iterations without crashing.

Configurations in scope:
- `xml/braitenberg_nocontroller.xml` (no controller)
- `xml/braitenberg_noise.xml` (no controller)
- `xml/falling_objects.xml` (no controller)
- `xml/test_capture.xml` (no controller)
- `xml/braitenberg.xml` (Braitenberg2b — requires contrib lib)
- `xml/braitenberg_logging.xml` (Braitenberg3b — requires contrib lib)
- `xml/braitenberg_light_source.xml` (Braitenberg2b — requires contrib lib)
- `xml/braitenberg_trace_projection.xml` (Braitenberg2b — requires contrib lib)
- `xml/braitenberg_zoo.xml` (Braitenberg2a — requires contrib lib)
- `xml/muscle.xml` (SquareWave — requires contrib lib)
- `xml/joints/generic_angular.xml` (Sine — requires contrib lib)
- `xml/joints/generic_force.xml` (Sine — requires contrib lib)

#### Scenario: Standalone simulation completes 500 iterations
- **WHEN** `yars --iterations 500 --nogui --xml <config>` is run for a standalone config
- **THEN** process exits with code 0

#### Scenario: Controller-based simulation completes 500 iterations
- **WHEN** `yars --iterations 500 --lib <contrib_lib> --xml <config>` is run with contrib library
- **THEN** process exits with code 0

### Requirement: --nogui mode runs all standalone simulations
The system SHALL support running any standalone XML configuration in headless mode via `--nogui`.

#### Scenario: Headless simulation produces no display errors
- **WHEN** YARS runs with `--nogui`
- **THEN** no display/rendering errors appear, physics runs normally, exit code is 0

### Requirement: hexapod.xml machine path is corrected
The `hexapod.xml` controller path SHALL use a path valid on the current machine.

#### Scenario: hexapod.xml references correct working directory
- **WHEN** hexapod.xml is inspected
- **THEN** the Python controller working directory path resolves correctly on the Mac Mini (`/Volumes/Eregion/projects/yars/contrib/controller`)
