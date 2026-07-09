#include "DataCylinder.h"
#include "DataBinding.h"
#include "DataPoseFactory.h"
#include "DataDimensionFactory.h"

#define YARS_STRING_DIMENSION (char *)"dimension"
#define YARS_STRING_POSE (char *)"pose"
#define YARS_STRING_NAME (char *)"name"
#define YARS_STRING_VISUALISE (char *)"visualise"
#define YARS_STRING_POSE_DEFINITION (char *)"pose_definition"

#define YARS_STRING_TEXTURE (char *)"texture"
#define YARS_STRING_TOP_TEXTURE (char *)"top"
#define YARS_STRING_BOTTOM_TEXTURE (char *)"bottom"
#define YARS_STRING_BODY_TEXTURE (char *)"body"

#define OPTION_1 "option1"
#define OPTION_2 "option2"
#define OPTION_3 "option3"

DataCylinder::DataCylinder(DataNode *parent)
    : DataObject(parent, DATA_OBJECT_CYLINDER)
{
  _texture.resize(3);
}

DataCylinder::DataCylinder(DataNode *parent, int type)
    : DataObject(parent, type)
{
}

DataCylinder::~DataCylinder()
{
}

RHDimension DataCylinder::dimension()
{
  return _dimension;
}

namespace
{
// Attribute binding table for the cylinder's own opening tag. Child-element
// dispatch (pose/dimension/physics/texture/mesh) stays hand-written below.
const std::vector<yars::AttributeBinding> &cylinderAttributeBindings()
{
  static const std::vector<yars::AttributeBinding> bindings = {
      {YARS_STRING_NAME,
       [](DataNode *self, const std::string &value)
       { static_cast<DataCylinder *>(self)->setName(value); },
       /*required=*/false, /*defaultValue=*/nullptr},
      {YARS_STRING_VISUALISE,
       [](DataNode *self, const std::string &value)
       { static_cast<DataCylinder *>(self)->setVisualise(value == "true"); },
       /*required=*/false, /*defaultValue=*/nullptr},
  };
  return bindings;
}
} // namespace

void DataCylinder::add(DataParseElement *element)
{
  if (element->closing(YARS_STRING_OBJECT_CYLINDER))
  {
    current = parent;
  }
  if (element->opening(YARS_STRING_OBJECT_CYLINDER))
  {
    yars::applyAttributes(this, element, cylinderAttributeBindings());
  }
  if (element->opening(YARS_STRING_POSE))
  {
    DataPoseFactory::set(_pose, element);
  }
  if (element->opening(YARS_STRING_DIMENSION))
  {
    DataDimensionFactory::set(_dimension, element);
  }
  if (element->opening(YARS_STRING_PHYSICS))
  {
    _physics->add(element);
    current = _physics;
  }

  if (element->opening(YARS_STRING_TEXTURE))
  {
    for (int i = 0; i < 3; i++)
      _texture[i] = element->requiredAttribute(YARS_STRING_NAME)->value();
  }
  if (element->opening(YARS_STRING_TOP_TEXTURE))
    _texture[0] = element->requiredAttribute(YARS_STRING_NAME)->value();
  if (element->opening(YARS_STRING_BOTTOM_TEXTURE))
    _texture[1] = element->requiredAttribute(YARS_STRING_NAME)->value();
  if (element->opening(YARS_STRING_BODY_TEXTURE))
    _texture[2] = element->requiredAttribute(YARS_STRING_NAME)->value();

  if (element->opening(YARS_STRING_OBJECT_MESH_VISUALISATION))
  {
    DataMeshVisualisation *mesh = new DataMeshVisualisation(this);
    _meshes.push_back(mesh);
    current = mesh;
    mesh->add(element);
  }
}

void DataCylinder::setDimension(RHDimension dimension)
{
  _dimension = dimension;
}

DataCylinder *DataCylinder::_copy()
{
  DataCylinder *copy = new DataCylinder(NULL); // no parent needed, because no parsing takes place after copying
  copy->setDimension(_dimension);
  copy->setVisualise(_visualise);
  copy->setPose(_pose);
  copy->setPhysics(_physics->copy());
  copy->setName(name());
  for (int i = 0; i < 3; i++)
    copy->_texture[i] = _texture[i];
  // if(_mesh != NULL) copy->_mesh = _mesh->copy();
  return copy;
}

void DataCylinder::_resetTo(const DataObject *other)
{
  DataCylinder *cylinder = (DataCylinder *)other;
  _name = cylinder->name();
  _dimension = cylinder->dimension();
  _visualise = cylinder->visualise();
  _pose = cylinder->pose();
  _physics = cylinder->physics();
}

string DataCylinder::texture(int index)
{
  return _texture[index];
}

void DataCylinder::setTexture(int index, string texture)
{
  _texture[index] = texture;
}
