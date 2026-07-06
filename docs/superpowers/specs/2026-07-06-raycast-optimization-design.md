# Sensor Raycast Optimization — Design

**Date:** 2026-07-06
**Status:** Approved (brainstorm session 2026-07-06)
**Sequencing:** After the CI quality gates (perf trend must exist) and
independent of the hardening batch. If the Bullet SIMD switch lands
first, re-baseline before measuring.

## Background

A CPU profile of headless braitenberg (2026-07-06, M4, ~42k steps/s
single-core) shows sensor raycasting as the largest hotspot:
`btDbvt::rayTestInternal`, `btSubsimplexConvexCast`, and the Voronoi
simplex solver dominate the sample. `GenericProximitySensor` fires **5
full-length rays per sensor per step**
(`src/yars/physics/bullet/GenericProximitySensor.cpp:44`), each an
independent `World::rayTest` (`src/yars/physics/bullet/World.cpp:134`)
with its own BVH traversal; only the minimum distance over the 5 rays
survives. `GenericLDRSensor` casts one ray per light source
(`GenericLDRSensor.cpp:47`).

## Scope decision

**Result-preserving optimizations only.** Sensor readings must be
bit-identical; the CSV regression references are untouched. Semantic
knobs (e.g. per-sensor update-rate attributes) are explicitly out of
scope — if the result-preserving ceiling proves too low, rate reduction
becomes a separate proposal.

## Design

### Measurement first

Add a sensor-heavy benchmark scene (one robot with several proximity +
LDR sensors in a cluttered world) under `xml/`, and capture a before
profile with `sample`/Instruments on the M4. Every optimization below
must show up in this measurement or it does not land.

### Optimization 1 — cross-ray length clamping (the algorithmic win)

In `GenericProximitySensor::postPhysicsUpdate`, shorten each subsequent
ray to the current running minimum distance: a hit farther than the
running min cannot change the result, and a shorter ray segment prunes
the `btDbvt` traversal and skips narrowphase tests entirely.
Result-preserving by construction (min over rays is unchanged).
Largest expected effect in cluttered scenes.

### Optimization 2 — per-call overhead

- Add a `World::rayTest` overload taking `btVector3` start/end directly,
  avoiding P3D conversions per ray.
- Reuse/reset a `ClosestRayResultCallback` instead of constructing one
  per call where profitable.
- In `prePhysicsUpdate`, hoist loop-invariant `P3D`/`Quaternion`
  computation out of the per-ray loop.

### Optimization 3 — LDR sensors

Per-light results are all needed (no cross-ray clamping possible), but:

- apply the same per-call overhead fixes;
- early-out (skip the ray) when a light source is beyond the sensor's
  maximum range, when a range exists in the data model.

## Acceptance

- Bit-exact CSV references for braitenberg and hexapod unchanged on
  both platforms.
- Measurable speedup on the sensor-heavy benchmark; target ≥10%
  end-to-end, but the profile decides — if investigation shows a lower
  ceiling, land what is clean and document the remainder in
  `docs/planning/v0.8.7-open-points.md`.
- GUI run with frame export still renders sensor visualizations
  correctly.
- Before/after profile samples archived in the PR description.

## Out of scope

- Sensor update-rate reduction or any XML schema change.
- GPU raycasting (assessed 2026-07-06: dispatch overhead exceeds the
  per-step budget at YARS scene sizes).
- Bullet multithreading.
