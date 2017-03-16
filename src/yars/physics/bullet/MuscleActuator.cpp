#include "MuscleActuator.h"

#include "yars/configuration/data/DataMuscleActuator.h"
#include "yars/physics/bullet/Robot.h"

MuscleActuator::MuscleActuator(DataMuscleActuator* data, Robot* robot)
  : Actuator("MuscleActuator", data->source(), data->destination(), robot)
{
  _data = data;
}

MuscleActuator::~MuscleActuator()
{
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
