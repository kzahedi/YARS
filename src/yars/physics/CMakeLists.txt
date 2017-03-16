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
                bullet/FixedActuator.cpp
                bullet/GenericActuator.cpp
                bullet/GenericSpringActuator.cpp
                bullet/MuscleActuator.cpp

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
                bullet/BinaryContactSensor.cpp
)

set(PHYSICS_SRC ${PHYSICS_SRC} bullet/Ply.cpp)

add_library(YarsPhysics ${LIBRARY_TYPE} ${PHYSICS_SRC})

target_link_libraries(YarsPhysics ${BULLET_LIBRARIES})
IF(UNIX AND NOT APPLE)
  target_link_libraries(YarsPhysics dl)
ENDIF(UNIX AND NOT APPLE)
