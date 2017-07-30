#include "YarsNamedPipeCom.h"

#include <yars/util/YarsErrorHandler.h>

#include <stdlib.h>

#include <sstream>

using namespace std;


YarsNamedPipeCom::YarsNamedPipeCom()
{
  _sizeOfInt    = -1;
  _sizeOfDouble = -1;
  _end          = false;
  _reset        = false;
  YarsErrorHandler::instance();
}

bool YarsNamedPipeCom::end()
{
  return _end;
}

bool YarsNamedPipeCom::reset()
{
  return _reset;
}

void YarsNamedPipeCom::update()
{
  __sendMotorCommand();
  __receiveSensorData();
}


void YarsNamedPipeCom::init(const string name) throw (YarsNamedPipeComException)
{
  _namedPipe.init(name.c_str(), false);
  __configuration();
  __printData();
}

void YarsNamedPipeCom::sendReset()
{
  _namedPipe << "RESET";
}

void YarsNamedPipeCom::sendQuit()
{
  _namedPipe << "QUIT";
}

void YarsNamedPipeCom::name(string *name)
{
  *name = _name;
}

void YarsNamedPipeCom::__printData()
{
  string conf;
  configuration(&conf);
  cout << conf << endl;
}

int YarsNamedPipeCom::sizeOfInt()
{
  return _sizeOfInt;
}

int YarsNamedPipeCom::sizeOfDouble()
{
  return _sizeOfDouble;
}

void YarsNamedPipeCom::__configuration() throw (YarsNamedPipeComException)
{
  _namedPipe << string("CONFIGURATION");
  string string;
  _namedPipe >> string;
  if(string != "BEGIN CONFIGURATION")
  {
    throw YarsNamedPipeComException("No configuration!");
  }
  while(string != "END CONFIGURATION")
  {
    _namedPipe >> string;
    if(string == "BEGIN DATA TYPES")
    {
      __readDataTypeSpecs();
    }
    if(string == "BEGIN ROBOT DATA")
    {
      __readRobotInformation();
    }
  }
}

void YarsNamedPipeCom::__readDataTypeSpecs()
{
  string mystring;
  while(mystring != "END DATA TYPES")
  {
    _namedPipe >> mystring;
    if(mystring.find("INTEGER") == 0)
    {
      string s = mystring.substr(mystring.find_first_of(" "), mystring.length());
      s = s.substr(0, s.find_first_of("bytes"));
      _sizeOfInt = atoi(s.c_str());
    }
    if(mystring.find("DOUBLE") == 0)
    {
      string s = mystring.substr(mystring.find_first_of(" "), mystring.length());
      s = s.substr(0, s.find_first_of("bytes"));
      _sizeOfDouble = atof(s.c_str());
    }
  }
}


void YarsNamedPipeCom::__readRobotInformation()
{
  string mystring;
  while(mystring != "END ROBOT DATA")
  {
    _namedPipe >> mystring;
    if(mystring.find("NAME ") == 0)
    {
      __readRobotName(mystring);
    }
    if(mystring.find("BEGIN SENSOR") == 0)
    {
      __readSensorInformation();
    }
    if(mystring.find("BEGIN ACTUATOR") == 0)
    {
      __readJointInformation();
    }
  }
}

void YarsNamedPipeCom::__readRobotName(string s)
{
  _name = s.substr(s.find_first_of(" "), s.length());
}

void YarsNamedPipeCom::__readSensorInformation()
{
  string mystring;
  Sensor s;
  while(mystring != "END SENSOR")
  {
    _namedPipe >> mystring;
    if(mystring.find("NAME") == 0)
    {
      __setName(mystring, &(s.name));
    }
    if(mystring.find("DIMENSION") == 0)
    {
      __setDimension(mystring, &(s));
    }
    if(mystring.find("INTERNAL DOMAIN") == 0)
    {
      __setDomain(mystring, &(s.robotDomain));
    }
    if(mystring.find("EXTERNAL DOMAIN") == 0)
    {
      __setDomain(mystring, &(s.domain));
    }
  }
  _sensors.push_back(s);
}

void YarsNamedPipeCom::__readJointInformation()
{
  string mystring;
  Joint j;
  while(mystring != "END ACTUATOR")
  {
    _namedPipe >> mystring;
    if(mystring.find("NAME") == 0)
    {
      __setName(mystring, &(j.name));
    }
    if(mystring.find("DIMENSION") == 0)
    {
      __setDimension(mystring, &(j));
    }
    if(mystring.find("INTERNAL DOMAIN") == 0)
    {
      __setDomain(mystring, &(j.robotDomain));
    }
    if(mystring.find("EXTERNAL DOMAIN") == 0)
    {
      __setDomain(mystring, &(j.domain));
    }
  }
  _joints.push_back(j);
}
void YarsNamedPipeCom::__setName(string s, string *name)
{
  *name = s.substr(s.find(" ") + 1, s.length());
}


