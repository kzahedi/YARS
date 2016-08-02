include_directories(..)
include_directories(../namedPipeClientCom)

include(${CMAKE_SOURCE_DIR}/modules/SetCompilerSettings.cmake)

add_library(YarsControllerNIL SHARED NIL.cpp)
add_library(YarsControllerSinus SHARED SinusController.cpp)

add_library(YarsControllerBraitenberg2b SHARED Braitenberg2b.cpp)
target_link_libraries(YarsControllerBraitenberg2b
  boost_system
  boost_date_time
  YarsRobotController)

add_library(YarsControllerBraitenberg2a SHARED Braitenberg2a.cpp)
target_link_libraries(YarsControllerBraitenberg2a
  boost_system
  boost_date_time
  YarsRobotController)

add_library(YarsControllerBraitenberg3b SHARED Braitenberg3b.cpp)
target_link_libraries(YarsControllerBraitenberg3b
  boost_system
  boost_date_time
  YarsRobotController)

add_library(YarsControllerBraitenberg3a SHARED Braitenberg3a.cpp)
target_link_libraries(YarsControllerBraitenberg3a
  boost_system
  boost_date_time
  YarsRobotController)

add_library(YarsControllerHexapod SHARED Hexapod.cpp)
target_link_libraries(YarsControllerHexapod
  boost_system
  YarsRobotController
  YarsConfiguration)

add_library(YarsControllerConstantValue SHARED ConstantValue.cpp)
target_link_libraries(YarsControllerConstantValue
  boost_system
  YarsRobotController)

add_library(YarsControllerNamedPipe SHARED NamedPipeController.cpp)
target_link_libraries(YarsControllerNamedPipe
  boost_system
  YarsRobotController
  YarsConfiguration
  YarsUtil
  ${Boost_LIBRARIES})

OPTION(YARS_USE_PYTHON "Compile PYTHON Controller" OFF)
if(YARS_USE_MATLAB)
  add_library(YarsControllerPython SHARED Python.cpp)
  target_link_libraries(YarsControllerPython
    YarsRobotController
    ${PYTHON_LIBRARIES})
endif(YARS_USE_MATLAB)

add_library(YarsControllerTCPIP  SHARED TCPIP.cpp)
target_link_libraries(YarsControllerTCPIP
  boost_system
  boost_filesystem
  boost_date_time
  YarsRobotController
  YarsConfiguration
  YarsUtil
  ${Boost_LIBRARIES})

OPTION(YARS_USE_MATLAB "Compile MATLAB Controller" OFF)
if(YARS_USE_MATLAB)
  include_directories(/Applications/MATLAB_R2015a.app/extern/include/)
  link_directories(/Applications/MATLAB_R2015a.app/bin/maci64)
  add_library(YarsControllerMATLAB SHARED MATLAB.cpp)
  target_link_libraries(YarsControllerMATLAB YarsRobotController eng mx m mat ut stdc++)
endif(YARS_USE_MATLAB)

OPTION(YARS_USE_JULIA "Compile Julia Controller" OFF)
if(YARS_USE_JULIA)
  SET(JULIA_DIR "/Applications/Julia.app/Contents/Resources/julia/" CACHE STRING "Where to find julia.")
  message("Julia include dir ${JULIA_DIR}/include/julia")
  add_definitions(-DJULIA_DIR='"${JULIA_DIR}"')
  include_directories(${JULIA_DIR}/include/julia)
  link_directories(${JULIA_DIR}/lib/julia)
  add_library(YarsControllerJulia                SHARED Julia.cpp)
  target_link_libraries(YarsControllerJulia                YarsRobotController julia)
endif(YARS_USE_JULIA)
