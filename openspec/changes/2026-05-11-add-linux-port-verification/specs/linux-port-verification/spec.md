## ADDED Requirements

### Requirement: YARS builds from a clean Linux checkout
A fresh clone of YARS on Ubuntu 22.04 LTS with documented apt-get dependencies SHALL configure and build without source modifications.

#### Scenario: Clean Linux build succeeds
- **WHEN** a fresh YARS clone is configured with `cmake -S . -B build` and built with `cmake --build build -j` on Ubuntu 22.04 with the documented dependencies installed
- **THEN** the build completes with no compile or link errors and produces `build/yars`

### Requirement: Headless simulation corpus runs on Linux
Each XML configuration in scope of the `simulation-audit` capability SHALL run for 500 iterations on Linux in `--nogui` mode without crashing.

#### Scenario: Standalone config runs headless on Linux
- **WHEN** any standalone config from the simulation-audit scope is run on Linux with `--iterations 500 --nogui`
- **THEN** the process exits with code 0

#### Scenario: Controller-based config runs headless on Linux
- **WHEN** any controller-based config from the simulation-audit scope is run on Linux with `--iterations 500 --nogui --lib <contrib_lib>`
- **THEN** the process exits with code 0

#### Scenario: Reference logfile matches macOS output
- **WHEN** `braitenberg_logging.xml` is run on Linux and the CSV output is diffed against `xml/reference_logfile.csv`
- **THEN** the diff is empty (or any difference is documented as a known cross-platform numerical variance)

### Requirement: X11 GUI session opens and renders
A representative GUI configuration SHALL launch on Linux with an X11 display, render geometry, and accept camera input.

#### Scenario: Window opens and renders shapes
- **WHEN** `yars --xml xml/braitenberg_logging.xml --lib <contrib_lib>` is launched on Linux with X11 available
- **THEN** a window opens, robot geometry and the ground plane are rendered, and the window remains stable for at least 30 seconds

#### Scenario: Camera input is received
- **WHEN** the GUI session is active and mouse drag / scroll-zoom events are issued
- **THEN** the camera responds to those inputs

### Requirement: FFmpeg video capture produces a playable MP4 on Linux
The frame-capture and MP4-assembly path SHALL produce a playable video on Linux.

#### Scenario: MP4 is produced and verified
- **WHEN** a frame-capture configuration is run on Linux for ~10 seconds of simulation and the MP4 assembly step is invoked
- **THEN** a playable `.mp4` is produced and `ffprobe` reports valid video stream metadata

### Requirement: Linux verification status is documented
A status document SHALL record the Linux distribution, package versions, build output, audit results, and any deviations.

#### Scenario: Status document exists
- **WHEN** the Linux verification is complete
- **THEN** `docs/planning/linux-port-verification-status.md` lists distribution, packages, build excerpt, per-config audit results, GUI screenshot path, and MP4 metadata
