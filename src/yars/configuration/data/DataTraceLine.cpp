#include <yars/configuration/data/DataTraceLine.h>
#include <yars/configuration/data/DataColourFactory.h>
#include <yars/configuration/data/DataPositionFactory.h>

#include <yars/configuration/YarsConfiguration.h>

# define YARS_STRING_TARGET                        (char*)"target"
# define YARS_STRING_COLOR                         (char*)"colour"
# define YARS_STRING_OFFSET                        (char*)"offset"
# define YARS_STRING_OFFSET_DEFINITION             (char*)"offset_divider_definition"
# define YARS_STRING_INIT                          (char*)"init"
# define YARS_STRING_INIT_DEFINITION               (char*)"init_traceline_divider_definition"
# define YARS_STRING_FINAL                         (char*)"final"
# define YARS_STRING_FINAL_DEFINITION              (char*)"final_traceline_divider_definition"
# define YARS_STRING_X                             (char*)"x"
# define YARS_STRING_Y                             (char*)"y"
# define YARS_STRING_Z                             (char*)"z"
# define YARS_STRING_SIZE                          (char*)"size"
# define YARS_STRING_COLOR_HEX_DEFINITION          (char*)"colour_hex_divider_definition"
# define YARS_STRING_LENGTH                        (char*)"length"
# define YARS_STRING_NR_OF_ELEMENTS                (char*)"max"
# define YARS_STRING_LOCAL                         (char*)"local"
# define YARS_STRING_COLOR_HEX_REG_EXP_DEFINITION  (char*)"colour_hex_rgba_definition"
# define YARS_STRING_XSD_HEX_COLOR                 (char*)"[A-Fa-f0-9]{6}([A-Fa-f0-9]{2}){0,1}"
# define YARS_STRING_TRUE_FALSE_DEFINITION         (char*)"true_false_divider_definition"
# define YARS_STRING_POSITIVE_NON_ZERO_INTEGER     (char*)"positive_non_zero_integer_divider_definition"
# define YARS_STRING_NAME                          (char*)"name"
# define YARS_STRING_LINE                          (char*)"line"
# define YARS_STRING_TIME                          (char*)"time"
# define YARS_STRING_PARTICLES                     (char*)"particles"
# define YARS_STRING_BILLBOARD                     (char*)"billboard"
# define YARS_STRING_TEXTURE                       (char*)"texture"

# define YARS_STRING_PROJECT                       (char*)"project"
# define YARS_STRING_PROJECT_XY                    (char*)"xy"
# define YARS_STRING_PROJECT_XZ                    (char*)"xz"
# define YARS_STRING_PROJECT_YZ                    (char*)"yz"

# define YARS_STRING_PLANES_DEFINITION             (char*)"planes_definition"

DataTraceLine::DataTraceLine(DataNode *parent)
  : DataNode(parent)
{
  _lineWidth       = 0.1;
  _finalWidth      = 0.0;
  _max             = 1;
  _length          = 10;
  _local           = true; // coordinates in local coordinate system
  _texture         = "YARS/TraceLine";
  _particles       = "YARS/TraceParticles";
  _billboard       = "YARS/FlyingLightMaterial";
  _useParticles    = true;
  _useLine         = true;
  _useBillboard    = true;
  _time            = 1.0;
  _nrOfElements    = 100;
  _project         = false;
  _projectionPlane = -1;
  set_capacity(1);
}

