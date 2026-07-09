#include "DataPhysicsParameter.h"
#include "DataPositionFactory.h"

#include <float.h>

# define YARS_STRING_VALUE          (char*)"value"
# define YARS_STRING_X              (char*)"x"
# define YARS_STRING_Y              (char*)"y"
# define YARS_STRING_Z              (char*)"z"
# define YARS_STRING_COM            (char*)"centreOfMass"
# define YARS_STRING_COM_DEFINITION (char*)"centreOfMass_definition"


DataPhysicsParameter::DataPhysicsParameter(DataNode *parent)
  : DataNode(parent)
{
  _linearDamping               = 0.0;
  _mass                        = 1.0;
  _angularDamping              = 0.0;
  _friction                    = 0.0;
  _rollingFriction             = 0.0;
  _restitution                 = 0.0;

  _useLinearDamping            = false;
  _useAngularDamping           = false;
  _useFriction                 = false;
  _useRollingFriction          = false;
  _useRestitution              = false;

}

DataPhysicsParameter::~DataPhysicsParameter()
{
  // nothing to be done.
}

void DataPhysicsParameter::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_PHYSICS))
  {
    current = parent;
  }

  if(element->opening(YARS_STRING_PHYSICS))
  {
  }

  if(element->opening(YARS_STRING_FRICTION))
  {
    // cout << "set friction to " << _friction << endl;
    // cout << element->requiredAttribute(YARS_STRING_VALUE)->value() << endl;
    element->set(YARS_STRING_VALUE, _friction);
    _useFriction = true;
  }

  if(element->opening(YARS_STRING_LINEAR_DAMPING))
  {
    element->set(YARS_STRING_VALUE, _linearDamping);
    _useLinearDamping = true;
  }

  if(element->opening(YARS_STRING_MASS))
  {
    element->set(YARS_STRING_KG, _mass);
  }

  if(element->opening(YARS_STRING_COM))
  {
    DataPositionFactory::set(_centreOfMass, element);
  }

  if(element->opening(YARS_STRING_ANGULAR_DAMPING))
  {
    element->set(YARS_STRING_VALUE, _angularDamping);
    _useAngularDamping = true;
  }

  if(element->opening(YARS_STRING_ROLLING_FRICTION))
  {
    element->set(YARS_STRING_VALUE, _rollingFriction);
    _useRollingFriction = true;
  }

  if(element->opening(YARS_STRING_RESTITUTION))
  {
    element->set(YARS_STRING_VALUE, _restitution);
    _useRestitution = true;
  }
}

DataPhysicsParameter* DataPhysicsParameter::copy()
{
  DataPhysicsParameter *copy = new DataPhysicsParameter(NULL); // NULL because no parsing takes place after copying

  copy->_linearDamping      = _linearDamping;
  copy->_mass               = _mass;
  copy->_angularDamping     = _angularDamping;
  copy->_friction           = _friction;
  copy->_rollingFriction    = _rollingFriction;
  copy->_restitution        = _restitution;
  copy->_centreOfMass       = _centreOfMass;

  copy->_useLinearDamping   = _useLinearDamping;
  copy->_useAngularDamping  = _useAngularDamping;
  copy->_useFriction        = _useFriction;
  copy->_useRollingFriction = _useRollingFriction;
  copy->_useRestitution     = _useRestitution;

  return copy;
}


double DataPhysicsParameter::linearDamping()
{
  return _linearDamping;
}

double DataPhysicsParameter::mass()
{
  return _mass;
}

double DataPhysicsParameter::angularDamping()
{
  return _angularDamping;
}

double DataPhysicsParameter::friction()
{
  return _friction;
}

double DataPhysicsParameter::rollingFriction()
{
  return _rollingFriction;
}

double DataPhysicsParameter::restitution()
{
  return _restitution;
}

P3D DataPhysicsParameter::centreOfMass()
{
  return _centreOfMass;
}

bool DataPhysicsParameter::useLinearDamping()
{
  return _useLinearDamping;
}

bool DataPhysicsParameter::useAngularDamping()
{
  return _useAngularDamping;
}

bool DataPhysicsParameter::useFriction()
{
  return _useFriction;
}

bool DataPhysicsParameter::useRollingFriction()
{
  return _useRollingFriction;
}

bool DataPhysicsParameter::useRestitution()
{
  return _useRestitution;
}


void DataPhysicsParameter::setMass(double mass)
{
  _mass = mass;
}
