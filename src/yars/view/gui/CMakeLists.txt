include_directories(/Library/Frameworks/OgreOverlay.framework/Headers)

set(GUI_SOURCES SdlWindow.cpp
                SceneGraph.cpp
                SceneGraphRobotNode.cpp
                SceneGraphMuscleNode.cpp
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
                GuiMutex.cpp
                CameraFactory.cpp
                OffsetCam.cpp
                CameraMan.cpp
                OrbitCam.cpp
                CenterOrbitCam.cpp
                TripodCam.cpp
                WindowConfiguration.cpp)

if(YARS_USE_CAPTURE_VIDEO)
  set(GUI_SOURCES ${GUI_SOURCES} VideoCapture.cpp)
endif(YARS_USE_CAPTURE_VIDEO)

set(GUI_SOURCES ${GUI_SOURCES} SceneGraphMeshNode.cpp SceneGraphPlyNode.cpp)

if(APPLE)
  add_library(YarsGUI ${LIBRARY_TYPE} ${GUI_SOURCES} OSX_wrap.mm)
else(APPLE)
  add_library(YarsGUI ${LIBRARY_TYPE} ${GUI_SOURCES})
endif(APPLE)


