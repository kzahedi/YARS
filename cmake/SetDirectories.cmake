include_directories(${PROJECT_BINARY_DIR}/include/)
include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(/usr/include)
include_directories(/usr/local/include)
link_directories(/usr/lib)
link_directories(/usr/local/lib)

IF(UNIX AND NOT APPLE)
  link_directories(/usr/lib/x86_64-linux-gnu/)
  include_directories(/usr/include/OGRE)
ENDIF(UNIX AND NOT APPLE)

include_directories(/Library/Frameworks/Ogre.framework/Versions/1.11.3/Headers)
include_directories(/Library/Frameworks/OgreOverlay.framework/Versions/1.11.3/Headers)
include_directories(/Library/Frameworks/RenderSystem_GL.framework/Versions/1.11.3/Headers/)
include_directories(/Library/Frameworks/Plugin_ParticleFX.framework/Versions/1.11.3/Headers/)


