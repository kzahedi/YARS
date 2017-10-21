#ifndef __ACTUATOR_FACTORY_H__
#define __ACTUATOR_FACTORY_H__

#include <yars/physics/bullet/Actuator.h>

#include <yars/configuration/YarsConfiguration.h>
#include <yars/physics/bullet/Robot.h>

#include <btBulletDynamicsCommon.h>

class ActuatorFactory
{
  public:
    static Actuator* create(DataActuator *actuator, Robot *robot);

  private:
    static Actuator* __createHinge(DataActuator   *actuator, Robot *robot);
    static Actuator* __createSlider(DataActuator  *actuator, Robot *robot);
    static Actuator* __createFixed(DataActuator   *actuator, Robot *robot);
    static Actuator* __createGeneric(DataActuator *actuator, Robot *robot);
    static Actuator* __createMuscle(DataActuator  *actuator, Robot *robot);
    static Actuator* __createPointConstraint(DataActuator  *actuator, Robot *robot);
};

#endif // __ACTUATOR_FACTORY_H__
