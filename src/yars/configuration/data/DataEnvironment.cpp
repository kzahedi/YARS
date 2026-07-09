#include <yars/configuration/data/DataEnvironment.h>
#include <yars/configuration/data/DataBinding.h>

#include <yars/util/macros.h>

# define YARS_STRING_NAME                            (char*)"name"
# define YARS_STRING_NAME_DEFINITION                 (char*)"name_definition"
# define YARS_STRING_X                               (char*)"x"
# define YARS_STRING_Y                               (char*)"y"
# define YARS_STRING_Z                               (char*)"z"
# define YARS_STRING_GRAVITATION                     (char*)"gravitation"
# define YARS_STRING_ORIENTATION                     (char*)"orientation"
# define YARS_STRING_NORMAL                          (char*)"normal"
# define YARS_STRING_XYZ_DEFINITION                  (char*)"xyz_definition"
# define YARS_STRING_XY_DEFINITION                   (char*)"xy_definition"
# define YARS_STRING_X                               (char*)"x"
# define YARS_STRING_Y                               (char*)"y"
# define YARS_STRING_FIXED                           (char*)"fixed"
# define YARS_STRING_OBJECT_GROUND                   (char*)"ground"
# define YARS_STRING_OBJECT_GROUND_DEFINTION         (char*)"ground_definition"
# define YARS_STRING_TRUE_FALSE_DEFINITION           (char*)"true_false_definition"
# define YARS_STRING_TEXTURE_GROUND_DEFINITION       (char*)"texture_ground_definition"
# define YARS_STRING_USE_FOG                         (char*)"fog"
# define YARS_STRING_FOG_INTENSITY                   (char*)"fogIntensity"

# define YARS_STRING_GROUND_VISUALISATION            (char*)"visualisation"
# define YARS_STRING_GROUND_VISUALISATION_DEFINITION (char*)"visualisation_definition"
# define YARS_STRING_TEXTURE                         (char*)"texture"

# define OPTION_1                                    (char*)"option1"
# define OPTION_2                                    (char*)"option2"


namespace
{
// Attribute binding table for the environment's own opening tag.
// Child-element dispatch stays hand-written below.
const std::vector<yars::AttributeBinding> &environmentAttributeBindings()
{
  static const std::vector<yars::AttributeBinding> bindings = {
      {YARS_STRING_NAME,
       [](DataNode *self, const std::string &value)
       { static_cast<DataEnvironment *>(self)->setName(value); },
       /*required=*/false, /*defaultValue=*/nullptr},
  };
  return bindings;
}
} // namespace

DataEnvironment::DataEnvironment(DataNode *parent)
  : DataNode(parent)
{
  _normal.x       = 0;
  _normal.y       = 0;
  _normal.z       = 1.0;
  _fixedGround    = false;
  _useFog         = true;
  _fogIntensity   = 0.15;
  _groundGiven    = false;
  _gravitation    = new double[3];
  _gravitation[0] = 0.0;
  _gravitation[1] = 0.0;
  _gravitation[2] = -9.81;
  _ambientLight   = NULL;
}

DataEnvironment::~DataEnvironment()
{
  for(DataObjects::iterator i = _objects.begin(); i != _objects.end(); i++)
  {
    delete (*i);
  }
  for(std::vector<DataPointLightSource*>::iterator i = _lightSources.begin(); i != _lightSources.end(); i++)
  {
    delete (*i);
  }
  delete[] _gravitation;
  if(_ambientLight != NULL) delete _ambientLight;
}

string DataEnvironment::name()
{
  return _name;
}

void DataEnvironment::add(DataParseElement *element)
{
  if(element->opening(YARS_STRING_ENVIRONMENT))
  {
    yars::applyAttributes(this, element, environmentAttributeBindings());
  }
  if(element->closing(YARS_STRING_ENVIRONMENT))
  {
    current = parent;
    __applyMacros();
  }

  if(element->opening(YARS_STRING_GRAVITATION))
  {
    element->set(YARS_STRING_X, _gravitation[0]);
    element->set(YARS_STRING_Y, _gravitation[1]);
    element->set(YARS_STRING_Z, _gravitation[2]);
  }

  DataObject *object = DataObjectFactory::object(element, this);
  if(object != NULL)
  {
    current = object;
    _objects.push_back(object);
  }
  if(element->opening(YARS_STRING_LIGHT_SOURCE))
  {
    DataPointLightSource *ls = new DataPointLightSource(this);
    current = ls;
    ls->add(element);
    _lightSources.push_back(ls);
  }
  if(element->opening(YARS_STRING_MACRO))
  {
    DataMacroInstance *macroInstance = new DataMacroInstance(this, _macrosDefinitions);
    current = macroInstance;
    macroInstance->add(element);
    _macros.push_back(macroInstance);
  }
  if(element->opening(YARS_STRING_NORMAL))
  {
    element->set(YARS_STRING_X, _normal.x);
    element->set(YARS_STRING_Y, _normal.y);
    element->set(YARS_STRING_Z, _normal.z);
    _normal.normalise();
  }
  if(element->opening(YARS_STRING_ORIENTATION))
  {
    P3D angles;
    element->set(YARS_STRING_X, angles.x);
    element->set(YARS_STRING_Y, angles.y);

    angles.x = DEG_TO_RAD(angles.x);
    angles.y = DEG_TO_RAD(angles.y);

    _normal.x = sin(angles.x) * cos(angles.y);
    _normal.y = sin(-angles.y);
    _normal.z = cos(angles.x) * cos(angles.y);
  }

  if(element->opening(YARS_STRING_OBJECT_GROUND))
  {
    element->set(YARS_STRING_FIXED,         _fixedGround);
    element->set(YARS_STRING_USE_FOG,       _useFog);
    element->set(YARS_STRING_FOG_INTENSITY, _fogIntensity);
    _groundGiven = true;
  }

  if(element->opening(YARS_STRING_TEXTURE))
  {
    _texture = element->attribute(YARS_STRING_NAME)->value();
  }

  if(element->opening(YARS_STRING_OBJECT_MESH_VISUALISATION))
  {
    DataMeshVisualisation *groundMesh = new DataMeshVisualisation(this);
    _meshes.push_back(groundMesh);
    current = groundMesh;
    groundMesh->add(element);
  }
}

