## ADDED Requirements

### Requirement: Ant XML file exists in xml/ directory
The system SHALL include `xml/ant.xml` — a valid rosiml 0.8.41 file defining a 4-legged ant robot with torso, 4 legs (each 2 segments: femur + tibia), and 8 hinge joints.

#### Scenario: File is present
- **WHEN** the YARS xml/ directory is listed
- **THEN** `ant.xml` SHALL be present

### Requirement: Ant simulation loads without errors
The system SHALL load `ant.xml` and initialise the physics simulation without parse errors or crashes.

#### Scenario: Launch with minimal iterations
- **WHEN** YARS is run with `--xml xml/ant.xml --iterations 10 --no-gui`
- **THEN** the process SHALL exit with code 0 and no error messages in stderr

### Requirement: Ant simulation runs with no-controller mode
The XML SHALL use `<noController/>` so the ant can be simulated without an external controller process.

#### Scenario: No external controller needed
- **WHEN** YARS is launched with `--xml xml/ant.xml --iterations 100 --no-gui`
- **THEN** the simulation SHALL complete all 100 iterations without blocking on controller input

### Requirement: Ant robot is physically plausible
The ant body SHALL have a torso, 4 legs symmetrically placed, and joints with sensible limits so the robot does not explode or clip through the ground on the first physics step.

#### Scenario: Robot stays near origin for first 100 steps
- **WHEN** YARS runs `ant.xml` for 100 iterations
- **THEN** the ant body position SHALL remain within 10 metres of the origin (robot falls under gravity, does not teleport)
