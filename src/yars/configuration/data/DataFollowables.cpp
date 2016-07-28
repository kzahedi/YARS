#include <yars/configuration/data/DataFollowables.h>

#include <yars/util/YarsErrorHandler.h>

# define YARS_STRING_FOLLOWABLE            (char*)"followable"
# define YARS_STRING_FOLLOWABLE_DEFINITION (char*)"followable" DIVIDER DEFINITION
# define YARS_STRING_NAME                  (char*)"name"

DataFollowables::DataFollowables(DataNode *parent)
  : DataNode(parent)
{ }

DataFollowables::~DataFollowables()
{
  clear();
}

void DataFollowables::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_FOLLOWABLES))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_FOLLOWABLE))
  {
    push_back(element->attribute(YARS_STRING_NAME)->value());
  }
}

void DataFollowables::createXsd(XsdSpecification *spec)
{
  XsdSequence *followablesConfiguration = new XsdSequence(YARS_STRING_FOLLOWABLES_DEFINITION);
  followablesConfiguration->add(NE(YARS_STRING_FOLLOWABLE, YARS_STRING_FOLLOWABLE_DEFINITION, 1));
  spec->add(followablesConfiguration);

  XsdSequence *followableConfiguration = new XsdSequence(YARS_STRING_FOLLOWABLE_DEFINITION);
  followableConfiguration->add(NA(YARS_STRING_NAME, YARS_STRING_XSD_STRING, true));
  spec->add(followableConfiguration);
}

DataFollowables* DataFollowables::copy()
{
  DataFollowables *copy = new DataFollowables(NULL);
  for(std::vector<string>::iterator i = begin(); i != end(); i++)
  {
    copy->push_back((*i));
  }
  return copy;
}

void DataFollowables::add(DataObject* o)
{
  _objects.push_back(o);
}

DataObjects::iterator DataFollowables::o_begin()
{
  return _objects.begin();
}

DataObjects::iterator DataFollowables::o_end()
{
  return _objects.end();
}
int DataFollowables::o_size()
{
  return _objects.size();
}

DataObject* DataFollowables::followable(int index)
{
  if(index > (int)_objects.size())
  {
    YarsErrorHandler *error = YarsErrorHandler::instance();
    (*error) << "DataFollowables::followable called with index out of bounce: " << index << " >= " << _objects.size();
    YarsErrorHandler::push();
  }
  return _objects[index];
}
