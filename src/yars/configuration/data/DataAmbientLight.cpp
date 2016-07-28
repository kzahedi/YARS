#include "DataAmbientLight.h"
#include "DataColourFactory.h"
#include "DataPositionFactory.h"

# define YARS_STRING_BRIGHTNESS  (char*)"brightness"

# define YARS_STRING_COLOR (char*)"colour"
# define YARS_STRING_COLOR_HEX_REG_EXP_DEFINITION  (char*)"colour_hex_rgb_definition"
# define YARS_STRING_XSD_HEX_COLOR                 (char*)"[A-Fa-f0-9]{6}"

DataAmbientLight::DataAmbientLight(DataNode *parent)
  : DataNode(parent)
{ }

DataAmbientLight::~DataAmbientLight()
{

}

void DataAmbientLight::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_AMBIENT_LIGHT))
  {
    current = parent;
  }

  if(element->opening(YARS_STRING_AMBIENT_LIGHT))
  {
    DataPositionFactory::set(_position, element);
    DataColourFactory::set(_color, element, YARS_STRING_COLOR);
    element->set(YARS_STRING_BRIGHTNESS, _brightness);
  }
}

P3D DataAmbientLight::position()
{
  return _position;
}


Colour DataAmbientLight::color()
{
  return _color;
}

yReal DataAmbientLight::brightness()
{
  return _brightness;
}

void DataAmbientLight::createXsd(XsdSpecification *spec)
{
  XsdSequence *lightSourceDefinition = new XsdSequence(YARS_STRING_AMBIENT_LIGHT_DEFINITION);
  lightSourceDefinition->add(NA(YARS_STRING_COLOR,      YARS_STRING_COLOR_HEX_REG_EXP_DEFINITION, true));
  lightSourceDefinition->add(NA(YARS_STRING_BRIGHTNESS, YARS_STRING_UNIT_INTERVAL,                true));
  spec->add(lightSourceDefinition);

  XsdRegularExpression *colorHex =
    new XsdRegularExpression(YARS_STRING_COLOR_HEX_REG_EXP_DEFINITION,
                             YARS_STRING_XSD_STRING, YARS_STRING_XSD_HEX_COLOR);
  spec->add(colorHex);
}


DataAmbientLight* DataAmbientLight::copy()
{
  DataAmbientLight *copy = new DataAmbientLight(NULL);
  copy->_position       = _position;
  copy->_color          = _color;
  copy->_brightness     = _brightness;
  return copy;
}
