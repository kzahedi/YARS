#ifndef __SENSOR_H__
#define __SENSOR_H__

class Robot;

#include <yars/physics/bullet/Robot.h>

#include <string>

using namespace std;

class Sensor
{
  public:
    Sensor(string name, string object, Robot *robot);
    virtual ~Sensor() { };

    virtual void prePhysicsUpdate()  = 0;
    virtual void postPhysicsUpdate() = 0;
    void    reset();

  protected:
    Object*   __findObject(string name);
    Actuator* __findAcutaor(string name);
    void      __findTarget();

    Object   *_targetObject;
    Actuator *_targetActuator;

  private:
    Robot *_robot;
    string _name;
    string _object;
};

#endif // __SENSOR_H__


