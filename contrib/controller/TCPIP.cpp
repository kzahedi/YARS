#include "TCPIP.h"

#include <yars/util/YarsErrorHandler.h>

#include <stdio.h>

#define __ACTUATORS_STRING     "ACTUATORS"
#define __SENSORS_STRING       "SENSORS"
#define __QUIT_STRING          "QUIT"
#define __CONFIGURATION_STRING "CONFIGURATION"
#define __RESET_STRING         "RESET"
#define __MESSAGE_STRING       "MESSAGE"
#define __SEED_STRING          "RANDOM SEED"

#define __ACTUATORS            1000
#define __SENSORS              1001
#define __QUIT                 1002
#define __CONFIGURATION        1003
#define __RESET                1004
#define __MESSAGE              1005
#define __RANDOM_SEED          1006

using namespace std;

void TCPIP::update()
{
  if(isQuit()) return;
  bool motorCommandsGiven = false;
  stringstream oss;
  string s;
  string message;

  unsetStatus();

  if(log)
  {
    logFile << sensors[0];
    for(size_t i = 0; i < sensors.size(); i++) logFile << " " << sensors[i];
  }
  while(!motorCommandsGiven && !isQuit())
  {
    socket >> s;
    switch(commands[s])
    {
      cout << "Command " << s << endl;
      case __ACTUATORS:
        if(debug) cout << "receiving motors" << endl;
        socket >> motors;
        if(debug)
        {
          cout << "values: ";
          for(vector<double>::iterator i = motors.begin(); i != motors.end(); i++)
          {
            cout << *i << " ";
          }
          cout << endl;
        }
        motorCommandsGiven = true;
        break;
      case __SENSORS:
        if(debug)
        {
          cout << "sending sensors: ";
          for(vector<double>::iterator i = sensors.begin(); i != sensors.end(); i++)
          {
            cout << *i << " ";
          }
          cout << endl;
        }
        socket << sensors;
        break;
      case __RESET:
        if(debug) cout << "received reset" << endl;
        setReset();
        break;
      case __RANDOM_SEED:
        if(debug) cout << "random seed set" << endl;
        socket >> seed;
        break;
      case __MESSAGE:
        if(debug) cout << "received message" << endl;
        socket >> message;
        if(debug) cout << "with content: \"" << message << "\"" << endl;
        clearOsd();
        addLine(message);
        break;
      case __QUIT:
        if(debug) cout << "received quit" << endl;
        setQuit();
        break;
      case __CONFIGURATION:
        if(debug) cout << "sending configuration" << endl;
        __configuration();
        break;
      default:
        YarsErrorHandler::push("TCPIP controller::update: unknown command \"%s\"", s.c_str());
    }
  }

  if(log)
  {
    for(size_t i = 0; i < motors.size(); i++) logFile << " " << motors[i];
    logFile << endl;
    logFile.flush();
  }

}

void TCPIP::init()
{
  parameter.set("port",  port,  4500);
  parameter.set("debug", debug, false);
  parameter.set("log",   log,   false);
  parameter.set("name",  name,  "TCP/IP communication");

  seed = -1; // not set

  if(log) logFile.open("tcpip_sensor_actuator.log");

  port = socket.accept(port);
  cout << "opened port " << port << endl;

  if(debug)
  {
    cout << "------------------------------------------------------------" << endl;
    cout << "                  Communication Structure                   " << endl;
    cout << "------------------------------------------------------------" << endl;
    cout << endl;
    cout << "  Communicating on port: " << port << endl;

    if(sensorConfiguration.size() > 0)
    {
      cout   << "  sensors: " << endl;
      for(unsigned int i = 0; i < sensorConfiguration.size(); i++)
      {
        cout << "    sensor name:      " << sensorConfiguration[i].name      << endl;
        cout << "                      dimension: " << sensorConfiguration[i].dimension << endl;
      }
    }
    if(motorConfiguration.size() > 0)
    {
      cout   << "  motors: " << endl;
      for(unsigned int i = 0; i < motorConfiguration.size(); i++)
      {
        cout << "    motor name:      " << motorConfiguration[i].name      << endl;
        cout << "                     dimension: " << motorConfiguration[i].dimension << endl;
      }
    }
    cout << "------------------------------------------------------------" << endl << endl;
  }
  if(debug)
  {
    cout << "communication started" << endl;
  }

  commands[__ACTUATORS_STRING]     = __ACTUATORS;
  commands[__SENSORS_STRING]       = __SENSORS;
  commands[__QUIT_STRING]          = __QUIT;
  commands[__CONFIGURATION_STRING] = __CONFIGURATION;
  commands[__RESET_STRING]         = __RESET;
  commands[__MESSAGE_STRING]       = __MESSAGE;

  if(debug) printSensorMotorConfiguration();

}

