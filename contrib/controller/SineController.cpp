#include "SineController.h"

#include <math.h>
#include <stdio.h>

using namespace std;

void SineController::update()
{
  for(int i = 0; i < _nr_of_outputs; i++)
  {
    motors[i] = _baseline[i] + sin(_count / _period[i] + _phaseShift[i]) * _amplitude[i];
  }
  if(_debug)
  {
    cout << "Time step: " << _count << endl;
    if(motors.size() > 0)
    {
      cout << "Motors: " << motors[0];
      for(int i = 1; i < _nr_of_outputs; i++)
      {
        cout << "," << motors[i];
      }
      cout << endl;
    }
    if(sensors.size() > 0)
    {
      cout << "Sensors: " << sensors[0];
      for(int i = 1; i < _nr_of_outputs; i++)
      {
        cout << "," << sensors[i];
      }
      cout << endl;
    }
  }
  _count += 1.0;
}

void SineController::init()
{
  double globalPeriod     = 0.0;
  double globalPhaseShift = 0.0;
  double globalAmplitude  = 0.0;
  double globalBaseline   = 0.0;
  _count = 0.0;

  parameter.set("period",        globalPeriod,     100.0);
  parameter.set("amplitude",     globalAmplitude,  1.0);
  parameter.set("phase shift",   globalPhaseShift, 0.0);
  parameter.set("base line",     globalBaseline,   0.0);
  parameter.set("nr of outputs", _nr_of_outputs,   (int)motors.size());

  _period.resize(motors.size());
  _amplitude.resize(motors.size());
  _phaseShift.resize(motors.size());
  _baseline.resize(motors.size());

  for(int i = 0; i < motors.size(); i++)
  {
    _period[i]     = globalPeriod;
    _amplitude[i]  = globalAmplitude;
    _phaseShift[i] = globalPhaseShift;
    _baseline[i]   = globalBaseline;
  }

  _debug = parameter.boolValue("debug");

  stringstream oss;
  oss << "period 1";
  int index = 0;
  while(parameter.exists(oss.str()))
  {
    _period[index++] = parameter.doubleValue(oss.str());
    oss.str("");
    oss << "period " << (index + 1);
  }

  oss.str("");
  oss << "amplitude 1";
  index = 0;
  while(parameter.exists(oss.str()))
  {
    _amplitude[index++] = parameter.doubleValue(oss.str());
    oss.str("");
    oss << "amplitude " << (index + 1);
  }

  oss.str("");
  oss << "phase shift 1";
  index = 0;
  while(parameter.exists(oss.str()))
  {
    _phaseShift[index++] = parameter.doubleValue(oss.str());
    oss.str("");
    oss << "phase shift " << (index + 1);
  }

  oss.str("");
  oss << "base line 1";
  index = 0;
  while(parameter.exists(oss.str()))
  {
    _baseline[index++] = parameter.doubleValue(oss.str());
    oss.str("");
    oss << "base line " << (index + 1);
  }

  // if(_debug)
  // {
    // cout << "Parametrisation: " << endl;
    // for(int i = 0; i < motors.size(); i++)
    // {
      // cout << "Period "      << i << ": " << _period[i]     << endl;
      // cout << "Phase shift " << i << ": " << _phaseShift[i] << endl;
      // cout << "Amplitude "   << i << ": " << _amplitude[i]  << endl;
      // cout << "Base line "   << i << ": " << _baseline[i]   << endl;
    // }
  // }

  if(_debug) printSensorMotorConfiguration();
}

void SineController::reset()
{
  _count = 0;
}

void SineController::close()
{ }

// the class factories
extern "C" RobotController* create()
{
  SineController *b = new SineController();
  return (RobotController*)b;
}

extern "C" void destroy(RobotController* controller)
{
  //delete controller;
}
