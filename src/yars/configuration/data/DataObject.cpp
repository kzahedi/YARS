#include <yars/configuration/data/DataObject.h>

#include <yars/defines/mutex.h>

DataObject::DataObject(DataNode *parent, int type)
: DataNode(parent)
{
  _type      = type;
  _physics   = new DataPhysicsParameter(this);
  _visualise = true;
  _isSoft    = false;
  YM_INIT;
}

DataObject::~DataObject()
{
  YM_CLOSE;
  if(_physics != NULL) delete _physics;
}

int DataObject::type()
{
  return _type;
}

DataPhysicsParameter* DataObject::physics()
{
  return _physics;
}

Pose DataObject::pose()
{
  YM_LOCK;
  Pose p = _pose;
  YM_UNLOCK;
  return p;
}

void DataObject::applyOffset(Pose p)
{
  YM_LOCK;
  _pose << p;
  YM_UNLOCK;
}

bool DataObject::visualise()
{
  YM_LOCK;
  bool r = _visualise;
  YM_UNLOCK;
  return r;
}

void DataObject::setPose(Pose pose)
{
  YM_LOCK;
  _pose = pose;
  YM_UNLOCK;
}

void DataObject::setPhysics(DataPhysicsParameter *physics)
{
  YM_LOCK;
  _physics = physics;
  YM_UNLOCK;
}


void DataObject::setVisualise(bool visualise)
{
  YM_LOCK;
  _visualise = visualise;
  YM_UNLOCK;
}

string DataObject::name()
{
  return _name;
}

void DataObject::setName(string name)
{
  _name = name;
}

Quaternion DataObject::quaternion()
{
  YM_LOCK;
  ::Quaternion q = _pose.q;
  YM_UNLOCK;
  return q;
}

void DataObject::addSensor(DataSensor* sensor)
{
  YM_LOCK;
  _sensors.push_back(sensor);
  YM_UNLOCK;
}

std::vector<DataSensor*>::iterator DataObject::s_begin()
{
  return _sensors.begin();
}

std::vector<DataSensor*>::iterator DataObject::s_end()
{
  return _sensors.end();
}

int DataObject::s_size()
{
  return (int)_sensors.size();
}

void DataObject::setPosition(P3D p)
{
  YM_LOCK;
  _pose.position = p;
  YM_UNLOCK;
}

vector<DataMeshVisualisation*>::iterator DataObject::m_begin()
{
  return _meshes.begin();
}

vector<DataMeshVisualisation*>::iterator DataObject::m_end()
{
  return _meshes.end();
}

int DataObject::m_size()
{
  return _meshes.size();
}

void DataObject::resetTo(const DataObject *other)
{
  _resetTo(other);
};

bool DataObject::isSoft()
{
  return _isSoft;
}


void DataObject::setCurrentForce(yReal x, yReal y, yReal z)
{
  YM_LOCK;
  _force.x = x;
  _force.y = y;
  _force.z = z;
  YM_UNLOCK;
}

P3D DataObject::getCurrentForce()
{
  YM_LOCK;
  P3D r = _force;
  YM_UNLOCK;
  return r;
}


void DataObject::setCurrentVelocity(yReal x, yReal y, yReal z)
{
  YM_LOCK;
  _velocity.x = x;
  _velocity.y = y;
  _velocity.z = z;
  YM_UNLOCK;
}


P3D DataObject::getCurrentVelocity()
{
  YM_LOCK;
  P3D r = _velocity;
  YM_UNLOCK;
  return r;
}



void DataObject::setCurrentAngularVelocity(yReal x, yReal y, yReal z)
{
  YM_LOCK;
  _angularVelocity.x = x;
  _angularVelocity.y = y;
  _angularVelocity.z = z;
  YM_UNLOCK;
}


P3D DataObject::getCurrentAngularVelocity()
{
  YM_LOCK;
  P3D r = _angularVelocity;
  YM_UNLOCK;
  return r;
}


void DataObject::setCurrentTorque(yReal x, yReal y, yReal z)
{
  YM_LOCK;
  _torque.x = x;
  _torque.y = y;
  _torque.z = z;
  YM_UNLOCK;
}

P3D DataObject::getCurrentTorque()
{
  YM_LOCK;
  P3D r = _torque;
  YM_UNLOCK;
  return r;
}

