#include "MuscleActuator.h"

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
  _sliderConstraint = sliderConstraint(_data);

  _sliderConstraint->setLowerAngLimit(0.0);
  _sliderConstraint->setUpperAngLimit(0.0);
  _sliderConstraint->setPoweredAngMotor(false);

  for(int i = 0; i < 6; i++) {
    _sliderConstraint->setParam(BT_CONSTRAINT_STOP_ERP, 1.0, i);
    _sliderConstraint->setParam(BT_CONSTRAINT_STOP_CFM, 0.0, i);
  }

  _sliderConstraint->setPoweredLinMotor(true);

  //if (_data.isDeflectionSet()) {
    //_sliderConstraint->setLowerLinLimit(_data.deflection().min);
    //_sliderConstraint->setUpperLinLimit(_data.deflection().max);
  //} else {
    //_sliderConstraint->setLowerLinLimit(1); // unset limit solving
    //_sliderConstraint->setUpperLinLimit(0);
  //}

  cout << "Data control type: " << _data.controlType() << endl;
  switch(_data.controlType()) {
  case DATA_ACTUATOR_CONTROL_POSITIONAL: _sliderType = positional; break;
  case DATA_ACTUATOR_CONTROL_VELOCITY: _sliderType = velocity; break;
  case DATA_ACTUATOR_CONTROL_FORCE: _sliderType = force; break;
  case DATA_ACTUATOR_CONTROL_FORCE_VELOCITY:
    _sliderType = force_velocity;
    break;
  default: cout << "unkown type: " << _data.controlType() << endl;
  }
}

MuscleActuator::~MuscleActuator()
{
  if (_sliderConstraint != nullptr) delete _sliderConstraint;
}

btSliderConstraint* MuscleActuator::sliderConstraint(DataMuscleActuator& _data)
{
  Pose axis = _data.pose();
  cout << "Pose: " << axis << endl;

  btRigidBody* source = _sourceObject->rigidBody();
  btRigidBody* destination = _destinationObject->rigidBody();

  ::Quaternion qa(axis.orientation);
  btQuaternion q(qa.x, qa.y, qa.z, qa.w);

  btTransform transformA(q,
    btVector3(axis.position.x, axis.position.y, axis.position.z));

  auto transformB = destination->getWorldTransform().inverse() * transformA;
  transformA = source->getWorldTransform().inverse() * transformA;

  return new btSliderConstraint(*source, *destination, transformA, transformB,
      true);
}

DataActuator* MuscleActuator::data()
{
  return &_data;
}

void MuscleActuator::prePhysicsUpdate()
{
  // Sine controller return values from -1 to 1. But we need positive ones only.
  yReal a_t = fabs(_data.getInternalDesiredValue(0) / 1.0); // A(t)
  cout << "Input: " << a_t << endl;
  // Controller gibt externen Wert. _data mapt internalDesired.

  // _data.force() returns Fmax and _data.velocity() vmax. For now it's
  // hardcoded in the class.
  //yReal force = * _data.force(); // _data.force() * xyz * A(t)

  // In this model elasticity isn't considered. The actuator simply stops at max
  // length.
  _sliderConstraint->setLowerLinLimit(0.0);
  _sliderConstraint->setUpperLinLimit(1.0); // Only works with values >= 1.
  _sliderConstraint->setDbgDrawSize(btScalar(1.0));

  // TODO: Only needed once. Move somewhere else.
  _forceVelocityModel = linear;
  _forceLengthModel = linear;
  yReal _Fv, _Fl;
  yReal v = a_t;
  yReal _mu = 0.25;
  yReal _k = 10;
  yReal _L0 = 1;
  yReal _L = min(_sliderConstraint->getLinearPos(), _L0);
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
  yReal velocity = 0.01;

  cout << "Fv: " << _Fv << endl;
  cout << "Fl: " << _Fl << endl;
  cout << "LinearPos: " << _sliderConstraint->getLinearPos() << endl;
  cout << "L: " << _L << endl;
  cout << "_data->force(): " << _data.force() << endl;
  cout << "_data->velocity(): " << _data.velocity() << endl;
  cout << "a_t: " << a_t << endl;
  cout << "_Fm: " << _Fm << endl;
  cout << "F: " << force << " " << "v: " << velocity << endl;

  _sliderConstraint->setMaxLinMotorForce(force);
  _sliderConstraint->setTargetLinMotorVelocity(velocity);
  _data.setAppliedForceAndVelocity(0, force, velocity);
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
}

void MuscleActuator::reset()
{
}

btTypedConstraint* MuscleActuator::constraint()
{
  return _sliderConstraint;
}
