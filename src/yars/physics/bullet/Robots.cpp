#include <yars/physics/bullet/Robots.h>

#include <yars/configuration/YarsConfiguration.h>
#include <yars/util/stl_macros.h>

Robots::Robots()
{
  _reset = false;
  _quit  = false;
  DataRobots *robots = __YARS_GET_ROBOTS;
  if(robots == NULL) return;

  FOREACH(DataRobot*, i, (*robots)) push_back(new Robot(*i));
}

Robots::~Robots()
{
  FOREACH(Robot*, i, (*this)) delete *i;
  clear();
}

void Robots::prePhysicsUpdate()
{
  _reset = false;
  _quit  = false;
  FOREACH(Robot*, i, (*this)) (*i)->prePhysicsUpdate();
}

void Robots::postPhysicsUpdate()
{
  for(std::vector<Robot*>::iterator i = begin(); i != end(); i++)
  {
    (*i)->postPhysicsUpdate();
    _reset |= (*i)->isReset();
    _quit  |= (*i)->isQuit();
    if(_seed == -1) _seed = (*i)->seed();
  }
}

void Robots::controllerUpdate()
{
  FOREACH(Robot*, i, (*this)) (*i)->controllerUpdate();
}

bool Robots::isReset()
{
  return _reset;
}

bool Robots::isQuit()
{
  return _quit;
}

void Robots::reset()
{
  if(_seed != -1)
  {
    // HIER RANDOM SEED SETZEN
  }
  FOREACH(Robot*, m, (*this)) (*m)->reset();
  _seed = -1;
}

int Robots::seed()
{
  return _seed;
}
