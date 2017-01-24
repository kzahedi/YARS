include_directories(..)
include_directories(../namedPipeClientCom)

include(${CMAKE_SOURCE_DIR}/cmake/SetCompilerSettings.cmake)

add_library(YarsControllerNIL SHARED NIL.cpp)
add_library(YarsControllerSinus SHARED SinusController.cpp)

add_library(YarsControllerBraitenberg2b SHARED Braitenberg2b.cpp)
target_link_libraries(YarsControllerBraitenberg2b
  YarsRobotController
  boost_system
  boost_date_time)

add_library(YarsControllerBraitenberg2a SHARED Braitenberg2a.cpp)
target_link_libraries(YarsControllerBraitenberg2a
  YarsRobotController
  boost_system
  boost_date_time
  )

add_library(YarsControllerBraitenberg3b SHARED Braitenberg3b.cpp)
target_link_libraries(YarsControllerBraitenberg3b
  YarsRobotController
  boost_system
  boost_date_time)

add_library(YarsControllerBraitenberg3a SHARED Braitenberg3a.cpp)
target_link_libraries(YarsControllerBraitenberg3a
  YarsRobotController
  boost_system
  boost_date_time)

add_library(YarsControllerHexapod SHARED Hexapod.cpp)
target_link_libraries(YarsControllerHexapod
  YarsConfiguration
  YarsRobotController
  boost_system)

add_library(YarsControllerConstantValue SHARED ConstantValue.cpp)
target_link_libraries(YarsControllerConstantValue
  YarsRobotController
  boost_system)

add_library(YarsControllerNamedPipe SHARED NamedPipeController.cpp)
target_link_libraries(YarsControllerNamedPipe
  YarsRobotController
  YarsConfiguration
  YarsUtil
  boost_system
  ${Boost_LIBRARIES})

OPTION(YARS_USE_PYTHON "Compile PYTHON Controller" OFF)
if(YARS_USE_PHYTON)
  add_library(YarsControllerPython SHARED Python.cpp)
  target_link_libraries(YarsControllerPython
    YarsRobotController
    ${PYTHON_LIBRARIES})
endif(YARS_USE_PHYTON)

add_library(YarsControllerTCPIP SHARED TCPIP.cpp)
target_link_libraries(YarsControllerTCPIP
  YarsUtil
  YarsConfiguration
  YarsRobotController
  boost_system
  boost_filesystem
  boost_date_time
  )

OPTION(YARS_USE_MATLAB_CONTROLLER "Compile MATLAB Controller" OFF)
if(YARS_USE_MATLAB_CONTROLLER)
  include_directories(/Applications/MATLAB_R2015a.app/extern/include/)
  link_directories(/Applications/MATLAB_R2015a.app/bin/maci64)
  add_library(YarsControllerMATLAB SHARED MATLAB.cpp)
  target_link_libraries(YarsControllerMATLAB YarsRobotController eng mx m mat ut stdc++)
endif(YARS_USE_MATLAB_CONTROLLER)

OPTION(YARS_USE_JULIA_CONTROLLER "Compile Julia Controller" OFF)
if(YARS_USE_JULIA_CONTROLLER)
  SET(JULIA_DIR "/Applications/Julia.app/Contents/Resources/julia/" CACHE STRING "Where to find julia.")
  message("Julia include dir ${JULIA_DIR}/include/julia")
  add_definitions(-DJULIA_DIR='"${JULIA_DIR}"')
  include_directories(${JULIA_DIR}/include/julia)
  link_directories(${JULIA_DIR}/lib/julia)
  add_library(YarsControllerJulia                SHARED Julia.cpp)
  target_link_libraries(YarsControllerJulia                YarsRobotController julia)
endif(YARS_USE_JULIA_CONTROLLER)

OPTION(YARS_USE_NMODE_CONTROLLER "Compile NMODE Controller" OFF)
if(YARS_USE_NMODE_CONTROLLER)
  add_library(YarsControllerNMODE SHARED NMODEController.cpp)
  target_link_libraries(YarsControllerNMODE
    NMODEe
    YarsRobotController
    YarsConfiguration
    YarsUtil
    ${Boost_LIBRARIES})
endif(YARS_USE_NMODE_CONTROLLER)

OPTION(YARS_USE_CRBM_CONTROLLER "Compile CRBM Controller" OFF)
if(YARS_USE_CRBM_CONTROLLER)
  add_library(YarsControllerCRBMCtrl SHARED CRBMCtrl.cpp)
  target_link_libraries(YarsControllerCRBMCtrl
    CRBM
    YarsRobotController
    YarsConfiguration
    YarsUtil
    Entropy++
    ${Boost_LIBRARIES})
endif(YARS_USE_CRBM_CONTROLLER)