void TCPIP::__configuration()
{
  stringstream oss;
  oss << "INTEGER " << sizeof(int) << " bytes, little endian";
  string s_integer = oss.str();
  oss.str("");
  oss << "DOUBLE " << sizeof(double) << " bytes, little endian";
  string s_double = oss.str();
  socket << "BEGIN CONFIGURATION";
  socket << "BEGIN DATA TYPES";
  socket << s_integer;
  socket << s_double;
  socket << "END DATA TYPES";
  socket << "BEGIN ROBOT DATA";
  oss.str("");
  oss << "NAME " << name;
  string robotName = oss.str();
  socket << robotName;
  for(unsigned int i = 0; i < sensorConfiguration.size(); i++)
  {
    uint dimension = sensorConfiguration[i].dimension;
    socket << "BEGIN SENSOR";
    oss.str("");
    oss << "NAME " << sensorConfiguration[i].name;
    socket << oss.str();
    oss.str("");
    oss << "DIMENSION " << (int)dimension;
    socket << oss.str();
    for(uint j = 0; j < dimension; j++)
    {
      Domain d = sensorConfiguration[i].internal[j];
      oss.str("");
      oss << "INTERNAL DOMAIN " << d.min << " " << d.max;
      socket << oss.str();
      oss.str("");
      d = sensorConfiguration[i].external[j];
      oss << "EXTERNAL DOMAIN " << d.min << " " << d.max;
      socket << oss.str();
    }
    socket << "END SENSOR";
  }
  for(unsigned int i = 0; i < motorConfiguration.size(); i++)
  {
    stringstream oss;
    uint dimension = motorConfiguration[i].dimension;
    socket << "BEGIN ACTUATOR";
    oss.str("");
    oss << "NAME " << motorConfiguration[i].name;
    socket << oss.str();
    oss.str("");
    oss << "DIMENSION " << dimension;
    socket << oss.str();
    oss.str("");
    for(uint j = 0; j < dimension; j++)
    {
      Domain d = motorConfiguration[i].internal[j];
      oss.str("");
      oss << "INTERNAL DOMAIN " << d.min << " " << d.max;
      socket << oss.str();
      oss.str("");
      d = motorConfiguration[i].external[j];
      oss << "EXTERNAL DOMAIN " << d.min << " " << d.max;
      socket << oss.str();
    }
    socket << "END ACTUATOR";
  }
  socket << "END ROBOT DATA";
  socket << "END CONFIGURATION";
}



void TCPIP::close()
{
  printf("***** TCPIP::close called\n");
  socket.close();
  if(log) logFile.close();
}

void TCPIP::reset()
{
  // TODO: send reset to client
}

// the class factories
extern "C" RobotController* create() {
  printf("***** TCPIP::create called\n");
  TCPIP *b = new TCPIP();
  return (RobotController*)b;
}

extern "C" void destroy(RobotController* controller) {
  printf("***** TCPIP::destroy called\n");
  // delete controller;
}

