#include <yars/physics/bullet/Actuator.h>

#include <yars/configuration/YarsConfiguration.h>
#include <yars/util/YarsErrorHandler.h>

#include <sstream>

Actuator::Actuator(string name, string source, string destination, Robot *robot)
{
  _robot = robot;

  if(source == "")
  {
    YarsErrorHandler *e = YarsErrorHandler::instance();
    (*e) << "In " << name << ": You need to define a source" << endl;
    YarsErrorHandler::push();
  }

  _sourceObject      = __find(source);
  _destinationObject = __find(destination);

  if(_sourceObject == NULL)
  {
    YarsErrorHandler *e = YarsErrorHandler::instance();
    (*e) << "In " << name << ": Source " << source << " was not found.";
    YarsErrorHandler::push();
  }
}

Object* Actuator::__find(string name)
{
  for(std::vector<Object*>::iterator o = _robot->o_begin(); o != _robot->o_end(); o++)
  {
    if((*o)->type() == DATA_OBJECT_COMPOSITE)
    {
      for(std::vector<Object*>::iterator co = (*o)->begin(); co != (*o)->end(); co++)
      {
        if((*co)->data()->name() == name) return (*co);
      }
    }
    else
    {
      if((*o)->data()->name() == name) return (*o);
    }
  }
  if(name != "")
  {
    YarsErrorHandler *e = YarsErrorHandler::instance();
    (*e) << "Actuators: Cannot find object \"" << name << "\"" << endl;
    YarsErrorHandler::push();

  }
  return NULL;
}
