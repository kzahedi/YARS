#include "MuscleActuator.h"

#include <cmath>
#include "yars/configuration/data/DataMuscleActuator.h"
#include "yars/physics/bullet/Actuator.h"
#include "yars/physics/bullet/Robot.h"
#include "yars/util/Timer.h"

using namespace std;

MuscleActuator::MuscleActuator(DataMuscleActuator& data, Robot& robot)
  : Actuator{"MuscleActuator", data.source(), data.destination(), &robot},
    _data(data),
    _vmax(-3.5),
    _forceVelocityModel(data.getForceVelocityModel()),
    _forceLengthModel(data.getForceLengthModel())
{
  _yarsConfig = YarsConfiguration::instance();

  _constraint = _createTransformedSliderConstraint();
  _setupConstraint();

  _lastTime = _yarsConfig->getCurrentRealTime();
  _lastPos = _constraint->getLinearPos();
  _lastVelocity = 0;

  _L0 = 0;

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

void MuscleActuator::_setupConstraint() const {
  _disableRotation(_constraint);
  _constraint->setLowerLinLimit(0.0);
  _constraint->setPoweredLinMotor(false);
}

void MuscleActuator::_disableRotation(btSliderConstraint *constraint) const {
  constraint->setLowerAngLimit(0.0);
  constraint->setUpperAngLimit(0.0);
  constraint->setPoweredAngMotor(false);
}

btSliderConstraint *MuscleActuator::_createTransformedSliderConstraint() const {
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
  btScalar rotation = sqrt(1 + dotProduct);

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


  // lifeTime variable: in evaluate.h in update controller function
  // Hier eher: __YARS_GET_STEP
  // in bulletphysics::reset (dort nur in bullet. Muss anpassen.
  // Konsequent: in robot::controlerUpdate. __yarsstep < intial_convergenceit
  //  In yars::configuration(abgefragt) wie in zeile 20  | in datasimulator
  if (__YARS_GET_STEP > 10) {
    if (_L0 == 0)
    {
      _L0 = _constraint->getLinearPos();
    }

    double Fm = _calcForce();

//    cout << "Force: " << Fm << endl;
//    cout << "--------------------------------------------------" << endl;

    // The velocity is the maximum speed of the contraction. It is slowed down if
    // there is not enough force generated to move the bodypart. The controller
    // should only tell the desired velocity.
    _constraint->setMaxLinMotorForce(static_cast<btScalar>(Fm));
    _constraint->setTargetLinMotorVelocity(static_cast<btScalar>(_vmax));
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
    _data.setCurrentAxisPosition(P3D(vec.getX(), vec.getY(), vec.getZ()));
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

double MuscleActuator::_calcVelocity()
{
  if (_lastTime == 0) // Init with healthy values.
  {
    _lastTime = _yarsConfig->getCurrentRealTime();
    _lastPos = _constraint->getLinearPos();
    return 0;
  }

  auto crntTime = _yarsConfig->getCurrentRealTime();
  auto crntPos = _constraint->getLinearPos();
  double v = (crntPos - _lastPos) / (crntTime - _lastTime);
  _lastPos = crntPos;
  _lastTime = crntTime;
  _lastVelocity = v;

  return v;
}

double MuscleActuator::_calcForce() {
  double internalDesired = _data.getInternalDesiredValue(0);

  if (!_isMotorEnabled())
  {
    _constraint->setPoweredLinMotor(true);
  }

  double a_t = internalDesired > 0.0 ? internalDesired : 0;
  double v = _calcVelocity();

  double Fv;
  double Fl;
  Fv = _calcForceVelocity(v);
  Fl = _calcForceLength();

  double _Fmax = _data.getMaxForce() == 0 ? 2000.0 : _data.getMaxForce();

//  cout << "Velocity: " << v << endl;
//  cout << "Max velocity: " << _vmax << "; A(t): " << a_t << endl;
//  cout << "Applied impulse: " << _constraint->getAppliedImpulse() << endl;
//  cout << "L:  " << _constraint->getLinearPos() << endl;
//  cout << "L0: " << _L0 << endl;


  return a_t * Fl * Fv * _Fmax; // Resulting muscle force.

//  cout << "Fv: " << Fv << endl;
//  cout << "Fl: " << Fl << endl;
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

double MuscleActuator::_calcForceVelocity(double v) const
{
  double _mu = 0.25;

  double Fv;
  if (_forceVelocityModel == "constant")
  {
    Fv = 1;
  }
  else if (_forceVelocityModel == "linear")
  {
    Fv = 1 - _mu * v;
  }
  else if (_forceVelocityModel == "hill")
  {
    //if (v > 0) {
    //Fv = (_vmax + v) / (_vmax - K * v);
    //} else {
    //Fv = N + (N - 1) * ((_vmax - v) / (-7.56 * K * v - _vmax));
    //}
  }
  else
  {
    //error
  }
  return Fv;
}

double MuscleActuator::_calcForceLength() const
{
  double _k = 10;
  double L = _constraint->getLinearPos();

  double Fl;
  if (_forceLengthModel == "constant")
  {
    Fl = 1;
  }
  else if (_forceLengthModel == "linear")
  {
    Fl = _k * (_L0 - L);
  }
  else if (_forceLengthModel == "hill")
  {
    //Fl = exp(c * pow(abs((L - Lopt) / (Lopt * w)), 3));
  }
  else
  {
    //error
  }
  return Fl;
}

bool MuscleActuator::_isMotorEnabled() const
{
  return _constraint->getPoweredLinMotor();
}
