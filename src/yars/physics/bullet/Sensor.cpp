#include <yars/physics/bullet/Sensor.h>

#include <yars/configuration/YarsConfiguration.h>

#include <sstream>

Sensor::Sensor(string name, string object, Robot *robot)
{
  _robot = robot;
  _name = name;
  _object = object;

  if(object == "")
  {
    YarsErrorHandler *e = YarsErrorHandler::instance();
    (*e) << "In " << name << ": You need to define a object" << endl;
    YarsErrorHandler::push();
  }

  _targetObject   = __findObject(object);
  _targetActuator = __findAcutaor(object);

  __findTarget();
}

Object* Sensor::__findObject(string name)
{
  for(std::vector<Object*>::iterator i = _robot->o_begin(); i != _robot->o_end(); i++)
  {
    if((*i)->data()->name() == name)
    {
      return *i;
    }
    FOREACHP(Object*, o, (*i))
    {
      if((*o)->data()->name() == name)
      {
        return *o;
      }
    }
  }
  return NULL;
}

Actuator* Sensor::__findAcutaor(string name)
{
  for(std::vector<Actuator*>::iterator i = _robot->a_begin(); i != _robot->a_end(); i++)
  {
    if((*i)->data()->name() == name)
    {
      return *i;
    }
  }
  return NULL;
}

void Sensor::__findTarget()
{
  if(_targetObject == NULL && _targetActuator == NULL)
  {
    YarsErrorHandler *e = YarsErrorHandler::instance();
    (*e) << "In " << _name << " (Sensor): Target " << _object << " was not found.";
    YarsErrorHandler::push();
  }
}

void Sensor::reset()
{
  __findTarget();
}

