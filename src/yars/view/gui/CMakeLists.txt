# set(GUI_SOURCES SdlWindow.cpp
                # SceneGraph.cpp
                # SceneGraphRobotNode.cpp
                # SceneGraphObjectFactory.cpp
                # SceneGraphCapsuleNode.cpp
                # SceneGraphCylinderNode.cpp
                # SceneGraphSphereNode.cpp
                # SceneGraphLightSourceNode.cpp
                # SceneGraphObjectNode.cpp
                # SceneGraphBoxNode.cpp
                # SceneGraphEnvironmentNode.cpp
                # SceneGraphTraceLineObject.cpp
                # SceneGraphProximitySensor.cpp
                # SceneGraphLDRSensor.cpp
                # SceneGraphJointAxisVisualisationNode.cpp
                # SceneGraphSoftPlyNode.cpp
                # TextOverlay.cpp
                # ColoredTextAreaOverlayElement.cpp
                # OgreHandler.cpp
                # KeyHandler.cpp
                # FollowCamera.cpp
                # CameraFactory.cpp
                # OffsetCam.cpp
                # CameraHandler.cpp
                # OrbitCam.cpp
                # CenterOrbitCam.cpp
                # TripodCam.cpp
                # WindowConfiguration.cpp
                # SceneGraphMeshNode.cpp
                # SceneGraphPlyNode.cpp)

# if(APPLE)
  # add_library(YarsGUI ${LIBRARY_TYPE} ${GUI_SOURCES} OSX_wrap.mm)
# else(APPLE)
  # add_library(YarsGUI ${LIBRARY_TYPE} ${GUI_SOURCES})
# endif(APPLE)

set(GUI_HDRS
  QtWindowHandler.h
  QtGlPanel.h
  WindowConfigurationDialog.h)

QT5_WRAP_CPP(qtview_MOC_SRCS ${GUI_HDRS})

set(GUI_SOURCES
  QtWindowHandler.cpp
  QtGlPanel.cpp
  SceneGraph.cpp
  SceneGraphRobotNode.cpp
  SceneGraphObjectFactory.cpp
  SceneGraphCapsuleNode.cpp
  SceneGraphCylinderNode.cpp
  SceneGraphSphereNode.cpp
  SceneGraphLightSourceNode.cpp
  SceneGraphObjectNode.cpp
  SceneGraphBoxNode.cpp
  SceneGraphEnvironmentNode.cpp
  SceneGraphTraceLineObject.cpp
  SceneGraphProximitySensor.cpp
  SceneGraphLDRSensor.cpp
  SceneGraphJointAxisVisualisationNode.cpp
  SceneGraphSoftPlyNode.cpp
  TextOverlay.cpp
  ColoredTextAreaOverlayElement.cpp
  OgreHandler.cpp
  KeyHandler.cpp
  FollowCamera.cpp
  CameraFactory.cpp
  OffsetCam.cpp
  CameraHandler.cpp
  OrbitCam.cpp
  CenterOrbitCam.cpp
  TripodCam.cpp
  WindowConfiguration.cpp
  WindowConfigurationDialog.cpp
  SceneGraphMeshNode.cpp
  SceneGraphPlyNode.cpp)

add_library(YarsGUI ${LIBRARY_TYPE} ${GUI_SOURCES} ${qtview_MOC_SRCS})
qt5_use_modules(YarsGUI Core Gui Widgets OpenGL)

if(APPLE)
  # target_link_libraries(YarsGUI ${SDL2_LIBRARY})
else(APPLE)
  target_link_libraries(YarsGUI 
    Xrandr Plugin_ParticleFXStatic
    OgreMeshLodGeneratorStatic
    Plugin_BSPSceneManagerStatic OgreOverlayStatic
    OgrePagingStatic Plugin_OctreeSceneManagerStatic OgrePropertyStatic
    Plugin_OctreeZoneStatic OgreRTShaderSystemStatic
    Plugin_PCZSceneManagerStatic OgreTerrainStatic OgreVolumeStatic
    RenderSystem_GLStatic OgreMainStatic
    X11 GL z Xt pthread Xaw GLU
    zzip freeimage freetype quicktime)
endif(APPLE)

