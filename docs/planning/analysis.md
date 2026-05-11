# YARS Codebase Analysis

## Overview

**YARS** (Yet Another Robot Simulator) is a physics-based robot simulation platform written in modern C++17 with:
- **3D visualization** via OGRE 14 (OpenGL 3.3+)
- **Physics simulation** via Bullet Physics
- **XML-based configuration** with XSD validation
- **Plugin controller system** for extensible robot control
- **~61,000 lines of code** across 535 files

---

## Directory Structure

```
YARS/
├── src/yars/                 # Core source (56,289 LOC)
│   ├── configuration/        # XML/XSD parsing & data models (31,123 LOC, 55%)
│   ├── physics/              # Bullet Physics integration (5,799 LOC)
│   ├── view/                 # OGRE visualization & GUI (6,691 LOC)
│   ├── logging/              # Multi-format logging (1,933 LOC)
│   ├── main/                 # Application lifecycle (531 LOC)
│   ├── types/                # Math & data types (4,518 LOC)
│   ├── util/                 # Utilities & filters (5,020 LOC)
│   └── rc/                   # Robot controller interface (headers)
├── contrib/                  # External controllers & clients (~5,026 LOC)
├── materials/                # 40 OGRE material definitions
├── shaders/                  # GLSL shader programs
├── xml/                      # 18 example simulation configs
└── ext/ogre/                 # OGRE as Git submodule
```

---

## Architecture Patterns

| Pattern | Usage |
|---------|-------|
| **Observer** | Cross-module events via `Observable`/`Observer` |
| **Singleton** | `OgreHandler`, `Data`, `World` instances |
| **Factory** | `ActuatorFactory`, `SensorFactory`, `ObjectFactory` |
| **Component** | Robot = Objects + Sensors + Actuators + Controller |
| **MVC** | Physics/View/Logging separated into Model/Control pairs |

---

## Core Components

### 1. Physics Engine (Bullet)

```
BulletPhysics → World (singleton)
     │
     └── Robot[]
           ├── Object[] (Box, Sphere, Cylinder, Capsule, Ply, Composite)
           ├── Actuator[] (Hinge, Slider, Fixed, Generic, Muscle, Spring)
           ├── Sensor[] (Position, Velocity, Contact, Proximity, LDR, etc.)
           └── RobotController (plugin .dylib/.so)
```

**5 Actuator Types:**
- `HingeActuator` - 1-DOF rotation
- `SliderActuator` - 1-DOF linear
- `FixedActuator` - Rigid joint
- `MuscleActuator` - Biomimetic with force/velocity dynamics
- `GenericSpringActuator` - Spring-damper

**11 Sensor Types:**
- Position, Orientation, Velocity, Angular Velocity
- Binary Contact, Proximity (ray-based), LDR
- Actuator Deflection/Velocity/Force, Muscle sensors

### 2. Visualization (OGRE 14 + SDL2)

```
OgreHandler (singleton)
     │
     └── SdlWindow[]
           ├── CameraMan (Orbit, Follow, Tripod, Offset modes)
           ├── SceneGraph
           │     ├── SceneGraphRobotNode[]
           │     ├── SceneGraphEnvironmentNode
           │     └── SceneGraphTraceLineObject[]
           └── TextOverlay (OSD)
```

### 3. Configuration System (55% of codebase)

- 200+ `Data*` classes representing XML elements
- SAX-based XML parsing with XSD validation
- Hierarchical: `DataRobotSimulationDescription` → `DataRobot` → `DataObject`/`DataSensor`/`DataActuator`

### 4. Logging System

```
LoggingHandler
     └── Logger[]
           ├── CSVLogger
           ├── FileLogger
           ├── ConsoleLogger
           ├── BlenderLogger
           └── GnuplotLogger
```

---

## Data Flow

```
┌─────────────────┐
│  XML Config     │
└────────┬────────┘
         ▼
┌─────────────────┐
│ YarsXSDParser   │  SAX parsing + validation
└────────┬────────┘
         ▼
┌─────────────────┐
│ Data* Objects   │  In-memory configuration model
└────────┬────────┘
         ▼
┌────────┴────────┬─────────────┬───────────────┐
▼                 ▼             ▼               ▼
Physics         View         Logging         Main
(Bullet)       (OGRE)      (CSV/File)    (Coordinator)
```

**Simulation Loop:**
1. **Input** → SDL events → Camera/OSD updates
2. **Control** → `RobotController::update()` → motor commands
3. **Physics** → Bullet step → sensor readings
4. **Render** → SceneGraph sync → OGRE frame
5. **Logging** → CSV/Blender/File output

---

## Key Classes

