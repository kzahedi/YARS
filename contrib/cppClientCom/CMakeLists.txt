project(YarsClientCom)

add_library(YarsClientCom SHARED
  YarsClientCom.cpp
  ${CMAKE_SOURCE_DIR}/src/yars/util/Socket.cpp
  ${CMAKE_SOURCE_DIR}/src/yars/util/YarsErrorHandler.cpp
  ${CMAKE_SOURCE_DIR}/src/yars/types/Domain.h
  )

INSTALL(DIRECTORY ${PROJECT_BINARY_DIR}/lib
  CONFIGURATIONS Release Debug
  DESTINATION ${CMAKE_INSTALL_PREFIX}
  FILES_MATCHING PATTERN "libYarsClientCom*" 
  )

INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/contrib/cppClientCom
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include/yars
  CONFIGURATIONS Release Debug
  PATTERN "*.cpp" EXCLUDE 
  PATTERN "*.txt" EXCLUDE 
  )

