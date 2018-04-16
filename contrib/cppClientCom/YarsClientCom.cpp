#include "YarsClientCom.h"

#include <yars/util/YarsErrorHandler.h>

#include <sstream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

using namespace std;

void* read_stdout(void* ydf)
{
  char buf[1];
  FILE* yarsFD = (FILE*)ydf;
  stringstream sst;
  while(feof(yarsFD) == 0)
  {
    fread(buf, 1, 1, yarsFD);
    sst << buf[0];
    if(buf[0] == '\n')
    {
      cout << sst.str();
      sst.str("");
    }
  }
  cout << sst.str();
  return NULL;
}


YarsClientCom::YarsClientCom()
{
  _port           = -1;
  _sizeOfDouble   = -1;
  _sizeOfInt      = -1;
  _end            = false;
  _reset          = false;
  YarsErrorHandler::instance();
}

bool YarsClientCom::end()
{
  return _end;
}

bool YarsClientCom::reset()
{
  return _reset;
}

void YarsClientCom::update()
{
  __sendMotorCommand();
  __receiveSensorData();
}

void YarsClientCom::init(const string host, const int port)
{
  _socket.connect(host, port);
  __configuration();
  // __printData();
}

void YarsClientCom::init(string workingDirectory, string xmlFile, string path)
{
  // cout << "chaining to working directory: " << workingDirectory << endl;
  chdir(workingDirectory.c_str());

  stringstream sst;
  sst << path << "yars " << xmlFile;
#if __APPLE__
  if((_yarsFD = popen(sst.str().c_str(), "r+")) == NULL)
#else // __APPLE__
    if((_yarsFD = popen(sst.str().c_str(), "r")) == NULL)
#endif // __APPLE__
    {
      cout << "Cannot open \"" << sst.str() << "\"" << endl;
      cout << "Error Nr.: " << errno << endl;
      exit(-1);
    }
  sst.str("");

  string token;
  int l = 0;
  char buf[1];
  bool lookForPort = false;
  int port = -1;
  string pstring = "port";
  stringstream sts;
  while(feof(_yarsFD) == 0)
  {
    fread(buf, 1, 1, _yarsFD);
    sts << buf[0];
    if(buf[0] == '\n')
    {
      // cout << sts.str();
      sts.str("");
    }

    if(buf[0] != ' ' || buf[0] == '\n')
    {
      sst << buf[0];
    }
    else
    {
      if(lookForPort)
      {
        string token = sst.str();
        sst.str("");
        port = atoi(token.c_str());
        cout << endl << ">>> found port: " << port << endl;
        break;
      }
      else
      {
        string token = sst.str();
        sst.str("");
        if(token == "port")
        {
          lookForPort = true;
        }
      }
    }
    if(buf[0] == 0)
    {
      break;
    }
  }

  init("localhost", port);

  pthread_create(&_thread, NULL, read_stdout, (void*)_yarsFD);
}

void YarsClientCom::sendReset()
{
  _socket << "RESET";
}

void YarsClientCom::sendMessage(string message)
{
  _socket << "MESSAGE";
  _socket << message;
}

void YarsClientCom::sendQuit()
{
  _socket << "QUIT";
  pclose(_yarsFD);
}

void YarsClientCom::name(string *name)
{
  *name = _name;
}

void YarsClientCom::__printData()
{
  string conf;
  configuration(&conf);
  cout << conf << endl;
}

int YarsClientCom::sizeOfInt()
{
  return _sizeOfInt;
}

int YarsClientCom::sizeOfDouble()
{
  return _sizeOfDouble;
}

