set(PHYSICS_SRC YarsPhysicsModel.cpp
                YarsPhysicsControl.cpp

                bullet/BulletPhysics.cpp
                bullet/World.cpp
                bullet/Environment.cpp
                bullet/CollisionCallback.cpp
                bullet/Object.cpp
                bullet/Sphere.cpp
                bullet/Box.cpp
                bullet/Cylinder.cpp
                bullet/Capsule.cpp
                bullet/Composite.cpp
                bullet/ObjectFactory.cpp
                bullet/MyMotionState.cpp
                bullet/MyMotionStateDebug.cpp
                bullet/MyCompositeMotionState.cpp

                bullet/Robots.cpp
                bullet/Robot.cpp

                bullet/Actuator.cpp
                bullet/ActuatorFactory.cpp
                bullet/HingeActuator.cpp
                bullet/SliderActuator.cpp
                bullet/MuscleActuator.cpp
                bullet/FixedActuator.cpp
                bullet/GenericActuator.cpp
                bullet/GenericSpringActuator.cpp

                bullet/Sensor.cpp
                bullet/SensorFactory.cpp
                bullet/ActuatorDeflectionSensor.cpp
                bullet/ActuatorVelocitySensor.cpp
                bullet/PositionSensor.cpp
                bullet/ObjectVelocitySensor.cpp
                bullet/ObjectAngularVelocitySensor.cpp
                bullet/OrientationSensor.cpp
                bullet/GenericProximitySensor.cpp
                bullet/GenericActuatorSensor.cpp
                bullet/GenericLDRSensor.cpp
                bullet/MuscleSensor.cpp
                bullet/BinaryContactSensor.cpp
)

set(PHYSICS_SRC ${PHYSICS_SRC} bullet/Ply.cpp)

add_library(YarsPhysics ${LIBRARY_TYPE} ${PHYSICS_SRC})

# Modern CMake: Use imported targets for better dependency management
target_link_libraries(YarsPhysics 
  PRIVATE
    ${BULLET_LIBRARIES}  # Keep current approach for now, as Bullet may not have modern targets
    $<$<AND:$<PLATFORM_ID:Linux>,$<NOT:$<PLATFORM_ID:Darwin>>>:dl>
)

# Bullet Physics headers (needed until Bullet provides modern CMake targets)
target_include_directories(YarsPhysics PRIVATE ${BULLET_INCLUDE_DIRS})

# Ensure proper C++ standard for this target
target_compile_features(YarsPhysics PRIVATE cxx_std_17)
