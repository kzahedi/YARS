#include <yars/configuration/data/DataTracePoint.h>
#include <yars/configuration/data/DataColourFactory.h>
#include <yars/configuration/data/DataPositionFactory.h>

#include <yars/configuration/YarsConfiguration.h>

# define YARS_STRING_TARGET                       (char*)"target"
# define YARS_STRING_COLOR                        (char*)"colour"
# define YARS_STRING_X                            (char*)"x"
# define YARS_STRING_Y                            (char*)"y"
# define YARS_STRING_Z                            (char*)"z"
# define YARS_STRING_SIZE                         (char*)"size"
# define YARS_STRING_COLOR_HEX_DEFINITION         (char*)"colour_hex_divider_definition"
# define YARS_STRING_STEP                         (char*)"step"
# define YARS_STRING_MAX                          (char*)"max"
# define YARS_STRING_FADE                         (char*)"fade"
# define YARS_STRING_LOCAL                        (char*)"local"
# define YARS_STRING_COLOR_HEX_REG_EXP_DEFINITION (char*)"colour_hex_rgba_definition"
# define YARS_STRING_XSD_HEX_COLOR                (char*)"[A-Fa-f0-9]{6}([A-Fa-f0-9]{2}){0,1}"
# define YARS_STRING_TRUE_FALSE_DEFINITION        (char*)"true_false_divider_definition"
# define YARS_STRING_POSITIVE_NON_ZERO_INTEGER    (char*)"positive_non_zero_integer_divider_definition"

DataTracePoint::DataTracePoint(DataNode *parent)
  : DataNode(parent)
{
  _pointSize = 10;
  _max       = 100;
  _step      = 0;
  _fade      = true;
  _local     = true; // coordinates in local coordinate system
}

void DataTracePoint::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_POINT))
  {
    set_capacity(_max);
    current = parent;
  }

  if(element->opening(YARS_STRING_POINT))
  {
    element->set(YARS_STRING_TARGET, _target);
    element->set(YARS_STRING_MAX,    _max);
    element->set(YARS_STRING_STEP,   _step);
    element->set(YARS_STRING_FADE,   _fade);
    element->set(YARS_STRING_LOCAL,  _local);
    element->set(YARS_STRING_X,      _offset.x);
    element->set(YARS_STRING_Y,      _offset.y);
    element->set(YARS_STRING_Z,      _offset.z);
    element->set(YARS_STRING_SIZE,   _pointSize);
    DataColourFactory::set(_color, element->attribute(YARS_STRING_COLOR)->value());
  }
}

void DataTracePoint::createXsd(XsdSpecification *spec)
{
  XsdSequence *point = new XsdSequence(YARS_STRING_TRACE_POINT_DEFINITION);
  point->add(NA(YARS_STRING_TARGET, YARS_STRING_XSD_STRING,                   true));
  point->add(NA(YARS_STRING_COLOR,  YARS_STRING_COLOR_HEX_REG_EXP_DEFINITION, true));
  point->add(NA(YARS_STRING_SIZE,   YARS_STRING_XSD_DECIMAL,                  true));
  point->add(NA(YARS_STRING_STEP,   YARS_STRING_POSITIVE_NON_ZERO_INTEGER,    false));
  point->add(NA(YARS_STRING_MAX,    YARS_STRING_POSITIVE_NON_ZERO_INTEGER,    false));
  point->add(NA(YARS_STRING_FADE,   YARS_STRING_TRUE_FALSE_DEFINITION,        false));
  point->add(NA(YARS_STRING_LOCAL,  YARS_STRING_TRUE_FALSE_DEFINITION,        false));
  point->add(NA(YARS_STRING_X,      YARS_STRING_XSD_DECIMAL,                  false));
  point->add(NA(YARS_STRING_Y,      YARS_STRING_XSD_DECIMAL,                  false));
  point->add(NA(YARS_STRING_Z,      YARS_STRING_XSD_DECIMAL,                  false));
  spec->add(point);

  XsdRegularExpression *colorHex = new XsdRegularExpression(YARS_STRING_COLOR_HEX_REG_EXP_DEFINITION,
                                                            YARS_STRING_XSD_STRING,
                                                            YARS_STRING_XSD_HEX_COLOR);
  spec->add(colorHex);
}

string DataTracePoint::target()
{
  return _target;
}

double DataTracePoint::pointSize()
{
  return _pointSize;
}

DataTracePoint* DataTracePoint::copy()
{
  DataTracePoint *copy = new DataTracePoint(NULL);
  copy->_target       = _target;
  copy->_color        = _color;
  copy->_offset       = _offset;
  copy->_fade         = _fade;
  copy->_step         = _step;
  copy->_max          = _max;
  copy->_pointSize    = _pointSize;
  copy->set_capacity(capacity());
  return copy;
}


