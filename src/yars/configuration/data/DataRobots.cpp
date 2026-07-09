#include "DataRobots.h"

#include <sstream>

using namespace std;

DataRobots::DataRobots(DataNode *parent)
  : DataNode(parent)
{ }

DataRobots::~DataRobots()
{ }

void DataRobots::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_ROBOTS))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_ROBOT))
  {
    DataRobot *robot = new DataRobot(this);
    push_back(robot);
    current = robot;
    robot->add(element);
  }
}

DataRobot* DataRobots::robot(int index)
{
  return at(index);
}

DataRobots* DataRobots::copy()
{
  DataRobots *copy = new DataRobots(NULL);
  for(std::vector<DataRobot*>::iterator i = begin(); i != end(); i++)
  {
    copy->push_back((*i)->copy());
  }
  return copy;
}


void DataRobots::resetTo(const DataRobots *robots)
{
  for(int i = 0; i < (int)size(); i++)
  {
    at(i)->resetTo((*robots)[i]);
  }
}
