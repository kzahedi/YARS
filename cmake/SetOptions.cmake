if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
    "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
    FORCE)
endif(NOT CMAKE_BUILD_TYPE)

SET(YARS_TARGET "yars" CACHE STRING
  "YARS binary name.")

SET(OGRE_PLUGINS_DIR "Ogre-Plugins-Dir-NOTDEFINED" CACHE STRING
  "Where to find the RenderSystem and Plugin Libraries.")

SET(OGRE_RESOURCES_DIRECTORIES "Ogre-Resources-Dirs-NOTDEFINED" CACHE STRING
  "Where to find the resource files.")

OPTION(CMAKE_VERBOSE
  "Printout cmake variables"
  OFF)

OPTION(YARS_USE_SOFT_BODIES
  "YARS use soft bodies"
  OFF)

OPTION(YARS_SUPPRESS_OUTPUT
  "Suppressing all output to console."
  OFF)

OPTION(YARS_TESTCASES
  "Building yars with test cases."
  OFF)

OPTION(YARS_TOOLS
  "Building yars tools."
  OFF)

OPTION(YARS_BUILD_JAVA_CLIENT
  "Building java client."
  OFF)

OPTION(YARS_BUILD_CPP_CLIENT
  "Building cpp client."
  ON)

OPTION(YARS_USE_VISUALISATION
  "Use visualisation."
  ON)

OPTION(YARS_USE_CAPTURE_VIDEO
  "Use capture video option."
  OFF)

OPTION(YARS_DOXYGEN_DOCUMENTATION
  "Build Doxygen documentation."
  OFF)

OPTION(YARS_USE_SHADOW_MAPS
  "Use shadow maps."
  OFF)

set(YARS_SET_LOCALE_ENGLISH ON)

