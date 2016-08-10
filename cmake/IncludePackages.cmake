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

IF(YARS_USE_VISUALISATION)
  find_package(SDL2)
  include_directories(${SDL2_INCLUDE_DIR})

  find_package(Ogre)
  # find_package(OGRE REQUIRED Overlay Paging Terrain Volume Plugin_BSPSceneManager
    # Plugin_CgProgramManager Plugin_OctreeSceneManager Plugin_OctreeZone
    # Plugin_PCZSceneManager Plugin_ParticleFX RenderSystem_GL)
  # include_directories("/Library/Frameworks/Ogre.framework/Versions/1.10.0/Headers")
  # include_directories("/Library/Frameworks/OgreOverlay.framework/Versions/1.10.0/Headers")
  include_directories(/usr/local/include/OGRE/)
  link_directories(/usr/local/lib/)
  link_directories(/usr/local/lib/OGRE)

  find_package(Qt5Widgets)
  set(CMAKE_AUTOMOC OFF)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)

  # Add the include directories for the Qt 5 Widgets module to
  # the compile lines.
  # include_directories(${Qt5Widgets_INCLUDE_DIRS})

  # Use the compile definitions defined in the Qt 5 Widgets module
  # add_definitions(${Qt5Widgets_DEFINITIONS})

  # Add compiler flags for building executables (-fPIE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

  # find_package(Qt5 5.6.1)
  # if(NOT QT5_FOUND)
    # message("Qt5 not found")
  # endif(NOT QT5_FOUND)
  # include_directories(${QT_INCLUDE_DIR})
  # link_libraries(${QT_LIBRARIES})

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
ENDIF(YARS_USE_VISUALISATION)

if(NOT BULLET_FOUND)
  set(stop TRUE)
endif(NOT BULLET_FOUND)
