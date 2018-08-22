#include <yars/physics/bullet/SliderActuator.h>
#include <yars/util/macros.h>
#include <math.h>

#define POSITIONAL 123400
#define VELOCITY   123500
#define FORCE      123600

#define MAP(min, max, value) ((value > max)?max:(value<min)?min:value)
#define _SIGN(value)         ((value < 0)?-1:1)
#define SET(a, b)            if(b >= 0.0) a(b)
#define SCALE(a)             (MIN(0.5, fabs(diff)) / 0.5)


SliderActuator::SliderActuator(DataSliderActuator *data, Robot *robot)
  : Actuator("SliderActuator", data->source(), data->destination(), robot)
{
  _data             = data;
  _sliderConstraint = NULL;
  _position         = 0.0;
  _lastPosition     = 0.0;
  _isActive         = true;
  _friction         = _data->friction();
  _hasFriction      = (_friction > 0.0);
  _isVisualised     = Data::instance()->current()->screens()->visualiseJoints();
  _parameter        = _data->parameter();

  if(_data->mode() == "passive") _isActive = false;

  Pose axis = _data->pose();

  if(_sourceObject != NULL && _destinationObject != NULL)
  {
    btRigidBody *source      = _sourceObject->rigidBody();
    btRigidBody *destination = _destinationObject->rigidBody();

    ::Quaternion qa(axis.orientation);
    btQuaternion q(qa.x, qa.y, qa.z, qa.w);

    btTransform transformA(q, btVector3(axis.position.x, axis.position.y, axis.position.z));

    btTransform transformB = destination->getWorldTransform().inverse() * transformA;
                transformA = source->getWorldTransform().inverse()     * transformA;

    _sliderConstraint  = new btSliderConstraint(*source, *destination, transformA, transformB, true);
  }
  else
  {
    btRigidBody *source = _sourceObject->rigidBody();

    ::Quaternion qa(axis.orientation);
    btQuaternion q(qa.x, qa.y, qa.z, qa.w);

    btTransform transform = btTransform::getIdentity();
    transform.setOrigin(btVector3(_sourceObject->data()->pose().position.x,
                                  _sourceObject->data()->pose().position.y,
                                  _sourceObject->data()->pose().position.z));

    transform.getBasis().setRotation(q);

    _sliderConstraint = new btSliderConstraint(*source, transform, true);
  }

  _sliderConstraint->setLowerAngLimit(0.0);
  _sliderConstraint->setUpperAngLimit(0.0);
  _sliderConstraint->setPoweredAngMotor(false);

  for(int i = 0; i < 6; i++)
  {
    _sliderConstraint->setParam(BT_CONSTRAINT_STOP_ERP, 1.0, i);
    _sliderConstraint->setParam(BT_CONSTRAINT_STOP_CFM, 0.0, i);
  }

  _sliderConstraint->setPoweredLinMotor(true);

  if(_data->isDeflectionSet())
  {
    _sliderConstraint->setLowerLinLimit(_data->deflection().min);
    _sliderConstraint->setUpperLinLimit(_data->deflection().max);
  }
  else
  {
    _sliderConstraint->setLowerLinLimit(1); // unset limit solving
    _sliderConstraint->setUpperLinLimit(0);
  }

  SET(_sliderConstraint->setSoftnessDirLin,    _parameter.softnessDir);
  SET(_sliderConstraint->setRestitutionDirLin, _parameter.restitutionDir);
  SET(_sliderConstraint->setDampingDirLin,     _parameter.dampingDir);

  SET(_sliderConstraint->setSoftnessLimLin,    _parameter.softnessLim);
  SET(_sliderConstraint->setRestitutionLimLin, _parameter.restitutionLim);
  SET(_sliderConstraint->setDampingLimLin,     _parameter.dampingLim);

  SET(_sliderConstraint->setSoftnessOrthoLin,    _parameter.softnessOrtho);
  SET(_sliderConstraint->setRestitutionOrthoLin, _parameter.restitutionOrtho);
  SET(_sliderConstraint->setDampingOrthoLin,     _parameter.dampingOrtho);

  switch(_data->controlType())
  {
    case DATA_ACTUATOR_CONTROL_POSITIONAL: _sliderType = POSITIONAL; break;
    case DATA_ACTUATOR_CONTROL_VELOCITY:   _sliderType = VELOCITY;   break;
    case DATA_ACTUATOR_CONTROL_FORCE:      _sliderType = FORCE;      break;
  }
}

SliderActuator::~SliderActuator()
{
  if(_sliderConstraint != NULL) delete _sliderConstraint;
}

void SliderActuator::prePhysicsUpdate()
{
  if(_isActive)
  {
    switch(_sliderType)
    {
      case POSITIONAL: __processPositional();     break;
      case VELOCITY:   __processVelocitySlider(); break;
      case FORCE:      __processForceSlider();    break;
    }
  }
  else
  {
    if(_hasFriction)
    {
      _sliderConstraint->setPoweredLinMotor(true);
      _sliderConstraint->setTargetLinMotorVelocity(0.0);
      _sliderConstraint->setMaxLinMotorForce(_friction);
    }
  }
}

void SliderActuator::postPhysicsUpdate()
{
  _position = _sliderConstraint->getLinearPos();

  _data->setCurrentTransitionalVelocity((_position - _lastPosition)
      * (double)__YARS_GET_SIMULATOR_FREQUENCY);
  _lastPosition = _position;
  if(_isVisualised)
  {
    btTransform  pose = _sliderConstraint->getCalculatedTransformA();
    btVector3    vec  = pose.getOrigin();
    btQuaternion q    = pose.getRotation();
    _data->setCurrentAxisPosition(P3D(vec[0], vec[1], vec[2]));
    _data->setCurrentAxisOrientation(::Quaternion(q.getW(), q.getX(), q.getY(), q.getZ()));
  }
}

DataSliderActuator* SliderActuator::data()
{
  return _data;
}

void SliderActuator::reset()
{
}

btTypedConstraint* SliderActuator::constraint()
{
  return _sliderConstraint;
}

void SliderActuator::__processPositional()
{
  double robotValue = _data->getInternalDesiredValue(0);

  if(_data->isDeflectionSet())
  {
    robotValue = _data->deflection().cut(robotValue);
    _position  = _data->deflection().cut(_position);
  }

  double diff     = _position - robotValue;
  double velocity = _parameter.pid.update(SCALE(diff) * SIGN(diff) * _data->velocity());
  double force    = _data->force();

  if(_parameter.forceScaling > 0.0)
  {
    force = force * MAX(0.01, fabs(tanh(_parameter.forceScaling * diff/M_PI)));
  }

  _sliderConstraint->setMaxAngMotorForce(force);
  _sliderConstraint->setTargetLinMotorVelocity(velocity);
}

void SliderActuator::__processVelocitySlider()
{
  double velocity   = _parameter.pid.update(_data->getInternalDesiredValue(0));

  _sliderConstraint->setMaxLinMotorForce(_parameter.maxForce);
  _sliderConstraint->setTargetLinMotorVelocity(velocity);
}

void SliderActuator::__processForceSlider()
{
  double v        = _data->getInternalDesiredValue(0);
  double force    = fabs(v) * _data->force();
  double velocity = _parameter.pid.update(v  * _data->velocity());
  _sliderConstraint->setMaxLinMotorForce(force);
  _sliderConstraint->setTargetLinMotorVelocity(velocity);
}