void YarsClientCom::__configuration()
{
  _socket << string("CONFIGURATION");
  string string;
  _socket >> string;
  // cout << string << endl;
  if(string != "BEGIN CONFIGURATION")
  {
    cerr << "No configuration!" << endl;
    exit(-1);
  }
  while(string != "END CONFIGURATION")
  {
    _socket >> string;
    // cout << string << endl;
    if(string == "BEGIN DATA TYPES")
    {
      __readDataTypeSpecs();
    }
    if(string == "BEGIN ROBOT DATA")
    {
      __readRobotInformation();
    }
  }

  _nrOfActuatorValues = 0;
  _nrOfSensorValues   = 0;

  for(int i = 0; i < (int)_actuators.size(); i++) _nrOfActuatorValues += _actuators[i].dimension;
  for(int i = 0; i < (int)_sensors.size();   i++) _nrOfSensorValues   += _sensors[i].dimension;

  _sensorValues.clear();
  _sensorValues.resize(_nrOfSensorValues);

  _actuatorValues.clear();
  _actuatorValues.resize(_nrOfActuatorValues);
}

void YarsClientCom::__readDataTypeSpecs()
{
  string mystring;
  while(mystring != "END DATA TYPES")
  {
    _socket >> mystring;
    // cout << mystring  << endl;
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


void YarsClientCom::__readRobotInformation()
{
  string mystring;
  while(mystring != "END ROBOT DATA")
  {
    _socket >> mystring;
    // cout << mystring  << endl;
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
      __readActuatorInformation();
    }
  }
}

void YarsClientCom::__readRobotName(string s)
{
  _name = s.substr(s.find_first_of(" "), s.length());
}

void YarsClientCom::__readSensorInformation()
{
  string mystring;
  Sensor s;
  while(mystring != "END SENSOR")
  {
    _socket >> mystring;
    // cout << mystring  << endl;
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

void YarsClientCom::__readActuatorInformation()
{
  string mystring;
  Actuator j;
  while(mystring != "END ACTUATOR")
  {
    _socket >> mystring;
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
  _actuators.push_back(j);
}

void YarsClientCom::__setName(string s, string *name)
{
  *name = s.substr(s.find(" ") + 1, s.length());
}

void YarsClientCom::__setDomain(string s, std::vector<Domain> *domains)
{
  string values = s.substr(s.find(" ") + 1, s.length()); // cut away "ROBOT" or "MAPPED"
  values = values.substr(values.find(" ") + 1, values.length());  // cut away "DOMAIN"
  string lowerBound = values.substr(0, values.find(" "));
  string upperBound = values.substr(values.find(" ") + 1, values.length());
  Domain d(atof(lowerBound.c_str()), atof(upperBound.c_str()));
  domains->push_back(d);
}

void YarsClientCom::__setDimension(string s, Entity *e)
{
  string value = s.substr(s.find_first_of(" "), s.length());
  e->dimension = atoi(value.c_str());
  e->value.resize(e->dimension);
  // others will be pushed
}

unsigned int YarsClientCom::getActuatorDimension(int index)
{
  if(index < 0 || index >= (int)_actuators.size())
  {
    cerr << "getActuatorDimension: Actuator index out of range." << endl;
    exit(-1);
  }
  return _actuators[index].dimension;
}

unsigned int YarsClientCom::getSensorDimension(int index)
{
  if(index < 0 || index >= (int)_sensors.size())
  {
    cerr << "getSensorDimension: Sensor index out of range." << endl;
    exit(-1);
  }

  return _sensors[index].dimension;
}

void YarsClientCom::getActuatorName(int index, string *name)
{
  if(index < 0 || index >= (int)_actuators.size())
  {
    cerr << "getActuatorName: Actuator index out of range." << endl;
    exit(-1);
  }

  *name = _actuators[index].name;
}

void YarsClientCom::getSensorName(int index, string *name)
{
  if(index < 0 || index >= (int)_sensors.size())
  {
    cerr << "getSensorName: Sensor index out of range." << endl;
    exit(-1);
  }

  *name = _sensors[index].name;
}

void YarsClientCom::setActuatorValue(int actuatorIndex, float value)
{
  setActuatorValue(actuatorIndex, (double)value);
}

void YarsClientCom::setActuatorValue(int actuatorIndex, double value)
{
  // if(actuatorIndex < 0 || actuatorIndex >= (int)_actuators.size())
  // {
  // throw YarsClientComException("setActuatorValue: Actuator index out of range.");
  // }
  // if(valueIndex >= (int)_actuators[actuatorIndex].dimension)
  // {
  // throw YarsClientComException("setActuatorValue: Value index out of range.");
  // }
  // if(valueIndex >= (int)_actuators[actuatorIndex].value.size() )
  // {
  // _actuators[actuatorIndex].value.resize(_actuators[actuatorIndex].dimension);
  // }

  // Domain d = _actuators[actuatorIndex].robotDomain[valueIndex];

  // d.cut(&value);

  // _actuators[actuatorIndex].value[valueIndex] = value;
  _actuatorValues[actuatorIndex] = value;
}

double YarsClientCom::getSensorValue(int sensorIndex)
{
  // if(sensorIndex < 0 || sensorIndex >= (int)_sensors.size())
  // {
  // throw YarsClientComException("getSensorValue: Sensor index out of range.");
  // }
  // if(valueIndex >= (int)_sensors[sensorIndex].dimension)
  // {
  // throw YarsClientComException("getSensorValue: Value index out of range.");
  // }
  // if(valueIndex >= (int)_sensors[sensorIndex].value.size() )
  // {
  // _sensors[sensorIndex].value.resize(_sensors[sensorIndex].dimension);
  // }

  return _sensorValues[sensorIndex];

  // *value = _sensors[sensorIndex].value[valueIndex];
}

void YarsClientCom::getActuatorRobotDomain(Domain *d, int actuatorIndex, int valueIndex)
{
  if(actuatorIndex < 0 || actuatorIndex >= (int)_actuators.size())
  {
    cerr << "getActuatorRobotDomain: Actuator index out of range." << endl;
    exit(-1);
  }
  if(valueIndex < 0 || valueIndex >= (int)_actuators[actuatorIndex].dimension)
  {
    cerr << "getActuatorRobotDomain: Value index out of range." << endl;
    exit(-1);
  }
  *d = _actuators[actuatorIndex].robotDomain[valueIndex];
}

void YarsClientCom::getSensorRobotDomain(Domain *d, int sensorIndex, int valueIndex)
{
  if(sensorIndex < 0 || sensorIndex >= (int)_sensors.size())
  {
    cerr << "getSensorRobotDomain: Sensor index out of range." << endl;
    exit(-1);
  }
  if(valueIndex >= (int)_sensors[sensorIndex].dimension)
  {
    cerr << "getSensorRobotDomain: Value index out of range." << endl;
    exit(-1);
  }

  *d = _sensors[sensorIndex].robotDomain[valueIndex];
}

void YarsClientCom::getActuatorMappedDomain(Domain *d, int actuatorIndex, int valueIndex)
{
  if(actuatorIndex < 0 || actuatorIndex >= (int)_actuators.size())
  {
    cerr << "getActuatorMappedDomain: Actuator index out of range." << endl;
    exit(-1);
  }
  if(valueIndex < 0 || valueIndex >= (int)_actuators[actuatorIndex].dimension)
  {
    cerr << "getActuatorMappedDomain: Value index out of range." << endl;
    exit(-1);
  }

  *d = _actuators[actuatorIndex].domain[valueIndex];
}

void YarsClientCom::getSensorMappedDomain(Domain *d, int sensorIndex, int valueIndex)
{
  if(sensorIndex < 0 || sensorIndex >= (int)_sensors.size())
  {
    cerr << "getSensorMappedDomain: Sensor index out of range." << endl;
    exit(-1);
  }
  if(valueIndex >= (int)_sensors[sensorIndex].dimension)
  {
    cerr << "getSensorMappedDomain: Value index out of range." << endl;
    exit(-1);
  }

  *d = _sensors[sensorIndex].domain[valueIndex];
}


void YarsClientCom::__sendMotorCommand()
{
  // std::vector<double> values;
  // for(std::vector<Actuator>::iterator i = _actuators.begin(); i != _actuators.end(); i++)
  // {
  // for(std::vector<double>::iterator d = i->value.begin(); d != i->value.end(); d++)
  // {
  // values.push_back(*d);
  // }
  // }
  // cout << "Sending actuator values:";
  // for(int i = 0; i < (int)_actuatorValues.size(); i++) cout << " " << _actuatorValues[i];
  // cout << endl;
  _socket << "ACTUATORS";
  _socket << _actuatorValues;
}

void YarsClientCom::__receiveSensorData()
{
  _socket << "SENSORS";
  _socket >> _sensorValues;
  // int index = 0;
  // for(std::vector<Sensor>::iterator i = _sensors.begin(); i != _sensors.end(); i++)
  // {
  // for(std::vector<double>::iterator d = i->value.begin(); d != i->value.end(); d++)
  // {
  // *d = values[index];
  // index++;
  // }
  // }
}

void YarsClientCom::configuration(string *configurationString)
{
  stringstream oss;
  oss << "integer: " << _sizeOfInt << endl;
  oss << "double:  " << _sizeOfDouble << endl;
  oss << "Sensors: (" << _sensors.size() << ")" << endl;
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

  oss << "Actuators (" << _actuators.size() << "): " << endl;
  for(std::vector<Actuator>::iterator i = _actuators.begin(); i != _actuators.end(); i++)
  {
    oss << "  Actuator name:       " << i->name << endl;
    oss << "    Actuator dimension:  " << i->dimension << endl;
    oss << "    Actuator domain:     " << endl;
    for(std::vector<Domain>::iterator d = i->domain.begin(); d != i->domain.end(); d++)
    {
      oss << "      [" << d->min << ", " << d->max << "]"  << endl;
    }
    oss << "    Actuator robot domain: " << endl;
    for(std::vector<Domain>::iterator d = i->robotDomain.begin(); d != i->robotDomain.end(); d++)
    {
      oss << "      [" << d->min << ", " << d->max << "]"  << endl;
    }
  }
  *configurationString = oss.str();
}

void YarsClientCom::data(string *dataString)
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
  for(std::vector<Actuator>::iterator i = _actuators.begin(); i != _actuators.end(); i++)
  {
    oss << "  Actuator name:       " << i->name << endl;
    oss << "    Values:  ";
    for(std::vector<double>::iterator d = i->value.begin(); d != i->value.end(); d++)
    {
      oss << (*d) << " ";
    }
    oss << endl;
  }
  *dataString = oss.str();
}

int YarsClientCom::numberOfActuators()
{
  return _actuators.size();
}

int YarsClientCom::numberOfSensors()
{
  return _sensors.size();
}

int YarsClientCom::numberOfActuatorsValues()
{
  return _nrOfActuatorValues;
}

int YarsClientCom::numberOfSensorsValues()
{
  return _nrOfSensorValues;
}

void YarsClientCom::printSensorMotorConfiguration()
{
  cout << "Actuator: " << endl;
  int index = 0;
  for(vector<Actuator>::iterator a = _actuators.begin(); a != _actuators.end(); a++)
  {
    for(int i = 0; i < (*a).dimension; i++)
    {
      cout << "Actuator " << (index++) << ": " << (*a).name << " dimension " << i << endl;
    }
  }

  index = 0;
  cout << "Sensor: " << endl;
  for(vector<Sensor>::iterator s = _sensors.begin(); s != _sensors.end(); s++)
  {
    for(int i = 0; i < (*s).dimension; i++)
    {
      cout << "Sensor " << (index++) << ": " << (*s).name << " dimension " << i << endl;
    }
  }
}