void YarsNamedPipeCom::__setDomain(string s, std::vector<Domain> *domains)
{
  string values = s.substr(s.find(" ") + 1, s.length()); // cut away "ROBOT" or "MAPPED"
         values = values.substr(values.find(" ") + 1, values.length());  // cut away "DOMAIN"
  string lowerBound = values.substr(0, values.find(" "));
  string upperBound = values.substr(values.find(" ") + 1, values.length());
  Domain d(atof(lowerBound.c_str()), atof(upperBound.c_str()));
  domains->push_back(d);
}

void YarsNamedPipeCom::__setDimension(string s, Entity *e)
{
  string value = s.substr(s.find_first_of(" "), s.length());
  e->dimension = atoi(value.c_str());
  e->value.resize(e->dimension);
  // others will be pushed
}

unsigned int YarsNamedPipeCom::getJointDimension(int index) throw (YarsNamedPipeComException)
{
  if(index < 0 || index >= (int)_joints.size())
  {
    throw YarsNamedPipeComException("getJointDimension: Joint index out of range.");
  }
  return _joints[index].dimension;
}

unsigned int YarsNamedPipeCom::getSensorDimension(int index) throw (YarsNamedPipeComException)
{
  if(index < 0 || index >= (int)_sensors.size())
  {
    throw YarsNamedPipeComException("getSensorDimension: Sensor index out of range.");
  }

  return _sensors[index].dimension;
}

void YarsNamedPipeCom::getJointName(int index, string *name) throw (YarsNamedPipeComException)
{
  if(index < 0 || index >= (int)_joints.size())
  {
    throw YarsNamedPipeComException("getJointName: Joint index out of range.");
  }

  *name = _joints[index].name;
}

void YarsNamedPipeCom::getSensorName(int index, string *name) throw (YarsNamedPipeComException)
{
  if(index < 0 || index >= (int)_sensors.size())
  {
    throw YarsNamedPipeComException("getSensorName: Sensor index out of range.");
  }

  *name = _sensors[index].name;
}

void YarsNamedPipeCom::setJointValue(double value, int jointIndex, int valueIndex) throw (YarsNamedPipeComException)
{
  if(jointIndex < 0 || jointIndex >= (int)_joints.size())
  {
    throw YarsNamedPipeComException("setJointValue: Joint index out of range.");
  }
  if(valueIndex >= (int)_joints[jointIndex].dimension)
  {
    throw YarsNamedPipeComException("setJointValue: Value index out of range.");
  }
  if(valueIndex >= (int)_joints[jointIndex].value.size() )
  {
    _joints[jointIndex].value.resize(_joints[jointIndex].dimension);
  }

  Domain d = _joints[jointIndex].robotDomain[valueIndex];

  d.cut(&value);

  _joints[jointIndex].value[valueIndex] = value;
}

void YarsNamedPipeCom::getSensorValue(double *value, int sensorIndex, int valueIndex) throw (YarsNamedPipeComException)
{
  if(sensorIndex < 0 || sensorIndex >= (int)_sensors.size())
  {
    throw YarsNamedPipeComException("getSensorValue: Sensor index out of range.");
  }
  if(valueIndex >= (int)_sensors[sensorIndex].dimension)
  {
    throw YarsNamedPipeComException("getSensorValue: Value index out of range.");
  }
  if(valueIndex >= (int)_sensors[sensorIndex].value.size() )
  {
    _sensors[sensorIndex].value.resize(_sensors[sensorIndex].dimension);
  }

  *value = _sensors[sensorIndex].value[valueIndex];
}

void YarsNamedPipeCom::getJointRobotDomain(Domain *d, int jointIndex, int valueIndex) throw (YarsNamedPipeComException)
{
  if(jointIndex < 0 || jointIndex >= (int)_joints.size())
  {
    throw YarsNamedPipeComException("getJointRobotDomain: Joint index out of range.");
  }
  if(valueIndex < 0 || valueIndex >= (int)_joints[jointIndex].dimension)
  {
    throw YarsNamedPipeComException("getJointRobotDomain: Value index out of range.");
  }
  *d = _joints[jointIndex].robotDomain[valueIndex];
}

void YarsNamedPipeCom::getSensorRobotDomain(Domain *d, int sensorIndex, int valueIndex) throw (YarsNamedPipeComException)
{
  if(sensorIndex < 0 || sensorIndex >= (int)_sensors.size())
  {
    throw YarsNamedPipeComException("getSensorRobotDomain: Sensor index out of range.");
  }
  if(valueIndex >= (int)_sensors[sensorIndex].dimension)
  {
    throw YarsNamedPipeComException("getSensorRobotDomain: Value index out of range.");
  }

  *d = _sensors[sensorIndex].robotDomain[valueIndex];
}

