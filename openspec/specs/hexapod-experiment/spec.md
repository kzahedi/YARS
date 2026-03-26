## ADDED Requirements

### Requirement: Hexapod MPI XML file exists in xml/ directory
The system SHALL include `xml/hexapod_mpi.xml` — a valid rosiml 0.8.41 file adapted from the MPI prototype.hexapod version4 experiment (`hexapod_reduced.xml`).

#### Scenario: File is present
- **WHEN** the YARS xml/ directory is listed
- **THEN** `hexapod_mpi.xml` SHALL be present

### Requirement: Hexapod MPI simulation loads without errors
The system SHALL load `hexapod_mpi.xml` and initialise the physics simulation without parse errors or crashes.

#### Scenario: Launch with minimal iterations
- **WHEN** YARS is run with `--xml xml/hexapod_mpi.xml --iterations 10 --no-gui`
- **THEN** the process SHALL exit with code 0 and no error messages in stderr

### Requirement: Hexapod MPI simulation runs with no-controller mode
The XML SHALL use `<noController/>` so the hexapod can be simulated without an external controller process.

#### Scenario: No external controller needed
- **WHEN** YARS is launched with `--xml xml/hexapod_mpi.xml --iterations 100 --no-gui`
- **THEN** the simulation SHALL complete all 100 iterations without blocking on controller input

### Requirement: Hexapod MPI robot is physically plausible
The hexapod body SHALL have a main body and 6 legs (3 per side), with joints positioned so the robot does not explode or clip through the ground on the first physics step.

#### Scenario: Robot stays near origin for first 100 steps
- **WHEN** YARS runs `hexapod_mpi.xml` for 100 iterations
- **THEN** the hexapod body position SHALL remain within 10 metres of the origin
