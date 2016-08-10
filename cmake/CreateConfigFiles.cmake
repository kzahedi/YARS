set(include_string "")

macro(GET_HEADER_FILES dir)
  file(GLOB_RECURSE files ${dir})

  foreach(header ${files})
    string(REPLACE "${CMAKE_SOURCE_DIR}/src" "" file_name ${header})
    set(include_string "${include_string}\n#include <yars${file_name}>")
  endforeach(header ${files})
endmacro(GET_HEADER_FILES dir)

GET_HEADER_FILES("${CMAKE_SOURCE_DIR}/src/configuration/*.h")
GET_HEADER_FILES("${CMAKE_SOURCE_DIR}/src/rc/*.hpp")

configure_file(${CMAKE_SOURCE_DIR}/src/cfg/yars.h.in ${PROJECT_BINARY_DIR}/include/yars/yars.h)

if(OGRE_RESOURCES_DIRECTORIES MATCHES "Ogre-Resources-Dirs-NOTDEFINED")
  set(OGRE_RESOURCES_DIRECTORIES "")
  LIST(APPEND OGRE_RESOURCES_DIRECTORIES "FileSystem=${CMAKE_SOURCE_DIR}/materials")
  LIST(APPEND OGRE_RESOURCES_DIRECTORIES "FileSystem=${CMAKE_SOURCE_DIR}/fonts")
  LIST(APPEND OGRE_RESOURCES_DIRECTORIES "FileSystem=${CMAKE_SOURCE_DIR}/meshes")
  LIST(APPEND OGRE_RESOURCES_DIRECTORIES "FileSystem=${CMAKE_SOURCE_DIR}/particles")
  LIST(APPEND OGRE_RESOURCES_DIRECTORIES "FileSystem=${CMAKE_SOURCE_DIR}/shaders")
  LIST(APPEND OGRE_RESOURCES_DIRECTORIES "FileSystem=${CMAKE_SOURCE_DIR}/shaders/programs")
endif(OGRE_RESOURCES_DIRECTORIES MATCHES "Ogre-Resources-Dirs-NOTDEFINED")

set(tmp ${OGRE_RESOURCES_DIRECTORIES})
set(OGRE_RESOURCES_DIRECTORIES "")
foreach(var ${tmp})
  set(OGRE_RESOURCES_DIRECTORIES "${OGRE_RESOURCES_DIRECTORIES}${var}\n")
endforeach(var)

if(OGRE_PLUGINS_DIR MATCHES "Ogre-Plugins-Dir-NOTDEFINED")
  set(OGRE_PLUGINS_DIR "$ENV{HOME}/.local/lib/")
endif(OGRE_PLUGINS_DIR MATCHES "Ogre-Plugins-Dir-NOTDEFINED")

configure_file(${CMAKE_SOURCE_DIR}/src/cfg/plugins.cfg.in   ${PROJECT_BINARY_DIR}/plugins.cfg)
configure_file(${CMAKE_SOURCE_DIR}/src/cfg/ogre.cfg.in      ${PROJECT_BINARY_DIR}/ogre.cfg)
configure_file(${CMAKE_SOURCE_DIR}/src/cfg/resources.cfg.in ${PROJECT_BINARY_DIR}/resources.cfg)

set(VERSION    "0.7.8")
set(INC_PREFIX "${CMAKE_INSTALL_PREFIX}/include/yars")
set(LIB_PREFIX "${CMAKE_INSTALL_PREFIX}/lib")
set(BIN_PREFIX "${CMAKE_INSTALL_PREFIX}/bin")
configure_file(${CMAKE_SOURCE_DIR}/src/cfg/yars-config ${PROJECT_BINARY_DIR}/bin/yars-config)


