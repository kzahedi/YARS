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
  for(std::vector<Object*>::iterator i = begin(); i != end(); i++) delete *i;
  if(_groundShape != NULL) delete _groundShape;
  clear();
}

void Environment::__create()
{
  for(DataObjects::iterator i = _data->o_begin(); i != _data->o_end(); i++)
  {
    push_back(ObjectFactory::create(*i));
  }
}

void Environment::reset()
{
  for(std::vector<Object*>::iterator o = begin(); o != end(); o++) (*o)->reset();
}

void Environment::prePhysicsUpdate()
{
  //FOREACH(Object*, o, (*this)) (*o)->prePhysicsUpdate();
}

void Environment::postPhysicsUpdate()
{
  FOREACH(Object*, o, (*this)) (*o)->postPhysicsUpdate();
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

    Object *ground = new Object(NULL);
    ground->setRigidBody(groundRigidBody);
    push_back(ground);
  }


  if(__YARS_HAS_ENVIRONMENTS)
  {
    _data = __YARS_DATA_GET_CURRENT_ENVIRONMENT(0);
    __create();
  }

}