| Class | Responsibility |
|-------|----------------|
| `YarsMainControl` | Application lifecycle orchestration |
| `BulletPhysics` | Physics engine interface |
| `World` | Bullet dynamics world singleton |
| `Robot` | Physical robot entity with components |
| `OgreHandler` | OGRE initialization & management |
| `SdlWindow` | Window, input, rendering |
| `SceneGraph` | 3D scene hierarchy |
| `Data` | Configuration data singleton |
| `RobotController` | Plugin controller interface |

---

## External Dependencies

| Library | Purpose |
|---------|---------|
| **Bullet 3.0+** | Physics simulation |
| **OGRE 14.0+** | 3D rendering (GL3Plus) |
| **SDL2** | Window/input management |
| **Xerces-C 3.2+** | XML parsing |
| **Boost** | program_options (CLI) |
| *FFmpeg* (optional) | Video recording |

---

## Build System (CMake 3.16+)

```bash
cd ~/code/YARS/build
cmake .. && make -j4
./bin/yars --xml ../xml/braitenberg.xml
```

**Build Flags:**
- `YARS_USE_VISUALISATION` - Enable OGRE GUI
- `YARS_USE_CAPTURE_VIDEO` - FFmpeg video recording
- `YARS_USE_SOFT_BODIES` - Soft body physics

---

## Code Statistics

| Module | Files | LOC | % |
|--------|-------|-----|---|
| Configuration | 258 | 31,123 | 55.3% |
| View/GUI | 75 | 6,691 | 11.9% |
| Physics | 81 | 5,799 | 10.3% |
| Util | 44 | 5,020 | 8.9% |
| Types | 29 | 4,518 | 8.0% |
| Logging | 32 | 1,933 | 3.4% |
| **Total** | **535** | **56,289** | - |

---

## Recent Modernization (2026)

- **C++17**: `std::thread`, `std::filesystem`, `std::function`, `std::chrono`
- **OGRE 14**: Migrated from 1.x, using Runtime Shader System (RTSS)
- **Iterator modernization**: 74% reduction in verbose iterator declarations
- **nullptr/override**: Replaced NULL, added override specifiers

---

## Architecture Diagram

```
┌─────────────────────────────────────────────────────────┐
│            YarsMainControl (Orchestrator)               │
├──────────────┬──────────────┬──────────────┬────────────┤
│   Physics    │     View     │   Logging    │   Config   │
│  (Bullet)    │   (OGRE)     │  (CSV/etc)   │   (XML)    │
├──────────────┴──────────────┴──────────────┴────────────┤
│              Types & Utilities Library                  │
└─────────────────────────────────────────────────────────┘
```

This architecture enables:
- Headless physics simulation (`--nogui`)
- Multiple synchronized windows
- Plugin controller loading at runtime
- Clean subsystem lifecycle management

---

## Key Source Files

| File | Path | Description |
|------|------|-------------|
| `YarsMainControl.cpp` | main/ | Application entry & lifecycle |
| `BulletPhysics.cpp` | physics/bullet/ | Physics engine wrapper |
| `Robot.cpp` | physics/bullet/ | Robot entity management |
| `World.cpp` | physics/bullet/ | Bullet world singleton |
| `OgreHandler.cpp` | view/gui/ | OGRE initialization |
| `SdlWindow.cpp` | view/gui/ | Window & input handling |
| `SceneGraph.cpp` | view/gui/ | 3D scene management |
| `DataRobot.cpp` | configuration/data/ | Robot configuration |
| `DataRobotSimulationDescription.cpp` | configuration/data/ | Root config object |
| `RobotController.hpp` | rc/ | Controller plugin interface |

---

## XML Configuration Structure

```xml
<rosiml version="0.7.28">
  <simulator frequency="100"/>

  <screens>
    <screen name="main" autoShow="true">
      <geometry x="0" y="0" width="800" height="600"/>
      <camera type="follow">
        <position x="0" y="-5" z="2"/>
        <lookAt x="0" y="0" z="0"/>
      </camera>
    </screen>
  </screens>

  <environment>
    <ground fog="true"/>
    <gravitation x="0" y="0" z="-9.81"/>
  </environment>

  <robots>
    <robot name="braitenberg">
      <body>
        <box name="chassis">
          <dimension width="0.2" height="0.1" depth="0.05"/>
          <physics><mass value="1.0"/></physics>
        </box>
      </body>
      <sensors>
        <genericProximitySensor name="left_eye" object="chassis"/>
      </sensors>
      <actuators>
        <hinge name="left_wheel" source="chassis" destination="wheel_l"/>
      </actuators>
      <controller module="libBraitenberg2b.dylib" frequency="10"/>
    </robot>
  </robots>

  <logging>
    <csv filename="output" useDate="true">
      <target>left_eye</target>
    </csv>
  </logging>
</rosiml>
```

---

## Related Documents

- [[Refactoring_Plan]] - Modernization progress and phases
- [[CLAUDE]] - Project-specific Claude Code instructions
