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



#if(APPLE)
#add_executable(${YARS_TARGET} MACOSX_BUNDLE yarsMain.cpp)
#else(APPLE)
  add_executable(${YARS_TARGET} yarsMain.cpp)
#endif(APPLE)

add_subdirectory(types)
add_subdirectory(configuration)
add_subdirectory(view/console)
add_subdirectory(physics)
add_subdirectory(util)
add_subdirectory(main)
add_subdirectory(logging)
add_subdirectory(rc)


include_directories(.)

INSTALL(TARGETS ${YARS_TARGET}
  RUNTIME DESTINATION bin
  CONFIGURATIONS Release Debug)


target_link_libraries(${YARS_TARGET} ${XERCESC_LIBRARY_FILE}) # ${Boost_LIBRARIES}) # ${BULLET_LIBRARIES})

target_link_libraries(${YARS_TARGET} m stdc++ pthread dl)

IF(WINDOWS OR MINGW)
  target_link_libraries(${YARS_TARGET} ${ODE_LIBRARY_FILE} Ws2_32.lib)
ENDIF(WINDOWS OR MINGW)

if(YARS_USE_CAPTURE_VIDEO)
  target_link_libraries(${YARS_TARGET} quicktime)
endif(YARS_USE_CAPTURE_VIDEO)

if(YARS_USE_VISUALISATION)
  add_subdirectory(view)
  target_link_libraries(${YARS_TARGET} YarsView)

  link_directories("/usr/local/lib/")
  if(UNIX AND NOT APPLE)
    link_directories(/usr/local/lib /usr/local/lib/OGRE)
    target_link_libraries(${YARS_TARGET}
      X11 GL z Xt pthread ${OGRE_LIBRARIES}
      zzip freeimage freetype quicktime)
      # zzip freeimage freetype GLEW quicktime)

  else(UNIX AND NOT APPLE)
    target_link_libraries(${YARS_TARGET} zzip freeimage freetype)
      # zzip freeimage freetype GLEW)
    # Plugin_CgProgramManagerStatic
  endif(UNIX AND NOT APPLE)

  # message("Ogre: ${OGRE_LIBRARIES}")
  # target_link_libraries(${YARS_TARGET} ${OGRE_LIBRARIES} zzip freeimage freetype GLEW)

  if(APPLE)
    SET_TARGET_PROPERTIES(${YARS_TARGET} PROPERTIES LINK_FLAGS
      "-framework OgreOverlay -framework RenderSystem_GL -framework Plugin_ParticleFX -framework Ogre")
  endif(APPLE)

endif(YARS_USE_VISUALISATION)

target_link_libraries(${YARS_TARGET} YarsMain)
target_link_libraries(${YARS_TARGET} YarsConfiguration)
target_link_libraries(${YARS_TARGET} YarsPhysics)
target_link_libraries(${YARS_TARGET} YarsUtil)
target_link_libraries(${YARS_TARGET} YarsLogging)
target_link_libraries(${YARS_TARGET} YarsViewConsole)
target_link_libraries(${YARS_TARGET} YarsDataTypes)
target_link_libraries(${YARS_TARGET} boost_program_options)
