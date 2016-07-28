#include "SinusController.h"

#include <math.h>
#include <stdio.h>

using namespace std;

void SinusController::update()
{
  for(int i = 0; i < _nr_of_outputs; i++)
  {
    motors[i] = sin(_count / _period[i] + _phaseShift[i]) * _amplitude[i];
  }
  _count += 1.0;
}

void SinusController::init()
{
  yReal globalPeriod     = 0.0;
  yReal globalPhaseShift = 0.0;
  yReal globalAmplitude  = 0.0;
  _count = 0.0;

  parameter.set("period",        globalPeriod,     100.0);
  parameter.set("amplitude",     globalAmplitude,  1.0);
  parameter.set("phase shift",   globalPhaseShift, 0.0);
  parameter.set("nr of outputs", _nr_of_outputs,   (int)motors.size());

  _period.resize(motors.size());
  _amplitude.resize(motors.size());
  _phaseShift.resize(motors.size());

  for(int i = 0; i < motors.size(); i++)
  {
    _period[i]     = globalPeriod;
    _amplitude[i]  = globalAmplitude;
    _phaseShift[i] = globalPhaseShift;
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

  if(_debug)
  {
    cout << "Parametrisation: " << endl;
    for(int i = 0; i < motors.size(); i++)
    {
      cout << "Period "      << i << ":      " << _period[i]     << endl;
      cout << "Phase shift " << i << ": " << _phaseShift[i] << endl;
      cout << "Amplitude "   << i << ":   " << _amplitude[i]  << endl;
    }
  }

  if(_debug) printSensorMotorConfiguration();
}

void SinusController::reset()
{
  _count = 0;
}

void SinusController::close()
{ }

// the class factories
extern "C" RobotController* create()
{
  SinusController *b = new SinusController();
  return (RobotController*)b;
}

extern "C" void destroy(RobotController* controller)
{
  //delete controller;
}
