#include "DataMeshVisualisation.h"
#include "DataPoseFactory.h"

# define YARS_STRING_NAME            (char*)"name"

# define YARS_STRING_X               (char*)"x"
# define YARS_STRING_Y               (char*)"y"
# define YARS_STRING_Z               (char*)"z"

# define YARS_STRING_POSE            (char*)"pose"
# define YARS_STRING_POSE_DEFINITION (char*)"pose" DIVIDER DEFINITION

# define YARS_STRING_SCALE            (char*)"scale"
# define YARS_STRING_SCALE_DEFINITION (char*)"scale" DIVIDER DEFINITION

#define YARS_STRING_TEXTURE         (char*)"texture"
#define YARS_STRING_NAME            (char*)"name"

DataMeshVisualisation::DataMeshVisualisation(DataNode *parent)
  : DataNode(parent)
{
  _use     = false;
  _scale   = 1;
  _texture = "";
}

DataMeshVisualisation::~DataMeshVisualisation() { }

void DataMeshVisualisation::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_OBJECT_MESH_VISUALISATION))
  {
    current = parent;
  }

  if(element->opening(YARS_STRING_OBJECT_MESH_VISUALISATION))
    _name = element->attribute(YARS_STRING_NAME)->value();

  if(element->opening(YARS_STRING_POSE))    DataPoseFactory::set(_pose, element);
  if(element->opening(YARS_STRING_TEXTURE)) _texture = element->attribute(YARS_STRING_NAME)->value();

  if(element->opening(YARS_STRING_SCALE))
  {
    element->set(YARS_STRING_X, _scale.x);
    element->set(YARS_STRING_Y, _scale.y);
    element->set(YARS_STRING_Z, _scale.z);
  }
}

void DataMeshVisualisation::createXsd(XsdSpecification *spec)
{

  XsdSequence *meshDefinition = new XsdSequence(YARS_STRING_OBJECT_MESH_VISUALISATION_DEFINTION);
  meshDefinition->add(NA(YARS_STRING_NAME,    YARS_STRING_XSD_STRING,       true));
  meshDefinition->add(NE(YARS_STRING_TEXTURE, YARS_STRING_NAME_DEFINITION, 0, 1));
  meshDefinition->add(NE(YARS_STRING_SCALE,   YARS_STRING_SCALE_DEFINITION, 0, 1));
  meshDefinition->add(NE(YARS_STRING_POSE,    YARS_STRING_POSE_DEFINITION,  0, 1));
  spec->add(meshDefinition);

  XsdSequence *scaleDefinition = new XsdSequence(YARS_STRING_SCALE_DEFINITION);
  scaleDefinition->add(NA(YARS_STRING_X, YARS_STRING_POSITIVE_NON_ZERO_DECIMAL, false));
  scaleDefinition->add(NA(YARS_STRING_Y, YARS_STRING_POSITIVE_NON_ZERO_DECIMAL, false));
  scaleDefinition->add(NA(YARS_STRING_Z, YARS_STRING_POSITIVE_NON_ZERO_DECIMAL, false));
  spec->add(scaleDefinition);
}

DataMeshVisualisation* DataMeshVisualisation::copy()
{
  DataMeshVisualisation* copy = new DataMeshVisualisation(NULL);
  copy->_name    = _name;
  copy->_pose    = _pose;
  copy->_scale   = _scale;
  copy->_use     = _use;
  copy->_texture = _texture;
  return copy;
}

P3D DataMeshVisualisation::scale()
{
  return _scale;
}

string DataMeshVisualisation::name()
{
  return _name;
}

bool DataMeshVisualisation::use()
{
  return _use;
}

Pose DataMeshVisualisation::pose()
{
  return _pose;
}

string DataMeshVisualisation::texture()
{
  return _texture;
}
