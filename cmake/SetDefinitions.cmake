## TODO make optional
add_definitions(-D${YARS_LOCALE})

add_definitions(-DYARS_VERSION='"${YARS_MAJOR}.${YARS_MINOR}.${YARS_PATCH}"')

add_definitions(-DYARS_COMPILED_OS='"${CMAKE_SYSTEM}"')

add_definitions(-DYARS_COMPILER_VERSION='"${CMAKE_CXX_COMPILER}"')

add_definitions(-DYARS_INSTALL_PATH='"${CMAKE_INSTALL_PREFIX}"')

add_definitions(-DBOOST_VERSION='${Boost_VERSION}')

if(YARS_EXAMPLES)
  add_definitions(-DWITH_EXAMPLES)
endif(YARS_EXAMPLES)

if(YARS_USE_VISUALISATION)
  add_definitions(-DUSE_VISUALISATION)
ENDif(YARS_USE_VISUALISATION)

if(YARS_USE_CAPTURE_VIDEO)
  add_definitions(-DUSE_CAPTURE_VIDEO)
ENDif(YARS_USE_CAPTURE_VIDEO)

if(YARS_USE_GL_CALL_LIST_WORKAROUND)
  add_definitions(-DUSE_glCallList_WORKAROUND)
endif(YARS_USE_GL_CALL_LIST_WORKAROUND)

add_definitions(-DYARS_NAME="${YARS_TARTGET}")

if(YARS_USE_OPEN_SG)
  add_definitions(-DUSE_OPEN_SG)
endif(YARS_USE_OPEN_SG)

IF(YARS_SUPPRESS_OUTPUT)
  add_definitions(-DSUPPRESS_ALL_OUTPUT)
endif(YARS_SUPPRESS_OUTPUT)

IF(YARS_USE_SOFT_BODIES)
  add_definitions(-DUSE_SOFT_BODIES)
endif(YARS_USE_SOFT_BODIES)

IF(YARS_USE_OGRE)
  add_definitions(-DUSE_OGRE)
endif(YARS_USE_OGRE)

IF(YARS_USE_SHADOW_MAPS)
  add_definitions(-DUSE_SHADOW_MAPS)
endif(YARS_USE_SHADOW_MAPS)

IF(MINGW)
  add_definitions(-DXML_LIBRARY)
ENDIF(MINGW)

