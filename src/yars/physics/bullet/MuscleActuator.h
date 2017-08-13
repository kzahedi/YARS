#ifndef __MUSCLE_ACTUATOR_H__
#define __MUSCLE_ACTUATOR_H__

#include "yars/physics/bullet/Actuator.h"

class DataMuscleActuator;

enum SliderType { positional, velocity, force, force_velocity };

enum RelationModel { constant, linear, hill };

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
    void processPositional();
    void processVelocitySlider();
    void processForceSlider();

    btSliderConstraint* sliderConstraint(DataMuscleActuator& _data);

    DataMuscleActuator& _data;
    btSliderConstraint* _sliderConstraint;
    SliderType _sliderType;
    RelationModel _forceVelocityModel;
    RelationModel _forceLengthModel;

    //yReal               _position;
    //yReal               _lastPosition;
    //yReal               _friction;
    // bool                _isActive;
    //bool                _isVisualised;
    //bool                _hasFriction;

    // TODO: implement calculation
    //const double _l0;
    //const double _lopt;
    //const double _m;
    //const double _fmax;
    //const double _w;
    //const double _c;
    //const double _vmax;
    //const double _K;
    //const double _n;
    //const double _k;
    //const double _mu;
    //constexpr static double g = 9.81;
};

#endif // __MUSCLE_ACTUATOR_H__
