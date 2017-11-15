#ifndef __YARS_CLIENT_COM_H__
#define __YARS_CLIENT_COM_H__

#include <yars/util/NamedPipe.h>
#include <yars/types/Domain.h>

#include <string>
#include <exception>
#include <iostream>
#include <fstream>

using namespace std;

class YarsNamedPipeComException : public exception
{
  public:
    explicit YarsNamedPipeComException(const string& what)
      :
        m_what(what)
  {}

    virtual ~YarsNamedPipeComException() throw() {}

    virtual const char * what() const throw()
    {
      return m_what.c_str();
    }

    virtual void message() const throw()
    {
      cerr << "YarsNamedPipeComException: " << m_what << endl;
    }

  private:
    string m_what;
};

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

class Joint : public Entity
{ };

class YarsNamedPipeCom
{
  public:
    YarsNamedPipeCom();

    void init(const string name)  noexcept(false);

    unsigned int getJointDimension(int index)   noexcept(false);
    unsigned int getSensorDimension(int index)  noexcept(false);

    void getJointName(int index,  string *name) noexcept(false);
    void getSensorName(int index, string *name) noexcept(false);

    void setJointValue(double value,       int jointIndex,  int valueIndex = 0) noexcept(false);
    void getSensorValue(double *value,     int sensorIndex, int valueIndex = 0) noexcept(false);

    void getJointRobotDomain(Domain   *d, int jointIndex,  int valueIndex = 0) noexcept(false);
    void getSensorRobotDomain(Domain  *d, int sensorIndex, int valueIndex = 0) noexcept(false);

    void getJointMappedDomain(Domain  *d, int jointIndex,  int valueIndex = 0) noexcept(false);
    void getSensorMappedDomain(Domain *d, int sensorIndex, int valueIndex = 0) noexcept(false);

    void update(); // send & receive

    int  sizeOfInt();
    int  sizeOfDouble();

    bool end();
    bool reset();

    void sendReset();
    void sendQuit();

    void name(string *name);
    void configuration(string *configurationString);
    void data(string *dataString);

    int numberOfJoints();
    int numberOfSensors();

  private:
    void __sendMotorCommand();
    void __receiveSensorData();
    void __printData();
    void __configuration() noexcept(false);
    void __readJointInformation();
    void __readSensorInformation();
    void __readRobotInformation();
    void __readDataTypeSpecs();
    void __readRobotName(string s);

    void __setName(string s, string *name);
    void __setDomain(string s, std::vector<Domain> *d);
    void __setDimension(string s, Entity *e);

    NamedPipe _namedPipe;

    int _sizeOfInt;
    int _sizeOfDouble;
    bool _end;
    bool _reset;

    std::vector<Sensor> _sensors;
    std::vector<Joint>  _joints;
    string              _name;

};

#endif // __YARS_CLIENT_COM_H__
