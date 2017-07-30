#ifndef __HINGE_ACTUATOR_H__
#define __HINGE_ACTUATOR_H__

#include <yars/physics/bullet/Actuator.h>
#include <yars/configuration/YarsConfiguration.h>

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

    DataHingeActuator* data();

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
    double               _angle;
    double               _lastAngle;
    double               _angleDiff;
    double               _friction;
    bool                _isActive;
    bool                _isVisualised;
    bool                _hasFriction;
    Pose                _axisPose;
    ::Quaternion        _axisQ;
};

#endif // __HINGE_ACTUATOR_H__


