#include <yars/physics/YarsPhysicsModel.h>
#include <yars/view/console/ConsoleView.h>
#include <yars/configuration/YarsConfiguration.h>

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
    // Physics layer signalled a reset request. The old Observer chain
    // would notify YarsMainControl which called reset(); that chain is
    // gone, so reset directly. Bracketing with pause prevents the
    // physics from stepping while the reset is in progress.
    __YARS_SET_USE_PAUSE(true);
    __YARS_SET_RESET_SIMULATION;
    __YARS_SET_USE_PAUSE(false);
  }
  if (_physics->isQuit())
  {
    // Physics layer signalled a quit request. Set the global exit flag
    // that YarsMainControl::run()'s loop polls.
    __YARS_SET_EXIT(true);
  }
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
