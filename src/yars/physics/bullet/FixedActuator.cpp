#include "FixedActuator.h"
#include "yars/configuration/data/DataFixedActuator.h"
#include <math.h>

#define ANGULAR  1234
#define VELOCITY 1235

#define MAP(min, max, value) ((value > max)?max:(value<min)?min:value)
#define _SIGN(value)         ((value < 0)?-1:1)

FixedActuator::FixedActuator(DataFixedActuator *data, Robot *robot)
  : Actuator("FixedActuator", data->source(), data->destination(), robot)
{
  _data = data;

  btTransform sTransform = btTransform::getIdentity();
  btTransform dTransform = btTransform::getIdentity();

  if(_sourceObject != NULL && _destinationObject != NULL)
  {
    btRigidBody *source      = _sourceObject->rigidBody();
    btRigidBody *destination = _destinationObject->rigidBody();

    dTransform = destination->getWorldTransform().inverse() * source->getWorldTransform() * sTransform;

    _fixedConstraint = new btSliderConstraint(*source, *destination, sTransform, dTransform, true);
  }
  else if(_sourceObject != NULL)
  {
    btRigidBody *source = _sourceObject->rigidBody();
    _fixedConstraint    = new btSliderConstraint(*source, sTransform, true);
  }
  else 
  {
    btRigidBody *destination = _destinationObject->rigidBody();
    _fixedConstraint         = new btSliderConstraint(*destination, dTransform, true);
  }

  _fixedConstraint->setLowerLinLimit(0.0);
  _fixedConstraint->setUpperLinLimit(0.0);
  _fixedConstraint->setLowerAngLimit(0.0);
  _fixedConstraint->setUpperAngLimit(0.0);
  for(int i = 0; i < 6; i++)
  {
    _fixedConstraint->setParam(BT_CONSTRAINT_STOP_ERP, 1.0, i);
    _fixedConstraint->setParam(BT_CONSTRAINT_STOP_CFM, 0.0, i);
  }

}

DataActuator* FixedActuator::data()
{
  return _data;
}

btTypedConstraint* FixedActuator::constraint()
{
  return _fixedConstraint;
}

void FixedActuator::prePhysicsUpdate()  { }
void FixedActuator::postPhysicsUpdate() { }
void FixedActuator::reset()             { }
