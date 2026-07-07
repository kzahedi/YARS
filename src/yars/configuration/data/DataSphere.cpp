#include "DataSphere.h"
#include "DataBinding.h"
#include "DataPoseFactory.h"

# define YARS_STRING_RADIUS                      (char*)"radius"
# define YARS_STRING_DIMENSION                   (char*)"dimension"
# define YARS_STRING_POSE                        (char*)"pose"
# define YARS_STRING_NAME                        (char*)"name"
# define YARS_STRING_TEXTURE                     (char*)"texture"
# define YARS_STRING_VISUALISE                   (char*)"visualise"
# define YARS_STRING_POSE_DEFINITION             (char*)"pose_definition"
# define YARS_STRING_RADIUS_DIMENSION_DEFINITION (char*)"radius_dimension_definition"

# define OPTION_1 "option1"
# define OPTION_2 "option2"

#include <math.h>

DataSphere::DataSphere(DataNode *parent)
  : DataObject(parent, DATA_OBJECT_SPHERE)
{
}

DataSphere::~DataSphere()
{
}


double DataSphere::radius()
{
  return _radius;
}


namespace
{
// Attribute binding table for the sphere's own opening tag. Child-element
// dispatch (dimension/pose/physics/texture/mesh) stays hand-written below.
const std::vector<yars::AttributeBinding> &sphereAttributeBindings()
{
  static const std::vector<yars::AttributeBinding> bindings = {
      {YARS_STRING_NAME,
       [](DataNode *self, const std::string &value)
       { static_cast<DataSphere *>(self)->setName(value); },
       /*required=*/false, /*defaultValue=*/nullptr},
      {YARS_STRING_VISUALISE,
       [](DataNode *self, const std::string &value)
       { static_cast<DataSphere *>(self)->setVisualise(value == "true"); },
       /*required=*/false, /*defaultValue=*/nullptr},
  };
  return bindings;
}
} // namespace

void DataSphere::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_OBJECT_SPHERE))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_OBJECT_SPHERE))
  {
    yars::applyAttributes(this, element, sphereAttributeBindings());
  }
  if(element->opening(YARS_STRING_DIMENSION))
  {
    element->set(YARS_STRING_RADIUS, _radius);
  }
  if(element->opening(YARS_STRING_POSE))
  {
    DataPoseFactory::set(_pose, element);
  }
  if(element->opening(YARS_STRING_PHYSICS))
  {
    _physics->add(element);
    current = _physics;
  }

  if(element->opening(YARS_STRING_TEXTURE)) _texture = element->attribute(YARS_STRING_NAME)->value();

  if(element->opening(YARS_STRING_OBJECT_MESH_VISUALISATION))
  {
    DataMeshVisualisation *mesh = new DataMeshVisualisation(this);
    _meshes.push_back(mesh);
    current = mesh;
    mesh->add(element);
  }

}

void DataSphere::setRadius(double radius)
{
  _radius = radius;
}

DataSphere* DataSphere::_copy()
{
  DataSphere *copy = new DataSphere(NULL); // no parent needed, because no parsing takes place after copying
  copy->setRadius(_radius);
  copy->setVisualise(_visualise);
  copy->setPose(_pose);
  copy->setPhysics(_physics->copy());
  copy->setName(name());
  copy->_texture = _texture;
  // if(_mesh != NULL) copy->_mesh = _mesh->copy();
  return copy;
}


void DataSphere::_resetTo(const DataObject* other)
{
  DataSphere *sphere = (DataSphere*)other;
  _name          = sphere->name();
  _radius        = sphere->radius();
  _visualise     = sphere->visualise();
  _pose          = sphere->pose();
  _physics       = sphere->physics();
}

string DataSphere::texture()
{
  return _texture;
}

void DataSphere::setTexture(string texture)
{
  _texture = texture;
}
