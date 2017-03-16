#include "DataMuscleActuator.h"

#include "yars/configuration/data/DataActuator.h"

DataMuscleActuator::DataMuscleActuator(DataNode* parent)
  : DataActuator(parent, DATA_ACTUATOR_MUSCLE)
{
}

DataMuscleActuator::~DataMuscleActuator()
{
}

void DataMuscleActuator::applyOffset(Pose pose)
{
}

std::string DataMuscleActuator::source()
{
}

std::string DataMuscleActuator::destination()
{
}

std::string DataMuscleActuator::name()
{
}

void DataMuscleActuator::setInternalValue(int index, yReal value)
{
}

void DataMuscleActuator::setExternalValue(int index, yReal value)
{
}

yReal DataMuscleActuator::internalValue(int index)
{
}

yReal DataMuscleActuator::externalValue(int index)
{
}

int DataMuscleActuator::dimension()
{
}

void DataMuscleActuator::setDesiredValue(int index, yReal value)
{
}

yReal DataMuscleActuator::getInternalDesiredValue(int index)
{
}

yReal DataMuscleActuator::getExternalDesiredValue(int index)
{
}

Domain DataMuscleActuator::getInternalDomain(int index)
{
}

Domain DataMuscleActuator::getExternalDomain(int index)
{
}

bool DataMuscleActuator::isActive(int index)
{
}

yReal DataMuscleActuator::getAppliedForce(int index)
{
}

yReal DataMuscleActuator::getAppliedVelocity(int index)
{
}

void DataMuscleActuator::setAppliedForceAndVelocity(int index, yReal force,
                                                    yReal velocity)
{
}

Pose DataMuscleActuator::pose()
{
}

DataActuator* DataMuscleActuator::_copy()
{
}

