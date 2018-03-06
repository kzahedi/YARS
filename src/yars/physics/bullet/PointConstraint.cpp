#include "PointConstraint.h"

#include <cmath>
#include "yars/configuration/data/DataPointConstraint.h"
#include "yars/physics/bullet/Actuator.h"
#include "yars/physics/bullet/Robot.h"

using namespace std;

PointConstraint::PointConstraint(DataPointConstraint& data, Robot& robot)
  : Actuator{"PointConstraint", data.source(), data.destination(), &robot},
    _data{data}
{
  _constraint = createConstraint();
  _constraint->setEnabled(true);

  _isVisualised = Data::instance()->current()->screens()->visualiseJoints();
}

std::unique_ptr<btPoint2PointConstraint> PointConstraint::createConstraint()
{
  btRigidBody* source = _sourceObject->rigidBody();
  btRigidBody* destination = _destinationObject->rigidBody();

  Pose pose = _data.pose();
  btVector3 position(pose.position.x, pose.position.y, pose.position.z);

 if (_data.mode() == "muscle")
  {
    // Destination is the sphere that connects source to MuscleActuator.
    return std::make_unique<btPoint2PointConstraint>(*source, *destination,
        position, btVector3(0,0,0));
  }
  else
  {
    return std::make_unique<btPoint2PointConstraint>(*source, position);
  }
}

PointConstraint::~PointConstraint()
{
}

DataActuator* PointConstraint::data()
{
  return &_data;
}

void PointConstraint::prePhysicsUpdate()
{
}

void PointConstraint::postPhysicsUpdate()
{
}

void PointConstraint::reset()
{
}

btTypedConstraint* PointConstraint::constraint()
{
  return _constraint.get();
}
