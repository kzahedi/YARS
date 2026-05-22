# Ogre 14 headers come from ${OGRE_ROOT}/include/OGRE/... on both platforms;
# the legacy macOS framework include lines are obsolete.

set(GUI_SOURCES SdlWindow.cpp
                NativeOgreWindow.cpp
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
                ShadowMapper.cpp
                MaterialManager.cpp
                ShaderManager.cpp
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
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(FFMPEG REQUIRED libavformat libavcodec libavutil libavfilter libswscale libavdevice libswresample)
endif(YARS_USE_CAPTURE_VIDEO)

set(GUI_SOURCES ${GUI_SOURCES} SceneGraphMeshNode.cpp SceneGraphPlyNode.cpp)

if(APPLE)
  add_library(YarsGUI ${LIBRARY_TYPE} ${GUI_SOURCES} OSX_wrap.mm)
else(APPLE)
  add_library(YarsGUI ${LIBRARY_TYPE} ${GUI_SOURCES})
endif(APPLE)

if(YARS_USE_CAPTURE_VIDEO)
  target_include_directories(YarsGUI PRIVATE ${FFMPEG_INCLUDE_DIRS})
endif(YARS_USE_CAPTURE_VIDEO)


