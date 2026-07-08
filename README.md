# YARS
YARS - Yet Another Robot Simulator

A selection of creatures in action can be found [here](https://www.youtube.com/watch?v=T3gDxaLcmUw).

Other videos can be found [here](https://www.youtube.com/playlist?list=PLrIVgT56nVQ55q0DNvVbGHfMf87PBnOM-).

## Configuration

Simulations are described by JSON files (examples in [`xml/`](xml/)):

```json
{
  "yars": {
    "version": "0.8.41",
    "simulator": { "frequency": 100, "solver_iterations": 10 },
    "environment": {
      "ground": { "fixed": true, "visualisation": { "texture_name": "YARS/DryGroundSmall" } }
    },
    "robots": { "robot": { "...": "body, sensors, actuators, controller" } }
  }
}
```

The format supports comments, `$include` composition, native JSON
types, and ships with a JSON Schema for live validation in VS Code.
**Full format documentation: [docs/JSON_Format.md](docs/JSON_Format.md).**

```bash
./bin/yars --xml xml/braitenberg.json                 # run a simulation
python3 scripts/json-schema-check.py my.json          # validate a config
python3 scripts/json-canonicalize.py my.json          # normalize formatting
```

## Building

- macOS: [docs/macOS_Build.md](docs/macOS_Build.md)
- Linux: [docs/Linux_Build.md](docs/Linux_Build.md)


### Third party files
<b> Fonts </b><br>
http://www.fontspace.com

<b>Images</b><br>
CBT.jpg    - http://www.flickr.com/photos/stallio/3149911976/sizes/o/in/photostream/<br>
wall.jpg   - http://agf81.deviantart.com/art/Brick-Seamless-160259435<br>
clouds.jpg - http://www.ode.org<br>
wheel.jpg  - source no longer available<br>
wood2.jpg  - source no longer available<br>
ground.jpg - source no longer available



<!--
# Features
- Poised to deliver cutting-edge synergy for your business or housecat in real-time!
- Twitter-ready out of the box!
- Both HAL9000 and Skynet proof!
- Low calorie, 100% vegan, and homeopathic friendly!
- Excellent source of vitamin Q!
- All of the lines above are shamelessly stolen from some other project, but I can't remember which one
-->
