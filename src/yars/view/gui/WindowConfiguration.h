#ifndef __WINDOW_CONFIGURATION_H__
#define __WINDOW_CONFIGURATION_H__

#include <yars/types/P3D.h>
#include <yars/types/Pose.h>
#include <yars/types/ScreenGeometry.h>
#include <yars/types/Colour.h>

#include <string>
#include <vector>

using namespace std;

class WindowConfiguration
{

  public:
    WindowConfiguration(int index);
    void getNextCaptureName();

    P3D            cameraPosition;
    P3D            cameraLookAt;
    P3D            cameraUp;
    ScreenGeometry geometry;
    bool           followObjects;
    bool           useTraces;
    bool           useFollow;
    bool           useShadows;
    bool           visualiseAxes;
    bool           onScreenDisplay;
    bool           osdElapsedTime;
    bool           osdFramePerSecond;
    bool           osdRobotInformation;
    bool           followModeChanged;
    int            followMode;
    string         osdElapsedTimeFont;
    string         osdFramePerSecondFont;
    string         name;
    string         captureName;
    string         orgCaptureName;
    vector<string> camNames;
    int            captureIndex;
    int            index;

    string         fontRobotName;
    int            fontRobotSize;
    Colour         fontRobotColour;

    string         fontTimeName;
    int            fontTimeSize;
    Colour         fontTimeColour;

};

#endif // __WINDOW_CONFIGURATION_H__