DataObjects::iterator DataEnvironment::o_begin()
{
  return _objects.begin();
}

DataObjects::iterator DataEnvironment::o_end()
{
  return _objects.end();
}

int DataEnvironment::o_size()
{
  return _objects.size();
}

DataObjects::iterator DataEnvironment::g_begin()
{
  return _geoms.begin();
}

DataObjects::iterator DataEnvironment::g_end()
{
  return _geoms.end();
}

int DataEnvironment::g_size()
{
  return _geoms.size();
}

std::vector<DataPointLightSource*>::iterator DataEnvironment::l_begin()
{
  return _lightSources.begin();
}

std::vector<DataPointLightSource*>::iterator DataEnvironment::l_end()
{
  return _lightSources.end();
}

int DataEnvironment::l_size()
{
  return _lightSources.size();
}

DataObject* DataEnvironment::object(int index)
{
  return _objects[index];
}

DataPointLightSource* DataEnvironment::lightSource(int index)
{
  return _lightSources[index];
}

void DataEnvironment::setMacros(DataMacros *macros)
{
  _macrosDefinitions = macros;
}

DataMacros* DataEnvironment::macros()
{
  return _macrosDefinitions;
}

void DataEnvironment::__applyMacros()
{
  for(std::vector<DataMacroInstance*>::iterator i = _macros.begin(); i != _macros.end(); i++)
  {
    for(DataObjects::iterator o = (*i)->begin(); o != (*i)->end(); o++)
    {
      _objects.push_back(*o);
    }
  }
}

P3D DataEnvironment::normal()
{
  return _normal;
}

// DataTexture* DataEnvironment::texture()
// {
  // return _textureDefinition;
// }

DataEnvironment* DataEnvironment::copy()
{
  DataEnvironment *copy = new DataEnvironment(NULL);
  copy->_name = _name;
  for(int i = 0; i < 3; i++) copy->_gravitation[i] = _gravitation[i];
  if(_macrosDefinitions != NULL) copy->_macrosDefinitions = _macrosDefinitions->copy();

  for(vector<DataMeshVisualisation*>::iterator m = m_begin(); m != m_end(); m++)
  {
    copy->_meshes.push_back((*m)->copy());
  }
  for(DataObjects::iterator i = _objects.begin(); i != _objects.end(); i++)
  {
    copy->_objects.push_back((*i)->copy());
  }
  for(std::vector<DataPointLightSource*>::iterator i = _lightSources.begin(); i != _lightSources.end(); i++)
  {
    copy->_lightSources.push_back((*i)->copy());
  }
  for(std::vector<DataMacroInstance*>::iterator i = _macros.begin(); i != _macros.end(); i++)
  {
    copy->_macros.push_back((*i)->copy(this));
  }
  copy->_normal = _normal;
  // copy->_textureDefinition = NULL;
  // if(_textureDefinition != NULL)
  // {
    // copy->_textureDefinition = _textureDefinition->copy();
  // }
  copy->__gatherGeoms();
  copy->_fixedGround  = _fixedGround;
  copy->_useFog       = _useFog;
  copy->_fogIntensity = _fogIntensity;
  copy->_texture      = _texture;
  copy->_groundGiven  = _groundGiven;
  return copy;
}

void DataEnvironment::__gatherGeoms()
{
  for(DataObjects::iterator o = _objects.begin(); o != _objects.end(); o++)
  {
    if((*o)->type() == DATA_OBJECT_COMPOSITE)
    {
      DataComposite *composite = (DataComposite*)(*o);
      for(DataObjects::iterator oo = composite->g_begin();
          oo != composite->g_end(); oo++)
      {
        _geoms.push_back(*oo);
      }
    }
    else
    {
      _geoms.push_back(*o);
    }
  }
}

bool DataEnvironment::groundIsFixed()
{
  return _fixedGround;
}

bool DataEnvironment::useFog()
{
  return _useFog;
}

double DataEnvironment::fogIntensity()
{
  return _fogIntensity;
}

string DataEnvironment::texture()
{
  return _texture;
}

vector<DataMeshVisualisation*>::iterator DataEnvironment::m_begin()
{
  return _meshes.begin();
}

vector<DataMeshVisualisation*>::iterator DataEnvironment::m_end()
{
  return _meshes.end();
}

int DataEnvironment::m_size()
{
  return _meshes.size();
}

bool DataEnvironment::groundGiven()
{
  return _groundGiven;
}

void DataEnvironment::resetTo(DataEnvironment *other)
{
  for(int i = 0; i < (int)_objects.size(); i++) _objects[i]->resetTo(other->_objects[i]);
}

double DataEnvironment::gravitation(int index)
{
  return _gravitation[index];
}

DataAmbientLight* DataEnvironment::ambientLight()
{
  return _ambientLight;
}
