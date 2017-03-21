#include <yars/configuration/data/DataGenericActuatorSensor.h>

#include <yars/configuration/data/DataDomainFactory.h>

#include <yars/util/noise/NoiseFactory.h>
#include <yars/defines/mutex.h>

# define YARS_STRING_MAPPING            (char*)"mapping"
# define YARS_STRING_POSE               (char*)"pose"
# define YARS_STRING_OBJECT             (char*)"object"
# define YARS_STRING_TYPE               (char*)"type"
# define YARS_STRING_TYPE_DEFINITION    (char*)"type"                         DIVIDER DEFINITION
# define YARS_STRING_DOMAIN             (char*)"domain"
# define YARS_STRING_MIN_MAX_DEFINITION (char*)"min_max"                      DIVIDER DEFINITION

# define YARS_STRING_X                  (char*)"x"
# define YARS_STRING_Y                  (char*)"y"
# define YARS_STRING_Z                  (char*)"z"

# define YARS_STRING_AXIS_DEFINITION    (char*)"generic_actuator_sensor_axis" DIVIDER DEFINITION
# define YARS_STRING_DEFLECTION         (char*)"deflection"
# define YARS_STRING_VELOCITY           (char*)"velocity"
# define YARS_STRING_DOMAIN_DEFINITION  (char*)"generic_actuator_domain"      DIVIDER DEFINITION
# define YARS_STRING_ANGULAR            (char*)"angular"
# define YARS_STRING_LINEAR             (char*)"linear"

DataGenericActuatorSensor::DataGenericActuatorSensor(DataNode* parent)
  : DataSensor(parent, DATA_GENERIC_ACTUATOR_SENSOR)
{
  _dimension       = 0;
  _processingXAxis = false;
  _processingYAxis = false;
  _processingZAxis = false;

  YM_INIT;
}

DataGenericActuatorSensor::~DataGenericActuatorSensor()
{
  YM_CLOSE;
}

void DataGenericActuatorSensor::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_GENERIC_ACTUATOR_SENSOR))
  {
    _internalValue.resize(_dimension);
    _externalValue.resize(_dimension);
    __setMapping();

    current = parent;
  }
  if(element->opening(YARS_STRING_GENERIC_ACTUATOR_SENSOR))
  {
    element->set(YARS_STRING_NAME, _name);
  }
  if(element->opening(YARS_STRING_OBJECT))
  {
    element->set(YARS_STRING_NAME, _object);
  }

  if(element->opening(YARS_STRING_DEFLECTION))
  {
    if(_currentType == YARS_STRING_ANGULAR)
    {
      if(_processingXAxis)
      {
        _types.push_back(GENERIC_ACTUATOR_SENSOR_X_DEFLECTION_ANGULAR);
        _logName.push_back(_name + " (x angular deflection)");
      }
      if(_processingYAxis)
      {
        _types.push_back(GENERIC_ACTUATOR_SENSOR_Y_DEFLECTION_ANGULAR);
        _logName.push_back(_name + " (y angular deflection)");
      }
      if(_processingZAxis)
      {
        _types.push_back(GENERIC_ACTUATOR_SENSOR_Z_DEFLECTION_ANGULAR);
        _logName.push_back(_name + " (z angular deflection)");
      }
    }
    else
    {
      if(_processingXAxis)
      {
        _types.push_back(GENERIC_ACTUATOR_SENSOR_X_DEFLECTION_LINEAR);
        _logName.push_back(_name + " (x linear deflection)");
      }
      if(_processingYAxis)
      {
        _types.push_back(GENERIC_ACTUATOR_SENSOR_Y_DEFLECTION_LINEAR);
        _logName.push_back(_name + " (y linear deflection)");
      }
      if(_processingZAxis)
      {
        _types.push_back(GENERIC_ACTUATOR_SENSOR_Z_DEFLECTION_LINEAR);
        _logName.push_back(_name + " (z linear deflection)");
      }
    }
    _dimension++;
  }

  if(element->opening(YARS_STRING_VELOCITY))
  {
    if(_currentType == string(YARS_STRING_ANGULAR))
    {
      if(_processingXAxis)
      {
        _types.push_back(GENERIC_ACTUATOR_SENSOR_X_VELOCITY_ANGULAR);
        _logName.push_back(_name + " (x angular velocity)");
      }
      if(_processingYAxis)
      {
        _types.push_back(GENERIC_ACTUATOR_SENSOR_Y_VELOCITY_ANGULAR);
        _logName.push_back(_name + " (y angular velocity)");
      }
      if(_processingZAxis)
      {
        _types.push_back(GENERIC_ACTUATOR_SENSOR_Z_VELOCITY_ANGULAR);
        _logName.push_back(_name + " (z angular velocity)");
      }
    }
    else
    {
      if(_processingXAxis)
      {
        _types.push_back(GENERIC_ACTUATOR_SENSOR_X_VELOCITY_LINEAR);
        _logName.push_back(_name + " (x linear velocity)");
      }
      if(_processingYAxis)
      {
        _types.push_back(GENERIC_ACTUATOR_SENSOR_Y_VELOCITY_LINEAR);
        _logName.push_back(_name + " (y linear velocity)");
      }
      if(_processingZAxis)
      {
        _types.push_back(GENERIC_ACTUATOR_SENSOR_Z_VELOCITY_LINEAR);
        _logName.push_back(_name + " (z linear velocity)");
      }
    }
    _dimension++;
  }

  if(element->opening(YARS_STRING_MAPPING))
  {
    Domain m;
    DataDomainFactory::set(m, element);
    _mapping.push_back(m);
  }
  if(element->opening(YARS_STRING_DOMAIN))
  {
    Domain d;
    DataDomainFactory::set(d, element);
    _domain.push_back(d);
  }
  if(element->opening(YARS_STRING_NOISE))
  {
    DataNoise *noise  = new DataNoise(this);
    current = noise;
    noise->add(element);
    _noise.push_back(noise);
  }
  if(element->opening(YARS_STRING_FILTER))
  {
    DataFilter *filter = new DataFilter(this);
    current = filter;
    filter->add(element);
    _filter.push_back(filter);
  }

  if(element->closing(YARS_STRING_X) ||
     element->closing(YARS_STRING_Y) ||
     element->closing(YARS_STRING_Z))
  {
    _processingXAxis = false;
    _processingYAxis = false;
    _processingZAxis = false;
    _currentType     = "";
  }

  if(element->opening(YARS_STRING_X) ||
     element->opening(YARS_STRING_Y) ||
     element->opening(YARS_STRING_Z))
  {
    element->set(YARS_STRING_TYPE, _currentType);
  }

  if(element->opening(YARS_STRING_X)) _processingXAxis = true;
  if(element->opening(YARS_STRING_Y)) _processingYAxis = true;
  if(element->opening(YARS_STRING_Z)) _processingZAxis = true;

}

