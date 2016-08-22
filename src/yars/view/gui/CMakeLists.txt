set(GUI_HDRS
  QtOgreWindow.h
  WindowConfigurationDialog.h)

QT5_WRAP_CPP(qtview_MOC_SRCS ${GUI_HDRS})

set(GUI_SOURCES
  QtOgreWindow.cpp
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
  GuiMutex.cpp
  ColoredTextAreaOverlayElement.cpp
  SceneGraphHandler.cpp
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


