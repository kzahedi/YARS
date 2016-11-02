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
    std::vector<yReal> value;        // values only delivered in (mapped) domain
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

    unsigned int getActuatorDimension(int)          throw (YarsClientComException);
    void getActuatorName(int, string*)              throw (YarsClientComException);
    void getActuatorRobotDomain(Domain*, int, int)  throw (YarsClientComException);
    void getActuatorMappedDomain(Domain*, int, int) throw (YarsClientComException);
    void setActuatorValue(int, double);
    void setActuatorValue(int, float);

    unsigned int getSensorDimension(int)            throw (YarsClientComException);
    void getSensorName(int, string*)                throw (YarsClientComException);
    void getSensorRobotDomain(Domain*, int, int)    throw (YarsClientComException);
    void getSensorMappedDomain(Domain*, int, int)   throw (YarsClientComException);
    yReal getSensorValue(int);

    void sendMessage(string) throw (YarsClientComException);

    void update() throw (YarsClientComException); // send & receive

    int  sizeOfInt();
    int  sizeOfDouble();

    bool end();
    bool reset();

    void sendReset() throw (YarsClientComException);
    void sendQuit() throw (YarsClientComException);

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
    void __sendMotorCommand()  throw (YarsClientComException);
    void __receiveSensorData() throw (YarsClientComException);
    void __configuration()     throw (YarsClientComException);
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

    std::vector<yReal>    _actuatorValues;
    std::vector<yReal>    _sensorValues;
};

#endif // __YARS_CLIENT_COM_H__