MARK_AS_ADVANCED(
  BIBTEX_COMPILER
  CMAKE_BACKWARDS_COMPATIBILITY
  CMAKE_Java_ARCHIVE
  CMAKE_OSX_ARCHITECTURES
  CMAKE_OSX_SYSROOT
  CMAKE_VERBOSE
  DOXYFILE_IN
  DVIPS_CONVERTER
  EXECUTABLE_OUTPUT_PATH
  IMAGEMAGICK_CONVERT
  LATEX2HTML_CONVERTER
  LATEX_COMPILER
  LATEX_OUTPUT_PATH
  LATEX_SMALL_IMAGES
  LIBRARY_OUTPUT_PATH
  MAKEINDEX_COMPILER
  OGRE_PLUGINS_DIR
  OGRE_RESOURCES_DIRECTORIES
  PDFLATEX_COMPILER
  PS2PDF_CONVERTER
  XERCESC_LIBRARY_FILE
  XERCESC_ROOT_DIR
  YARS_TESTCASES
  YARS_SET_LOCALE_DEUTSCH
  YARS_SET_LOCALE_ENGLISH

  OGRE_Overlay_FOUND
  OGRE_Overlay_INCLUDE_DIR
  OGRE_Overlay_LIBRARY_DBG
  OGRE_Overlay_LIBRARY_REL
  OGRE_Overlay_LIBRARY_FWK

  OGRE_Paging_FOUND
  OGRE_Paging_INCLUDE_DIR
  OGRE_Paging_LIBRARY_DBG
  OGRE_Paging_LIBRARY_REL
  OGRE_Paging_LIBRARY_FWK

  OGRE_Plugin_BSPSceneManager_FOUND
  OGRE_Plugin_BSPSceneManager_INCLUDE_DIR
  OGRE_Plugin_BSPSceneManager_LIBRARY_DBG
  OGRE_Plugin_BSPSceneManager_LIBRARY_REL
  OGRE_Plugin_BSPSceneManager_LIBRARY_FWK

  OGRE_Plugin_CgProgramManager_FOUND
  OGRE_Plugin_CgProgramManager_INCLUDE_DIR
  OGRE_Plugin_CgProgramManager_LIBRARY_DBG
  OGRE_Plugin_CgProgramManager_LIBRARY_REL
  OGRE_Plugin_CgProgramManager_LIBRARY_FWK

  OGRE_Plugin_OctreeSceneManager_FOUND
  OGRE_Plugin_OctreeSceneManager_INCLUDE_DIR
  OGRE_Plugin_OctreeSceneManager_LIBRARY_DBG
  OGRE_Plugin_OctreeSceneManager_LIBRARY_REL
  OGRE_Plugin_OctreeSceneManager_LIBRARY_FWK

  OGRE_Plugin_OctreeSceneManager_FOUND
  OGRE_Plugin_OctreeSceneManager_INCLUDE_DIR
  OGRE_Plugin_OctreeSceneManager_LIBRARY_DBG
  OGRE_Plugin_OctreeSceneManager_LIBRARY_REL
  OGRE_Plugin_OctreeSceneManager_LIBRARY_FWK

  OGRE_Plugin_OctreeZone_FOUND
  OGRE_Plugin_OctreeZone_INCLUDE_DIR
  OGRE_Plugin_OctreeZone_LIBRARY_DBG
  OGRE_Plugin_OctreeZone_LIBRARY_REL
  OGRE_Plugin_OctreeZone_LIBRARY_FWK

  OGRE_Plugin_OctreeZone_FOUND
  OGRE_Plugin_OctreeZone_INCLUDE_DIR
  OGRE_Plugin_OctreeZone_LIBRARY_DBG
  OGRE_Plugin_OctreeZone_LIBRARY_REL
  OGRE_Plugin_OctreeZone_LIBRARY_FWK

  OGRE_Plugin_PCZSceneManager_FOUND
  OGRE_Plugin_PCZSceneManager_INCLUDE_DIR
  OGRE_Plugin_PCZSceneManager_LIBRARY_DBG
  OGRE_Plugin_PCZSceneManager_LIBRARY_REL
  OGRE_Plugin_PCZSceneManager_LIBRARY_FWK

  OGRE_Plugin_PCZSceneManager_FOUND
  OGRE_Plugin_PCZSceneManager_INCLUDE_DIR
  OGRE_Plugin_PCZSceneManager_LIBRARY_DBG
  OGRE_Plugin_PCZSceneManager_LIBRARY_REL
  OGRE_Plugin_PCZSceneManager_LIBRARY_FWK

  OGRE_Plugin_ParticleFX_FOUND
  OGRE_Plugin_ParticleFX_INCLUDE_DIR
  OGRE_Plugin_ParticleFX_LIBRARY_DBG
  OGRE_Plugin_ParticleFX_LIBRARY_REL
  OGRE_Plugin_ParticleFX_LIBRARY_FWK

  OGRE_RenderSystem_Direct3D11_FOUND
  OGRE_RenderSystem_Direct3D11_INCLUDE_DIR
  OGRE_RenderSystem_Direct3D11_LIBRARY_DBG
  OGRE_RenderSystem_Direct3D11_LIBRARY_REL
  OGRE_RenderSystem_Direct3D11_LIBRARY_FWK

  OGRE_RenderSystem_Direct3D9_FOUND
  OGRE_RenderSystem_Direct3D9_INCLUDE_DIR
  OGRE_RenderSystem_Direct3D9_LIBRARY_DBG
  OGRE_RenderSystem_Direct3D9_LIBRARY_REL
  OGRE_RenderSystem_Direct3D9_LIBRARY_FWK

  OGRE_RenderSystem_GL3Plus_FOUND
  OGRE_RenderSystem_GL3Plus_INCLUDE_DIR
  OGRE_RenderSystem_GL3Plus_LIBRARY_DBG
  OGRE_RenderSystem_GL3Plus_LIBRARY_REL
  OGRE_RenderSystem_GL3Plus_LIBRARY_FWK

  OGRE_RenderSystem_GLES2_FOUND
  OGRE_RenderSystem_GLES2_INCLUDE_DIR
  OGRE_RenderSystem_GLES2_LIBRARY_DBG
  OGRE_RenderSystem_GLES2_LIBRARY_REL
  OGRE_RenderSystem_GLES2_LIBRARY_FWK

  OGRE_RenderSystem_GLES_FOUND
  OGRE_RenderSystem_GLES_INCLUDE_DIR
  OGRE_RenderSystem_GLES_LIBRARY_DBG
  OGRE_RenderSystem_GLES_LIBRARY_REL
  OGRE_RenderSystem_GLES_LIBRARY_FWK

  OGRE_RenderSystem_GL_FOUND
  OGRE_RenderSystem_GL_INCLUDE_DIR
  OGRE_RenderSystem_GL_LIBRARY_DBG
  OGRE_RenderSystem_GL_LIBRARY_REL
  OGRE_RenderSystem_GL_LIBRARY_FWK

  OGRE_Terrain_FOUND
  OGRE_Terrain_INCLUDE_DIR
  OGRE_Terrain_LIBRARY_DBG
  OGRE_Terrain_LIBRARY_REL
  OGRE_Terrain_LIBRARY_FWK

  OGRE_Volume_FOUND
  OGRE_Volume_INCLUDE_DIR
  OGRE_Volume_LIBRARY_DBG
  OGRE_Volume_LIBRARY_REL
  OGRE_Volume_LIBRARY_FWK

  OGRE_FRAMEWORK_PATH

  SDL2MAIN_LIBRARY
  SDL2_INCLUDE_DIR
  SDL2_LIBRARY

  BULLET_INCLUDE_DIR

  )

SET(YARS_THREAD_SAFE OFF)

if(YARS_SET_LOCALE_ENGLISH AND YARS_SET_LOCALE_DEUTSCH)
  message(SEND_ERROR "Please choose only one language")
endif(
  YARS_SET_LOCALE_ENGLISH AND YARS_SET_LOCALE_DEUTSCH)

if(YARS_SET_LOCALE_ENGLISH)
  set(YARS_LOCALE YARS_LOCALE_ENGLISH)
endif(YARS_SET_LOCALE_ENGLISH)

if(YARS_SET_LOCALE_DEUTSCH)
  set(YARS_LOCALE YARS_LOCALE_DEUTSCH)
endif(YARS_SET_LOCALE_DEUTSCH)

IF(WINDOWS)
  IF(NOT DEFINED CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS)
    SET(CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS ON)
  ENDIF(NOT DEFINED CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS_NO_WARNINGS)
ENDIF(WINDOWS)