void DataGenericActuatorSensor::createXsd(XsdSpecification *spec)
{
  XsdSequence *sensor = new XsdSequence(YARS_STRING_GENERIC_ACTUATOR_SENSOR_DEFINITION);
  sensor->add(NA(YARS_STRING_NAME,    YARS_STRING_XSD_STRING,         false));
  sensor->add(NE(YARS_STRING_OBJECT,  YARS_STRING_NAME_DEFINITION,    1, 1));

  sensor->add(NE(YARS_STRING_X,       YARS_STRING_AXIS_DEFINITION,    0, 1));
  sensor->add(NE(YARS_STRING_Y,       YARS_STRING_AXIS_DEFINITION,    0, 1));
  sensor->add(NE(YARS_STRING_Z,       YARS_STRING_AXIS_DEFINITION,    0, 1));
  spec->add(sensor);

  XsdSequence *axisDefinition = new XsdSequence(YARS_STRING_AXIS_DEFINITION);
  axisDefinition->add(NA(YARS_STRING_TYPE,       YARS_STRING_TYPE_DEFINITION,   true));
  axisDefinition->add(NE(YARS_STRING_DEFLECTION, YARS_STRING_DOMAIN_DEFINITION, 0, 1));
  axisDefinition->add(NE(YARS_STRING_VELOCITY,   YARS_STRING_DOMAIN_DEFINITION, 0, 1));
  spec->add(axisDefinition);

  XsdEnumeration *typeDefinition = new XsdEnumeration(YARS_STRING_TYPE_DEFINITION,
      YARS_STRING_XSD_STRING);
  typeDefinition->add(YARS_STRING_ANGULAR);
  typeDefinition->add(YARS_STRING_LINEAR);
  spec->add(typeDefinition);

  XsdSequence *domainDefinition = new XsdSequence(YARS_STRING_DOMAIN_DEFINITION);
  domainDefinition->add(NE(YARS_STRING_DOMAIN,  YARS_STRING_MIN_MAX_DEFINITION, 0, 1));
  domainDefinition->add(NE(YARS_STRING_MAPPING, YARS_STRING_MIN_MAX_DEFINITION, 0, 1));
  domainDefinition->add(NE(YARS_STRING_NOISE,   YARS_STRING_NOISE_DEFINITION,   0, 1));
  domainDefinition->add(NE(YARS_STRING_FILTER,  YARS_STRING_FILTER_DEFINITION,  0, 1));
  spec->add(domainDefinition);

  DataNoise::createXsd(spec);
  DataFilter::createXsd(spec);
}

Domain DataGenericActuatorSensor::domain(int index)
{
  YM_LOCK;
  Domain d = _domain[index];
  YM_UNLOCK;
  return d;
}

