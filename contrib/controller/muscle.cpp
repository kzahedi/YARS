#include "muscle.h"
#include <stdio.h>

void Muscle::update()
{
  stringstream oss;
  if(osd)
  {
    clearOsd();
    oss.setf(ios_base::fixed, ios_base::floatfield);
    oss.precision(2);
  }
  i1 = -(sensors[0] + sensors[1] + sensors[2]) / 3.0;
  i2 = -(sensors[3] + sensors[4] + sensors[5]) / 3.0;
  motors[0] = tanh(i2);
  motors[1] = tanh(i1);
  if(osd) oss << "^7Left IR sensor: ^1"  << i1 << "^0\n" << "Right IR sensor: ^1" << i2 << "^0";
  if(osd) addLine(oss.str());
}

void Muscle::init()
{
  parameter.set("osd", osd, true);
}

void Muscle::close() {}

void Muscle::reset() {}

// the class factories
extern "C" RobotController* create()
{
  Muscle *b = new Muscle();
  return (RobotController*)b;
}

extern "C" void destroy(RobotController* controller) {}
