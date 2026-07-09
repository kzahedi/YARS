#include "DataCapsule.h"
#include "DataBinding.h"
#include "DataPoseFactory.h"
#include "DataDimensionFactory.h"

# define YARS_STRING_DIMENSION       (char*)"dimension"
# define YARS_STRING_POSE            (char*)"pose"
# define YARS_STRING_NAME            (char*)"name"
# define YARS_STRING_NAME_DEFINITION (char*)"name_definition"
# define YARS_STRING_VISUALISE       (char*)"visualise" // for composite objects
# define YARS_STRING_POSE_DEFINITION (char*)"pose_definition"

# define YARS_STRING_TEXTURE         (char*)"texture"
# define YARS_STRING_TOP_TEXTURE     (char*)"top"
# define YARS_STRING_BOTTOM_TEXTURE  (char*)"bottom"
# define YARS_STRING_BODY_TEXTURE    (char*)"body"

# define OPTION_1                    "option1"
# define OPTION_2                    "option2"
# define OPTION_3                    "option3"

DataCapsule::DataCapsule(DataNode *parent)
  : DataCylinder(parent, DATA_OBJECT_CAPPED_CYLINDER)
{
  _texture.resize(3);
}

DataCapsule::~DataCapsule()
{
}

namespace
{
// Attribute binding table for the capsule's own opening tag. Child-element
// dispatch (pose/dimension/physics/texture/mesh) stays hand-written below.
const std::vector<yars::AttributeBinding> &capsuleAttributeBindings()
{
  static const std::vector<yars::AttributeBinding> bindings = {
      {YARS_STRING_NAME,
       [](DataNode *self, const std::string &value)
       { static_cast<DataCapsule *>(self)->setName(value); },
       /*required=*/false, /*defaultValue=*/nullptr},
      {YARS_STRING_VISUALISE,
       [](DataNode *self, const std::string &value)
       { static_cast<DataCapsule *>(self)->setVisualise(value == "true"); },
       /*required=*/false, /*defaultValue=*/nullptr},
  };
  return bindings;
}
} // namespace

void DataCapsule::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_OBJECT_CAPPED_CYLINDER))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_OBJECT_CAPPED_CYLINDER))
  {
    yars::applyAttributes(this, element, capsuleAttributeBindings());
  }
  if(element->opening(YARS_STRING_POSE))
  {
    DataPoseFactory::set(_pose, element);
  }
  if(element->opening(YARS_STRING_DIMENSION))
  {
    DataDimensionFactory::set(_dimension, element);
  }
  if(element->opening(YARS_STRING_PHYSICS))
  {
    _physics->add(element);
    current = _physics;
  }

  if(element->opening(YARS_STRING_TEXTURE))
  {
    for(int i = 0; i < 3; i++) _texture[i] = element->requiredAttribute(YARS_STRING_NAME)->value();
  }

  if(element->opening(YARS_STRING_TOP_TEXTURE))    _texture[0] = element->requiredAttribute(YARS_STRING_NAME)->value();
  if(element->opening(YARS_STRING_BOTTOM_TEXTURE)) _texture[1] = element->requiredAttribute(YARS_STRING_NAME)->value();
  if(element->opening(YARS_STRING_BODY_TEXTURE))   _texture[2] = element->requiredAttribute(YARS_STRING_NAME)->value();

  if(element->opening(YARS_STRING_OBJECT_MESH_VISUALISATION))
  {
    DataMeshVisualisation *mesh = new DataMeshVisualisation(this);
    current = mesh;
    _meshes.push_back(mesh);
    mesh->add(element);
  }
}

DataCapsule* DataCapsule::_copy()
{
  DataCapsule *copy = new DataCapsule(NULL); // no parent needed, because no parsing takes place after copying
  copy->setDimension(_dimension);
  copy->setVisualise(_visualise);
  copy->setPose(_pose);
  copy->setPhysics(_physics->copy());
  copy->setName(name());
  copy->_texture.resize(3);
  for(vector<DataMeshVisualisation*>::iterator m = _meshes.begin(); m != _meshes.end(); m++)
    copy->_meshes.push_back((*m)->copy());
  for(int i = 0; i < 3; i++) copy->_texture[i] = _texture[i];
  return copy;
}

void DataCapsule::_resetTo(const DataObject* other)
{
  DataCapsule *ccylinder = (DataCapsule*)other;
  _name          = ccylinder->name();
  _dimension     = ccylinder->dimension();
  _visualise     = ccylinder->visualise();
  _pose          = ccylinder->pose();
  _physics       = ccylinder->physics();
}

string DataCapsule::texture(int index)
{
  return _texture[index];
}

void DataCapsule::setTexture(int index, string texture)
{
  _texture[index] = texture;
}
