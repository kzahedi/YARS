#include <yars/configuration/data/DataCamera.h>

#include <yars/configuration/data/DataPositionFactory.h>

#include <yars/defines/ymath.h>

# define YARS_STRING_POSITION       (char*)"position"
# define YARS_STRING_LOOK_AT        (char*)"lookAt"
# define YARS_STRING_UP             (char*)"up"
# define YARS_STRING_ANGLES         (char*)"angles"
# define YARS_STRING_XYZ_DEFINITION (char*)"xyz_definition"
# define YARS_CAMERA_OPTION_A       (char*)"data_camera_option_a"
# define YARS_CAMERA_OPTION_B       (char*)"data_camera_option_b"

DataCamera::DataCamera(DataNode *parent)
  : DataNode(parent)
{
  _up       = P3D(0,0,1); // standard values
  _position = P3D(1,1,1);
  _lookAt   = P3D(0,0,0);
}

DataCamera::~DataCamera()
{
  // nothing to be done
}

P3D DataCamera::position()
{
  return _position;
}

P3D DataCamera::lookAt()
{
  return _lookAt;
}

P3D DataCamera::up()
{
  return _up;
}

void DataCamera::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_CAMERA))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_POSITION))
  {
    DataPositionFactory::set(_position, element);
  }
  if(element->opening(YARS_STRING_LOOK_AT))
  {
    DataPositionFactory::set(_lookAt, element);
  }
  if(element->opening(YARS_STRING_UP))
  {
    DataPositionFactory::set(_up, element);
  }
}

void DataCamera::createXsd(XsdSpecification *spec)
{
  XsdSequence *options = new XsdSequence(YARS_STRING_SCREEN_CAMERA_DEFINITION);
  options->add(XE(YARS_STRING_POSITION, YARS_STRING_XYZ_DEFINITION, 1, 1));
  options->add(XE(YARS_STRING_LOOK_AT,  YARS_STRING_XYZ_DEFINITION, 1, 1));
  spec->add(options);
}

DataCamera* DataCamera::copy()
{
  DataCamera *copy = new DataCamera(NULL);
  copy->_lookAt    = _lookAt;
  copy->_position  = _position;
  return copy;
}

void DataCamera::setPosition(P3D p)
{
  _position = p;
}

void DataCamera::setLookAt(P3D l)
{
  _lookAt = l;
}
