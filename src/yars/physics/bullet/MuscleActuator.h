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
    enum Type { positional, velocity, force, force_velocity };
    enum RelationModel { constant, linear, hill };

    void processPositional();
    void processVelocitySlider();
    void processForceSlider();

    btSliderConstraint* createConstraint();

    DataMuscleActuator& _data;
    btSliderConstraint* _constraint;
    Type _type;
    RelationModel _forceVelocityModel;
    RelationModel _forceLengthModel;

    YarsConfiguration *_yarsConfig;

    //double               _position;
    //double               _lastPosition;
    //double               _friction;
    /*bool                _isActive;*/
    bool                _isVisualised;
    //bool                _hasFriction;
    double _lastTime;
    double _lastPos;
    double _lastVelocity;

    // TODO: implement calculation
    double _L0; // Rest length of muscle.
    //const double _lopt;
    //const double _m;
    //const double _fmax;
    //const double _w;
    //const double _c;
    const double _vmax;
    //const double _K;
    //const double _n;
    //const double _k;
    //const double _mu;
    //constexpr static double g = 9.81;
    double calcVelocity();

    double calcForce();
};

#endif // __MUSCLE_ACTUATOR_H__
