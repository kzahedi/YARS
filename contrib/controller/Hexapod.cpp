#include "Hexapod.h"

#include <math.h>
#include <stdio.h>

using namespace std;

void HexapodController::update()
{
  motors[0] = sin(        _count / _period * M_PI) * _amplitude;
  motors[1] = cos(        _count / _period * M_PI) * _amplitude;

  motors[2]  = sin(M_PI + _count / _period * M_PI) * _amplitude;
  motors[3]  = cos(M_PI + _count / _period * M_PI) * _amplitude;

  motors[4]  = sin(M_PI + _count / _period * M_PI) * _amplitude;
  motors[5]  = cos(M_PI + _count / _period * M_PI) * _amplitude;

  motors[6]  = sin(       _count / _period * M_PI) * _amplitude;
  motors[7]  = cos(       _count / _period * M_PI) * _amplitude;

  motors[8]  = sin(       _count / _period * M_PI) * _amplitude;
  motors[9]  = cos(       _count / _period * M_PI) * _amplitude;

  motors[10] = sin(M_PI + _count / _period * M_PI) * _amplitude;
  motors[11] = cos(M_PI + _count / _period * M_PI) * _amplitude;

  _count += 1.0;
}

void HexapodController::init()
{
  _count      = 0;
  _period     = 10;
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

