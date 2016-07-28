set(STOP False)

# includes
include(CMakeDetermineCXXCompiler)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED program_options filesystem date_time system thread)
IF(Boost_FOUND)
  MESSAGE("Boost lib dir     = ${Boost_LIBRARY_DIRS}")
  MESSAGE("Boost include dir = ${Boost_INCLUDE_DIRS}")
  include_directories(${Boost_INCLUDE_DIRS})
ENDIF(Boost_FOUND)

#find_package(Subversion)

find_package(XercesC)
IF(XERCESC_FOUND)
  MESSAGE("Xerces-C libs     = ${XERCESC_LIBRARY_DIR}")
  MESSAGE("Xerces-C includes = ${XERCESC_INCLUDE_DIR}")
  include_directories(${XERCESC_INCLUDE_DIR})
ENDIF(XERCESC_FOUND)

find_package(PythonLibs)
IF(PYTHONLIBS_FOUND)
  MESSAGE("Python libs     = ${PYTHON_LIBRARIES}")
  MESSAGE("Python includes = ${PYTHON_INCLUDE_DIRS}")
  include_directories(${PYTHON_INCLUDE_PATH})
ENDIF(PYTHONLIBS_FOUND)

find_package(Bullet)
if(BULLET_FOUND)
  MESSAGE("Bullet libs       = ${BULLET_LIBRARIES}")
  MESSAGE("Bullet includes   = ${BULLET_INCLUDE_DIRS}")
  include_directories(${BULLET_INCLUDE_DIR})
endif(BULLET_FOUND)

IF(YARS_USE_VISUALISATION)
  find_package(SDL2)
  include_directories(${SDL2_INCLUDE_DIR})
  find_package(OGRE)
  # find_package(OGRE REQUIRED Overlay Paging Terrain Volume Plugin_BSPSceneManager
    # Plugin_CgProgramManager Plugin_OctreeSceneManager Plugin_OctreeZone
    # Plugin_PCZSceneManager Plugin_ParticleFX RenderSystem_GL)
  # include_directories("/Library/Frameworks/Ogre.framework/Versions/1.10.0/Headers")
  # include_directories("/Library/Frameworks/OgreOverlay.framework/Versions/1.10.0/Headers")
  # message(${OGRE_Overlay_INCLUDE_DIRS}/OgreOverlay)
  include_directories(/usr/local/include/OGRE/)
  link_directories(/usr/local/lib/)
  link_directories(/usr/local/lib/OGRE)

  if(UNIX AND NOT APPLE)
    # for ogre
    add_definitions(-pthread -D_FORTIFY_SOURCE=2)
    include_directories(/usr/local/include /usr/local/include/OGRE /usr/include/freetype2 /usr/include/libdrm)
  endif(UNIX AND NOT APPLE)
ENDIF(YARS_USE_VISUALISATION)

if(YARS_TESTCASES)
  find_package(UnitTest++)
  IF(UNITTEST++_FOUND)
    include_directories(${UNITTEST++_INCLUDE_DIR})
    link_directories(${UNITTEST++_LIBRARY_DIR})
  ENDIF(UNITTEST++_FOUND)
endif(YARS_TESTCASES)


if(NOT Boost_FOUND)
  set(stop TRUE)
  MESSAGE("\nBoost not found")
endif(NOT Boost_FOUND)

if(NOT XERCESC_FOUND)
  set(stop TRUE)
  MESSAGE("\nXerces-C not found\n")
endif(NOT XERCESC_FOUND)

IF(YARS_USE_VISUALISATION)
  if(NOT SDL2_FOUND)
    set(stop TRUE)
    MESSAGE("\nSDL2 not found")
  endif(NOT SDL2_FOUND)
  # if(NOT OGRE_FOUND)
    # set(stop TRUE)
    # MESSAGE("\nOGRE not found")
  # endif(NOT OGRE_FOUND)
ENDIF(YARS_USE_VISUALISATION)

if(NOT BULLET_FOUND)
  set(stop TRUE)
  MESSAGE("Bullet not found")
endif(NOT BULLET_FOUND)
