#include "DataCapsule.h"
#include "DataPoseFactory.h"
#include "DataDimensionFactory.h"

# define YARS_STRING_DIMENSION       (char*)"dimension"
# define YARS_STRING_POSE            (char*)"pose"
# define YARS_STRING_NAME            (char*)"name"
# define YARS_STRING_NAME_DEFINITION (char*)"name" DIVIDER DEFINITION
# define YARS_STRING_VISUALISE       (char*)"visualise" // for composite objects
# define YARS_STRING_POSE_DEFINITION (char*)"pose"      DIVIDER DEFINITION

# define YARS_STRING_TEXTURE         (char*)"texture"
# define YARS_STRING_TOP_TEXTURE     (char*)"top"
# define YARS_STRING_BOTTOM_TEXTURE  (char*)"bottom"
# define YARS_STRING_BODY_TEXTURE    (char*)"body"

# define OPTION_1 "option1"
# define OPTION_2 "option2"
# define OPTION_3 "option3"

DataCapsule::DataCapsule(DataNode *parent)
  : DataCylinder(parent, DATA_OBJECT_CAPPED_CYLINDER)
{
  _texture.resize(3);
}

DataCapsule::~DataCapsule()
{
}

void DataCapsule::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_OBJECT_CAPPED_CYLINDER))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_OBJECT_CAPPED_CYLINDER))
  {
    element->set(YARS_STRING_NAME, _name);
    element->set(YARS_STRING_VISUALISE, _visualise);
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
    for(int i = 0; i < 3; i++) _texture[i] = element->attribute(YARS_STRING_NAME)->value();
  }

  if(element->opening(YARS_STRING_TOP_TEXTURE))    _texture[0] = element->attribute(YARS_STRING_NAME)->value();
  if(element->opening(YARS_STRING_BOTTOM_TEXTURE)) _texture[1] = element->attribute(YARS_STRING_NAME)->value();
  if(element->opening(YARS_STRING_BODY_TEXTURE))   _texture[2] = element->attribute(YARS_STRING_NAME)->value();

  if(element->opening(YARS_STRING_OBJECT_MESH_VISUALISATION))
  {
    DataMeshVisualisation *mesh = new DataMeshVisualisation(this);
    current = mesh;
    _meshes.push_back(mesh);
    mesh->add(element);
  }
}

void DataCapsule::createXsd(XsdSpecification *spec)
{
  XsdSequence *capsuleDefinition = new XsdSequence(YARS_STRING_OBJECT_CAPPED_CYLINDER_DEFINTION);
  capsuleDefinition->add(NA(YARS_STRING_NAME,             YARS_STRING_XSD_STRING,                  false));
  capsuleDefinition->add(NA(YARS_STRING_VISUALISE,        YARS_STRING_TRUE_FALSE_DEFINITION,       false));
  capsuleDefinition->add(NE(YARS_STRING_DIMENSION,        YARS_STRING_RADIUS_HEIGHT_DEFINITION,    1, 1));
  capsuleDefinition->add(NE(YARS_STRING_POSE,             YARS_STRING_POSE_DEFINITION,             0, 1));
  capsuleDefinition->add(NE(YARS_STRING_CC_VISUALISATION, YARS_STRING_CC_VISUALISATION_DEFINITION, 0, 1));
  capsuleDefinition->add(NE(YARS_STRING_PHYSICS,          YARS_STRING_PHYSICS_DEFINITION,          1, 1));
  spec->add(capsuleDefinition);

  XsdChoice *options = new XsdChoice(YARS_STRING_CC_VISUALISATION_DEFINITION, 1, 1);
  spec->add(options);

  XsdSequence *option1 = new XsdSequence(OPTION_1);
  option1->add(NE(YARS_STRING_TEXTURE,  YARS_STRING_NAME_DEFINITION, 1, 1));
  options->add(option1);

  XsdSequence *option2 = new XsdSequence(OPTION_2);
  option2->add(NE(YARS_STRING_TOP_TEXTURE,    YARS_STRING_NAME_DEFINITION, 1, 1));
  option2->add(NE(YARS_STRING_BOTTOM_TEXTURE, YARS_STRING_NAME_DEFINITION, 1, 1));
  option2->add(NE(YARS_STRING_BODY_TEXTURE,   YARS_STRING_NAME_DEFINITION, 1, 1));
  options->add(option2);

  XsdSequence *option3 = new XsdSequence(OPTION_3);
  option3->add(NE(YARS_STRING_OBJECT_MESH_VISUALISATION,
                  YARS_STRING_OBJECT_MESH_VISUALISATION_DEFINTION, "1", YARS_STRING_XSD_UNBOUNDED));
  options->add(option3);

  DataMeshVisualisation::createXsd(spec);
  DataPhysicsParameter::createXsd(spec);
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
