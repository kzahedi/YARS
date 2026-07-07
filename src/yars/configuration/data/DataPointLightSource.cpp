#include "DataPointLightSource.h"
#include "DataColourFactory.h"
#include "DataPositionFactory.h"

# define YARS_STRING_X                            (char*)"x"
# define YARS_STRING_Y                            (char*)"y"
# define YARS_STRING_Z                            (char*)"z"
# define YARS_STRING_BRIGHTNESS                   (char*)"brightness"

# define YARS_STRING_DRAW                         (char*)"draw"
# define YARS_STRING_SIZE                         (char*)"size"
# define YARS_STRING_NAME                         (char*)"name"

# define YARS_STRING_COLOR                        (char*)"colour"
# define YARS_STRING_COLOR_HEX_REG_EXP_DEFINITION (char*)"colour_hex_rgb_definition"
# define YARS_STRING_XSD_HEX_COLOR                (char*)"[A-Fa-f0-9]{6}"

DataPointLightSource::DataPointLightSource(DataNode *parent)
  : DataNode(parent)
{
  _draw = false;
  _size = 0.1;
}

DataPointLightSource::~DataPointLightSource()
{

}

void DataPointLightSource::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_LIGHT_SOURCE))
  {
    current = parent;
  }

  if(element->opening(YARS_STRING_LIGHT_SOURCE))
  {
    DataPositionFactory::set(_position, element);
    DataColourFactory::set(_color, element, YARS_STRING_COLOR);
    element->set(YARS_STRING_NAME, _name);
    element->set(YARS_STRING_DRAW, _draw);
    element->set(YARS_STRING_SIZE, _size);
    element->set(YARS_STRING_BRIGHTNESS, _brightness);
  }
}

P3D DataPointLightSource::position()
{
  return _position;
}

Colour DataPointLightSource::colour()
{
  return _color;
}

double DataPointLightSource::brightness()
{
  return _brightness;
}

DataPointLightSource* DataPointLightSource::copy()
{
  DataPointLightSource *copy = new DataPointLightSource(NULL);
  copy->_position       = _position;
  copy->_color          = _color;
  copy->_brightness     = _brightness;
  copy->_name           = _name;
  copy->_size           = _size;
  return copy;
}


bool DataPointLightSource::draw()
{
  return _draw;
}

double DataPointLightSource::size()
{
  return _size;
}

string DataPointLightSource::name()
{
  return _name;
}