DataGenericActuatorSensor* DataGenericActuatorSensor::_copy()
{
  DataGenericActuatorSensor *copy = new DataGenericActuatorSensor(NULL);
  copy->_name      = _name;
  copy->_object    = _object;
  copy->_dimension = _dimension;
  for(int i = 0; i < _types.size();   i++) copy->_types.push_back(_types[i]);
  for(int i = 0; i < _mapping.size(); i++) copy->_mapping.push_back(_mapping[i]);
  for(int i = 0; i < _domain.size();  i++) copy->_domain.push_back(_domain[i]);
  for(int i = 0; i < _filter.size();  i++) copy->_filter.push_back(_filter[i]->copy());
  for(int i = 0; i < _noise.size();   i++) copy->_noise.push_back(_noise[i]->copy());
  for(int i = 0; i < _logName.size(); i++) copy->_logName.push_back(_logName[i]);
  copy->_internalValue.resize(_dimension);
  copy->_externalValue.resize(_dimension);
  copy->__setMapping();
  return copy;
}

yReal DataGenericActuatorSensor::internalValue(int index)
{
  YM_LOCK;
  yReal r = _internalValue[index];
  YM_UNLOCK;
  return r;
}

yReal DataGenericActuatorSensor::externalValue(int index)
{
  YM_LOCK;
  yReal r = _externalValue[index];
  YM_UNLOCK;
  return r;
}

void DataGenericActuatorSensor::setInternalValue(int index, yReal v)
{
  YM_LOCK;
  // cout << "internal domain " << index << " = " << _internalDomain[index] << endl;
  _internalValue[index] = _internalDomain[index].cut(v);
  _externalValue[index] = _externalDomain[index].cut(_n[index]->calculate(_internalExternalMapping[index].map(_internalValue[index])));
  YM_UNLOCK;
}

void DataGenericActuatorSensor::setExternalValue(int index, yReal v)
{
  YM_LOCK;
  _externalValue[index] = _externalDomain[index].cut(v);
  _internalValue[index] = _internalExternalMapping[index].invMap(_externalValue[index]);
  YM_UNLOCK;
}

void DataGenericActuatorSensor::__setMapping()
{
  _externalDomain.clear();
  _internalDomain.clear();
  _internalExternalMapping.clear();
  _internalExternalMapping.clear();
  _n.clear();
  _externalDomain.resize(_dimension);
  _internalDomain.resize(_dimension);
  _internalExternalMapping.resize(_dimension);
  _internalExternalMapping.resize(_dimension);
  _n.resize(_dimension);
  for(int i = 0; i < _dimension; i++)
  {
    _externalDomain[i] = _mapping[i];
    _internalDomain[i] = _domain[i];
    // cout << "for actuator " << i << " domain " << _domain[i] << endl;
    _internalExternalMapping[i].setInputDomain(_internalDomain[i]);
    _internalExternalMapping[i].setOutputDomain(_externalDomain[i]);
    if(i < _noise.size()) _n[i] = NoiseFactory::create(_noise[i]);
    else                  _n[i] = NoiseFactory::create(NULL);
  }

  for(int i = 0; i < _dimension; i++)
    if(_types[i] & (GENERIC_ACTUATOR_SENSOR_ANGULAR | GENERIC_ACTUATOR_SENSOR_DEFLECTION))
      __setDegToRad(i);

}

void DataGenericActuatorSensor::setInternalDomain(int index, Domain d)
{
  YM_LOCK;
  _internalDomain[index] = d;
  __setMapping();
  YM_UNLOCK;
}

void DataGenericActuatorSensor::setExternalDomain(int index, Domain d)
{
  YM_LOCK;
  _externalDomain[index] = d;
  __setMapping();
  YM_UNLOCK;
}

Domain DataGenericActuatorSensor::getInternalDomain(int index)
{
  YM_LOCK;
  Domain d = _internalDomain[index];
  YM_UNLOCK;
  return d;
}

Domain DataGenericActuatorSensor::getExternalDomain(int index)
{
  YM_LOCK;
  Domain d = _externalDomain[index];
  YM_UNLOCK;
  return d;
}

void DataGenericActuatorSensor::_resetTo(const DataSensor *sensor)
{
  DataGenericActuatorSensor *other = (DataGenericActuatorSensor*)sensor;
  _name    = other->name();
  _object  = other->object();
  _dimension = other->_dimension;
  _types.clear();
  _mapping.clear();
  _domain.clear();
  _filter.clear();
  _noise.clear();
  for(int i = 0; i < _types.size();   i++) other->_types.push_back(_types[i]);
  for(int i = 0; i < _mapping.size(); i++) other->_mapping.push_back(_mapping[i]);
  for(int i = 0; i < _domain.size();  i++) other->_domain.push_back(_domain[i]);
  for(int i = 0; i < _filter.size();  i++) other->_filter.push_back(_filter[i]->copy());
  for(int i = 0; i < _noise.size();   i++) other->_noise.push_back(_noise[i]->copy());
  __setMapping();
}

void DataGenericActuatorSensor::__setDegToRad(int index)
{
  _internalDomain[index] *= M_PI/180.0;
  _internalExternalMapping[index].setInputDomain(_internalDomain[index]);
}

int DataGenericActuatorSensor::dimension()
{
  return _dimension;
}

int DataGenericActuatorSensor::type(int index)
{
  return _types[index];
}

string DataGenericActuatorSensor::logName(int index)
{
  return _logName[index];
}
