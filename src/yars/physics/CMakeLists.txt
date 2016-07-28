#########################################################################
#                                                                       #
# This file is part of Yet Another Robot Simulator (YARS).              #
# Copyright (C) 2003-2015 Keyan Ghazi-Zahedi.                           #
# All rights reserved.                                                  #
# Email: keyan.zahedi@googlemail.com                                    #
# Web: https://github.com/kzahedi/YARS                                  #
#                                                                       #
# For a list of contributors see the file AUTHORS.                      #
#                                                                       #
# YARS is free software; you can redistribute it and/or modify it under #
# the terms of the GNU General Public License as published by the Free  #
# Software Foundation; either version 2 of the License, or (at your     #
# option) any later version.                                            #
#                                                                       #
# YARS is distributed in the hope that it will be useful, but WITHOUT   #
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or #
# FITNESS FOR A PARTICULAR PURPOSE.                                     #
#                                                                       #
# You should have received a copy of the GNU General Public License     #
# along with YARS in the file COPYING; if not, write to the Free        #
# Software Foundation, Inc., 51 Franklin St, Fifth Floor,               #
# Boston, MA 02110-1301, USA                                            #
#                                                                       #
#########################################################################


include_directories("/usr/local/Cellar/bullet/2.81/")
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

add_library(YarsPhysics ${LIBRARY_TYPE} ${PHYSICS_SRC}
)

IF(APPLE)
  target_link_libraries(YarsPhysics ${BULLET_LIBRARIES})
ELSE(APPLE)
  target_link_libraries(YarsPhysics ${BULLET_LIBRARIES} dl)
  # this is for the RZG
  # target_link_libraries(YarsPhysics /afs/ipp/home/z/zahedi/local/lib/libBulletDynamics.a /afs/ipp/home/z/zahedi/local/lib/libBulletCollision.a /afs/ipp/home/z/zahedi/local/lib/libLinearMath.a)
ENDIF(APPLE)
