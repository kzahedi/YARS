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
    btSliderConstraint* _constraint;
    YarsConfiguration *_yarsConfig;

    const string _forceVelocityModel;
    const string _forceLengthModel;

    bool _isVisualised;
    double _lastTime;
    double _lastPos;

    double _L0;
    double _Lopt;
    const double _fmax;
    double _vmax;
    double _calcVelocity();

    double _calcForce();
    void _disableRotation(btSliderConstraint *constraint) const;
    btSliderConstraint * _createTransformedSliderConstraint() const;
    void _setupConstraint() const;
    double _calcForceLength() const;
    double _calcForceVelocity(double v) const;
    btVector3 _getDirectionFromSourceToDestination(btRigidBody *source,
                                                   btRigidBody *destination) const;
};

#endif // __MUSCLE_ACTUATOR_H__
