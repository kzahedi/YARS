#ifndef _NIL_H_
#define _NIL_H_

#include <iostream>
#include <yars/rc/RobotController.hpp>

#include <nmode/Data.h>
#include <nmode/RNN.h>

#include <string>

using namespace std;

class NMODEController: public RobotController
{

  public:
    virtual void update();

    void init();
    void close();
    void reset();
  private:
    bool   _debug;
    string _filename;
    int    _individual;

    Data* _data;
    RNN*  _rnn;


};


#endif

