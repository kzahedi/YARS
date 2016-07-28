#include "DataPly.h"
#include "DataPoseFactory.h"
#include "DataMeshVisualisation.h"


# define YARS_STRING_POSE                     (char*)"pose"
# define YARS_STRING_NAME                     (char*)"name"
# define YARS_STRING_POSE_DEFINITION          (char*)"pose"          DIVIDER DEFINITION
# define YARS_STRING_VISUALISE                (char*)"visualise"
# define YARS_STRING_CONVEX                   (char*)"convex"
# define YARS_STRING_SOFT                     (char*)"soft"
# define YARS_STRING_TEXTURE                  (char*)"texture"
# define YARS_STRING_VISUALISATION            (char*)"visualisation"
# define YARS_STRING_VISUALISATION_OPTION_1   (char*)"moption1"
# define YARS_STRING_VISUALISATION_OPTION_2   (char*)"moption2"

DataPly::DataPly(DataNode *parent)
  : DataObject(parent, DATA_OBJECT_PLY)
{
  _isConvex         = false;
  _isSoft           = false;
  _numberOfVertices = 0;
  _parameters       = new DataSoftBodyParameters(this);
  pthread_mutexattr_t pthread_mutexattr_default;
  pthread_mutexattr_init(&pthread_mutexattr_default);
  pthread_mutex_init(&_mutex, &pthread_mutexattr_default);
}

DataPly::~DataPly()
{
  pthread_mutex_destroy(&_mutex);
  delete _parameters;
}

void DataPly::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_OBJECT_PLY)) current = parent;

  if(element->opening(YARS_STRING_OBJECT_PLY))
  {
    element->set(YARS_STRING_NAME,      _name);
    element->set(YARS_STRING_VISUALISE, _visualise);
    element->set(YARS_STRING_FILE_NAME, _filename);
    element->set(YARS_STRING_CONVEX,    _isConvex);
    element->set(YARS_STRING_SOFT,      _isSoft);
  }

  if(element->opening(YARS_STRING_POSE))    DataPoseFactory::set(_pose,       element);
  if(element->opening(YARS_STRING_TEXTURE)) element->set(YARS_STRING_NAME, _texture);

  if(element->opening(YARS_STRING_PHYSICS))
  {
    _physics->add(element);
    current = _physics;
  }

  if(element->opening(YARS_STRING_SOFT_BODY_PARAMETER))
  {
    _parameters->add(element);
    current = _parameters;
  }

  if(element->opening(YARS_STRING_OBJECT_MESH_VISUALISATION))
  {
    DataMeshVisualisation *mesh = new DataMeshVisualisation(this);
    _meshes.push_back(mesh);
    current = mesh;
    mesh->add(element);
  }

}

void DataPly::createXsd(XsdSpecification *spec)
{
  XsdSequence *plyDefinition = new XsdSequence(YARS_STRING_OBJECT_PLY_DEFINTION);
  plyDefinition->add(NA(YARS_STRING_NAME,                YARS_STRING_XSD_STRING,                     true));
  plyDefinition->add(NA(YARS_STRING_CONVEX,              YARS_STRING_TRUE_FALSE_DEFINITION,          false));
  plyDefinition->add(NA(YARS_STRING_SOFT,                YARS_STRING_TRUE_FALSE_DEFINITION,          false));
  plyDefinition->add(NA(YARS_STRING_VISUALISE,           YARS_STRING_TRUE_FALSE_DEFINITION,          false));
  plyDefinition->add(NA(YARS_STRING_FILE_NAME,           YARS_STRING_XSD_STRING,                     true));
  plyDefinition->add(NE(YARS_STRING_POSE,                YARS_STRING_POSE_DEFINITION,                0, 1));
  plyDefinition->add(NE(YARS_STRING_VISUALISATION,       YARS_STRING_PLY_VISUALISATION_DEFINITION,   1, 1));
  plyDefinition->add(NE(YARS_STRING_PHYSICS,             YARS_STRING_PHYSICS_DEFINITION,             1, 1));
  // plyDefinition->add(NE(YARS_STRING_SOFT_BODY_PARAMETER, YARS_STRING_SOFT_BODY_PARAMETER_DEFINITION, 0, 1));
  spec->add(plyDefinition);

  XsdChoice *options = new XsdChoice(YARS_STRING_PLY_VISUALISATION_DEFINITION, 1, 1);
  spec->add(options);

  XsdSequence *option1 = new XsdSequence(YARS_STRING_VISUALISATION_OPTION_1);
  option1->add(NE(YARS_STRING_TEXTURE,  YARS_STRING_NAME_DEFINITION, 1, 1));
  options->add(option1);

  XsdSequence *option2 = new XsdSequence(YARS_STRING_VISUALISATION_OPTION_2);
  option2->add(NE(YARS_STRING_OBJECT_MESH_VISUALISATION,
                  YARS_STRING_OBJECT_MESH_VISUALISATION_DEFINTION, "1", YARS_STRING_XSD_UNBOUNDED));
  options->add(option2);

  DataSoftBodyParameters::createXsd(spec);
}

