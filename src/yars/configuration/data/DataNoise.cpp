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
