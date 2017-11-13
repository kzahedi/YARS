#include "DataCenterOrbitCam.h"

#include "DataPIDFactory.h"

#include <yars/util/macros.h>

# define YARS_STRING_CENTER_ORBIT_CAM_SPEED     (char*)"speed"

DataCenterOrbitCam::DataCenterOrbitCam(DataNode *parent)
  : DataNode(parent)
{
  _speed = 0.01;
}

DataCenterOrbitCam::~DataCenterOrbitCam()
{

}

void DataCenterOrbitCam::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_CENTER_ORBIT_CAM))
  {
    current = parent;
  }

  if(element->opening(YARS_STRING_CENTER_ORBIT_CAM))
  {
    element->set(YARS_STRING_CENTER_ORBIT_CAM_SPEED, _speed);
    _speed = DEG_TO_RAD(_speed);
  }
}

DataCenterOrbitCam* DataCenterOrbitCam::copy()
{
  DataCenterOrbitCam *copy = new DataCenterOrbitCam(NULL);
  copy->_speed = _speed;
  return copy;
}

void DataCenterOrbitCam::createXsd(XsdSpecification *spec)
{
  XsdSequence *orbitCamConfig = new XsdSequence(YARS_STRING_CENTER_ORBIT_CAM_DEFINITION);
  orbitCamConfig->add(NA(YARS_STRING_CENTER_ORBIT_CAM_SPEED,     YARS_STRING_XSD_DECIMAL,    false));
  spec->add(orbitCamConfig);
}

double DataCenterOrbitCam::speed()
{
  return _speed;
}
