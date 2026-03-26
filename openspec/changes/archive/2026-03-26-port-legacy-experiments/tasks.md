## 1. Explore Source XMLs

- [x] 1.1 Read `ant_basic.xml` in full and catalogue: body types, joint types, controller type, sensor types used
- [x] 1.2 Read `hexapod_reduced.xml` in full and catalogue: body types, mesh/ply references, joint types, controller type
- [x] 1.3 Check which mesh/ply assets referenced by hexapod_reduced.xml exist in the YARS build output

## 2. Port Hexapod MPI Experiment (prototype.hexapod/version4)

- [x] 2.1 Copy PLY mesh files from SVN experiments to `ply/h4/` in YARS repo
- [x] 2.2 Copy `hexapod_reduced.xml` to `xml/hexapod_mpi.xml`, update version to 0.8.41, fix paths to `ply/h4/`
- [x] 2.3 Fix structural differences vs working `hexapod.xml` (screen size → resolution, followables)
- [x] 2.4 Replace `<controller module="Sinus"/>` with comment (no controller)
- [x] 2.5 Remove cube obstacle PLY references (missing CubeObstacle.mesh asset)
- [x] 2.6 Run `./build/bin/yars --xml xml/hexapod_mpi.xml --iterations 10 --nogui` and fix parse errors
- [x] 2.7 Run 100 iterations and confirm simulation completes without crash

## 3. Port Hexaboard Experiment (nmode.hexaboard) — easiest port

- [x] 3.1 Copy `hexaboard.xml` to `xml/hexaboard.xml`, update version to 0.8.41, fix paths from `meshes/` to `ply/`
- [x] 3.2 Replace `<controller module="Sinus"/>` block (no controller)
- [x] 3.3 Fix structural differences (screen, sky, followables)
- [x] 3.4 Run `./build/bin/yars --xml xml/hexaboard.xml --iterations 10 --nogui` and fix parse errors
- [x] 3.5 Run 100 iterations and confirm simulation completes without crash

## 4. Port Ralf Hexapod Experiment

- [x] 4.1 Copy PLY mesh files from `ralf.hexapod/data/` to `ply/ralf/` in YARS repo
- [x] 4.2 Used `rl_exp_template/exp.xml` (fully instantiated, no template placeholders) as source
- [x] 4.3 Remove Julia controller block
- [x] 4.4 Fix 8-char hex colours, update version to 0.8.41
- [x] 4.5 Run `./build/bin/yars --xml xml/hexapod_ralf.xml --iterations 10 --nogui` and fix parse errors
- [x] 4.6 Run 100 iterations and confirm simulation completes without crash

## 5. Port Ant Experiment (rewrite from RoSiML)

- [x] 5.1 Write `xml/ant.xml` in rosiml 0.8.41 with box torso + 6 legs × 2 capsule segments
- [x] 5.2 Add 12 hinge joints (6 femur↔tibia + 6 torso↔femur) with limits from ant_basic.xml
- [x] 5.3 Passive joints (no controller needed)
- [x] 5.4 Run `./build/bin/yars --xml xml/ant.xml --iterations 10 --nogui` and fix parse errors
- [x] 5.5 Run 100 iterations and confirm simulation completes without crash

## 6. Visual Validation

- [x] 6.1 Run hexapod_mpi.xml with GUI and capture screenshot
- [x] 6.2 Run hexaboard.xml with GUI and capture screenshot
- [x] 6.3 Run hexapod_ralf.xml with GUI and capture screenshot
- [x] 6.4 Run ant.xml with GUI and capture screenshot
