#########################################################################
#                                                                       #
# This file is part of Yet Another Robot Simulator (YARS).              #
# Copyright (C) 2003-2015 Keyan Ghazi-Zahedi.                           #
# All rights reserved.                                                  #
# Email: keyan.zahedi@googlemail.com                                    #
# Web: https://github.com/kzahedi/YARS                                  #
#                                                                       #
# For a list of contributors see the file AUTHORS.                      #
#                                                                       #
# YARS is free software; you can redistribute it and/or modify it under #
# the terms of the GNU General Public License as published by the Free  #
# Software Foundation; either version 2 of the License, or (at your     #
# option) any later version.                                            #
#                                                                       #
# YARS is distributed in the hope that it will be useful, but WITHOUT   #
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or #
# FITNESS FOR A PARTICULAR PURPOSE.                                     #
#                                                                       #
# You should have received a copy of the GNU General Public License     #
# along with YARS in the file COPYING; if not, write to the Free        #
# Software Foundation, Inc., 51 Franklin St, Fifth Floor,               #
# Boston, MA 02110-1301, USA                                            #
#                                                                       #
#########################################################################


set(GUI_SOURCES SdlWindow.cpp
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
                WindowConfiguration.cpp)

  set(GUI_SOURCES ${GUI_SOURCES} SceneGraphMeshNode.cpp SceneGraphPlyNode.cpp)

if(APPLE)
  add_library(YarsGUI ${LIBRARY_TYPE} ${GUI_SOURCES} OSX_wrap.mm)
else(APPLE)
  add_library(YarsGUI ${LIBRARY_TYPE} ${GUI_SOURCES})
endif(APPLE)

if(APPLE)
  target_link_libraries(YarsGUI ${SDL2_LIBRARY})
else(APPLE)
  target_link_libraries(YarsGUI ${SDL2_LIBRARY}
    Xrandr Plugin_ParticleFXStatic
    OgreMeshLodGeneratorStatic
    Plugin_BSPSceneManagerStatic OgreOverlayStatic
    OgrePagingStatic Plugin_OctreeSceneManagerStatic OgrePropertyStatic
    Plugin_OctreeZoneStatic OgreRTShaderSystemStatic
    Plugin_PCZSceneManagerStatic OgreTerrainStatic OgreVolumeStatic
    RenderSystem_GLStatic OgreMainStatic
      X11 GL z Xt pthread Xaw GLU
      zzip freeimage freetype GLEW quicktime)

endif(APPLE)
