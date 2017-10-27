#include "MuscleActuator.h"

#include <cmath>
#include "yars/configuration/data/DataMuscleActuator.h"
#include "yars/physics/bullet/Actuator.h"
#include "yars/physics/bullet/Robot.h"

using namespace std;

MuscleActuator::MuscleActuator(DataMuscleActuator& data, Robot& robot)
  : Actuator{"MuscleActuator", data.source(), data.destination(), &robot},
    _data{data}
    //_l0{0},
    //_lopt{0.9 * _l0}
{
  _constraint = createConstraint();

  // Disable rotation.
  _constraint->setLowerAngLimit(0.0);
  _constraint->setUpperAngLimit(0.0);
  _constraint->setPoweredAngMotor(false);

  _constraint->setLowerLinLimit(0.0);
  _constraint->setUpperLinLimit(2.0);

  // Enable/Disable active movement. Can always be changed during simulation.
  _constraint->setPoweredLinMotor(false);

  //for(int i = 0; i < 6; i++) {
    //_constraint->setParam(BT_CONSTRAINT_STOP_ERP, 1.0, i);
    //_constraint->setParam(BT_CONSTRAINT_STOP_CFM, 0.0, i);
  //}

  //if (_data.isDeflectionSet()) {
  //_constraint->setLowerLinLimit(_data.deflection().min);
  //_constraint->setUpperLinLimit(_data.deflection().max); } else {
  //_constraint->setLowerLinLimit(1); // unset limit solving
  //_constraint->setUpperLinLimit(0); }

  cout << "Data control type: " << _data.controlType() << endl;
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

  // In case vectors are perpendicular there is no unique solution. We choose an
  // arbitrary rotation then.
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
  if (_constraint != nullptr) delete _constraint;
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
    cout << "Visual Output:\n" << endl;
  }
  
  /** 
   * To make the constraint move passively the motor has to be disabled and only
   * enabled when there is a force to put on the body.
   * **/

  yReal internalDesired = _data.getInternalDesiredValue(0);
  cout << "Internal Desired: " << internalDesired << endl;

  if (internalDesired > 0.1 && internalDesired <= 0.7)
  {
    if (!_constraint->getPoweredLinMotor()) // If motor is disabled.
    {
      _constraint->setPoweredLinMotor(true);
    }
  }
  else
  {
    _constraint->setPoweredLinMotor(false);
  }

  // Sine controller returns values from -1 to 1 after mapping. But we need
  // positive ones only.
  yReal a_t = fabs(internalDesired / 10.0); // A(t)

  // _data.force() returns Fmax and _data.velocity() vmax. For now it's
  // hardcoded in the class.
  //yReal force = * _data.force(); // _data.force() * xyz * A(t)

  // In this model elasticity isn't considered. The actuator simply stops at max
  // length.

  // TODO: Only needed once. Move somewhere else.
  _forceVelocityModel = linear;
  _forceLengthModel = linear;
  yReal _Fv, _Fl;
  yReal v = a_t;
  yReal _mu = 0.25;
  yReal _k = 10;
  yReal _L0 = 1;
  yReal _L = min(_constraint->getLinearPos(), _L0);
  //yReal _L = 0.4;
  switch (_forceVelocityModel) {
    case constant:
      _Fv = 1;
      break;
    case linear:
      _Fv = 1 - _mu * v;
      break;
    case hill:
      //if (v > 0) {
        //_Fv = (vmax + v) / (vmax - K * v);
      //} else {
        //_Fv = N + (N - 1) * ((vmax - v) / (-7.56 * K * v - vmax));
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
      _Fl = _k * (_L0 - _L);
      break;
    case hill:
      //_Fl = exp(c * pow(abs((L - Lopt) / (Lopt * w)), 3));
      break;
    //default:
      //error
  }
  yReal _Fmax = 2500;
  yReal _Fm = a_t * _Fl * _Fv * _Fmax;

  yReal _m = 80; // Adult man.
  yReal _g = 10; // Rounded gravitational constant as in paper.
  yReal force = - _m * _g + _Fm;

  //yReal velocity = fabs(a_t) * _data.velocity(); // _data.velocity() * abc * A(T)
  // Force = -mg + Fm (Force at ground contact. Else it's 0.)
  // Fm = A(t) * Fl * Fv * Fmax

  //yReal velocity = _data.velocity();
  yReal velocity = 10 * -a_t;
  force = 2000;
 
  // The velocity is the maximum speed of the contraction. It is slowed down, if
  // there is not enough force generated to move the bodypart.
  _constraint->setMaxLinMotorForce(force);
  // The maximum force to Seems to make no difference in movement after a
  // certain threshold.
  _constraint->setTargetLinMotorVelocity(velocity);

  // Logging.
  _data.setAppliedForceAndVelocity(0, force, velocity);

  cout << "Fv: " << _Fv << endl;
  cout << "Fl: " << _Fl << endl;
  cout << "LinearPos: " << _constraint->getLinearPos() << endl;
  cout << "Motor State: " << _constraint->getPoweredLinMotor() << endl;
  cout << "L: " << _L << endl;
  cout << "_data->force(): " << _data.force() << endl;
  cout << "_data->velocity(): " << _data.velocity() << endl;
  cout << "a_t: " << a_t << endl;
  cout << "_Fm: " << _Fm << endl;
  cout << "F: " << force << " " << "v: " << velocity << endl;
}

void MuscleActuator::processPositional()
{
}

void MuscleActuator::processVelocitySlider()
{
}

void MuscleActuator::processForceSlider()
{
  cout << "process force slider" << endl;
}

void MuscleActuator::postPhysicsUpdate()
{
  if(_isVisualised)
  {
    btTransform  pose = _constraint->getCalculatedTransformA();
    btVector3    vec  = pose.getOrigin();
    btQuaternion q    = pose.getRotation();
    _data.setCurrentAxisPosition(P3D(vec[0], vec[1], vec[2]));
    _data.setCurrentAxisOrientation(::Quaternion(q.getW(), q.getX(), q.getY(), q.getZ()));
  }
}

void MuscleActuator::reset()
{
}

btTypedConstraint* MuscleActuator::constraint()
{
  return _constraint;
}
