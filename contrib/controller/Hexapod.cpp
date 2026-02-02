#include "Hexapod.h"

#include <math.h>
#include <stdio.h>

using namespace std;

void HexapodController::update()
{
  // Gait pattern matching the Python hexapod controller
  // Phase shifts: s1=0, s2=3π/2, s3=π, s4=5π/2
  double phase = _count * 2.0 * M_PI / _period;
  double s1 = sin(phase) * _amplitude;
  double s2 = sin(phase + 3.0 * M_PI / 2.0) * _amplitude;
  double s3 = sin(phase + M_PI) * _amplitude;
  double s4 = sin(phase + 5.0 * M_PI / 2.0) * _amplitude;

  // Front right leg
  motors[0] = s1;
  motors[1] = s2;

  // Front left leg
  motors[2] = s3;
  motors[3] = s4;

  // Centre right leg
  motors[4] = s3;
  motors[5] = s4;

  // Centre left leg
  motors[6] = s1;
  motors[7] = s2;

  // Rear right leg
  motors[8] = s1;
  motors[9] = s2;

  // Rear left leg
  motors[10] = s3;
  motors[11] = s4;

  _count += 1.0;
}

void HexapodController::init()
{
  _count      = 0;
  _period     = 50;  // Match Python controller period
  _amplitude  = 1.0;
  _debug      = true;

  parameter.set("period",    _period,    10.0);
  parameter.set("amplitude", _amplitude, 1.0);
  parameter.set("debug",     _debug,     false);

  if(_debug) cout << "HexapodController: setting period to      " << _period << endl;
  if(_debug) cout << "HexapodController: setting amplitude to   " << _amplitude << endl;
  if(_debug) cout << "HexapodController: setting debug to       " << _debug << endl;

  if(_debug) printSensorMotorConfiguration();
}


void HexapodController::close()
{
}

void HexapodController::reset()
{
  _count = 0;
}

// the class factories
extern "C" RobotController* create()
{
  HexapodController *b = new HexapodController();
  return (RobotController*)b;
}

extern "C" void destroy(RobotController* controller)
{
  printf("***** HexapodController::destroy called\n");
  controller->close();
  //delete controller;
}

