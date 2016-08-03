#ifndef __WINDOW_CONFIGURATION_H__
#define __WINDOW_CONFIGURATION_H__

#include <yars/types/P3D.h>
#include <yars/types/ScreenGeometry.h>
#include <yars/types/Colour.h>

#include <string>
#include <vector>

using namespace std;

class WindowConfiguration
{

  public:
    WindowConfiguration(int index);
    void           getNextCaptureName();

    P3D                 cameraPosition;
    P3D                 cameraLookAt;
    P3D                 cameraUp;
    ScreenGeometry      geometry;
    bool                useFollow;
    bool                useTraces;
    bool                useTextures;
    bool                visualiseAxes;
    bool                onScreenDisplay;
    bool                osdElapsedTime;
    bool                osdFramePerSecond;
    bool                osdRobotInformation;
    bool                followModeChanged;
    bool                useCapture;
    int                 followMode;
    string              name;
    string              captureName;
    string              orgCaptureName;
    std::vector<string> camNames;
    int                 captureIndex;
    int                 maxTraceLines;
    int                 maxTracePoints;

    string              fontRobotName;
    int                 fontRobotSize;
    Colour              fontRobotColour;

    string         fontTimeName;
    int            fontTimeSize;
    Colour          fontTimeColour;

    int            index;
};

#endif // __WINDOW_CONFIGURATION_H__
