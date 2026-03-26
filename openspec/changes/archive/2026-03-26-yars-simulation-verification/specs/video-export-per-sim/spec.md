## ADDED Requirements

### Requirement: Frame capture produces PNG sequence per simulation
The system SHALL capture a sequence of PNG frames via `--framesDirectory` for each simulation that runs with GUI enabled.

#### Scenario: Frames directory is populated
- **WHEN** YARS runs with `--framesDirectory <path> --iterations 500`
- **THEN** the directory contains sequentially numbered PNG files (one per rendered frame)

#### Scenario: Frames directory is created automatically
- **WHEN** the specified frames directory does not exist
- **THEN** YARS creates it before writing the first frame

### Requirement: MP4 video is assembled from frames for each simulation
For each simulation that successfully exports frames, an MP4 video SHALL be assembled using ffmpeg at 30fps.

#### Scenario: MP4 file is created
- **WHEN** ffmpeg is run on a frames directory containing at least 1 PNG
- **THEN** an MP4 file is produced in `docs/videos/<simulation-name>.mp4`

#### Scenario: Videos are organized by simulation name
- **WHEN** all simulations complete
- **THEN** `docs/videos/` contains one MP4 per simulation, named after the XML config (e.g., `braitenberg.mp4`, `falling_objects.mp4`)

### Requirement: Video generation script automates the full pipeline
A shell script SHALL automate: run simulation → capture frames → assemble video → clean up frames.

#### Scenario: Script runs all compatible simulations
- **WHEN** the script is executed
- **THEN** it iterates over all compatible XML configs, runs each with GUI + frame capture, assembles video, and reports success/failure per simulation
