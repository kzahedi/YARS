#include "MuscleActuator.h"

#include "yars/configuration/data/DataMuscleActuator.h"
#include "yars/physics/bullet/Actuator.h"
#include "yars/physics/bullet/Robot.h"

MuscleActuator::MuscleActuator(DataMuscleActuator& data, Robot& robot)
  : Actuator("MuscleActuator", data.source(), data.destination(), &robot),
    _data(data)
{
}

MuscleActuator::~MuscleActuator()
{
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
}
