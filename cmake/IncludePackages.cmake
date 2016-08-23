set(STOP False)

# includes
include(CMakeDetermineCXXCompiler)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED program_options filesystem date_time system thread)
IF(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
ENDIF(Boost_FOUND)

#find_package(Subversion)

find_package(XercesC)
IF(XERCESC_FOUND)
  include_directories(${XERCESC_INCLUDE_DIR})
ENDIF(XERCESC_FOUND)

find_package(PythonLibs)
IF(PYTHONLIBS_FOUND)
  include_directories(${PYTHON_INCLUDE_PATH})
ENDIF(PYTHONLIBS_FOUND)

find_package(Bullet)
if(BULLET_FOUND)
  include_directories(${BULLET_INCLUDE_DIR})
endif(BULLET_FOUND)

if(YARS_DOCS)
  find_package(LATEX)
endif(YARS_DOCS)

IF(YARS_USE_VISUALISATION)

  find_package(SDL2)
  include_directories(${SDL2_INCLUDE_DIR})

  find_package(OGRE REQUIRED Overlay Plugin_ParticleFX RenderSystem_GL)

  # find_package(OGRE REQUIRED Overlay Paging Terrain Volume Plugin_BSPSceneManager
    # Plugin_CgProgramManager Plugin_OctreeSceneManager Plugin_OctreeZone
    # Plugin_PCZSceneManager Plugin_ParticleFX RenderSystem_GL)
  # include_directories("/Library/Frameworks/Ogre.framework/Versions/1.10.0/Headers")
  # include_directories("/Library/Frameworks/OgreOverlay.framework/Versions/1.10.0/Headers")

  # find_package(OGRE REQUIRED Overlay Paging Terrain Volume Plugin_BSPSceneManager
    # Plugin_CgProgramManager Plugin_OctreeSceneManager Plugin_OctreeZone
    # Plugin_PCZSceneManager Plugin_ParticleFX RenderSystem_GL)
  # include_directories("/Library/Frameworks/Ogre.framework/Versions/1.10.0/Headers")
  # include_directories("/Library/Frameworks/OgreOverlay.framework/Versions/1.10.0/Headers")
  # message(${OGRE_Overlay_INCLUDE_DIRS}/OgreOverlay)

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
endif(NOT Boost_FOUND)

if(NOT XERCESC_FOUND)
  set(stop TRUE)
endif(NOT XERCESC_FOUND)

IF(YARS_USE_VISUALISATION)
  if(NOT SDL2_FOUND)
    set(stop TRUE)
  endif(NOT SDL2_FOUND)
  # if(NOT OGRE_FOUND)
    # set(stop TRUE)
    # MESSAGE("\nOGRE not found")
  # endif(NOT OGRE_FOUND)
ENDIF(YARS_USE_VISUALISATION)

if(NOT BULLET_FOUND)
  set(stop TRUE)
endif(NOT BULLET_FOUND)
