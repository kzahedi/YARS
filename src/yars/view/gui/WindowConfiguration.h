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
 

#ifndef __WINDOW_CONFIGURATION_H__
#define __WINDOW_CONFIGURATION_H__

#include <yars/types/P3D.h>
#include <yars/types/Pose.h>
#include <yars/types/ScreenGeometry.h>

#include <string>
#include <vector>

#include <QFont>

using namespace std;

class WindowConfiguration
{

  public:
    WindowConfiguration(int index);
    void getNextCaptureName();

    Pose           cameraPose;
    ScreenGeometry geometry;
    bool           followObjects;
    bool           useTraces;
    bool           useTextures;
    bool           useShadows;
    bool           visualiseAxes;
    bool           onScreenDisplay;
    bool           osdElapsedTime;
    bool           osdFramePerSecond;
    bool           followModeChanged;
    int            followMode;
    QFont          osdElapsedTimeFont;
    QFont          osdFramePerSecondFont;
    string         name;
    string         captureName;
    string         orgCaptureName;
    vector<string> camNames;
    int            captureIndex;
    int            maxTraceLines;
    int            maxTracePoints;
    int            index;
};

#endif // __WINDOW_CONFIGURATION_H__


