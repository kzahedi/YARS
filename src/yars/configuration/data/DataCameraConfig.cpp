#include "DataCameraConfig.h"

DataCameraConfig::DataCameraConfig(DataNode *parent)
  : DataNode(parent)
{
  _orbit  = new DataOrbitCam(this);
  _center = new DataCenterOrbitCam(this);
  _offset = new DataOffsetCam(this);
}

DataCameraConfig::~DataCameraConfig()
{

}

void DataCameraConfig::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_CAMERA_CONFIG))
  {
    current = parent;
  }

  if(element->opening(YARS_STRING_ORBIT_CAM))
  {
    current = _orbit;
    current->add(element);
  }
  if(element->opening(YARS_STRING_CENTER_ORBIT_CAM))
  {
    current = _center;
    current->add(element);
  }
  if(element->opening(YARS_STRING_OFFSET_CAM))
  {
    current = _offset;
    current->add(element);
  }
}

DataCameraConfig* DataCameraConfig::copy()
{
  DataCameraConfig *copy = new DataCameraConfig(NULL);
  copy->_orbit  = _orbit->copy();
  copy->_center = _center->copy();
  copy->_offset = _offset->copy();
  return copy;
}

DataOrbitCam* DataCameraConfig::orbit()
{
  return _orbit;
}

DataCenterOrbitCam* DataCameraConfig::center()
{
  return _center;
}

DataOffsetCam* DataCameraConfig::offset()
{
  return _offset;
}
