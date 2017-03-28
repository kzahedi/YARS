#include <yars/rc/RobotController.hpp>

#include <cmath>

#include <string>
#include <iostream>
#include <vector>
#include <math.h>
#include <stdio.h>

class Muscle : public RobotController
{
  public:
    void init();
    void close();
    void reset();
    void update();

  private:
    yReal i1;
    yReal i2;
    bool  osd;
};
