#include "PID.h"

#include <iostream>

PID::PID(double p, double i, double d, double c, int size)
{
  _p                 = p;
  _i                 = i;
  _d                 = d;
  _errors.resize(size);
  __reset(c);
}

PID::PID(const PID &p)
{
  _p                 = p._p;
  _i                 = p._i;
  _d                 = p._d;
  _currentValue      = p._currentValue;
  _error             = p._error;
  _currentErrorIndex = p._currentErrorIndex;
  _integral          = p._integral;

  _errors.resize(p._errors.size());
  for(int i = 0; i < (int)p._errors.size(); i++)
  {
    _errors[i] = p._errors[i];
  }

}

PID & PID::operator=(const PID &p)
{
  _p                 = p._p;
  _i                 = p._i;
  _d                 = p._d;
  _currentValue      = p._currentValue;
  _error             = p._error;
  _currentErrorIndex = p._currentErrorIndex;
  _integral          = p._integral;

  _errors.resize(p._errors.size());
  for(int i = 0; i < (int)p._errors.size(); i++)
  {
    _errors[i] = p._errors[i];
  }
  return *this;
}

double PID::update(double value, bool debug)
{
  _lastError                    = _error;
  _error                        = value - _currentValue;
  _derivative                   = _error - _lastError;
  _integral                    -= _errors[_currentErrorIndex]; // remove last entry (ring implementation)
  _integral                    += _error;
  _errors[_currentErrorIndex]   = _error;

  _currentValue                 = _p * _error + _i * _integral + _d * _derivative + _currentValue;

  _currentErrorIndex            = _currentErrorIndex + 1;
  _currentErrorIndex            = _currentErrorIndex % _errors.size();

  if(debug) cout << _error << " " << _integral << " " << _derivative << " -> " << _currentValue << endl;

  return _currentValue;
}

void PID::setPID(double p, double i, double d, int size)
{
  _p = p;
  _i = i;
  _d = d;
  __resize(size);
}

void PID::setInitialValue(double c)
{
  __reset(c);
}

void PID::setP(double p)
{
  _p = p;
}

void PID::setI(double i)
{
  _i = i;
}

void PID::setD(double d)
{
  _d = d;
}

double PID::p()
{
  return _p;
}

double PID::i()
{
  return _i;
}

double PID::d()
{
  return _d;
}

double PID::c()
{
  return _currentValue;
}

double PID::e()
{
  return _error;
}

int PID::size()
{
  return _errors.size();
}

void PID::__resize(int size)
{
  _errors.resize(size);
  for(std::vector<double>::iterator i = _errors.begin(); i != _errors.end(); i++) *i = 0.0;
}


void PID::__reset(double c)
{
  _currentValue      = c;
  _currentErrorIndex = 0;
  _error             = 0;
  _lastError         = 0;
  _integral          = 0;
  for(std::vector<double>::iterator i = _errors.begin(); i != _errors.end(); i++) *i = 0.0;
}

void PID::reset()
{
  __resize(_errors.size());
}
