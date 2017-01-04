if(UNIX AND NOT APPLE)
  set(CMAKE_CXX_FLAGS_DEBUG   "-g -O0  ${WARNINGS} -pthread -fPIC")
  set(CMAKE_CXX_FLAGS_RELEASE "   -O3  ${WARNINGS} -pthread -fPIC")
endif(UNIX AND NOT APPLE)

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
