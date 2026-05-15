#include <yars/physics/bullet/Environment.h>

#include <yars/physics/bullet/ObjectFactory.h>
#include <yars/configuration/YarsConfiguration.h>

#include <sstream>

Environment::Environment()
{
  _data = Data::instance()->current()->environment();
  _groundShape = NULL;
  __init();
}

Environment::~Environment()
{
  // unique_ptr handles cleanup of Object* entries automatically.
  if (_groundShape != NULL) delete _groundShape;
}

void Environment::__create()
{
  for (auto i = _data->o_begin(); i != _data->o_end(); ++i)
  {
    push_back(std::unique_ptr<Object>(ObjectFactory::create(*i)));
  }
}

void Environment::reset()
{
  for (auto &o : *this) o->reset();
}

void Environment::prePhysicsUpdate()
{
}

void Environment::postPhysicsUpdate()
{
  for (auto &o : *this) o->postPhysicsUpdate();
}


void Environment::__init()
{

  if(_data->groundGiven())
  {
    _groundShape = new btStaticPlaneShape(btVector3(0,0,1),0);

    // initial pose
    btDefaultMotionState* groundMotionState = new btDefaultMotionState(
        btTransform(btQuaternion(0,0,0,1),
          btVector3(0,0,0)));

    btRigidBody::btRigidBodyConstructionInfo
      groundRigidBodyCI(0, groundMotionState, _groundShape, btVector3(0,0,0));
    groundRigidBodyCI.m_friction    = 1;
    groundRigidBodyCI.m_restitution = 1;

    btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
    groundRigidBody->setCollisionFlags(groundRigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

    auto ground = std::make_unique<Object>(nullptr);
    ground->setRigidBody(groundRigidBody);
    push_back(std::move(ground));
  }


  if(__YARS_HAS_ENVIRONMENTS)
  {
    _data = __YARS_DATA_GET_CURRENT_ENVIRONMENT(0);
    __create();
  }

}
