#include "WindowConfiguration.h"

#include "configuration/YarsConfiguration.h"

#include <yars/view/console/ConsoleView.h>
#include <yars/configuration/data/Data.h>

#include <sstream>


using namespace std;

// : osdElapsedTimeFont("Helvetica", 15),
// osdFramePerSecondFont("Helvetica", 15)

WindowConfiguration::WindowConfiguration(int _i)
{
  index          = _i;
  name           = __YARS_GET_VIEW_NAME(index);
  geometry       = __YARS_GET_VIEW_GEOMETRY(index);
  cameraPosition = Data::instance()->current()->screens()->screen(index)->camera()->position();
  cameraLookAt   = Data::instance()->current()->screens()->screen(index)->camera()->lookAt();
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
  // if(index > 0)
  // {
    // string ext = captureName.substr(captureName.find_last_of("."), captureName.size() - 1);
    // captureName = captureName.substr(0, captureName.find_last_of("."));
    // stringstream oss;
    // oss << captureName << "-" << index << ext;
    // captureName = oss.str();
    // orgCaptureName = captureName;
  // }
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
