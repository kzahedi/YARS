#include <yars/util/noise/GaussianNoise.h>
#include <yars/configuration/data/DataParameter.h>
#include <yars/util/Random.h>

#ifdef __APPLE__
#  include <limits.h>
#  include <float.h>
#else
#  include <cfloat>
#endif
#include <stdlib.h>


#include <math.h>


GaussianNoise::GaussianNoise(DataNoise *data)
  : Noise(data)
{
  _mean  = 0.0;
  _sigma = 1.0;
  for(std::vector<DataParameter*>::iterator p = data->begin(); p != data->end(); p++)
  {
    if((*p)->name() == "mean")  _mean  = (*p)->realValue();
    if((*p)->name() == "sigma") _sigma = (*p)->realValue();
  }
}

double GaussianNoise::calculate(double in)
{
  double rnd1, rnd2, x, y, z, radius;

  radius = 1.0;
  do
  {
    rnd1 = Random::unit();
    rnd1 = rnd1/(double)RAND_MAX;
    rnd2 = Random::unit();
    rnd2 = rnd2/(double)RAND_MAX;

    x = 2.0 * rnd1 - 1.0;
    y = 2.0 * rnd2 - 1.0;
    radius = x*x + y*y;
  }
  while ((radius >= 1.0) || (radius < DBL_MIN));

  z = _mean + _sigma * x * sqrt(-2.0 * log(radius) / radius);
  return in + z;
}
