#ifndef __MATLAB_TEST_CONTROLLER_H__
#define __MATLAB_TEST_CONTROLLER_H__


#include <yars/rc/RobotController.hpp>

#include <string>
#include <iostream>

#include <engine.h>

using namespace std;

class MATLABController: public RobotController
{
  public:
    void init();
    void close();
    void reset();
    void update();

  private:
    string _init;
    string _update;
    string _close;
    string _reset;
    string _workingDir;
    Engine *ep;
    mxArray *S;
    mxArray *A;
    double *_sensors;
    double *_motors;
    bool _debug;

};

#endif


