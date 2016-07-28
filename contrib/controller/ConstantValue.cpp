#include "ConstantValue.h"

#include <math.h>
#include <iostream>
#include <stdio.h>

using namespace std;

void ConstantValue::update()
{
  cout << "nr of motors: " << motors.size() << endl;
  cout << "values:";
  for(int i = 0; i < (int)motors.size(); i++) cout << " " << _values[i];
  cout << endl;
  for(int i = 0; i < (int)motors.size(); i++) motors[i] = _values[i % _values.size()];
}

void ConstantValue::init()
{
  std::vector<string> names = parameter.names();
  //cout << "found " << names.size() << " different values" << endl;
  for(std::vector<string>::iterator i = names.begin(); i != names.end(); i++)
  {
    cout << "setting: " << *i << " to " << parameter.doubleValue(*i) << endl;
    _values.push_back(parameter.doubleValue(*i));
  }
}

void ConstantValue::reset()
{ }

void ConstantValue::close()
{ }

// the class factories
extern "C" RobotController* create()
{
  ConstantValue *b = new ConstantValue();
  return (RobotController*)b;
}

extern "C" void destroy(RobotController* controller)
{
  // printf("***** ConstantValue::destroy called\n");
  //delete controller;
}


