#ifndef __ACTUATOR_H__
#define __ACTUATOR_H__

class Robot;

#include <yars/physics/bullet/Object.h>
#include <yars/physics/bullet/Robot.h>

#include <string>

using namespace std;

class Actuator
{
  public:
    Actuator(string name, string source, string destination, Robot *robot);
    virtual ~Actuator() { };

    virtual DataActuator*      data()              = 0;
    virtual void               prePhysicsUpdate()  = 0;
    virtual void               postPhysicsUpdate() = 0;
    virtual void               reset()             = 0;
    virtual btTypedConstraint* constraint()        = 0;

    vector<btTypedConstraint*>::iterator c_begin();
    vector<btTypedConstraint*>::iterator c_end();
    int                                  c_size();

  protected:
    Object *__find(string name);
    Object *_sourceObject;
    Object *_destinationObject;

    vector<btTypedConstraint*> _constraints;

  private:
    Robot *_robot;
};

#endif // __ACTUATOR_H__
