#ifndef __LOGGING_MODULE_H__
#define __LOGGING_MODULE_H__

#include <yars/configuration/data/DataObject.h>
#include <yars/configuration/data/DataActuator.h>
#include <yars/configuration/data/DataSensor.h>
#include <yars/configuration/data/DataController.h>
#include <yars/defines/types.h>

#include <string>
#include <sstream>
#include <vector>


using namespace std;

class LoggingModule; // forward declaration

typedef string (LoggingModule::*LoggingModuleFunctionPtr)(string);

/**
 * Superclass for
 * LoggingModuleObject
 * LoggingModuleSensor
 * LoggingModuleActuator
 * LoggingModuleController
 */
class LoggingModule : public std::vector<string>
{
  public:
    LoggingModule();
    ~LoggingModule();

    void update();

    string name();

    void setPrecision(int precision);

    std::vector<string>::iterator v_begin();
    std::vector<string>::iterator v_end();
    int                      v_size();

    string getObjectX(string arg);
    string getObjectY(string arg);
    string getObjectZ(string arg);
    string getObjectAlpha(string arg);
    string getObjectBeta(string arg);
    string getObjectGamma(string arg);
    string getObjectR(string arg);
    string getObjectG(string arg);
    string getObjectB(string arg);
    string getObjectA(string arg);

    string getSensorInternalValues(string arg);
    string getSensorExternalValues(string arg);

    string getActuatorExternalValues(string arg);
    string getActuatorInternalValues(string arg);
    string getActuatorDesiredValues(string arg);
    string getActuatorAppliedForce(string arg);
    string getActuatorAppliedVelocity(string arg);

    string getControllerVariable(string arg);

    void addMethod(LoggingModuleFunctionPtr method, string argument = "");

  protected:
    DataObject                        *_object;
    DataActuator                      *_actuator;
    DataSensor                        *_sensor;
    DataController                    *_controller;
    string                             _name;
    std::vector<string>                _variables;

  private:
    //std::vector<string(LoggingModule::*)(string)> _methods;
    std::vector<LoggingModuleFunctionPtr> _methods;
    std::vector<string>                   _arguments;

    stringstream       _oss;
    stringstream       _out;
    string             _delimiter;
    int                _precision;
    double              _real;
    int                _integer;
    ::Matrix           _matrix;
    std::vector<double> _vector;
};

#endif // __LOGGING_MODULE_H__
