#include <yars/configuration/data/DataNoise.h>

#include <yars/defines/ymath.h>

#ifdef __APPLE__
#  include <limits.h>
#  include <float.h>
#else
#  include <stdlib.h>
#endif


# define YARS_STRING_MODULE                (char*)"module"
# define YARS_STRING_NOISE_TYPE_DEFINITION (char*)"noise_type_definition"
# define YARS_STRING_GAUSS                 (char*)"gauss"
# define YARS_STRING_WHITE                 (char*)"white"
# define YARS_STRING_NAME                  (char*)"name"

DataNoise::DataNoise(DataNode *parent)
  : DataNode(parent)
{
}

DataNoise::~DataNoise()
{
  for(std::vector<DataParameter*>::iterator i = begin(); i != end(); i++)
  {
    delete (*i);
  }
  clear();
}

void DataNoise::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_NOISE))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_NOISE))
  {
    element->set(YARS_STRING_MODULE, _module);
  }
  if(element->opening(YARS_STRING_PARAMETER))
  {
    DataParameter *parameter = new DataParameter(this);
    parameter->add(element);
    current = parameter;
    push_back(parameter);
  }
}

void DataNoise::createXsd(XsdSpecification *spec)
{
  XsdSequence *noiseDefinition = new XsdSequence(YARS_STRING_NOISE_DEFINITION);
  noiseDefinition->add(NA(YARS_STRING_MODULE,    YARS_STRING_NOISE_TYPE_DEFINITION, true));
  noiseDefinition->add(NE(YARS_STRING_PARAMETER, YARS_STRING_PARAMETER_DEFINITION,  true));
  spec->add(noiseDefinition);

  XsdEnumeration *noiseTypesDefinition = new XsdEnumeration(YARS_STRING_NOISE_TYPE_DEFINITION, YARS_STRING_XSD_STRING);
  noiseTypesDefinition->add(YARS_STRING_GAUSS);
  noiseTypesDefinition->add(YARS_STRING_WHITE);
  spec->add(noiseTypesDefinition);

  XsdSequence *parameterDefinition = new XsdSequence(YARS_STRING_PARAMETER_DEFINITION);
  parameterDefinition->add(NA(YARS_STRING_NAME,  YARS_STRING_XSD_STRING, true));
  parameterDefinition->add(NA(YARS_STRING_VALUE, YARS_STRING_XSD_STRING, true));
  spec->add(parameterDefinition);
}

DataParameter* DataNoise::parameter(int index)
{
  return at(index);
}

DataNoise* DataNoise::copy()
{
  DataNoise *copy = new DataNoise(NULL);
  copy->_module = _module;
  for(std::vector<DataParameter*>::iterator i = begin(); i != end(); i++)
  {
    copy->push_back((*i)->copy());
  }
  return copy;
}

string DataNoise::module()
{
  return _module;
}
