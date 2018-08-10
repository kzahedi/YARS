#include "GoController.h"
#include <string>
#include <fstream>
#include <streambuf>
#include <dlfcn.h>

using namespace std;

void Go::update()
{
  size_t n    = sensors.size();
  size_t m    = motors.size();
  size_t l    = 256;
  char   str[l];

  for(int i = 0; i < sensors.size(); i++) _sensorData[i] = sensors[i];

  _update(_sensorSlice, _motorSlice);

  for(int i = 0; i < motors.size(); i++) motors[i] = _motorData[i];

  // clearOsd();
  // addLine(str);

  if(_debug)
  {
    _oss.str("");
    _oss << sensors[0];
    for(size_t i = 1; i < sensors.size(); i++) _oss << " " << sensors[i];
    for(size_t i = 0; i < motors.size();  i++) _oss << " " << motors[i];
    _oss << endl;

    _log << _oss.str();
  }

  // switch(status)
  // {
    // case 1:
      // setReset();
      // break;
    // case 2:
      // setQuit();
      // break;
  // }

}

void Go::reset()
{ }

void Go::init()
{
  void *handle;
  char *error;
  string goCtrl;

  parameter.set("debug",         _debug, false);
  parameter.set("go controller", goCtrl, "");

  if(_debug) cout << "Go controller " << goCtrl << endl;

  handle = dlopen (goCtrl.c_str(), RTLD_LAZY);
  if (!handle) {
    cerr << dlerror() << endl;
    exit(1);
  }

  _update = (UpdatePtr)dlsym(handle, "Update");

  int n = (int)sensors.size();
  _sensorData = new go_float64[sensors.size()];
  _sensorSlice = {_sensorData, n, n};

  int m = (int)motors.size();
  _motorData = new go_float64[motors.size()];
  _motorSlice = {_motorData, m, m};

  // go_int (*add)(go_int, go_int) = dlsym(handle, "Add");
  // if ((error = dlerror()) != NULL)  {
      // fputs(error, stderr);
      // exit(1);
  // }
}

void Go::close()
{ }

// the class factories

extern "C" RobotController* create() {return new Go; }

extern "C" void destroy(RobotController* controller)
{
  // delete controller;
}

