#include <yars/physics/bullet/Robots.h>

#include <yars/configuration/YarsConfiguration.h>

Robots::Robots()
{
  _reset = false;
  _quit  = false;
  _seed  = -1;
  DataRobots *robots = __YARS_GET_ROBOTS;
  if (robots == NULL) return;

  for (auto *i : *robots) push_back(std::make_unique<Robot>(i));
}

void Robots::prePhysicsUpdate()
{
  _reset = false;
  _quit  = false;
  for (auto &robot : *this) robot->prePhysicsUpdate();
}

void Robots::postPhysicsUpdate()
{
  for (auto &robot : *this)
  {
    robot->postPhysicsUpdate();
    _reset |= robot->isReset();
    _quit  |= robot->isQuit();
    if (_seed == -1) _seed = robot->seed();
  }
}

void Robots::controllerUpdate()
{
  for (auto &robot : *this) robot->controllerUpdate();
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
  if (_seed != -1)
  {
    // HIER RANDOM SEED SETZEN
  }
  for (auto &robot : *this) robot->reset();
  _seed = -1;
}

int Robots::seed()
{
  return _seed;
}
