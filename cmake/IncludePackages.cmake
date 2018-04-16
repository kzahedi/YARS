set(STOP False)

# includes
include(CMakeDetermineCXXCompiler)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if(APPLE)
  set(Boost_USE_STATIC_LIBS ON)
endif(APPLE)
find_package(Boost REQUIRED program_options filesystem date_time system)
IF(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
ENDIF(Boost_FOUND)

find_package(XercesC)
IF(XERCESC_FOUND)
  include_directories(${XERCESC_INCLUDE_DIR})
ENDIF(XERCESC_FOUND)

if(YARS_USE_PYTHON)
find_package(PythonLibs 3 REQUIRED)
IF(PYTHONLIBS_FOUND)
  include_directories(${PYTHON_INCLUDE_PATH})
ENDIF(PYTHONLIBS_FOUND)
endif(YARS_USE_PYTHON)

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

  find_package(OGRE REQUIRED OgreOverlay Plugin_ParticleFX RenderSystem_GL)
  include_directories(${OGRE_INCLUDE_DIR})

  if(UNIX AND NOT APPLE)
    add_definitions(-pthread)
    if(CMAKE_BUILD_TYPE MATCHES Release)
      add_definitions(-D_FORTIFY_SOURCE=2)
    endif(CMAKE_BUILD_TYPE MATCHES Release)
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
  if(NOT OGRE_FOUND)
    set(stop TRUE)
    MESSAGE("\nOGRE not found")
  endif(NOT OGRE_FOUND)
ENDIF(YARS_USE_VISUALISATION)

if(NOT BULLET_FOUND)
  set(stop TRUE)
endif(NOT BULLET_FOUND)
