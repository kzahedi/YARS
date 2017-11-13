#include "MuscleActuator.h"

#include <cmath>
#include "yars/configuration/data/DataMuscleActuator.h"
#include "yars/physics/bullet/Actuator.h"
#include "yars/physics/bullet/Robot.h"
#include "yars/util/Timer.h"

using namespace std;

MuscleActuator::MuscleActuator(DataMuscleActuator& data, Robot& robot)
  : Actuator{"MuscleActuator", data.source(), data.destination(), &robot},
    _data{data},
    _vmax{-3.5}
    //_lopt{0.9 * _l0}
{
  _constraint = createConstraint();

  // Disable rotation.
  _constraint->setLowerAngLimit(0.0);
  _constraint->setUpperAngLimit(0.0);
  _constraint->setPoweredAngMotor(false);

  _constraint->setLowerLinLimit(0.0);
  _constraint->setUpperLinLimit(2.0);

  // Enable/Disable active movement.
  _constraint->setPoweredLinMotor(false);

  _lastTime = Timer::getTime();
  _lastPos = _constraint->getLinearPos();
  _startTime = _lastTime;

  _L0 = 0;

  //for(int i = 0; i < 6; i++) {
    //_constraint->setParam(BT_CONSTRAINT_STOP_ERP, 1.0, i);
    //_constraint->setParam(BT_CONSTRAINT_STOP_CFM, 0.0, i);
  //}

  //if (_data.isDeflectionSet()) {
  //_constraint->setUpperLinLimit(_data.deflection().max); } else {
  //_constraint->setLowerLinLimit(1); // unset limit solving
  //_constraint->setUpperLinLimit(0); }

  switch(_data.controlType()) {
    case DATA_ACTUATOR_CONTROL_POSITIONAL:
      _type = positional;
      break;
    case DATA_ACTUATOR_CONTROL_VELOCITY:
      _type = velocity;
      break;
    case DATA_ACTUATOR_CONTROL_FORCE:
      _type = force;
      break;
    case DATA_ACTUATOR_CONTROL_FORCE_VELOCITY:
      _type = force_velocity;
      break;
    default:
      cout << "unkown type: " << _data.controlType() << endl;
  }

  _isVisualised = Data::instance()->current()->screens()->visualiseJoints();
}

btSliderConstraint* MuscleActuator::createConstraint()
{
  btRigidBody* source = _sourceObject->rigidBody();
  btRigidBody* destination = _destinationObject->rigidBody();

  // Get direction from source x-Axis to destination.
  btVector3 direction = (destination->getWorldTransform().getOrigin() -
      source->getWorldTransform().getOrigin()).normalize();

  btVector3 sourceAxis = source->getOrientation().getAxis().normalize();

  btVector3 rotationAxis = sourceAxis.cross(direction);
  btScalar dotProduct = sourceAxis.dot(direction);

  // Formula: sqrt(|a|^2 * |b|^2 + dotproduct). In our case the magnitude is
  // 1 because of normalization.
  btScalar rotation = std::sqrt(1 + dotProduct);

  // In case vectors are perpendicular there is no unique solution. In that case
  // we choose an arbitrary rotation.
  btQuaternion q;
  if (dotProduct < 0.0 && rotationAxis == btVector3(0.0, 0.0, 0.0))
  {
    q = btQuaternion(sourceAxis.getX(), sourceAxis.getY(), sourceAxis.getZ(),
        0.0);
  }
  else
  {
    q = btQuaternion(rotationAxis.getX(), rotationAxis.getY(),
        rotationAxis.getZ(), rotation);
  }

  // Positions are relative to the rigid bodies local origin.
  btTransform frameInA = btTransform::getIdentity();
  btTransform frameInB = btTransform::getIdentity();

  frameInA.setRotation(q * frameInA.getRotation());

  return new btSliderConstraint(*source, *destination, frameInA, frameInB,
      true);
}

MuscleActuator::~MuscleActuator()
{
}

DataActuator* MuscleActuator::data()
{
  return &_data;
}

