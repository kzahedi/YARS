#ifndef __CONSTANT_VALUE_H__
#define __CONSTANT_VALUE_H__

#include <yars/rc/RobotController.hpp>

#include <string>
#include <iostream>

using namespace std;

class ConstantValue: public RobotController
{
  public:
    void init();
    void close();
    void update();
    void reset();

  private:
    void __readValues();

    std::vector<double> _values;

};

#endif // __CONSTANT_VALUE_H__



