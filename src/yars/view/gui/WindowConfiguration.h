#ifndef __WINDOW_CONFIGURATION_H__
#define __WINDOW_CONFIGURATION_H__

#include <yars/types/P3D.h>
#include <yars/types/Pose.h>
#include <yars/types/ScreenGeometry.h>

#include <string>
#include <vector>

// #include <QFont>

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
    // QFont          osdElapsedTimeFont;
    // QFont          osdFramePerSecondFont;
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