void DataTraceLine::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_TRACE))
  {
    current = parent;
  }

  if(element->opening(YARS_STRING_TRACE))
  {
    element->set(YARS_STRING_TARGET,         _target);
    element->set(YARS_STRING_LENGTH,         _length);
    element->set(YARS_STRING_NR_OF_ELEMENTS, _nrOfElements);
    element->set(YARS_STRING_TIME,           _time);
    element->set(YARS_STRING_PARTICLES,      _useParticles);
    element->set(YARS_STRING_LINE,           _useLine);
    element->set(YARS_STRING_BILLBOARD,      _useBillboard);
    if(element->hasAttriute(YARS_STRING_PROJECT))
    {
      string s;
      element->set(YARS_STRING_PROJECT, s);
      if(s == YARS_STRING_PROJECT_XY) _projectionPlane = DATA_TRACE_PROJECT_XY;
      if(s == YARS_STRING_PROJECT_XZ) _projectionPlane = DATA_TRACE_PROJECT_XZ;
      if(s == YARS_STRING_PROJECT_YZ) _projectionPlane = DATA_TRACE_PROJECT_YZ;
      _project = true;
    }
  }


  if(element->opening(YARS_STRING_OFFSET))
  {
    element->set(YARS_STRING_LOCAL, _local);
    element->set(YARS_STRING_X,     _offset.x);
    element->set(YARS_STRING_Y,     _offset.y);
    element->set(YARS_STRING_Z,     _offset.z);
  }

  if(element->opening(YARS_STRING_TEXTURE))   _texture   = element->attribute(YARS_STRING_NAME)->value();
  if(element->opening(YARS_STRING_PARTICLES)) _particles = element->attribute(YARS_STRING_NAME)->value();
  if(element->opening(YARS_STRING_BILLBOARD)) _billboard = element->attribute(YARS_STRING_NAME)->value();

  if(element->opening(YARS_STRING_INIT))
  {
    element->set(YARS_STRING_SIZE,   _lineWidth);
    DataColourFactory::set(_color, element->attribute(YARS_STRING_COLOR)->value());
  }

  if(element->opening(YARS_STRING_FINAL))
  {
    element->set(YARS_STRING_SIZE, _finalWidth);
    DataColourFactory::set(_finalColour, element->attribute(YARS_STRING_COLOR)->value());
  }

}

void DataTraceLine::createXsd(XsdSpecification *spec)
{
  XsdSequence *line = new XsdSequence(YARS_STRING_TRACE_LINE_DEFINITION);
  line->add(NA(YARS_STRING_TARGET,         YARS_STRING_XSD_STRING,                true));
  line->add(NA(YARS_STRING_LENGTH,         YARS_STRING_POSITIVE_NON_ZERO_DECIMAL, false));
  line->add(NA(YARS_STRING_NR_OF_ELEMENTS, YARS_STRING_POSITIVE_NON_ZERO_INTEGER, false));
  line->add(NA(YARS_STRING_TIME,           YARS_STRING_POSITIVE_NON_ZERO_DECIMAL, false));
  line->add(NA(YARS_STRING_LINE,           YARS_STRING_TRUE_FALSE_DEFINITION,     false));
  line->add(NA(YARS_STRING_PARTICLES,      YARS_STRING_TRUE_FALSE_DEFINITION,     false));
  line->add(NA(YARS_STRING_BILLBOARD,      YARS_STRING_TRUE_FALSE_DEFINITION,     false));
  line->add(NA(YARS_STRING_PROJECT,        YARS_STRING_PLANES_DEFINITION,         false));

  line->add(NE(YARS_STRING_INIT,           YARS_STRING_INIT_DEFINITION,           0, 1));
  line->add(NE(YARS_STRING_FINAL,          YARS_STRING_FINAL_DEFINITION,          0, 1));
  line->add(NE(YARS_STRING_OFFSET,         YARS_STRING_OFFSET_DEFINITION,         0, 1));
  line->add(NE(YARS_STRING_TEXTURE,        YARS_STRING_NAME_DEFINITION,           0, 1));
  line->add(NE(YARS_STRING_PARTICLES,      YARS_STRING_NAME_DEFINITION,           0, 1));
  line->add(NE(YARS_STRING_BILLBOARD,      YARS_STRING_NAME_DEFINITION,           0, 1));
  spec->add(line);

  XsdSequence *init = new XsdSequence(YARS_STRING_INIT_DEFINITION);
  init->add(NA(YARS_STRING_COLOR, YARS_STRING_COLOR_HEX_REG_EXP_DEFINITION, false));
  init->add(NA(YARS_STRING_SIZE,  YARS_STRING_POSITIVE_NON_ZERO_DECIMAL, false));
  spec->add(init);

  XsdSequence *final = new XsdSequence(YARS_STRING_FINAL_DEFINITION);
  final->add(NA(YARS_STRING_COLOR, YARS_STRING_COLOR_HEX_REG_EXP_DEFINITION, false));
  final->add(NA(YARS_STRING_SIZE,  YARS_STRING_POSITIVE_DECIMAL, false));
  spec->add(final);

  XsdSequence *offset = new XsdSequence(YARS_STRING_OFFSET_DEFINITION);
  offset->add(NA(YARS_STRING_LOCAL, YARS_STRING_TRUE_FALSE_DEFINITION, false));
  offset->add(NA(YARS_STRING_X,     YARS_STRING_XSD_DECIMAL,           false));
  offset->add(NA(YARS_STRING_Y,     YARS_STRING_XSD_DECIMAL,           false));
  offset->add(NA(YARS_STRING_Z,     YARS_STRING_XSD_DECIMAL,           false));
  spec->add(offset);

  XsdEnumeration *planes = new XsdEnumeration(YARS_STRING_PLANES_DEFINITION, YARS_STRING_XSD_STRING);
  planes->add(YARS_STRING_PROJECT_XZ);
  planes->add(YARS_STRING_PROJECT_YZ);
  planes->add(YARS_STRING_PROJECT_XY);
  spec->add(planes);

  XsdRegularExpression *colorHex = new XsdRegularExpression(YARS_STRING_COLOR_HEX_REG_EXP_DEFINITION,
                                                            YARS_STRING_XSD_STRING,
                                                            YARS_STRING_XSD_HEX_COLOR);
  spec->add(colorHex);
}

