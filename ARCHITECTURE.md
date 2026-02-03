# YARS Architecture Overview

This document describes the high-level architecture and design of YARS (Yet Another Robot Simulator).

## System Overview

YARS is a modular robot simulation platform consisting of several interconnected subsystems:

```
┌─────────────────────────────────────────────────────────────────┐
│                        YARS Application                          │
├─────────────────────────────────────────────────────────────────┤
│                      YarsMainControl                             │
│                   (Application Lifecycle)                        │
├────────────────┬────────────────┬────────────────┬──────────────┤
│  Configuration │    Physics     │ Visualization  │   Logging    │
│    (XML/XSD)   │   (Bullet)     │  (OGRE 14)     │  (Multi-out) │
├────────────────┼────────────────┼────────────────┼──────────────┤
│     Data       │    Robot       │   SceneGraph   │    File      │
│    Models      │  Controllers   │    Views       │    CSV       │
│    Parsers     │   Actuators    │   Overlays     │   Blender    │
│    Schemas     │    Sensors     │   SDL2 Input   │   Console    │
└────────────────┴────────────────┴────────────────┴──────────────┘
```

## Core Components

### 1. Main Control (`src/yars/main/`)

The application entry point and lifecycle manager.

**Key Classes:**
- `YarsMainControl` - Central coordinator for simulation lifecycle
- Handles initialization, simulation loop, and shutdown
- Manages communication between subsystems

**Flow:**
```
main() → YarsMainControl::init() → loadConfiguration() → startSimulation()
                                         ↓
                              YarsMainControl::step()
                                    ↓
                    ┌───────────────┼───────────────┐
                    ↓               ↓               ↓
              Physics::step()  View::step()  Logging::step()
```

### 2. Configuration System (`src/yars/configuration/`)

XML-based configuration with XSD schema validation.

**Components:**
- `YarsConfiguration` - Central configuration manager
- `data/` - Data model classes for all configuration elements
- XSD schema defines valid simulation configurations

**Configuration Hierarchy:**
```
<rosiml>
  ├── <simulator>     # Simulation parameters
  ├── <macros>        # Reusable definitions
  ├── <screens>       # Visualization settings
  ├── <environment>   # World setup (ground, walls, etc.)
  └── <robots>        # Robot definitions
        └── <robot>
              ├── <body>        # Physical components
              ├── <sensors>     # Sensor definitions
              ├── <actuators>   # Motor/joint definitions
              └── <controller>  # Control logic
```

### 3. Physics Engine (`src/yars/physics/`)

Bullet Physics integration for rigid body dynamics.

**Key Classes:**
- `Robot` - Physical robot representation
- `Body` - Rigid body wrapper
- `Joint` - Constraint/joint handling
- `Sensor` / `Actuator` - Physics-aware I/O

**Physics Loop:**
```
PhysicsEngine::step()
    → updateActuators()    # Apply motor forces
    → bulletWorld.step()   # Bullet simulation
    → updateSensors()      # Read sensor values
    → updateBodies()       # Sync transforms
```

### 4. Visualization (`src/yars/view/`)

OGRE 14-based 3D rendering with SDL2 window management.

**GUI Components (`view/gui/`):**
- `OgreHandler` - OGRE initialization and resource management
- `SdlWindow` - SDL2 window + OGRE viewport integration
- `SceneGraph` - Scene node hierarchy
- `TextOverlay` - On-screen display (OSD)
- `CameraMan` - Camera control and following modes

**Rendering Pipeline:**
```
OgreHandler::setupSceneManager()
    → RTSS initialization (shader generation)
    → Resource loading (materials, meshes, fonts)
    → SceneGraph creation

SdlWindow::step()
    → Handle input events
    → Update camera
    → OgreHandler::step() → root->renderOneFrame()
```

**OGRE 14 Specifics:**
- Uses GL3Plus render system (OpenGL 3.3+)
- RTSS (Runtime Shader System) for material shaders
- No fixed-function pipeline (all materials need shaders)
- Cameras and lights attached to SceneNodes

### 5. Logging System (`src/yars/logging/`)

Multi-target logging framework.

**Available Loggers:**
- `FileLogger` - Text file output
- `CSVLogger` - Structured CSV export
- `ConsoleLogger` - Terminal output
- `BlenderLogger` - Blender animation export
- `SelforgLogger` - Self-organization metrics

**Logger Configuration:**
```xml
<logging>
  <csv filename="data.csv" precision="6">
    <sensor name="sensor1"/>
    <actuator name="motor1"/>
  </csv>
</logging>
```

### 6. Controllers (`contrib/controller/`)

Plugin-based robot controller system.

