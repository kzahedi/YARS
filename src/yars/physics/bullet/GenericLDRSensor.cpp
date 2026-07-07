#include "GenericLDRSensor.h"
#include "CollisionCallback.h"

#include <yars/physics/bullet/World.h>

#include <math.h>


GenericLDRSensor::GenericLDRSensor(DataGenericLightDependentResistorSensor *data, Robot *robot)
  : Sensor(data->name(), data->object(), robot)
{
  _data         = data;
  _ambientLight = NULL;
  _pose         = _data->pose();
  _ambientLight = Data::instance()->current()->environment()->ambientLight();
  _env          = Data::instance()->current()->environment();
  _z = (_data->colour().red() + _data->colour().green() + _data->colour().blue());
}

GenericLDRSensor::~GenericLDRSensor()
{
}

void GenericLDRSensor::prePhysicsUpdate()
{
}

void GenericLDRSensor::postPhysicsUpdate()
{
  P3D z(0.0, 0.0, 1.0);
  Quaternion q(_pose.orientation);
  Quaternion r(_targetObject->data()->pose().orientation);
  z *= q;
  z *= r;
  z.normalise();

  double red   = 0.0;
  double green = 0.0;
  double blue  = 0.0;

  P3D o = _targetObject->data()->pose().position + _pose.position;

  for(DataPointLightSources::iterator l = _env->l_begin(); l != _env->l_end(); l++)
  {
    P3D lightPos    = (*l)->position();
    double dist     = o.dist(lightPos);

    btVector3 start(o.x, o.y, o.z);
    btVector3 end(lightPos.x, lightPos.y, lightPos.z);
    btVector3 hitVec;
    // On a miss, mirror the P3D overload's exact semantics: return the
    // original double-precision light position, never the float round-trip
    // through hitOut (which World::rayTest sets to `end` on miss).
    P3D hit = World::rayTest(start, end, hitVec) ?
      P3D(hitVec[0], hitVec[1], hitVec[2]) : lightPos;
    double hit_dist = hit.dist(o);

    P3D d = lightPos - o;
    d.normalise();

    double angle = acos(d.x * z.x + d.y * z.y + d.z * z.z);

    if(fabs(dist - hit_dist) < 0.00001 && fabs(angle) < _data->opening())
    {
      red   += (*l)->brightness() * (*l)->colour().red()   / (dist * dist);
      green += (*l)->brightness() * (*l)->colour().green() / (dist * dist);
      blue  += (*l)->brightness() * (*l)->colour().blue()  / (dist * dist);
    }
  }

  double value =
    (MIN(red,   _data->colour().red())   +
     MIN(green, _data->colour().green()) +
     MIN(blue,  _data->colour().blue())) / _z;

  _data->setInternalValue(0, value);
  // cout << red << " " << green << " " << blue << " -> " << value << endl;
}

