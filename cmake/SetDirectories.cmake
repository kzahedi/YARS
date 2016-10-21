include_directories("${PROJECT_BINARY_DIR}/include/")
include_directories("${PROJECT_SOURCE_DIR}/src")
link_directories(/usr/local/lib/)

IF(UNIX AND NOT APPLE)
  link_directories(/usr/lib/x86_64-linux-gnu/)
  link_directories(/lib/x86_64-linux-gnu/)
  link_directories(/usr/lib/x86_64-linux-gnu/)
  include_directories(/usr/include/OGRE)
ENDIF(UNIX AND NOT APPLE)

IF(UNIX OR APPLE OR MINGW OR MSYS)
  include_directories(/usr/local/include)
  include_directories(/usr/include)
  link_directories(/usr/local/lib)
  link_directories(/usr/lib)
ENDIF(UNIX OR APPLE OR MINGW OR MSYS)


