#include "Braitenberg3a.h"
#include <stdio.h>

using namespace std;

void Braitenberg3a::update()
{
  stringstream oss;
  if(osd)
  {
    clearOsd();
    oss.setf(ios_base::fixed, ios_base::floatfield);
    oss.precision(2);
  }
  switch(sensors.size())
  {
    case 10: // with light source
      i1 = -(sensors[0] + sensors[1] + sensors[2]) / 3.0;
      i2 = -(sensors[3] + sensors[4] + sensors[5]) / 3.0;

      motors[0] = tanh(i2) - 0.5 * tanh(sensors[6]);
      motors[1] = tanh(i1) - 0.5 * tanh(sensors[7]);

      if(osd) oss << "^7Left LDR sensor: ^1"  << sensors[6] << "^0\n" << "Right LDR sensor: ^1" << sensors[7] << "^0\n"
        << "^7Left IR sensor: ^1"  << i1 << "^0\n" << "Right IR sensor: ^1" << i2 << "^0";
      break;
    case 8: // without light source
      motors[0] = tanh(-(sensors[3] + sensors[4] + sensors[5]) / 3.0);
      motors[1] = tanh(-(sensors[0] + sensors[1] + sensors[2]) / 3.0);
      if(osd) oss << "^7Left IR sensor: ^1"  << i1 << "^0\n" << "Right IR sensor: ^1" << i2 << "^0";
      break;
  }
  if(osd) addLine(oss.str());
}

void Braitenberg3a::init()
{
  parameter.set("osd", osd, true);
}

void Braitenberg3a::close() {}

void Braitenberg3a::reset() {}

// the class factories
extern "C" RobotController* create()
{
  Braitenberg3a *b = new Braitenberg3a();
  return (RobotController*)b;
}

extern "C" void destroy(RobotController* controller) {}
