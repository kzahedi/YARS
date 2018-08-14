#ifndef __GO_H__
#define __GO_H__

#include <yars/rc/RobotController.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

typedef long long go_int;
typedef double go_float64;
typedef struct{void *arr; go_int len; go_int cap;} go_slice;
typedef struct{const char *p; go_int len;} go_str;

typedef void (*UpdatePtr)(go_slice, go_slice);
typedef void (*SimplePtr)();

class Go: public RobotController
{

  virtual void update();

  void init();
  void close();
  void reset();

  ofstream     _log;
  bool         _debug;
  stringstream _oss;
  UpdatePtr    _update;
  SimplePtr    _reset;
  SimplePtr    _close;
  go_slice     _motorSlice;
  go_float64*  _motorData;
  go_slice     _sensorSlice;
  go_float64*  _sensorData;

};


#endif // __GO_H__
