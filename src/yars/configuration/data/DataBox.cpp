#include "DataBox.h"
#include "DataPoseFactory.h"
#include "DataDimensionFactory.h"

#define YARS_STRING_DIMENSION              (char*)"dimension"
#define YARS_STRING_POSE                   (char*)"pose"
#define YARS_STRING_POSE_DEFINITION        (char*)"pose_definition"
#define YARS_STRING_VISUALISE              (char*)"visualise"
#define YARS_STRING_SCALE                  (char*)"scale"
#define YARS_STRING_NAME                   (char*)"name"

#define YARS_STRING_USE                    (char*)"use"
#define YARS_STRING_TEXTURE                (char*)"texture"
#define YARS_STRING_FIRST_TEXTURE          (char*)"first"
#define YARS_STRING_SECOND_TEXTURE         (char*)"second"
#define YARS_STRING_THIRD_TEXTURE          (char*)"third"
#define YARS_STRING_FOURTH_TEXTURE         (char*)"fourth"
#define YARS_STRING_FIFTH_TEXTURE          (char*)"fifth"
#define YARS_STRING_SIXTH_TEXTURE          (char*)"sixth"

#define YARS_STRING_VISUALISATION_OPTION_1 (char*)"option1"
#define YARS_STRING_VISUALISATION_OPTION_2 (char*)"option2"
#define YARS_STRING_VISUALISATION_OPTION_3 (char*)"option3"

DataBox::DataBox(DataNode *parent)
  : DataObject(parent, DATA_OBJECT_BOX)
{
  _texture.resize(6);
}

DataBox::~DataBox()
{ }


WHDDimension DataBox::dimension()
{
  return _dimension;
}

void DataBox::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_OBJECT_BOX))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_OBJECT_BOX))
  {
    element->set(YARS_STRING_NAME,      _name);
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

  // for composite objects
  if(element->opening(YARS_STRING_MASS)) _physics->setMass(element->attribute(YARS_STRING_KG)->realValue());

  if(element->opening(YARS_STRING_TEXTURE))
  {
    for(int i = 0; i < 6; i++) _texture[i] = element->attribute(YARS_STRING_NAME)->value();
  }
  if(element->opening(YARS_STRING_FIRST_TEXTURE))  _texture[0] = element->attribute(YARS_STRING_NAME)->value();
  if(element->opening(YARS_STRING_SECOND_TEXTURE)) _texture[1] = element->attribute(YARS_STRING_NAME)->value();
  if(element->opening(YARS_STRING_THIRD_TEXTURE))  _texture[2] = element->attribute(YARS_STRING_NAME)->value();
  if(element->opening(YARS_STRING_FOURTH_TEXTURE)) _texture[3] = element->attribute(YARS_STRING_NAME)->value();
  if(element->opening(YARS_STRING_FIFTH_TEXTURE))  _texture[4] = element->attribute(YARS_STRING_NAME)->value();
  if(element->opening(YARS_STRING_SIXTH_TEXTURE))  _texture[5] = element->attribute(YARS_STRING_NAME)->value();
  if(element->opening(YARS_STRING_OBJECT_MESH_VISUALISATION))
  {
    DataMeshVisualisation *mesh = new DataMeshVisualisation(this);
    _meshes.push_back(mesh);
    current = mesh;
    mesh->add(element);
  }
}

void DataBox::setDimension(WHDDimension dimension)
{
  _dimension = dimension;
}

DataBox* DataBox::_copy()
{
  DataBox *copy = new DataBox(NULL); // no parent needed, because no parsing takes place after copying
  copy->setDimension(_dimension);
  copy->setVisualise(_visualise);
  copy->setPose(_pose);
  copy->setPhysics(_physics->copy());
  copy->setName(_name);
  copy->_isSoft = _isSoft;
  // if(_mesh != NULL) copy->_mesh = _mesh->copy();
  for(int i = 0; i < 6; i++) copy->_texture[i] = _texture[i];
  return copy;
}

void DataBox::_resetTo(const DataObject* other)
{
  delete _physics;
  DataBox *box = (DataBox*)other;
  _name        = box->name();
  _dimension   = box->dimension();
  _visualise   = box->visualise();
  _pose        = box->pose();
  _physics     = box->physics()->copy();
}

string DataBox::texture(int index)
{
  return _texture[index];
}

void DataBox::setTexture(int index, string texture)
{
  _texture[index] = texture;
}
