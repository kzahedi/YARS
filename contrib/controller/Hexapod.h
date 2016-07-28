#ifndef __SENSOR_TEST_CONTROLLER_H__
#define __SENSOR_TEST_CONTROLLER_H__

#include <yars/rc/RobotController.hpp>

#include <string>
#include <iostream>

using namespace std;

class HexapodController: public RobotController
{
  public:
    void init();
    void close();
    void reset();

  private:
    virtual void update();

    double _period;
    double _amplitude;
    double _count;
    bool   _debug;
};

#endif


