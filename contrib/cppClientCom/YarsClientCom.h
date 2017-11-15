#ifndef __YARS_CLIENT_COM_H__
#define __YARS_CLIENT_COM_H__

#include <yars/util/Socket.h>
#include <yars/types/Domain.h>

#include <string>
#include <exception>
#include <iostream>
#include <fstream>

#include <pthread.h>
#include <stdio.h>


using namespace std;

class Entity
{
  public:
    Entity()
    {
      name = "";
      dimension = 0;
      domain.clear();
      value.clear();
    };

    string name;
    unsigned int dimension;
    std::vector<Domain> domain;
    std::vector<double> value;        // values only delivered in (mapped) domain
    std::vector<Domain> robotDomain; // just to read out, nothing else
};

class Sensor : public Entity
{ };

class Actuator : public Entity
{ };

class YarsClientCom
{
  public:
    YarsClientCom();

    void init(const string host="localhost", const int port=9500);
    void init(string workingDirectory, string xmlFile, string path="");

    unsigned int getActuatorDimension(int)          noexcept(false);
    void getActuatorName(int, string*)              noexcept(false);
    void getActuatorRobotDomain(Domain*, int, int)  noexcept(false);
    void getActuatorMappedDomain(Domain*, int, int) noexcept(false);
    void setActuatorValue(int, double);
    void setActuatorValue(int, float);

    unsigned int getSensorDimension(int)            noexcept(false);
    void getSensorName(int, string*)                noexcept(false);
    void getSensorRobotDomain(Domain*, int, int)    noexcept(false);
    void getSensorMappedDomain(Domain*, int, int)   noexcept(false);
    double getSensorValue(int);

    void sendMessage(string) noexcept(false);

    void update() noexcept(false); // send & receive

    int  sizeOfInt();
    int  sizeOfDouble();

    bool end();
    bool reset();

    void sendReset() noexcept(false);
    void sendQuit() noexcept(false);

    void name(string *name);
    void configuration(string *configurationString);
    void data(string *dataString);

    int numberOfActuators();
    int numberOfSensors();

    int numberOfActuatorsValues();
    int numberOfSensorsValues();

    void printSensorMotorConfiguration();

    void throwException(bool);

  private:
    void __sendMotorCommand()  noexcept(false);
    void __receiveSensorData() noexcept(false);
    void __configuration()     noexcept(false);
    void __printData();
    void __readActuatorInformation();
    void __readSensorInformation();
    void __readRobotInformation();
    void __readDataTypeSpecs();
    void __readRobotName(string s);

    void __setName(string s, string *name);
    void __setDomain(string s, std::vector<Domain> *d);
    void __setDimension(string s, Entity *e);

    void __openYars();

    FILE*                 _yarsFD;
    Socket                _socket;
    bool                  _end;
    bool                  _reset;
    int                   _port;
    int                   _sizeOfDouble;
    int                   _sizeOfInt;
    pthread_t             _thread;
    std::vector<Actuator> _actuators;
    std::vector<Sensor>   _sensors;
    string                _name;
    string                _options;
    string                _workingDirectory;

    int                   _nrOfActuatorValues;
    int                   _nrOfSensorValues;
    bool                  _throwException;

    std::vector<double>    _actuatorValues;
    std::vector<double>    _sensorValues;
};

#endif // __YARS_CLIENT_COM_H__
