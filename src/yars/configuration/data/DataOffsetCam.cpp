#include "DataOffsetCam.h"

#include "DataPIDFactory.h"

# define YARS_STRING_PID_DEFINITION       (char*)"pid_definition"
# define YARS_STRING_P                    (char*)"p"
# define YARS_STRING_I                    (char*)"i"
# define YARS_STRING_D                    (char*)"d"

# define YARS_STRING_OFFSET_CAM_LOOK_AT_X (char*)"lookAtX"
# define YARS_STRING_OFFSET_CAM_LOOK_AT_Y (char*)"lookAtY"
# define YARS_STRING_OFFSET_CAM_LOOK_AT_Z (char*)"lookAtZ"
# define YARS_STRING_OFFSET_CAM_FROM_X    (char*)"fromX"
# define YARS_STRING_OFFSET_CAM_FROM_Y    (char*)"fromY"
# define YARS_STRING_OFFSET_CAM_FROM_Z    (char*)"fromZ"
# define YARS_STRING_OFFSET_CAM_SPEED     (char*)"speed"

DataOffsetCam::DataOffsetCam(DataNode *parent)
  : DataNode(parent),
  _lookAtX(1, 0, 0, 0, 100),
  _lookAtY(1, 0, 0, 0, 100),
  _lookAtZ(1, 0, 0, 0, 100),
  _fromX(  1, 0, 0, 0, 100),
  _fromY(  1, 0, 0, 0, 100),
  _fromZ(  1, 0, 0, 0, 100)
{
}

DataOffsetCam::~DataOffsetCam()
{

}

void DataOffsetCam::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_OFFSET_CAM))
  {
    current = parent;
  }

  if(element->opening(YARS_STRING_OFFSET_CAM))
  {
    // nothing to do
  }

  if(element->opening(YARS_STRING_OFFSET_CAM_LOOK_AT_X))
  {
    DataPIDFactory::set(_lookAtX, element);
  }

  if(element->opening(YARS_STRING_OFFSET_CAM_LOOK_AT_Y))
  {
    DataPIDFactory::set(_lookAtY, element);
  }

  if(element->opening(YARS_STRING_OFFSET_CAM_LOOK_AT_Z))
  {
    DataPIDFactory::set(_lookAtZ, element);
  }

  if(element->opening(YARS_STRING_OFFSET_CAM_FROM_X))
  {
    DataPIDFactory::set(_fromX, element);
  }

  if(element->opening(YARS_STRING_OFFSET_CAM_FROM_Y))
  {
    DataPIDFactory::set(_fromY, element);
  }

  if(element->opening(YARS_STRING_OFFSET_CAM_FROM_Z))
  {
    DataPIDFactory::set(_fromZ, element);
  }
}

DataOffsetCam* DataOffsetCam::copy()
{
  DataOffsetCam *copy = new DataOffsetCam(NULL);
  copy->_lookAtX = _lookAtX;
  copy->_lookAtY = _lookAtY;
  copy->_lookAtZ = _lookAtZ;
  copy->_fromX   = _fromX;
  copy->_fromY   = _fromY;
  copy->_fromZ   = _fromZ;
  return copy;
}

void DataOffsetCam::createXsd(XsdSpecification *spec)
{
  XsdSequence *offsetCamConfig = new XsdSequence(YARS_STRING_OFFSET_CAM_DEFINITION);
  offsetCamConfig->add(XE(YARS_STRING_OFFSET_CAM_LOOK_AT_X, YARS_STRING_PID_DEFINITION, 0, 1));
  offsetCamConfig->add(XE(YARS_STRING_OFFSET_CAM_LOOK_AT_Y, YARS_STRING_PID_DEFINITION, 0, 1));
  offsetCamConfig->add(XE(YARS_STRING_OFFSET_CAM_LOOK_AT_Z, YARS_STRING_PID_DEFINITION, 0, 1));
  offsetCamConfig->add(XE(YARS_STRING_OFFSET_CAM_FROM_X,    YARS_STRING_PID_DEFINITION, 0, 1));
  offsetCamConfig->add(XE(YARS_STRING_OFFSET_CAM_FROM_Y,    YARS_STRING_PID_DEFINITION, 0, 1));
  offsetCamConfig->add(XE(YARS_STRING_OFFSET_CAM_FROM_Z,    YARS_STRING_PID_DEFINITION, 0, 1));
  spec->add(offsetCamConfig);
}

PID DataOffsetCam::lookAtX()
{
  return _lookAtX;
}

PID DataOffsetCam::lookAtY()
{
  return _lookAtY;
}

PID DataOffsetCam::lookAtZ()
{
  return _lookAtZ;
}

PID DataOffsetCam::fromX()
{
  return _fromX;
}

PID DataOffsetCam::fromY()
{
  return _fromY;
}

PID DataOffsetCam::fromZ()
{
  return _fromZ;
}
