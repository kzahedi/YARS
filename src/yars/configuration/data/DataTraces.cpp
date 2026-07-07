#include <yars/configuration/data/DataTraces.h>


# define YARS_STRING_TRACE (char*)"trace"
# define YARS_STRING_POINT (char*)"point"

DataTraces::DataTraces(DataNode *parent)
  : DataNode(parent)
{ }

DataTraces::~DataTraces()
{ }

void DataTraces::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_TRACES))
  {
    current = parent;
  }

  if(element->opening(YARS_STRING_TRACE))
  {
    DataTraceLine *line = new DataTraceLine(this);
    _lines.push_back(line);
    current = line;
    line->add(element);
  }

  if(element->opening(YARS_STRING_POINT))
  {
    DataTracePoint *point = new DataTracePoint(this);
    _points.push_back(point);
    current = point;
    point->add(element);
  }
}

std::vector<DataTraceLine*>::iterator DataTraces::l_begin()
{
  return _lines.begin();
}

std::vector<DataTraceLine*>::iterator DataTraces::l_end()
{
  return _lines.end();
}

int DataTraces::l_size()
{
  return (int)_lines.size();
}

std::vector<DataTracePoint*>::iterator DataTraces::p_begin()
{
  return _points.begin();
}

std::vector<DataTracePoint*>::iterator DataTraces::p_end()
{
  return _points.end();
}

int DataTraces::p_size()
{
  return (int)_points.size();
}

DataTraces* DataTraces::copy()
{
  DataTraces *copy = new DataTraces(NULL);
  copy->_name = _name;
  for(std::vector<DataTraceLine*>::iterator i = _lines.begin(); i != _lines.end(); i++)
  {
    copy->_lines.push_back((*i)->copy());
  }
  for(std::vector<DataTracePoint*>::iterator i = _points.begin(); i != _points.end(); i++)
  {
    copy->_points.push_back((*i)->copy());
  }
  return copy;
}

void DataTraces::resetTo(DataTraces* other)
{
  for (auto *l : _lines)  l->clear();
  for (auto *p : _points) p->clear();
}
