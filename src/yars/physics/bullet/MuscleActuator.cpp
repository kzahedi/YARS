#include "MuscleActuator.h"

#include <algorithm>
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
    _fmax(data.getMaxForce() == 0 ? 2000.0 : _data.getMaxForce()),
    _forceVelocityModel(data.getForceVelocityModel()),
    _forceLengthModel(data.getForceLengthModel())
{
  _yarsConfig = YarsConfiguration::instance();

  _constraint = _createTransformedSliderConstraint();
  _setupConstraint();

  _lastTime = _yarsConfig->getCurrentRealTime();
  _lastPos = _constraint->getLinearPos();

  _L0 = 0;

  _isVisualised = Data::instance()->current()->screens()->visualiseJoints();
}

void MuscleActuator::_setupConstraint() const
{
  _disableRotation(_constraint);
  _constraint->setLowerLinLimit(0.0);
  _constraint->setPoweredLinMotor(false);
}

void MuscleActuator::_disableRotation(btSliderConstraint *constraint) const
{
  constraint->setLowerAngLimit(0.0);
  constraint->setUpperAngLimit(0.0);
  constraint->setPoweredAngMotor(false);
}

btSliderConstraint *MuscleActuator::_createTransformedSliderConstraint() const
{
  auto source = _sourceObject->rigidBody();
  auto destination = _destinationObject->rigidBody();

  // Get direction from source x-Axis to destination.
  auto direction = _getDirectionFromSourceToDestination(source, destination);

  auto sourceAxis = source->getOrientation().getAxis().normalize();

  auto rotationAxis = sourceAxis.cross(direction);
  auto dotProduct = sourceAxis.dot(direction);

  // Formula: sqrt(|a|^2 * |b|^2 + dotproduct). In our case the magnitude is
  // 1 because of normalization.
  auto rotation = sqrt(1 + dotProduct);

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

btVector3 MuscleActuator::_getDirectionFromSourceToDestination(
    btRigidBody *source, btRigidBody *destination) const
{
  return (destination->getWorldTransform().getOrigin() -
      source->getWorldTransform().getOrigin()).normalize();
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
  if (__YARS_GET_STEP > 10) {
    if (_L0 == 0)
    {
      _initialCalcs();
      _L0 = _constraint->getLinearPos();
      _Lopt = 0.9 * _L0;
    }

    auto Fm = _calcForce();

    // The velocity is the maximum speed of the contraction. It is slowed down if
    // there is not enough force generated to move the segment. The controller
    // should only tell the desired velocity.
    _constraint->setMaxLinMotorForce(static_cast<btScalar>(Fm));
    _constraint->setTargetLinMotorVelocity(static_cast<btScalar>(_vmax));
  }
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
  if (_lastTime == 0) { return 0.0; }

  auto crntTime = _yarsConfig->getCurrentRealTime();
  auto crntPos = _constraint->getLinearPos();
  auto v = (crntPos - _lastPos) / (crntTime - _lastTime);
  _lastPos = crntPos;
  _lastTime = crntTime;

  return v;
}

double MuscleActuator::_calcForce() {
  if (_lastTime == 0)
  {
    _lastTime = _yarsConfig->getCurrentRealTime();
    _lastPos = _constraint->getLinearPos();
    return 0.0;
  }

  auto internalDesired = _data.getInternalDesiredValue(0);

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

  return a_t * Fl * Fv * _fmax;

  // Logging.
//  _data.setAppliedForceAndVelocity(0, Fm, v);
}

double MuscleActuator::_calcForceVelocity(double v) const
{

  double Fv;
  if (_forceVelocityModel == "constant")
  {
    Fv = 1;
  }
  else if (_forceVelocityModel == "linear")
  {
    auto _mu = 0.25;
    Fv = 1 - _mu * v;
  }
  else if (_forceVelocityModel == "hill")
  {
    auto K = 1.5;
    auto N = 1.5;

    if (v < 0)
    {
      Fv = (_vmax - v) / (_vmax + K * v);
    }
    else
    {
      Fv = N + (N - 1) * ((_vmax + v) / (-7.56 * K * v - _vmax));
    }
  }
  else
  {
    throw new YarsException("Force-Velocity model not recognized.");
  }
  return Fv;
}

double MuscleActuator::_calcForceLength() const
{
  double Fl;
  if (_forceLengthModel == "constant")
  {
    Fl = 1;
  }
  else if (_forceLengthModel == "linear")
  {
    auto L = std::min(static_cast<double>(_constraint->getLinearPos()), _L0);
    Fl = _k * (_L0 - L);
  }
  else if (_forceLengthModel == "hill")
  {
    auto L = _constraint->getLinearPos();
    auto w = 0.4 * _Lopt;
    auto c = log(0.05);
    Fl = exp(c * pow(fabs((L - _Lopt) / (_Lopt * w)), 3));
  }
  else
  {
    throw new YarsException("Force-Length model not recognized.");
  }
  return Fl;
}

bool MuscleActuator::_isMotorEnabled() const
{
  return _constraint->getPoweredLinMotor();
}

double MuscleActuator::_calcSpringConstant(btSliderConstraint *constraint) const
{
  auto initialLength = constraint->getLinearPos();
  auto maxLength = constraint->getLinearPos() * 1.1;
  return 10;
}

void MuscleActuator::_initialCalcs() {
  _k = _calcSpringConstant(_constraint);
}
