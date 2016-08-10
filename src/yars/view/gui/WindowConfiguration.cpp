/*************************************************************************
 *                                                                       *
 * This file is part of Yet Another Robot Simulator (YARS).              *
 * Copyright (C) 2003-2006 Keyan Zahedi, Arndt von Twickel.              *
 * All rights reserved.                                                  *
 * Email: {keyan,twickel}@users.sourceforge.net                          *
 * Web: http://sourceforge.net/projects/yars                             *
 *                                                                       *
 * For a list of contributors see the file AUTHORS.                      *
 *                                                                       *
 * YARS is free software; you can redistribute it and/or modify it under *
 * the terms of the GNU General Public License as published by the Free  *
 * Software Foundation; either version 2 of the License, or (at your     *
 * option) any later version.                                            *
 *                                                                       *
 * YARS is distributed in the hope that it will be useful, but WITHOUT   *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or *
 * FITNESS FOR A PARTICULAR PURPOSE.                                     *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with YARS in the file COPYING; if not, write to the Free        *
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor,               *
 * Boston, MA 02110-1301, USA                                            *
 *                                                                       *
 *************************************************************************/
 

#include "WindowConfiguration.h"

#include "configuration/YarsConfiguration.h"

#include <yars/view/console/ConsoleView.h>

#include <sstream>


using namespace std;

WindowConfiguration::WindowConfiguration(int _i)
  : osdElapsedTimeFont("Helvetica", 15),
    osdFramePerSecondFont("Helvetica", 15)
{
  this->index       = _i;
  name              = __YARS_GET_VIEW_NAME(index);
  geometry          = __YARS_GET_VIEW_GEOMETRY(index);
  // cameraPose        = __YARS_GET_VIEW_POSE(index);
  // captureName       = __YARS_GET_CAPTURE_NAME;
  // useTextures       = __YARS_GET_USE_TEXTURES;
  // useTraces         = __YARS_GET_USE_TRACES;
  // followMode        = __YARS_GET_FOLLOW_MODE;
  // followObjects     = __YARS_GET_USE_FOLLOW_MODE;
  // onScreenDisplay   = __YARS_GET_USE_OSD;
  orgCaptureName    = captureName;
  visualiseAxes     = true;
  followModeChanged = false;
  osdFramePerSecond = true;
  osdElapsedTime    = true;
  // useShadows        = __YARS_GET_USE_SHADOWS;
  maxTraceLines     = -1;
  maxTracePoints    = -1;
  if(index > 0)
  {
    string ext = captureName.substr(captureName.find_last_of("."), captureName.size() - 1);
    captureName = captureName.substr(0, captureName.find_last_of("."));
    stringstream oss;
    oss << captureName << "-" << index << ext;
    captureName = oss.str();
    orgCaptureName = captureName;
  }
  captureIndex     = 0;

  // TODO TODO overwrite values by DataScreen-values
}

void WindowConfiguration::getNextCaptureName()
{
  captureIndex++;
  string ext  = orgCaptureName.substr(orgCaptureName.find_last_of("."), orgCaptureName.size() - 1);
  captureName = orgCaptureName.substr(0, orgCaptureName.find_last_of("."));
  stringstream oss;
  oss << captureIndex;
  string index = oss.str();
  int l = index.length();
  oss.str("");
  for(int i = 0; i < 4-l; i++)
  {
    index = "0" + index;
  }
  oss << captureName << "-" << index << ext;
  captureName = oss.str();
  cout << "captureName: " << captureName << endl;
}
