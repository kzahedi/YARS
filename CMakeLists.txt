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



project(yars)

# check for cmake version first:
cmake_minimum_required(VERSION 2.4.3 FATAL_ERROR)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0005 OLD)
  cmake_policy(SET CMP0003 NEW)
  cmake_policy(SET CMP0006 OLD)
  cmake_policy(SET CMP0007 OLD) # list does not count empty elements
  cmake_policy(SET CMP0002 OLD)
  cmake_policy(SET CMP0042 OLD)
  cmake_policy(SET CMP0009 NEW) # don't follow symlink for file(GLOB_RECURSE
endif(COMMAND cmake_policy)


# Set Version Information
# TODO: read this from defines/version.h
set(YARS_MAJOR 0)
set(YARS_MINOR 7)
set(YARS_PATCH 28)

INCLUDE(modules/IncludePackages.cmake) # must be called first
INCLUDE(modules/SetOptions.cmake)
INCLUDE(modules/SetDirectories.cmake)
INCLUDE(modules/SetCompilerSettings.cmake)
INCLUDE(modules/SetDefinitions.cmake)
INCLUDE(modules/PrintConfiguration.cmake)
INCLUDE(modules/CreateConfigFiles.cmake)

if(YARS_DOXYGEN_DOCUMENTATION)
  add_subdirectory(doxygen)
endif(YARS_DOXYGEN_DOCUMENTATION)
add_subdirectory(src/yars)
add_subdirectory(contrib)

IF(YARS_TESTCASES)
  ENABLE_TESTING()
  add_subdirectory(testCases)
ENDIF(YARS_TESTCASES)

IF(APPLE)
  INCLUDE(modules/InstallConfiguration.cmake)
ENDIF(APPLE)
INCLUDE(modules/CPackConfiguration.cmake)
INCLUDE(modules/UninstallConfiguration.cmake)
INCLUDE(modules/CreateSymbolicLinks.cmake)
