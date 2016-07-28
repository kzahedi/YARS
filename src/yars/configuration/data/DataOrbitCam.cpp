#include "DataOrbitCam.h"

#include "DataPIDFactory.h"

#include "yars/util/macros.h"

# define YARS_STRING_ORBIT_CAM_LOOK_AT_X (char*)"lookAtX"
# define YARS_STRING_ORBIT_CAM_LOOK_AT_Y (char*)"lookAtY"
# define YARS_STRING_ORBIT_CAM_LOOK_AT_Z (char*)"lookAtZ"
# define YARS_STRING_ORBIT_CAM_FROM_X    (char*)"fromX"
# define YARS_STRING_ORBIT_CAM_FROM_Y    (char*)"fromY"
# define YARS_STRING_ORBIT_CAM_FROM_Z    (char*)"fromZ"
# define YARS_STRING_ORBIT_CAM_SPEED     (char*)"speed"

DataOrbitCam::DataOrbitCam(DataNode *parent)
  : DataNode(parent),
  _lookAtX(1, 0, 0, 0, 100),
  _lookAtY(1, 0, 0, 0, 100),
  _lookAtZ(1, 0, 0, 0, 100),
  _fromX(  1, 0, 0, 0, 100),
  _fromY(  1, 0, 0, 0, 100),
  _fromZ(  1, 0, 0, 0, 100)
{
  _speed = 0.01;
}

DataOrbitCam::~DataOrbitCam()
{

}

void DataOrbitCam::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_ORBIT_CAM))
  {
    current = parent;
  }

  if(element->opening(YARS_STRING_ORBIT_CAM))
  {
    element->set(YARS_STRING_ORBIT_CAM_SPEED, _speed);
    _speed = DEG_TO_RAD(_speed);
  }

  if(element->opening(YARS_STRING_ORBIT_CAM_LOOK_AT_X))
  {
    DataPIDFactory::set(_lookAtX, element);
  }

  if(element->opening(YARS_STRING_ORBIT_CAM_LOOK_AT_Y))
  {
    DataPIDFactory::set(_lookAtY, element);
  }

  if(element->opening(YARS_STRING_ORBIT_CAM_LOOK_AT_Z))
  {
    DataPIDFactory::set(_lookAtZ, element);
  }

  if(element->opening(YARS_STRING_ORBIT_CAM_FROM_X))
  {
    DataPIDFactory::set(_fromX, element);
  }

  if(element->opening(YARS_STRING_ORBIT_CAM_FROM_Y))
  {
    DataPIDFactory::set(_fromY, element);
  }

  if(element->opening(YARS_STRING_ORBIT_CAM_FROM_Z))
  {
    DataPIDFactory::set(_fromZ, element);
  }
}

DataOrbitCam* DataOrbitCam::copy()
{
  DataOrbitCam *copy = new DataOrbitCam(NULL);
  copy->_lookAtX = _lookAtX;
  copy->_lookAtY = _lookAtY;
  copy->_lookAtZ = _lookAtZ;
  copy->_fromX   = _fromX;
  copy->_fromY   = _fromY;
  copy->_fromZ   = _fromZ;
  copy->_speed   = _speed;
  return copy;
}

void DataOrbitCam::createXsd(XsdSpecification *spec)
{
  XsdSequence *orbitCamConfig = new XsdSequence(YARS_STRING_ORBIT_CAM_DEFINITION);
  orbitCamConfig->add(NE(YARS_STRING_ORBIT_CAM_LOOK_AT_X, YARS_STRING_PID_DEFINITION, 0, 1));
  orbitCamConfig->add(NE(YARS_STRING_ORBIT_CAM_LOOK_AT_Y, YARS_STRING_PID_DEFINITION, 0, 1));
  orbitCamConfig->add(NE(YARS_STRING_ORBIT_CAM_LOOK_AT_Z, YARS_STRING_PID_DEFINITION, 0, 1));
  orbitCamConfig->add(NE(YARS_STRING_ORBIT_CAM_FROM_X,    YARS_STRING_PID_DEFINITION, 0, 1));
  orbitCamConfig->add(NE(YARS_STRING_ORBIT_CAM_FROM_Y,    YARS_STRING_PID_DEFINITION, 0, 1));
  orbitCamConfig->add(NE(YARS_STRING_ORBIT_CAM_FROM_Z,    YARS_STRING_PID_DEFINITION, 0, 1));
  orbitCamConfig->add(NA(YARS_STRING_ORBIT_CAM_SPEED,     YARS_STRING_XSD_DECIMAL,    false));
  spec->add(orbitCamConfig);
}

PID DataOrbitCam::lookAtX()
{
  return _lookAtX;
}

PID DataOrbitCam::lookAtY()
{
  return _lookAtY;
}

PID DataOrbitCam::lookAtZ()
{
  return _lookAtZ;
}

PID DataOrbitCam::fromX()
{
  return _fromX;
}

PID DataOrbitCam::fromY()
{
  return _fromY;
}

PID DataOrbitCam::fromZ()
{
  return _fromZ;
}

yReal DataOrbitCam::speed()
{
  return _speed;
}

