#include "GenericLDRSensor.h"
#include "CollisionCallback.h"

#include <yars/util/stl_macros.h>
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

  yReal red   = 0.0;
  yReal green = 0.0;
  yReal blue  = 0.0;

  P3D o = _targetObject->data()->pose().position + _pose.position;

  for(DataPointLightSources::iterator l = _env->l_begin(); l != _env->l_end(); l++)
  {
    yReal dist     = o.dist((*l)->position());

    P3D hit        = World::rayTest(o, (*l)->position());
    yReal hit_dist = hit.dist(o);

    P3D d = (*l)->position() - o;
    d.normalise();

    yReal angle = acos(d.x * z.x + d.y * z.y + d.z * z.z);

    if(fabs(dist - hit_dist) < 0.00001 && fabs(angle) < _data->opening())
    {
      red   += (*l)->brightness() * (*l)->colour().red()   / (dist * dist);
      green += (*l)->brightness() * (*l)->colour().green() / (dist * dist);
      blue  += (*l)->brightness() * (*l)->colour().blue()  / (dist * dist);
    }
  }

  yReal value =
    (MIN(red,   _data->colour().red())   +
     MIN(green, _data->colour().green()) +
     MIN(blue,  _data->colour().blue())) / _z;

  _data->setInternalValue(0, value);
  // cout << red << " " << green << " " << blue << " -> " << value << endl;
}

