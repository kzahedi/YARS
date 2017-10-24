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
    //btPoint2PointConstraint* createConstraint();
    std::unique_ptr<btPoint2PointConstraint> createConstraint();

    DataPointConstraint &_data;
    std::unique_ptr<btPoint2PointConstraint> _constraint;

    bool _isVisualised;
};

#endif // __POINT_CONSTRAINT_H__
