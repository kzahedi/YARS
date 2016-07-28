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

## if you change this list, please adapt modules/InstallConfiguration.cmake
set(YARS_ROBOT_CONTROLLER_FILES "${CMAKE_SOURCE_DIR}/src/yars/configuration/data/DataNode.cpp"
                                "${CMAKE_SOURCE_DIR}/src/yars/configuration/data/DataObject.cpp"
                                "${CMAKE_SOURCE_DIR}/src/yars/configuration/data/DataController.cpp"
                                "${CMAKE_SOURCE_DIR}/src/yars/configuration/data/DataParseAttribute.cpp"
                                "${CMAKE_SOURCE_DIR}/src/yars/configuration/data/DataParseElement.cpp"
                                "${CMAKE_SOURCE_DIR}/src/yars/configuration/data/DataPhysicsParameter.cpp"
                                "${CMAKE_SOURCE_DIR}/src/yars/configuration/data/DataSensor.cpp"
                                "${CMAKE_SOURCE_DIR}/src/yars/configuration/data/DataNoise.cpp"
                                "${CMAKE_SOURCE_DIR}/src/yars/configuration/data/DataFilter.cpp"
                                "${CMAKE_SOURCE_DIR}/src/yars/configuration/data/DataParameter.cpp"
                                "${CMAKE_SOURCE_DIR}/src/yars/configuration/data/DataController.cpp"
                                "${CMAKE_SOURCE_DIR}/src/yars/util/YarsErrorHandler.cpp"
                                "${CMAKE_SOURCE_DIR}/src/yars/types/Colour.cpp"
                                "${CMAKE_SOURCE_DIR}/src/yars/types/P3D.cpp"
                                "${CMAKE_SOURCE_DIR}/src/yars/types/Pose.cpp"
                                "${CMAKE_SOURCE_DIR}/src/yars/types/Matrix.cpp"
                                "${CMAKE_SOURCE_DIR}/src/yars/types/Quaternion.cpp"
                                "${CMAKE_SOURCE_DIR}/src/yars/util/Filter.cpp"
                                "${CMAKE_SOURCE_DIR}/src/yars/util/Mapping.cpp"
                                "${CMAKE_SOURCE_DIR}/src/yars/configuration/xsd/specification/XsdAttribute.cpp"
                                "${CMAKE_SOURCE_DIR}/src/yars/configuration/xsd/specification/XsdChoice.cpp"
                                "${CMAKE_SOURCE_DIR}/src/yars/configuration/xsd/specification/XsdElement.cpp"
                                "${CMAKE_SOURCE_DIR}/src/yars/configuration/xsd/specification/XsdEnumeration.cpp"
                                "${CMAKE_SOURCE_DIR}/src/yars/configuration/xsd/specification/XsdInterval.cpp"
                                "${CMAKE_SOURCE_DIR}/src/yars/configuration/xsd/specification/XsdNode.cpp"
                                "${CMAKE_SOURCE_DIR}/src/yars/configuration/xsd/specification/XsdRegularExpression.cpp"
                                "${CMAKE_SOURCE_DIR}/src/yars/configuration/xsd/specification/XsdSequence.cpp"
                                "${CMAKE_SOURCE_DIR}/src/yars/configuration/xsd/specification/XsdSpecification.cpp")


add_library(YarsRobotController STATIC ${YARS_ROBOT_CONTROLLER_FILES} ${XERCESC_LIBRARY_FILE})
target_link_libraries(YarsRobotController YarsUtil ${Boost_LIBRARIES})
