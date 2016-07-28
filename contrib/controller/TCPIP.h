#ifndef __TCP_IP_H__
#define __TCP_IP_H__

#include <yars/rc/RobotController.hpp>

#include <cmath>

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <stdio.h>

#include <yars/util/Socket.h>

using namespace std;

typedef std::map<std::string, int> CommunicationCommands;

class TCPIP: public RobotController
{
  public:
    void init();
    void close();
    void reset();

  private:
    virtual void update();
    void       __configuration();

    int                   port;
    bool                  debug;
    bool                  log;
    string                name;
    Socket                socket;
    CommunicationCommands commands;
    ofstream              logFile;

};

#endif // __TCP_IP_H__

