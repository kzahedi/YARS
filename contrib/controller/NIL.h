#ifndef _NIL_H_
#define _NIL_H_


#include <iostream>
#include <yars/rc/RobotController.hpp>

using namespace std;

class Nil: public RobotController
{

  virtual void update();

  void init();
  void close();
  void reset();


};


#endif

