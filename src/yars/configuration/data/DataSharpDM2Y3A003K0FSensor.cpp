#include "DataSharpDM2Y3A003K0FSensor.h"
#include "DataDomainFactory.h"
#include "DataPoseFactory.h"

# define YARS_STRING_MAPPING            (char*)"mapping"
# define YARS_STRING_POSE               (char*)"pose"
# define YARS_STRING_NAME               (char*)"name"
# define YARS_STRING_OBJECT             (char*)"object"
# define YARS_STRING_POSE_DEFINITION    (char*)"pose_definition"
# define YARS_STRING_MIN_MAX_DEFINITION (char*)"min_max_definition"
# define YARS_STRING_NAME_DEFINITION    (char*)"name_definition"

DataSharpDM2Y3A003K0FSensor::DataSharpDM2Y3A003K0FSensor(DataNode* parent)
  : DataSensor(parent, DATA_SHARP_DM2Y3A003K0F_SENSOR)
{ }

DataSharpDM2Y3A003K0FSensor::~DataSharpDM2Y3A003K0FSensor()
{
  // nothing to be done.
}

void DataSharpDM2Y3A003K0FSensor::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_SHARP_IR_DM2Y3A003K0F))
  {
    __setMapping();
    current = parent;
  }
  if(element->opening(YARS_STRING_SHARP_IR_DM2Y3A003K0F))
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

Pose DataSharpDM2Y3A003K0FSensor::pose()
{
  return _pose;
}

void DataSharpDM2Y3A003K0FSensor::createXsd(XsdSpecification *spec)
{
  XsdSequence *sensor = new XsdSequence(YARS_STRING_SHARP_IR_DM2Y3A003K0F_DEFINITION);
  sensor->add(NA(YARS_STRING_NAME,    YARS_STRING_XSD_STRING,         false));
  sensor->add(XE(YARS_STRING_OBJECT,  YARS_STRING_NAME_DEFINITION,    1, 1));
  sensor->add(XE(YARS_STRING_POSE,    YARS_STRING_POSE_DEFINITION,    1, 1));
  sensor->add(XE(YARS_STRING_MAPPING, YARS_STRING_MIN_MAX_DEFINITION, 1, 1));
  spec->add(sensor);
}

DataSharpDM2Y3A003K0FSensor* DataSharpDM2Y3A003K0FSensor::_copy()
{
  DataSharpDM2Y3A003K0FSensor *copy = new DataSharpDM2Y3A003K0FSensor(NULL);
  copy->_name = _name;
  copy->_object = _object;
  copy->_pose = _pose;
  copy->_mapping = _mapping;
  return copy;
}



double DataSharpDM2Y3A003K0FSensor::internalValue(int index)
{
  return _internalValue;
}

double DataSharpDM2Y3A003K0FSensor::externalValue(int index)
{
  return _externalValue;
}

void DataSharpDM2Y3A003K0FSensor::setInternalValue(int index, double v)
{
  _internalValue = _internalDomain.cut(v);
  _externalValue = _internalExternalMapping.map(_internalValue);
}

void DataSharpDM2Y3A003K0FSensor::setExternalValue(int index, double v)
{
  _externalValue = _externalDomain.cut(v);
  _internalValue = _internalExternalMapping.invMap(_externalValue);
}

void DataSharpDM2Y3A003K0FSensor::__setMapping()
{
  _externalDomain     = _mapping;
  _internalDomain.min = 0;
  _internalDomain.max = 1000; // TODO
  _internalExternalMapping.setInputDomain(_internalDomain);
  _internalExternalMapping.setOutputDomain(_externalDomain);
}


void DataSharpDM2Y3A003K0FSensor::_resetTo(const DataSensor *sensor)
{
  DataSharpDM2Y3A003K0FSensor *other = (DataSharpDM2Y3A003K0FSensor*)sensor;
  _name          = other->name();
  _object        = other->object();
  _pose          = other->pose();
  _mapping       = other->mapping();
  _internalValue = _internalDomain.cut(0.0);
  _externalValue = _internalExternalMapping.map(_internalValue);
}
