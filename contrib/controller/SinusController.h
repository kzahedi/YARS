#ifndef __SENSOR_TEST_CONTROLLER_H__
#define __SENSOR_TEST_CONTROLLER_H__


#include <yars/rc/RobotController.hpp>

#include <string>
#include <iostream>

using namespace std;

class SinusController: public RobotController
{
  public:
    void init();
    void close();
    void reset();
    void update();

  private:

    vector<double> _period;
    vector<double> _amplitude;
    vector<double> _phaseShift;
    double _count;
    bool   _debug;
    int    _nr_of_outputs;
};

#endif

