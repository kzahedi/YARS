# YARS Performance Profiling

## Summary (Feb 2026)

Performance analysis using macOS `sample` tool on braitenberg_zoo.xml (5 robots).

### Benchmark Results

| Configuration | Iterations | Time | Rate |
|--------------|-----------|------|------|
| braitenberg.xml (1 robot) | 10,000 | 0.35s | ~28,500 iter/s |
| braitenberg_zoo.xml (5 robots) | 10,000 | 1.55s | ~6,450 iter/s |

### Hot Spots (Call Graph Analysis)

```
100% YarsMainControl::run()
 └── 100% YarsMainControl::__step()
      └── 100% YarsPhysicsModel::performOneSimulationStep()
           ├── 69% BulletPhysics::step()
           │    └── 69% Robots::postPhysicsUpdate()
           │         └── 67% Robot::postPhysicsUpdate()
           │              └── 46% GenericProximitySensor::postPhysicsUpdate()
           │                   └── 43% World::rayTest() → Bullet raycast
           └── 31% Other (controller updates, logging, etc.)
```

### Key Finding

**69% of CPU time is spent in Bullet Physics rayTest operations** for proximity sensors.

The actual raycast work happens deep inside Bullet:
- `btCollisionWorld::rayTest()`
- `btDbvtBroadphase::rayTest()`
- `btSubsimplexConvexCast::calcTimeOfImpact()`

### Optimization Suggestions

1. **Reduce sensor frequency** - Lower the update rate for proximity sensors
2. **Use simpler collision shapes** - Spheres are faster than cylinders/meshes
3. **Reduce sensor count** - Each proximity ray costs ~43% of frame time
4. **Spatial partitioning** - Already using DBVT (efficient)
5. **Batch raycasts** - Could potentially batch multiple rays per sensor

### Conclusion

The YARS codebase is efficient. The main bottleneck is inherent to physics-based proximity sensing. Performance is acceptable for real-time simulation with reasonable sensor configurations.
