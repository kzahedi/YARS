#ifndef __MUSCLE_ACTUATOR_H__
#define __MUSCLE_ACTUATOR_H__

#include <yars/physics/bullet/Actuator.h>
#include <yars/configuration/YarsConfiguration.h>

#include <bullet/btBulletDynamicsCommon.h>

class MuscleActuator : public Actuator
{
  public:
    MuscleActuator(DataMuscleActuator *data, Robot *robot);
    ~MuscleActuator();

    void prePhysicsUpdate();
    void postPhysicsUpdate();
    void reset();
    btTypedConstraint* constraint();

    DataMuscleActuator* data();

  private:
    void __processPositional();
    void __processVelocityMuscle();
    void __processForceMuscle();
    void __initSource();
    void __initDestination();
    void __initSlider();
    void __setPoint2Point(btGeneric6DofConstraint *_genericConstraint);
    double __calculateLength();

    DataMuscleActuator*      _data;
    MuscleParameter          _parameter;
    double                   _position;
    double                   _lastPosition;
    bool                     _isActive;
    bool                     _isVisualised;
    double                   _velocity;
    double                   _muscleVelocity;
    double                   _maxForce;
    double                   _length;
    double                   _lastLength;
    double                   _lOpt;
    double                   _w;
    double                   _c;
    double                   _velMax;
    double                   _N;
    double                   _K;
    btSliderConstraint*      _muscleConstraint;
    btGeneric6DofConstraint* _sourceBall;
    btGeneric6DofConstraint* _destinationBall;
    btRigidBody*             _srcAnchor;
    btRigidBody*             _dstAnchor;
    btRigidBody*             _src;
    btRigidBody*             _dst;
    double                   _cfm;
    double                   _erp;
    bool                     _useLengthComponent;
    bool                     _useVelocityComponent;
};

#endif // __MUSCLE_ACTUATOR_H__


