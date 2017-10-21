#ifndef __POINT_CONSTRAINT_H__
#define __POINT_CONSTRAINT_H__

#include "yars/physics/bullet/Actuator.h"

class DataPointConstraint;

class PointConstraint : public Actuator
{
  public:
    PointConstraint(DataPointConstraint& data, Robot& robot);
    ~PointConstraint();

    DataActuator* data() override;
    void prePhysicsUpdate() override;
    void postPhysicsUpdate() override;
    void reset() override;
    btTypedConstraint* constraint() override;

  private:
    btPoint2PointConstraint* createConstraint();

    DataPointConstraint& _data;
    btPoint2PointConstraint *_constraint;

    //yReal               _position;
    //yReal               _lastPosition;
    //yReal               _friction;
    /*bool                _isActive;*/
    bool                _isVisualised;
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

#endif // __POINT_CONSTRAINT_H__
