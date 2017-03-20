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

    const double _l0;
    const double _lopt;
    const double _m;
    const double _fmax;
    const double _w;
    const double _c;
    const double _vmax;
    const double _K;
    const double _n;
    const double _k;
    const double _mu;
    constexpr static double g = 9.81;
};

#endif // __MUSCLE_ACTUATOR_H__
