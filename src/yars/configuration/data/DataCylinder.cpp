#include "DataCylinder.h"
#include "DataPoseFactory.h"
#include "DataDimensionFactory.h"

# define YARS_STRING_DIMENSION       (char*)"dimension"
# define YARS_STRING_POSE            (char*)"pose"
# define YARS_STRING_NAME            (char*)"name"
# define YARS_STRING_VISUALISE       (char*)"visualise"
# define YARS_STRING_POSE_DEFINITION (char*)"pose_definition"

# define YARS_STRING_TEXTURE         (char*)"texture"
# define YARS_STRING_TOP_TEXTURE     (char*)"top"
# define YARS_STRING_BOTTOM_TEXTURE  (char*)"bottom"
# define YARS_STRING_BODY_TEXTURE    (char*)"body"

# define OPTION_1                    "option1"
# define OPTION_2                    "option2"
# define OPTION_3                    "option3"

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


void DataCylinder::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_OBJECT_CYLINDER))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_OBJECT_CYLINDER))
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
    _meshes.push_back(mesh);
    current = mesh;
    mesh->add(element);
  }

}

void DataCylinder::setDimension(RHDimension dimension)
{
  _dimension = dimension;
}

void DataCylinder::createXsd(XsdSpecification *spec)
{
  XsdSequence *cylinderDefinition = new XsdSequence(YARS_STRING_OBJECT_CYLINDER_DEFINTION);
  cylinderDefinition->add(NA(YARS_STRING_NAME,            YARS_STRING_XSD_STRING,                 false));
  cylinderDefinition->add(NA(YARS_STRING_VISUALISE,       YARS_STRING_TRUE_FALSE_DEFINITION,      false));
  cylinderDefinition->add(NE(YARS_STRING_DIMENSION,       YARS_STRING_RADIUS_HEIGHT_DEFINITION,   true));
  cylinderDefinition->add(NE(YARS_STRING_POSE,            YARS_STRING_POSE_DEFINITION,            0, 1));
  cylinderDefinition->add(NE(YARS_STRING_C_VISUALISATION, YARS_STRING_C_VISUALISATION_DEFINITION, 0, 1));
  cylinderDefinition->add(NE(YARS_STRING_PHYSICS,         YARS_STRING_PHYSICS_DEFINITION,         1, 1));
  spec->add(cylinderDefinition);

  XsdSequence *radiusHeightDefinition = new XsdSequence(YARS_STRING_RADIUS_HEIGHT_DEFINITION);
  radiusHeightDefinition->add(NA(YARS_STRING_RADIUS, YARS_STRING_XSD_DECIMAL, true));
  radiusHeightDefinition->add(NA(YARS_STRING_HEIGHT, YARS_STRING_XSD_DECIMAL, true));
  spec->add(radiusHeightDefinition);


  XsdChoice *options = new XsdChoice(YARS_STRING_C_VISUALISATION_DEFINITION, 1, 1);
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
                  YARS_STRING_OBJECT_MESH_VISUALISATION_DEFINTION, 1, 1));
  options->add(option3);

  DataMeshVisualisation::createXsd(spec);
  DataPhysicsParameter::createXsd(spec);
}

DataCylinder* DataCylinder::_copy()
{
  DataCylinder *copy = new DataCylinder(NULL); // no parent needed, because no parsing takes place after copying
  copy->setDimension(_dimension);
  copy->setVisualise(_visualise);
  copy->setPose(_pose);
  copy->setPhysics(_physics->copy());
  copy->setName(name());
  for(int i = 0; i < 3; i++) copy->_texture[i] = _texture[i];
  // if(_mesh != NULL) copy->_mesh = _mesh->copy();
  return copy;
}

void DataCylinder::_resetTo(const DataObject* other)
{
  DataCylinder *cylinder = (DataCylinder*)other;
  _name          = cylinder->name();
  _dimension     = cylinder->dimension();
  _visualise     = cylinder->visualise();
  _pose          = cylinder->pose();
  _physics       = cylinder->physics();
}

string DataCylinder::texture(int index)
{
  return _texture[index];
}

void DataCylinder::setTexture(int index, string texture)
{
  _texture[index] = texture;
}
