#ifndef __MUSCLE_ACTUATOR_H__
#define __MUSCLE_ACTUATOR_H__

#include <yars/physics/bullet/Actuator.h>
#include <yars/configuration/YarsConfiguration.h>

#include <btBulletDynamicsCommon.h>

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

    DataMuscleActuator*      _data;
    MuscleParameter          _parameter;
    double                   _position;
    double                   _lastPosition;
    bool                     _isActive;
    bool                     _isVisualised;
    double                   _velocity;
    double                   _maxForce;
    double                   _length;
    double                   _lastLength;

    btSliderConstraint*      _muscleConstraint;
    btPoint2PointConstraint* _sourceBall;
    btPoint2PointConstraint* _dstBall;
};

#endif // __MUSCLE_ACTUATOR_H__


