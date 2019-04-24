#include "DataRobots.h"
#include "DataMacros.h"

#include <sstream>

using namespace std;

DataRobots::DataRobots(DataNode *parent)
  : DataNode(parent)
{
  _macros = NULL;
}

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
    robot->setMacros(_macros);
    robot->add(element);
  }
}

DataRobot* DataRobots::robot(int index)
{
  return at(index);
}

void DataRobots::setMacros(DataMacros *macros)
{
  _macros = macros;
}

void DataRobots::createXsd(XsdSpecification *spec)
{
  XsdSequence *robotsDefinition = new XsdSequence(YARS_STRING_ROBOTS_DEFINITION);
  robotsDefinition->add(XE(YARS_STRING_ROBOT, YARS_STRING_ROBOT_DEFINITION, 0));
  stringstream comment;
  comment << "A robot is a controllable physical object." << endl;
  comment << "  It can be controller via TCP/IP or a RobotController." << endl;
  comment << "  A rosiml description can contain any number of robots." << endl;
  robotsDefinition->setComment(comment.str());
  spec->add(robotsDefinition);
  DataRobot::createXsd(spec);
}

DataRobots* DataRobots::copy()
{
  DataRobots *copy = new DataRobots(NULL);
  for(std::vector<DataRobot*>::iterator i = begin(); i != end(); i++)
  {
    copy->push_back((*i)->copy());
  }
  if(_macros != NULL) copy->_macros = _macros->copy();
  return copy;
}


void DataRobots::resetTo(const DataRobots *robots)
{
  for(int i = 0; i < (int)size(); i++)
  {
    at(i)->resetTo((*robots)[i]);
  }
}
