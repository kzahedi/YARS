#ifndef __JULIA_H__
#define __JULIA_H__

#include <yars/rc/RobotController.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

typedef int (*FuncPtr)(size_t*, double*, size_t*, double*, size_t*, char*);

class Julia: public RobotController
{

  virtual void update();

  void init();
  void close();
  void reset();

  ofstream _log;
  bool     _debug;
  stringstream _oss;

  FuncPtr _update;

};


#endif // __JULIA_H__

