#include "MuscleActuator.h"

#include "yars/configuration/data/DataMuscleActuator.h"
#include "yars/physics/bullet/Actuator.h"
#include "yars/physics/bullet/Robot.h"

MuscleActuator::MuscleActuator(DataMuscleActuator& data, Robot& robot)
  : Actuator{"MuscleActuator", data.source(), data.destination(), &robot},
    _data{data}
    //_l0{0},
    //_lopt{0.9 * _l0}
{
  _sliderConstraint = sliderConstraint(_data);

  _sliderConstraint->setLowerAngLimit(0.0);
  _sliderConstraint->setUpperAngLimit(0.0);
  _sliderConstraint->setPoweredAngMotor(false);

  for(int i = 0; i < 6; i++)
  {
    _sliderConstraint->setParam(BT_CONSTRAINT_STOP_ERP, 1.0, i);
    _sliderConstraint->setParam(BT_CONSTRAINT_STOP_CFM, 0.0, i);
  }

  _sliderConstraint->setPoweredLinMotor(true);

  //if (_data.isDeflectionSet()) {
    //_sliderConstraint->setLowerLinLimit(_data.deflection().min);
    //_sliderConstraint->setUpperLinLimit(_data.deflection().max);
  //} else {
    //_sliderConstraint->setLowerLinLimit(1); // unset limit solving
    //_sliderConstraint->setUpperLinLimit(0);
  //}

  switch(_data.controlType()) {
  case DATA_ACTUATOR_CONTROL_POSITIONAL: _sliderType = positional; break;
  case DATA_ACTUATOR_CONTROL_VELOCITY: _sliderType = velocity; break;
  case DATA_ACTUATOR_CONTROL_FORCE: _sliderType = force; break;
  }
}

MuscleActuator::~MuscleActuator()
{
  if (_sliderConstraint != nullptr) delete _sliderConstraint;
}

btSliderConstraint* MuscleActuator::sliderConstraint(DataMuscleActuator& _data)
{
  Pose axis = _data.pose();

  btRigidBody* source = _sourceObject->rigidBody();
  btRigidBody* destination = _destinationObject->rigidBody();

  ::Quaternion qa(axis.orientation);
  btQuaternion q(qa.x, qa.y, qa.z, qa.w);

  btTransform transformA(q,
    btVector3(axis.position.x, axis.position.y, axis.position.z));

  auto transformB = destination->getWorldTransform().inverse() * transformA;
  transformA = source->getWorldTransform().inverse() * transformA;

  return new btSliderConstraint(*source, *destination, transformA, transformB,
      true);
}

DataActuator* MuscleActuator::data()
{
  return &_data;
}

void MuscleActuator::prePhysicsUpdate()
{
}

void MuscleActuator::postPhysicsUpdate()
{
}

void MuscleActuator::reset()
{
}

btTypedConstraint* MuscleActuator::constraint()
{
  return _sliderConstraint;
}
