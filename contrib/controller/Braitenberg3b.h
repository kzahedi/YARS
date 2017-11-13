#include <yars/rc/RobotController.hpp>

#include <cmath>

#include <string>
#include <iostream>
#include <vector>
#include <math.h>
#include <stdio.h>

using namespace std;

class Braitenberg3b: public RobotController
{


  public:
    void init();
    void close();
    void reset();
    void update();

  private:
    double i1;
    double i2;
    bool  osd;
};
