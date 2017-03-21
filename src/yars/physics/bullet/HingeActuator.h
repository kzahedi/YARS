#ifndef __HINGE_ACTUATOR_H__
#define __HINGE_ACTUATOR_H__

#include "yars/configuration/data/DataHingeActuator.h"
#include "yars/physics/bullet/Actuator.h"

#include <btBulletDynamicsCommon.h>

class HingeActuator : public Actuator
{
  public:
    HingeActuator(DataHingeActuator *data, Robot *robot);
    ~HingeActuator();

    void prePhysicsUpdate();
    void postPhysicsUpdate();
    void reset();
    btTypedConstraint* constraint();

    DataActuator* data();

  private:
    void __processAngularHinge();
    void __processVelocityHinge();
    void __processForceHinge();
    void __processForceVelocityHinge();
    void __processForceAngularHinge();
    void __initHinge();

    DataHingeActuator  *_data;
    btSliderConstraint *_hingeConstraint;
    HingeParameter      _parameter;
    int                 _hingeType;
    yReal               _angle;
    yReal               _lastAngle;
    yReal               _angleDiff;
    yReal               _friction;
    bool                _isActive;
    bool                _isVisualised;
    bool                _hasFriction;
    Pose                _axisPose;
    ::Quaternion        _axisQ;
};

#endif // __HINGE_ACTUATOR_H__


