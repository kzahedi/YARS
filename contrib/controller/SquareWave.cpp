#include "SquareWave.h"

#include <math.h>
#include <iostream>
#include <stdio.h>

using namespace std;

void SquareWave::update()
{
  _count++;
  for(int i = 0; i < (int)motors.size(); i++)
  {
    if((_count + _phaseShift[i]) % _period[i] == 0)
    {
      if(_values[i] == _min[i])
      {
        _values[i] = _max[i];
      }
      else
      {
        _values[i] = _min[i];
      }
    }
    motors[i] = _values[i % _values.size()];
  }
  if(_debug)
  {
    cout << "values:";
    for (int i = 0; i < (int)motors.size(); i++)
      cout << " " << motors[i];
    cout << endl;
  }
}

void SquareWave::init()
{
  double globalPeriod     = 0.0;
  double globalPhaseShift = 0.0;
  double globalMin        = 0.0;
  double globalMax        = 0.0;
  _count = 0;

  parameter.set("period",        globalPeriod,     100.0);
  parameter.set("phase shift",   globalPhaseShift, 0);
  parameter.set("min",           globalMin,        -1);
  parameter.set("max",           globalMax,        1);
  parameter.set("nr of outputs", _nr_of_outputs,   (int)motors.size());

  _period.resize(motors.size());
  _phaseShift.resize(motors.size());
  _min.resize(motors.size());
  _max.resize(motors.size());
  _values.resize(motors.size());



  for(int i = 0; i < motors.size(); i++)
  {
    _period[i]     = globalPeriod;
    _phaseShift[i] = globalPhaseShift;
    _min[i]        = globalMin;
    _max[i]        = globalMax;
  }

  _debug = parameter.boolValue("debug");

  stringstream oss;
  oss << "period 1";
  int index = 0;
  while(parameter.exists(oss.str()))
  {
    _period[index++] = parameter.intValue(oss.str());
    oss.str("");
    oss << "period " << (index + 1);
  }

  oss.str("");
  oss << "phase shift 1";
  index = 0;
  while(parameter.exists(oss.str()))
  {
    _phaseShift[index++] = parameter.intValue(oss.str());
    oss.str("");
    oss << "phase shift " << (index + 1);
  }

  oss.str("");
  oss << "min 1";
  index = 0;
  while(parameter.exists(oss.str()))
  {
    _min[index++] = parameter.doubleValue(oss.str());
    oss.str("");
    oss << "min " << (index + 1);
  }

  oss.str("");
  oss << "min 1";
  index = 0;
  while(parameter.exists(oss.str()))
  {
    _max[index++] = parameter.doubleValue(oss.str());
    oss.str("");
    oss << "max " << (index + 1);
  }

  if(_debug)
  {
    cout << "Parametrisation: " << endl;
    for(int i = 0; i < motors.size(); i++)
    {
      cout << "Period      " << i << ": " << _period[i]     << endl;
      cout << "Phase shift " << i << ": " << _phaseShift[i] << endl;
      cout << "Min         " << i << ": " << _min[i]        << endl;
      cout << "Max         " << i << ": " << _max[i]        << endl;
    }
  }

  for (int i = 0; i < motors.size(); i++)
  {
    _values[i] = _min[i];
  }

  if(_debug) printSensorMotorConfiguration();
}

void SquareWave::reset()
{ }

void SquareWave::close()
{ }

// the class factories
extern "C" RobotController* create()
{
  SquareWave *b = new SquareWave();
  return (RobotController*)b;
}

extern "C" void destroy(RobotController* controller)
{
  // printf("***** SquareWave::destroy called\n");
  //delete controller;
}


