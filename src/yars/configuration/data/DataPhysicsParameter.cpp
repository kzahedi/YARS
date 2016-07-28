#include "DataPhysicsParameter.h"
#include "DataPositionFactory.h"

#include <float.h>

# define YARS_STRING_VALUE                       (char*)"value"
# define YARS_STRING_X                           (char*)"x"
# define YARS_STRING_Y                           (char*)"y"
# define YARS_STRING_Z                           (char*)"z"
# define YARS_STRING_COM                         (char*)"centreOfMass"
# define YARS_STRING_COM_DEFINITION              (char*)"centreOfMass"    DIVIDER DEFINITION


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
    // cout << element->attribute(YARS_STRING_VALUE)->value() << endl;
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

void DataPhysicsParameter::createXsd(XsdSpecification *spec)
{
  XsdSequence *physicsParameterDef = new XsdSequence(YARS_STRING_PHYSICS_DEFINITION);
  physicsParameterDef->add(NE(YARS_STRING_MASS,             YARS_STRING_MASS_DEFINITION,             0, 1));
  physicsParameterDef->add(NE(YARS_STRING_COM,              YARS_STRING_COM_DEFINITION,              0, 1));
  physicsParameterDef->add(NE(YARS_STRING_FRICTION,         YARS_STRING_FRICTION_DEFINITION,         0, 1));
  physicsParameterDef->add(NE(YARS_STRING_ROLLING_FRICTION, YARS_STRING_ROLLING_FRICTION_DEFINITION, 0, 1));
  physicsParameterDef->add(NE(YARS_STRING_LINEAR_DAMPING,   YARS_STRING_LINEAR_DAMPING_DEFINITION,   0, 1));
  physicsParameterDef->add(NE(YARS_STRING_ANGULAR_DAMPING,  YARS_STRING_ANGULAR_DAMPING_DEFINITION,  0, 1));
  physicsParameterDef->add(NE(YARS_STRING_RESTITUTION,      YARS_STRING_RESTITUTION_DEFINITION,      0, 1));
  spec->add(physicsParameterDef);

  XsdSequence *frictionDefinition = new XsdSequence(YARS_STRING_FRICTION_DEFINITION        );
  frictionDefinition->add(NA(YARS_STRING_VALUE, YARS_STRING_XSD_DECIMAL, true));
  spec->add(frictionDefinition);

  XsdSequence *comDefinition = new XsdSequence(YARS_STRING_COM_DEFINITION);
  comDefinition->add(NA(YARS_STRING_X, YARS_STRING_XSD_DECIMAL, false));
  comDefinition->add(NA(YARS_STRING_Y, YARS_STRING_XSD_DECIMAL, false));
  comDefinition->add(NA(YARS_STRING_Z, YARS_STRING_XSD_DECIMAL, false));
  spec->add(comDefinition);

  XsdSequence *linearDampingDefinition = new XsdSequence(YARS_STRING_LINEAR_DAMPING_DEFINITION  );
  linearDampingDefinition->add(NA(YARS_STRING_VALUE, YARS_STRING_XSD_DECIMAL, true));
  spec->add(linearDampingDefinition);

  XsdSequence *massDefinition = new XsdSequence(YARS_STRING_MASS_DEFINITION);
  // massDefinition->add(NA(YARS_STRING_KG, YARS_STRING_POSITIVE_NON_ZERO_DECIMAL, true));
  massDefinition->add(NA(YARS_STRING_KG, YARS_STRING_POSITIVE_DECIMAL, true));
  spec->add(massDefinition);

  XsdSequence *angularDampingDefinition = new XsdSequence(YARS_STRING_ANGULAR_DAMPING_DEFINITION );
  angularDampingDefinition->add(NA(YARS_STRING_VALUE, YARS_STRING_XSD_DECIMAL, true));
  spec->add(angularDampingDefinition);

  XsdSequence *rollingFrictionDefinition = new XsdSequence(YARS_STRING_ROLLING_FRICTION_DEFINITION);
  rollingFrictionDefinition->add(NA(YARS_STRING_VALUE, YARS_STRING_XSD_DECIMAL, true));
  spec->add(rollingFrictionDefinition);

  XsdSequence *restitutionDefinition = new XsdSequence(YARS_STRING_RESTITUTION_DEFINITION     );
  restitutionDefinition->add(NA(YARS_STRING_VALUE, YARS_STRING_XSD_DECIMAL, true));
  spec->add(restitutionDefinition);
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


yReal DataPhysicsParameter::linearDamping()
{
  return _linearDamping;
}

yReal DataPhysicsParameter::mass()
{
  return _mass;
}

yReal DataPhysicsParameter::angularDamping()
{
  return _angularDamping;
}

yReal DataPhysicsParameter::friction()
{
  return _friction;
}

yReal DataPhysicsParameter::rollingFriction()
{
  return _rollingFriction;
}

yReal DataPhysicsParameter::restitution()
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


void DataPhysicsParameter::setMass(yReal mass)
{
  _mass = mass;
}
