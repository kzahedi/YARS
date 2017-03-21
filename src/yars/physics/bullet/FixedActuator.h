#ifndef __FIXED_ACTUATOR_H__
#define __FIXED_ACTUATOR_H__

#include <yars/physics/bullet/Actuator.h>
#include <yars/configuration/YarsConfiguration.h>

#include <btBulletDynamicsCommon.h>

class FixedActuator : public Actuator
{
  public:
    FixedActuator(DataFixedActuator *data, Robot *robot);

    // applies the motor value to hinge
    void prePhysicsUpdate();
    void postPhysicsUpdate();
    void reset();

    DataActuator* data();
    btTypedConstraint* constraint();

  private:

    DataActuator *_data;
    btSliderConstraint *_fixedConstraint;
};

#endif // __FIXED_ACTUATOR_H__