void MuscleActuator::prePhysicsUpdate()
{
  { // visualize contact points.

    //btTransform  pose = _constraint->getCalculatedTransformA(); // Same as for B.
    //
    // That's how it's calculated. transA and transB seem to be the world
    // transform. m_frameInA is transformA at passing to the constructor in our case.
      //m_calculatedTransformA = transA * m_frameInA;
      //m_calculatedTransformB = transB * m_frameInB;
      //
      //m_frameInA = rbB.getCenterOfMassTransform() * m_frameInB;
      //oder wird beim Initialisieren direkt mitgegenben.

    //btTransform  pose = _constraint->getFrameOffsetA(); // At point of origin.
    //btVector3    vec  = pose.getOrigin();
    //btQuaternion q    = pose.getRotation();
    // Coordinates are relative to world.
    //_data.setCurrentAxisPosition(P3D(vec[0], vec[1], vec[2]));
    //_data.setCurrentAxisOrientation(::Quaternion(q.getW(), q.getX(), q.getY(), q.getZ()));
  }

  if (Timer::getTime() - _startTime > 2000) {
    if (_L0 == 0)
    {
      _L0 = _constraint->getLinearPos();
    }

    double Fm = calcForce();

    cout << "Force: " << Fm << endl;
    cout << "--------------------------------------------------" << endl;

    // The velocity is the maximum speed of the contraction. It is slowed down if
    // there is not enough force generated to move the bodypart. The controller
    // should only tell the desired velocity.
    _constraint->setMaxLinMotorForce(Fm);
    _constraint->setTargetLinMotorVelocity(_vmax);
  }
}

void MuscleActuator::processPositional()
{
}

void MuscleActuator::processVelocitySlider()
{
}

void MuscleActuator::processForceSlider()
{
}

void MuscleActuator::postPhysicsUpdate()
{
  if(_isVisualised)
  {
    btTransform  pose = _constraint->getCalculatedTransformA();
    btVector3    vec  = pose.getOrigin();
    btQuaternion q    = pose.getRotation();
    _data.setCurrentAxisPosition(P3D(vec[0], vec[1], vec[2]));
    _data.setCurrentAxisOrientation(::Quaternion(q.getW(), q.getX(), q.getY(),
                                                 q.getZ()));
  }
}

void MuscleActuator::reset()
{
}

btTypedConstraint* MuscleActuator::constraint()
{
  return _constraint;
}

double MuscleActuator::calcVelocity() {
  unsigned long crntTime = Timer::getTime();
  double crntPos = _constraint->getLinearPos();
  double v = (_lastPos - crntPos) / (_lastTime - crntTime) / 1000.0;
  _lastPos = crntPos;
  _lastTime = crntTime;
  return v;
}

double MuscleActuator::calcForce() {
  double internalDesired = _data.getInternalDesiredValue(0);

  if (!_constraint->getPoweredLinMotor()) // If motor is disabled.
  {
    _constraint->setPoweredLinMotor(true);
  }

  double a_t = internalDesired;

  // TODO:
  // _data.force() returns Fmax and _data.velocity() _vmax. For now it's
  // hardcoded in the class.

  _forceVelocityModel = linear;
  _forceLengthModel = linear;

  // TODO: Refactor.
  double _Fv, _Fl;
  double v = calcVelocity();
  double _mu = 0.25;
  double _k = 10;
  double L = _constraint->getLinearPos();
  double _Fmax = 5000;

  switch (_forceVelocityModel) {
    case constant:
      _Fv = 1;
      break;
    case linear:
      _Fv = 1 - _mu * v;
      break;
    case hill:
      //if (v > 0) {
      //_Fv = (_vmax + v) / (_vmax - K * v);
      //} else {
      //_Fv = N + (N - 1) * ((_vmax - v) / (-7.56 * K * v - _vmax));
      //}
      break;
      //default:
      //error
  }
  switch (_forceLengthModel) {
    case constant:
      _Fl = 1;
      break;
    case linear:
      _Fl = _k * (_L0 - L);
      break;
    case hill:
      //_Fl = exp(c * pow(abs((L - Lopt) / (Lopt * w)), 3));
      break;
      //default:
      //error
  }

  cout << "Velocity: " << v << endl;
  cout << "Max velocity: " << _vmax << "; A(t): " << a_t << endl;
  cout << "Applied impulse: " << _constraint->getAppliedImpulse() << endl;
  cout << "L:  " << _constraint->getLinearPos() << endl;
  cout << "L0: " << _L0 << endl;

  return a_t * _Fl * _Fv * _Fmax; // Resulting muscle force.

//  cout << "Fv: " << _Fv << endl;
//  cout << "Fl: " << _Fl << endl;
//  cout << "LinearPos: " << _constraint->getLinearPos() << endl;
//  cout << "Motor State: " << _constraint->getPoweredLinMotor() << endl;
//  cout << "L: " << L << endl;
//  cout << "_data->force(): " << _data.force() << endl;
//  cout << "_data->velocity(): " << _data.velocity() << endl;
//  cout << "a_t: " << a_t << endl;
//  cout << "_Fm: " << _Fm << endl;
//  cout << "F: " << force << " " << "v: " << velocity << endl;

  // Logging.
//  _data.setAppliedForceAndVelocity(0, Fm, v);
}