string DataTraceLine::target()
{
  return _target;
}

double DataTraceLine::lineWidth()
{
  return _lineWidth;
}

DataTraceLine* DataTraceLine::copy()
{
  DataTraceLine *copy    = new DataTraceLine(NULL);
  copy->_target          = _target;
  copy->_lineWidth       = _lineWidth;
  copy->_finalWidth      = _finalWidth;
  copy->_length          = _length;
  copy->_time            = _time;
  copy->_texture         = _texture;
  copy->_particles       = _particles;
  copy->_billboard       = _billboard;
  copy->_finalColour     = _finalColour;
  copy->_position        = _position;
  copy->_useLine         = _useLine;
  copy->_useParticles    = _useParticles;
  copy->_useBillboard    = _useBillboard;
  copy->_color           = _color;
  copy->_offset          = _offset;
  copy->_project         = _project;
  copy->_projectionPlane = _projectionPlane;
  copy->_nrOfElements    = _nrOfElements;
  copy->set_capacity(1);
  return copy;
}

P3D DataTraceLine::position()
{
  return back();
}

double DataTraceLine::finalWidth()
{
  return _finalWidth;
}

Colour DataTraceLine::finalColour()
{
  return _finalColour;
}

double DataTraceLine::length()
{
  return _length;
}

double DataTraceLine::time()
{
  return _time;
}

string DataTraceLine::texture()
{
  return _texture;
}

string DataTraceLine::billboard()
{
  return _billboard;
}

string DataTraceLine::particles()
{
  return _particles;
}

bool DataTraceLine::useParticles()
{
  return _useParticles;
}

bool DataTraceLine::useLine()
{
  return _useLine;
}

bool DataTraceLine::useBillboard()
{
  return _useBillboard;
}

int DataTraceLine::nrOfElements()
{
  return _nrOfElements;
}

bool DataTraceLine::useProjection()
{
  return _project;
}

int DataTraceLine::projectionPlane()
{
  return _projectionPlane;
}
