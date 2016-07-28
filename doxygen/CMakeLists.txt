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

find_package(Doxygen)

IF(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/doc)
  FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc)
ENDIF(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/doc)

IF(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/doc/html)
  FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/html)
ENDIF(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/doc/html)

IF(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/doc/latex)
  FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/latex)
ENDIF(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/doc/latex)

include(${CMAKE_SOURCE_DIR}/modules/UseDoxygen.cmake)

# INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/html
#   CONFIGURATIONS Release Debug
#   DESTINATION yars/docs/
#   PATTERN ".svn" EXCLUDE
#   PATTERN "*.txt" EXCLUDE
#   )
