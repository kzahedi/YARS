#ifndef __MUSCLE_ACTUATOR_H__
#define __MUSCLE_ACTUATOR_H__

#include "yars/physics/bullet/Actuator.h"

class DataMuscleActuator;

class MuscleActuator : public Actuator
{
  public:
    MuscleActuator(DataMuscleActuator& data, Robot& robot);
    ~MuscleActuator();

    DataActuator* data() override;
    void prePhysicsUpdate() override;
    void postPhysicsUpdate() override;
    void reset() override;
    btTypedConstraint* constraint() override;

  private:
    DataMuscleActuator& _data;
};

#endif // __MUSCLE_ACTUATOR_H__