**Controller Types:**
- Built-in: Braitenberg vehicles, constant value, sine wave
- External: TCP/IP, Named Pipe communication
- Custom: Dynamically loaded `.so`/`.dylib` plugins

**Controller Interface:**
```cpp
class Controller {
    virtual void init();
    virtual void update(double dt);
    virtual void close();
};
```

## Data Flow

### Simulation Step

```
1. Input Phase
   └── SDL2 events → Camera/UI updates

2. Control Phase
   └── Controllers read sensors → compute outputs → set actuators

3. Physics Phase
   └── Bullet applies forces → steps simulation → updates transforms

4. Render Phase
   └── SceneGraph sync → OGRE renders frame

5. Logging Phase
   └── Loggers capture state → write outputs
```

### Configuration Loading

```
XML File → SAX Parser → Data Models → Runtime Objects
                              ↓
                        Validation (XSD)
                              ↓
                      Configuration Events
                              ↓
                 Subsystem Initialization
```

## Design Patterns

### Observer Pattern
Used for event notification between components.
- `ObservableMessage` - Event base class
- Components register for events they care about

### Singleton Pattern
Used for global managers:
- `OgreHandler::instance()`
- `Data::instance()` (configuration data)

### Factory Pattern
Used for creating configurable objects:
- Sensor factories
- Actuator factories
- Controller factories

### Component Pattern
Robot composition:
- Robots contain Bodies, Sensors, Actuators
- Each component handles its own physics/rendering

## File Organization

```
src/yars/
├── configuration/
│   ├── data/           # Data model classes
│   ├── xsd/            # Schema definitions
│   └── container/      # Generic containers
├── defines/            # Constants, version info
├── logging/            # Logger implementations
├── main/               # Application control
├── physics/            # Bullet integration
├── types/              # Math types, matrices
├── util/               # Utilities (CircularBuffer, StringTokeniser, etc.)
└── view/
    ├── console/        # Text-mode view
    └── gui/            # OGRE visualization
```

## Extension Points

### Adding a New Controller

1. Create controller class in `contrib/controller/`
2. Implement `Controller` interface
3. Register in CMakeLists.txt
4. Reference in XML configuration

### Adding a New Sensor/Actuator

1. Define data model in `configuration/data/`
2. Implement physics behavior in `physics/`
3. Add visualization in `view/gui/`
4. Update XSD schema

### Adding a New Logger

1. Implement logger class in `logging/`
2. Add factory method
3. Update configuration parser
4. Update XSD schema

## Threading Model

YARS uses a single-threaded main loop:

```
Main Thread:
  └── Simulation Loop
        ├── Input handling
        ├── Physics step
        ├── Rendering
        └── Logging

Background (if enabled):
  └── Video capture encoding
```

Thread safety considerations:
- Physics and rendering are synchronized
- Logging may buffer for async writes
- Controller callbacks run on main thread

## Memory Management

- OGRE resources managed by ResourceManager
- Bullet physics objects owned by physics world
- Configuration data uses shared pointers
- Scene graph uses OGRE's node ownership

## Performance Considerations

- Physics step rate configurable (default: 100 Hz)
- Rendering rate tied to vsync or uncapped
- Large scenes benefit from spatial partitioning
- Shader compilation cached by RTSS
- **Hot path**: Physics raycast for sensors (~69% of CPU time)
- See `PROFILING.md` for detailed performance analysis

## Testing

YARS uses [Catch2](https://github.com/catchorg/Catch2) for unit and integration testing.

**Test Structure:**
```
tests/
├── unit/                    # Fast, isolated unit tests
│   ├── test_p3d.cpp
│   ├── test_quaternion.cpp
│   ├── test_matrix.cpp
│   ├── test_circular_buffer.cpp
│   ├── test_string_tokeniser.cpp
│   └── test_colour.cpp
└── integration/             # Tests requiring full subsystems
    ├── test_configuration.cpp
    └── test_simulation.cpp
```

**Running Tests:**
```bash
cmake .. -DYARS_BUILD_TESTS=ON
make
ctest --output-on-failure
```

## Dependencies

| Library | Purpose | Notes |
|---------|---------|-------|
| **Bullet Physics 3** | Rigid body dynamics | System library |
| **OGRE 14** | 3D rendering | Git submodule, macOS frameworks |
| **SDL2** | Window/input | System library |
| **Xerces-C** | XML parsing | System library |
| **CLI11** | Command-line parsing | Header-only, replaced Boost.ProgramOptions |
| **Catch2 3** | Unit testing | Optional, system library |

Note: YARS uses C++17 standard library features (`<filesystem>`, `<optional>`) instead of Boost equivalents.
