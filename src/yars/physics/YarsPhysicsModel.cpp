#include <yars/physics/YarsPhysicsModel.h>
#include <yars/view/console/ConsoleView.h>

YarsPhysicsModel::YarsPhysicsModel()
{
  _physics = new BulletPhysics();
}

YarsPhysicsModel::~YarsPhysicsModel()
{
  Y_DEBUG("YarsPhysicsModel destructor called.");
  if (_physics != NULL)
  {
    _physics->close();
    delete _physics;
  }
}

void YarsPhysicsModel::initialisePhysics()
{
  Y_DEBUG("YarsPhysicsModel will initialise the physics.");
  _physics->init();
  __YARS_SET_STEP(0);
}

void YarsPhysicsModel::performOneSimulationStep()
{
  Y_DEBUG("YarsPhysicsModel::performOneSimulationStep called.");
  _physics->step();
  if (_physics->isReset())
  {
    __YARS_SET_USE_PAUSE(true);
    notifyObservers(_m_reset);
    __YARS_SET_USE_PAUSE(false);
  }
  if (_physics->isQuit())
    notifyObservers(_m_quit_called);
}

void YarsPhysicsModel::performMultipleSimulationSteps(int numberOfSteps)
{
  Y_DEBUG("YarsPhysicsModel::performMultipleSimulationSteps called with parameter %d.", numberOfSteps);
  for (int i = 0; i < numberOfSteps; i++)
  {
    performOneSimulationStep();
  }
}

void YarsPhysicsModel::shutdown()
{
  _physics->close();
}

void YarsPhysicsModel::reset()
{
  Y_DEBUG("YarsPhysicsModel::reset()");
  _physics->reset();
}