void YarsNamedPipeCom::getJointMappedDomain(Domain *d, int jointIndex, int valueIndex) throw (YarsNamedPipeComException)
{
  if(jointIndex < 0 || jointIndex >= (int)_joints.size())
  {
    throw YarsNamedPipeComException("getJointMappedDomain: Joint index out of range.");
  }
  if(valueIndex < 0 || valueIndex >= (int)_joints[jointIndex].dimension)
  {
    throw YarsNamedPipeComException("getJointMappedDomain: Value index out of range.");
  }

  *d = _joints[jointIndex].domain[valueIndex];
}

void YarsNamedPipeCom::getSensorMappedDomain(Domain *d, int sensorIndex, int valueIndex) throw (YarsNamedPipeComException)
{
  if(sensorIndex < 0 || sensorIndex >= (int)_sensors.size())
  {
    throw YarsNamedPipeComException("getSensorMappedDomain: Sensor index out of range.");
  }
  if(valueIndex >= (int)_sensors[sensorIndex].dimension)
  {
    throw YarsNamedPipeComException("getSensorMappedDomain: Value index out of range.");
  }

  *d = _sensors[sensorIndex].domain[valueIndex];
}


void YarsNamedPipeCom::__sendMotorCommand()
{
  _namedPipe << "ACTUATORS";
  std::vector<double> values;
  for(std::vector<Joint>::iterator i = _joints.begin(); i != _joints.end(); i++)
  {
    for(std::vector<double>::iterator d = i->value.begin(); d != i->value.end(); d++)
    {
      values.push_back(*d);
    }
  }
  _namedPipe << values;
}

void YarsNamedPipeCom::__receiveSensorData()
{
  std::vector<double> values;
  _namedPipe << "SENSORS";
  _namedPipe >> values;
  int index = 0;
  for(std::vector<Sensor>::iterator i = _sensors.begin(); i != _sensors.end(); i++)
  {
    for(std::vector<double>::iterator d = i->value.begin(); d != i->value.end(); d++)
    {
      *d = values[index];
      index++;
    }
  }
}

void YarsNamedPipeCom::configuration(string *configurationString)
{
  stringstream oss;
  oss << "integer: " << _sizeOfInt << endl;
  oss << "double:  " << _sizeOfDouble << endl;
  oss << "Sensors:   " << endl;
  for(std::vector<Sensor>::iterator i = _sensors.begin(); i != _sensors.end(); i++)
  {
    oss << "  Sensor name:       " << i->name << endl;
    oss << "    Sensor dimension:  " << i->dimension << endl;
    oss << "    Sensor domain:     " << endl;
    for(std::vector<Domain>::iterator d = i->domain.begin(); d != i->domain.end(); d++)
    {
      oss << "      [" << d->min << ", " << d->max << "]"  << endl;
    }
    oss << "    Sensor robot domain: " << endl;
    for(std::vector<Domain>::iterator d = i->robotDomain.begin(); d != i->robotDomain.end(); d++)
    {
      oss << "      [" << d->min << ", " << d->max << "]"  << endl;
    }
  }
  for(std::vector<Joint>::iterator i = _joints.begin(); i != _joints.end(); i++)
  {
    oss << "  Joint name:       " << i->name << endl;
    oss << "    Joint dimension:  " << i->dimension << endl;
    oss << "    Joint domain:     " << endl;
    for(std::vector<Domain>::iterator d = i->domain.begin(); d != i->domain.end(); d++)
    {
      oss << "      [" << d->min << ", " << d->max << "]"  << endl;
    }
    oss << "    Joint robot domain: " << endl;
    for(std::vector<Domain>::iterator d = i->robotDomain.begin(); d != i->robotDomain.end(); d++)
    {
      oss << "      [" << d->min << ", " << d->max << "]"  << endl;
    }
  }
  *configurationString = oss.str();
}

void YarsNamedPipeCom::data(string *dataString)
{
  stringstream oss;
  oss.width(8);
  oss.setf(ios::fixed,ios::floatfield);
  for(std::vector<Sensor>::iterator i = _sensors.begin(); i != _sensors.end(); i++)
  {
    oss << "  Sensor name:       " << i->name << endl;
    oss << "    Values:  ";
    for(std::vector<double>::iterator d = i->value.begin(); d != i->value.end(); d++)
    {
      oss << (*d) << " ";
    }
    oss << endl;
  }
  for(std::vector<Joint>::iterator i = _joints.begin(); i != _joints.end(); i++)
  {
    oss << "  Joint name:       " << i->name << endl;
    oss << "    Values:  ";
    for(std::vector<double>::iterator d = i->value.begin(); d != i->value.end(); d++)
    {
      oss << (*d) << " ";
    }
    oss << endl;
  }
  *dataString = oss.str();
}

int YarsNamedPipeCom::numberOfJoints()
{
  return _joints.size();
}

int YarsNamedPipeCom::numberOfSensors()
{
  return _sensors.size();
}
