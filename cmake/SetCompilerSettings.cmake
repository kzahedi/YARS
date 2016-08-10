set(CMAKE_VERBOSE_MAKEFILE       OFF)
# set(CMAKE_BASE_NAME              "g++")
# set(CMAKE_CXX_COMPILER           "g++")
# set(CMAKE_CXX_COMPILER_FULLPATH  "g++")
set(EXECUTABLE_OUTPUT_PATH       ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH          ${PROJECT_BINARY_DIR}/lib)

#set(WARNINGS "-W -Wall")
set(WARNINGS "")

if(APPLE AND UNIX)
  set(CMAKE_CXX_FLAGS_DEBUG        "-g  -fPIC ${WARNINGS}")
  set(CMAKE_CXX_FLAGS_RELEASE      "-O3 -fPIC ${WARNINGS}")
  set(CMAKE_CXX_FLAGS_DISTRIBUTION "-O3 -fPIC ${WARNINGS}")
endif(APPLE AND UNIX)

if(UNIX AND NOT APPLE)
  # set(CMAKE_CXX_FLAGS_DEBUG   "-g -O0 -W -Wall -fpermissive")
  # set(CMAKE_CXX_FLAGS_RELEASE "   -O3 -W -Wall -fpermissive")
  set(CMAKE_CXX_FLAGS_DEBUG   "-g -O0 -fpermissive ${WARNINGS} -pthread -fPIC")
  set(CMAKE_CXX_FLAGS_RELEASE "   -O3 -fpermissive ${WARNINGS} -pthread -fPIC")
endif(UNIX AND NOT APPLE)

