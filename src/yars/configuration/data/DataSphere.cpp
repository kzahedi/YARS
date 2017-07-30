#include "DataSphere.h"
#include "DataPoseFactory.h"

# define YARS_STRING_RADIUS           (char*)"radius"
# define YARS_STRING_DIMENSION        (char*)"dimension"
# define YARS_STRING_POSE             (char*)"pose"
# define YARS_STRING_NAME             (char*)"name"
# define YARS_STRING_TEXTURE          (char*)"texture"
# define YARS_STRING_VISUALISE        (char*)"visualise" // for composite objects
# define YARS_STRING_POSE_DEFINITION  (char*)"pose"   DIVIDER DEFINITION
# define YARS_STRING_RADIUS_DIMENSION_DEFINITION (char*)"radius" DIVIDER "dimension" DIVIDER DEFINITION

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


void DataSphere::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_OBJECT_SPHERE))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_OBJECT_SPHERE))
  {
    element->set(YARS_STRING_NAME,      _name);
    element->set(YARS_STRING_VISUALISE, _visualise);
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

void DataSphere::createXsd(XsdSpecification *spec)
{
  XsdSequence *sphereDef = new XsdSequence(YARS_STRING_OBJECT_SPHERE_DEFINTION);
  sphereDef->add(NA(YARS_STRING_NAME,                 YARS_STRING_XSD_STRING,                      false));
  sphereDef->add(NA(YARS_STRING_VISUALISE,            YARS_STRING_TRUE_FALSE_DEFINITION,           false));
  sphereDef->add(NE(YARS_STRING_DIMENSION,            YARS_STRING_RADIUS_DIMENSION_DEFINITION,     1, 1));
  sphereDef->add(NE(YARS_STRING_POSE,                 YARS_STRING_POSE_DEFINITION,                 0, 1));
  sphereDef->add(NE(YARS_STRING_SPHERE_VISUALISATION, YARS_STRING_SPHERE_VISUALISATION_DEFINITION, 0, 1));
  sphereDef->add(NE(YARS_STRING_PHYSICS,              YARS_STRING_PHYSICS_DEFINITION,              1, 1));
  spec->add(sphereDef);

  XsdChoice *options = new XsdChoice(YARS_STRING_SPHERE_VISUALISATION_DEFINITION, 1, 1);
  spec->add(options);

  XsdSequence *textureDefinition = new XsdSequence(OPTION_1);
  textureDefinition->add(NE(YARS_STRING_TEXTURE,  YARS_STRING_NAME_DEFINITION, 1, 1));
  options->add(textureDefinition);

  XsdSequence *option2 = new XsdSequence(OPTION_2);
  option2->add(NE(YARS_STRING_OBJECT_MESH_VISUALISATION,
                  YARS_STRING_OBJECT_MESH_VISUALISATION_DEFINTION, 1, 1));
  options->add(option2);

  XsdSequence *radiusDimension = new XsdSequence(YARS_STRING_RADIUS_DIMENSION_DEFINITION);
  radiusDimension->add(NA(YARS_STRING_RADIUS, YARS_STRING_XSD_DECIMAL, true));
  spec->add(radiusDimension);

  DataMeshVisualisation::createXsd(spec);
  DataPhysicsParameter::createXsd(spec);
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
