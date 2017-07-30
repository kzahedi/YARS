#ifndef __GENERIC_PROXIMITY_SENSOR_H__
#define __GENERIC_PROXIMITY_SENSOR_H__

#include <yars/physics/bullet/Sensor.h>

#include <yars/configuration/data/DataGenericProximitySensor.h>
#include <yars/types/Quaternion.h>
#include <yars/physics/bullet/World.h>

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>


class RayCoordinates
{
  public:
    Pose sensorPose;
    Pose pose;
    P3D  end;
    Quaternion q;
};

class GenericProximitySensor : public Sensor
{
  public:

    GenericProximitySensor(DataGenericProximitySensor *data, Robot *robot);
   ~GenericProximitySensor();

   void prePhysicsUpdate();
   void postPhysicsUpdate();

  private:

   void __createRays();
   void __getEndP3D(P3D &p, P3D direction, double length);

   DataGenericProximitySensor *_data;
   Pose                       *_rayPose;
   Pose                        _objectPose;
   P3D                         _start;
   P3D                        *_end;
   std::vector<RayCoordinates> _rayCoordinates;
   btGhostObject              *_ghost;
};

#endif // __GENERIC_PROXIMITY_SENSOR_H__
