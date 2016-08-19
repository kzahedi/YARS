#include "WindowConfiguration.h"

#include "configuration/YarsConfiguration.h"

#include <yars/view/console/ConsoleView.h>
#include <yars/configuration/data/Data.h>

#include <sstream>


using namespace std;

WindowConfiguration::WindowConfiguration(int _i)
{
  index                 = _i;
  name                  = __YARS_GET_VIEW_NAME(index);
  geometry              = __YARS_GET_VIEW_GEOMETRY(index);
  cameraPosition        = Data::instance()->current()->screens()->screen(index)->camera()->position();
  cameraLookAt          = Data::instance()->current()->screens()->screen(index)->camera()->lookAt();
  captureName           = __YARS_GET_CAPTURE_NAME;
  useFollow             = __YARS_GET_USE_FOLLOW_MODE(index);
  osdElapsedTimeFont    = "Helvetica";
  osdFramePerSecondFont = "Helvetica";
  orgCaptureName        = captureName;
  visualiseAxes         = true;
  followModeChanged     = false;
  osdFramePerSecond     = true;
  osdElapsedTime        = true;
  // useShadows        = __YARS_GET_USE_SHADOWS;
  // useTextures       = __YARS_GET_USE_TEXTURES;
  // useTraces         = __YARS_GET_USE_TRACES;
  // followMode        = __YARS_GET_FOLLOW_MODE;
  // followObjects     = __YARS_GET_USE_FOLLOW_MODE;
  // onScreenDisplay   = __YARS_GET_USE_OSD;

  fontRobotName   = __YARS_GET_OSD_ROBOT_FONT_NAME(index);
  fontRobotSize   = __YARS_GET_OSD_ROBOT_FONT_SIZE(index);
  fontRobotColour = __YARS_GET_OSD_ROBOT_FONT_COLOR(index);

  fontTimeName    = __YARS_GET_OSD_TIME_FONT_NAME(index);
  fontTimeSize    = __YARS_GET_OSD_TIME_FONT_SIZE(index);
  fontTimeColour  = __YARS_GET_OSD_TIME_FONT_COLOR(index);

  // if(index > 0)
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
