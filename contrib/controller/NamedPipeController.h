#ifndef __NAMED_PIPE_CONTROLLER_H__
#define __NAMED_PIPE_CONTROLLER_H__

#include <yars/rc/RobotController.hpp>
#include <yars/util/NamedPipe.h>

#include <cmath>

#include <string>
#include <iostream>
#include <vector>
#include <math.h>
#include <stdio.h>


using namespace std;

typedef std::map<std::string, int> CommunicationCommands;

class NamedPipeController: public RobotController
{
  public:
    void init();
    void close();
    void reset();

  private:
    virtual void update();
    void       __configuration();

    bool                  debug;
    string                name;
    NamedPipe             pipe;
    CommunicationCommands commands;

};

#endif // __NAMED_PIPE_H__

