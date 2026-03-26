## ADDED Requirements

### Requirement: OSD displays simulation time
The system SHALL render an on-screen display (OSD) showing elapsed simulation time when the GUI is active and `--osd` flag is passed (or OSD is toggled on).

#### Scenario: OSD visible with --osd flag
- **WHEN** YARS is started with `--osd` and a valid XML config
- **THEN** the Ogre overlay renders elapsed time (days/hours/minutes/seconds/milliseconds) in the top-left corner of the render window

#### Scenario: OSD not shown without flag
- **WHEN** YARS is started without `--osd`
- **THEN** no OSD overlay is rendered

#### Scenario: OSD updates each frame
- **WHEN** the simulation is running with OSD enabled
- **THEN** the elapsed time string updates every rendered frame to reflect current simulation step

### Requirement: OSD does not crash in --nogui mode
The system SHALL NOT crash or error when OSD is requested but `--nogui` is active.

#### Scenario: OSD flag ignored in nogui mode
- **WHEN** YARS is started with `--osd --nogui`
- **THEN** simulation runs normally, OSD flag is silently ignored, no crash occurs
