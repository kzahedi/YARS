#include "NamedPipeController.h"
#include <stdio.h>

#include <yars/util/YarsErrorHandler.h>

#define __ACTUATORS_STRING     "ACTUATORS"
#define __SENSORS_STRING       "SENSORS"
#define __QUIT_STRING          "QUIT"
#define __CONFIGURATION_STRING "CONFIGURATION"
#define __RESET_STRING         "RESET"

#define __ACTUATORS            1000
#define __SENSORS              1001
#define __QUIT                 1002
#define __CONFIGURATION        1003
#define __RESET                1004

using namespace std;

void NamedPipeController::update()
{
  if(isQuit()) return;
  bool motorCommandsGiven = false;
  stringstream oss;
  string s;

  unsetStatus();

  while(!motorCommandsGiven && !isQuit())
  {
    pipe >> s;
    switch(commands[s])
    {
      case __ACTUATORS:
        if(debug) cout << "receiving motors" << endl;
        pipe >> motors;
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
        pipe << sensors;
        break;
      case __RESET:
        if(debug) cout << "received reset" << endl;
        setReset();
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
        YarsErrorHandler::push("NamedPipeController controller::update: unknown command \"%s\"", s.c_str());
    }
  }
}

void NamedPipeController::init()
{
  parameter.set("debug", debug, false);
  parameter.set("name",  name,  "yars-pipe");

  if(debug)
  {
    cout << "------------------------------------------------------------" << endl;
    cout << "                  Communication Structure                   " << endl;
    cout << "------------------------------------------------------------" << endl;
    cout << endl;
    cout << "  Communicating on name pipe: " << name << endl;

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

  if(debug) printSensorMotorConfiguration();

  pipe.init(name.c_str(), true);

}

void NamedPipeController::__configuration()
{
  stringstream oss;
  oss << "INTEGER " << sizeof(int) << " bytes, little endian";
  string s_integer = oss.str();
  oss.str("");
  oss << "DOUBLE " << sizeof(double) << " bytes, little endian";
  string s_yReal = oss.str();
  pipe << "BEGIN CONFIGURATION";
  pipe << "BEGIN DATA TYPES";
  pipe << s_integer;
  pipe << s_yReal;
  pipe << "END DATA TYPES";
  pipe << "BEGIN ROBOT DATA";
  oss.str("");
  oss << "NAME " << name;
  string robotName = oss.str();
  pipe << robotName;
  for(unsigned int i = 0; i < sensorConfiguration.size(); i++)
  {
    uint dimension = sensorConfiguration[i].dimension;
    pipe << "BEGIN SENSOR";
    oss.str("");
    oss << "NAME " << sensorConfiguration[i].name;
    pipe << oss.str();
    oss.str("");
    oss << "DIMENSION " << (int)dimension;
    pipe << oss.str();
    for(uint j = 0; j < dimension; j++)
    {
      Domain d = sensorConfiguration[i].internal[j];
      oss.str("");
      oss << "INTERNAL DOMAIN " << d.min << " " << d.max;
      pipe << oss.str();
      oss.str("");
      d = sensorConfiguration[i].external[j];
      oss << "EXTERNAL DOMAIN " << d.min << " " << d.max;
      pipe << oss.str();
    }
    pipe << "END SENSOR";
  }
  for(unsigned int i = 0; i < motorConfiguration.size(); i++)
  {
    stringstream oss;
    uint dimension = motorConfiguration[i].dimension;
    pipe << "BEGIN ACTUATOR";
    oss.str("");
    oss << "NAME " << motorConfiguration[i].name;
    pipe << oss.str();
    oss.str("");
    oss << "DIMENSION " << dimension;
    pipe << oss.str();
    oss.str("");
    for(uint j = 0; j < dimension; j++)
    {
      Domain d = motorConfiguration[i].internal[j];
      oss.str("");
      oss << "INTERNAL DOMAIN " << d.min << " " << d.max;
      pipe << oss.str();
      oss.str("");
      d = motorConfiguration[i].external[j];
      oss << "EXTERNAL DOMAIN " << d.min << " " << d.max;
      pipe << oss.str();
    }
    pipe << "END ACTUATOR";
  }
  pipe << "END ROBOT DATA";
  pipe << "END CONFIGURATION";
}



void NamedPipeController::close()
{
  // printf("***** NamedPipeController::close called\n");
  pipe.closePipe();
}

void NamedPipeController::reset()
{
  // TODO: send reset to client
}

// the class factories
extern "C" RobotController* create() {
  // printf("***** NamedPipeController::create called\n");
  NamedPipeController *b = new NamedPipeController();
  return (RobotController*)b;
}

extern "C" void destroy(RobotController* controller) {
  // printf("***** NamedPipeController::destroy called\n");
  // delete controller;
}

