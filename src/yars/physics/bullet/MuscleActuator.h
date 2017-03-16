#ifndef __MUSCLE_ACTUATOR_H__
#define __MUSCLE_ACTUATOR_H__

#include "yars/physics/bullet/Actuator.h"
#include "yars/configuration/data/DataMuscleActuator.h"
#include "yars/physics/bullet/Robot.h"

class MuscleActuator : public Actuator
{
  public:
    MuscleActuator(DataMuscleActuator* data, Robot* robot);
    ~MuscleActuator();

    DataActuator* data();
    void prePhysicsUpdate();
    void postPhysicsUpdate();
    void reset();
    btTypedConstraint* constraint();

  private:
    DataMuscleActuator* _data;
};

#endif // __MUSCLE_ACTUATOR_H__
