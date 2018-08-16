#include <yars/physics/bullet/MuscleActuator.h>
#include <yars/util/macros.h>
#include <math.h>

#define POSITIONAL 123400
#define VELOCITY   123500
#define FORCE      123600

#define MAP(min, max, value) ((value > max)?max:(value<min)?min:value)
#define _SIGN(value)         ((value < 0)?-1:1)
#define SET(a, b)            if(b >= 0.0) a(b)
#define SCALE(a)             (MIN(0.5, fabs(diff)) / 0.5)


MuscleActuator::MuscleActuator(DataMuscleActuator *data, Robot *robot)
  : Actuator("MuscleActuator", data->source(), data->destination(), robot)
{
  _data             = data;
  _muscleConstraint = NULL;
  _lastLength       = 0.0;
  _length           = 0.0;
  _isVisualised     = Data::instance()->current()->screens()->visualiseJoints();
  _parameter        = _data->parameter();
  _velocity         = _data->velocity();
  _maxForce         = _data->force();

  // 1. connect passive ball joints
  __initSource();

  // 2. connect slider joint
}

MuscleActuator::~MuscleActuator()
{
  if(_muscleConstraint != NULL) delete _muscleConstraint;
}

void MuscleActuator::prePhysicsUpdate()
{
  // double v        = _data->getInternalDesiredValue(0);
  // double force    = fabs(v) * _data->force();
  // _muscleConstraint->setMaxLinMotorForce(force);
  // _muscleConstraint->setTargetLinMotorVelocity(_velocity);
}

void MuscleActuator::postPhysicsUpdate()
{
  // _position = _muscleConstraint->getLinearPos();

  // _data->setCurrentTransitionalVelocity((_position - _lastPosition)
      // * (double)__YARS_GET_SIMULATOR_FREQUENCY);
  // _lastPosition = _position;

  // if(_isVisualised)
  // {
    // btTransform  pose = _muscleConstraint->getCalculatedTransformA();
    // btVector3    vec  = pose.getOrigin();
    // btQuaternion q    = pose.getRotation();
    // _data->setCurrentAxisPosition(P3D(vec[0], vec[1], vec[2]));
    // _data->setCurrentAxisOrientation(::Quaternion(q.getW(), q.getX(), q.getY(), q.getZ()));
  // }
}

DataMuscleActuator* MuscleActuator::data()
{
  return _data;
}

void MuscleActuator::reset()
{
}

btTypedConstraint* MuscleActuator::constraint()
{
  return _muscleConstraint;
}

void MuscleActuator::__initSource()
{
  string sourceAnchorName  = _data->sourceObject()->name();
  btRigidBody *source      = __find(sourceAnchorName)->rigidBody();
  btRigidBody *destination = _sourceObject->rigidBody();

  P3D p = _data->sourceObject()->pose().position;

  btVector3 a(p.x, p.y, p.z);
  btVector3 b = destination->getWorldTransform().inverse() * a;

  _sourceBall = new btPoint2PointConstraint(*source, *destination,
                                            btVector3(0,0,0), b);

  _constraints.push_back(_sourceBall);
}

