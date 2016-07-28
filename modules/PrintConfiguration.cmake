IF(CMAKE_VERBOSE)
  INCLUDE(modules/CMakeOutputVariables.cmake)
ENDIF(CMAKE_VERBOSE)

MESSAGE("----- Build Configuration --------------------")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  message("Compiling yars in DEBUG mode.")
else(CMAKE_BUILD_TYPE STREQUAL "Debug")
  message("Compiling yars in RELEASE mode.")
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")

IF(YARS_SUPPRESS_OUTPUT)
  message("- suppressing all output.")
endif(YARS_SUPPRESS_OUTPUT)

MESSAGE("Compiler location         = ${CMAKE_CXX_COMPILER_FULLPATH}")
IF(CMAKE_COMPILER_IS_GNUCXX)
  MESSAGE("Compiler GNUCXX is used")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)
IF(CMAKE_COMPILER_IS_MINGW)
  MESSAGE("Compiler MINGW is used")
ENDIF(CMAKE_COMPILER_IS_MINGW)
IF(CMAKE_COMPILER_IS_CYGWIN)
  MESSAGE("Compiler CYGWIN is used")
ENDIF(CMAKE_COMPILER_IS_CYGWIN)


IF(YARS_TARGET STREQUAL "yars")
  message("YARS Binary name: ${YARS_TARGET}.")
ELSE(YARS_TARGET STREQUAL "yars")
  message("YARS Binary name: ${YARS_TARGET}.")
endif(YARS_TARGET STREQUAL "yars")

IF(YARS_TESTCASES)
  message("\nBuilding Testcases.")
  set(BUILD_TESTCASES 1)
ELSE(YARS_TESTCASES)
  message("\nNot Building Testcases.")
endif(YARS_TESTCASES)

if(stop)
  MESSAGE(FATAL_ERROR "There were errors. See above.")
endif(stop)

MESSAGE("Installing to ${CMAKE_INSTALL_PREFIX}")
MESSAGE("--------------------------------------------------")
