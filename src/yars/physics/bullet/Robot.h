#ifndef __ROBOT_H__
#define __ROBOT_H__

class Actuator;
class Sensor;

#include <yars/configuration/YarsConfiguration.h>
#include <yars/physics/bullet/Actuator.h>
#include <yars/physics/bullet/Sensor.h>
#include <yars/physics/bullet/Object.h>
#include <yars/rc/RobotController.hpp>

#include <vector>
#include <memory>
#include <dlfcn.h>

using namespace std;

class Robot
{
  public:
    Robot(DataRobot* robot);
    ~Robot();

    void prePhysicsUpdate();
    void postPhysicsUpdate();
    void controllerUpdate();
    void reset();

    bool isReset();
    bool isQuit();
    int  seed();

    std::vector<std::unique_ptr<Object>>::iterator   o_begin();
    std::vector<std::unique_ptr<Object>>::iterator   o_end();
    std::vector<std::unique_ptr<Actuator>>::iterator a_begin();
    std::vector<std::unique_ptr<Actuator>>::iterator a_end();
    std::vector<std::unique_ptr<Sensor>>::iterator   s_begin();
    std::vector<std::unique_ptr<Sensor>>::iterator   s_end();

    unsigned int collisionMask();
    unsigned int collideWith();

    bool selfCollide();

    DataRobot* data();

  private:
    void __createBody();
    void __createActuators();
    void __createSensors();
    void __createController();
    void __setupController();

    bool                   _reset;
    bool                   _quit;

    DataRobot*             _data;
    std::vector<std::unique_ptr<Object>>   _objects;
    std::vector<std::unique_ptr<Actuator>> _actuators;
    std::vector<std::unique_ptr<Sensor>>   _sensors;
    std::unique_ptr<RobotController>       _controller;
    void*                  _controllerLib;
    create_c*              _create_controller;
    std::vector<string>    _dummyContainer;
    int                    _controllerFrequency;
    int                    _seed;
};

#endif // __ROBOT_H__

