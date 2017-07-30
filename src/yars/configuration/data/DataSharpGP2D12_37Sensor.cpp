#include "DataSharpGP2D12_37Sensor.h"
#include "DataDomainFactory.h"
#include "DataPoseFactory.h"

# define YARS_STRING_MAPPING            (char*)"mapping"
# define YARS_STRING_POSE               (char*)"pose"
# define YARS_STRING_NAME               (char*)"name"
# define YARS_STRING_OBJECT             (char*)"object"
# define YARS_STRING_POSE_DEFINITION    (char*)"pose"    DIVIDER DEFINITION
# define YARS_STRING_MIN_MAX_DEFINITION (char*)"min"     DIVIDER "max"      DIVIDER DEFINITION
# define YARS_STRING_NAME_DEFINITION    (char*)"name"          DIVIDER DEFINITION

DataSharpGP2D12_37Sensor::DataSharpGP2D12_37Sensor(DataNode* parent)
  : DataSensor(parent, DATA_SHARP_GP2D12_37_SENSOR)
{ }

DataSharpGP2D12_37Sensor::~DataSharpGP2D12_37Sensor()
{
  // nothing to be done.
}

void DataSharpGP2D12_37Sensor::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_SHARP_IR_GP2D12_37))
  {
    __setMapping();
    current = parent;
  }
  if(element->opening(YARS_STRING_SHARP_IR_GP2D12_37))
  {
    element->set(YARS_STRING_NAME, _name);
  }
  if(element->opening(YARS_STRING_OBJECT))
  {
    element->set(YARS_STRING_NAME, _object);
  }
  if(element->opening(YARS_STRING_MAPPING))
  {
    DataDomainFactory::set(_mapping, element);
  }
  if(element->opening(YARS_STRING_POSE))
  {
    DataPoseFactory::set(_pose, element);
  }
}

Pose DataSharpGP2D12_37Sensor::pose()
{
  return _pose;
}

void DataSharpGP2D12_37Sensor::createXsd(XsdSpecification *spec)
{
  XsdSequence *sensor = new XsdSequence(YARS_STRING_SHARP_IR_GP2D12_37_DEFINITION);
  sensor->add(NA(YARS_STRING_NAME,    YARS_STRING_XSD_STRING,         false));
  sensor->add(NE(YARS_STRING_OBJECT,  YARS_STRING_NAME_DEFINITION,    1, 1));
  sensor->add(NE(YARS_STRING_POSE,    YARS_STRING_POSE_DEFINITION,    1, 1));
  sensor->add(NE(YARS_STRING_MAPPING, YARS_STRING_MIN_MAX_DEFINITION, 1, 1));
  spec->add(sensor);
}


DataSharpGP2D12_37Sensor* DataSharpGP2D12_37Sensor::_copy()
{
  DataSharpGP2D12_37Sensor *copy = new DataSharpGP2D12_37Sensor(NULL);
  copy->_name = _name;
  copy->_object = _object;
  copy->_pose = _pose;
  copy->_mapping = _mapping;
  return copy;
}

double DataSharpGP2D12_37Sensor::internalValue(int index)
{
  return _internalValue;
}

double DataSharpGP2D12_37Sensor::externalValue(int index)
{
  return _externalValue;
}

void DataSharpGP2D12_37Sensor::setInternalValue(int index, double v)
{
  _internalValue = _internalDomain.cut(v);
  _externalValue = _internalExternalMapping.map(_internalValue);
}

void DataSharpGP2D12_37Sensor::setExternalValue(int index, double v)
{
  _externalValue = _externalDomain.cut(v);
  _internalValue = _internalExternalMapping.invMap(_externalValue);
}

void DataSharpGP2D12_37Sensor::__setMapping()
{
  _externalDomain     = _mapping;
  _internalDomain.min = 0; _internalDomain.max = 1000; // TODO
  _internalExternalMapping.setInputDomain(_internalDomain);
  _internalExternalMapping.setOutputDomain(_externalDomain);
}

void DataSharpGP2D12_37Sensor::_resetTo(const DataSensor *sensor)
{
  DataSharpGP2D12_37Sensor *other = (DataSharpGP2D12_37Sensor*)sensor;
  _name          = other->name();
  _object        = other->object();
  _pose          = other->pose();
  _mapping       = other->mapping();
  _internalValue = _internalDomain.cut(0.0);
  _externalValue = _internalExternalMapping.map(_internalValue);
}
