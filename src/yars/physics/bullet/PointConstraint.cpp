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

  //return new btPoint2PointConstraint(*source, *destination, btVector3(0,0.5,-0.01),
      //btVector3(0,0.5,0.1));
  return std::make_unique<btPoint2PointConstraint>(*source, *destination,
      btVector3(0,0,-0.2), btVector3(0,0,0.01));
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
  cout << "PointConstraint Impulse: " << _constraint->getAppliedImpulse() << endl;
}

void PointConstraint::postPhysicsUpdate()
{
  if(_isVisualised)
  {
    //_constraint->getPivontInA();

    //btTransform  pose = _constraint->getCalculatedTransformA();
    //btVector3    vec  = pose.getOrigin();
    //btQuaternion q    = pose.getRotation();
    //_data.setCurrentAxisPosition(P3D(vec[0], vec[1], vec[2]));
    //_data.setCurrentAxisOrientation(::Quaternion(q.getW(), q.getX(), q.getY(), q.getZ()));
  }
}

void PointConstraint::reset()
{
}

btTypedConstraint* PointConstraint::constraint()
{
  return _constraint.get();
}