DataPly* DataPly::_copy()
{
  DataPly *copy   = new DataPly(NULL);
  copy->_filename = _filename;
  copy->_pose     = _pose;
  copy->_texture  = _texture;
  copy->_name     = _name;
  copy->_isConvex = _isConvex;
  copy->_isSoft   = _isSoft;
  copy->_physics  = _physics->copy();
  copy->_parameters->resetTo(_parameters);
  for(int m = 0; m < _meshes.size(); m++) copy->_meshes.push_back(_meshes[m]->copy());;
  return copy;
}

void DataPly::_resetTo(const DataObject *other)
{
  DataPly *ply = (DataPly*)other;
  _pose    = ply->_pose;
}

string DataPly::texture()
{
  return _texture;
}

string DataPly::filename()
{
  return _filename;
}

bool DataPly::isConvex()
{
  return _isConvex;
}

bool DataPly::isSoft()
{
  return _isSoft;
}

void DataPly::setNumberOfVertices(int n)
{
  _numberOfVertices = n;
  _vertices.resize(n);
}

void DataPly::setNumberOfNormals(int n)
{
  _numberOfNormals = n;
  _normals.resize(n);
}

void DataPly::setNumberOfIndices(int n)
{
  _numberOfIndices = n;
  _indices.resize(n);
}

void DataPly::setVertex(int i, yReal x, yReal y, yReal z)
{
  _vertices[i].x = x;
  _vertices[i].y = y;
  _vertices[i].z = z;
}

void DataPly::setNormal(int i, yReal x, yReal y, yReal z)
{
  _normals[i].x = x;
  _normals[i].y = y;
  _normals[i].z = z;
}

vector<P3D>::const_iterator DataPly::v_begin()
{
  return _vertices.begin();
}

vector<P3D>::const_iterator DataPly::v_end()
{
  return _vertices.end();
}

vector<P3D>::const_iterator DataPly::n_begin()
{
  return _normals.begin();
}

vector<P3D>::const_iterator DataPly::n_end()
{
  return _normals.end();
}

vector<int>::const_iterator DataPly::i_begin()
{
  return _indices.begin();
}

vector<int>::const_iterator DataPly::i_end()
{
  return _indices.end();
}

P3D DataPly::vertex(int index)
{
  return _vertices[index];
}

P3D DataPly::normal(int index)
{
  return _normals[index];
}

void DataPly::setIndex(int i, int index)
{
  _indices[i] = index;
}

int DataPly::v_size()
{
  return _vertices.size();
}

int DataPly::index(int index)
{
  return _indices[index];
}

void DataPly::lock()
{
  pthread_mutex_lock(&_mutex);
}

void DataPly::unlock()
{
  pthread_mutex_unlock(&_mutex);
}

DataSoftBodyParameters* DataPly::parameter()
{
  return _parameters;
}
