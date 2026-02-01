#include <yars/util/noise/WhiteNoise.h>
#include <yars/configuration/data/DataParameter.h>

#ifdef __APPLE__
#  include <limits.h>
#  include <float.h>
#  include <math.h>
#else
#  ifdef _MSC_VER
#    include <cfloat>
#  else
#    include <values.h>
#  endif
#endif
#include <stdlib.h>


WhiteNoise::WhiteNoise(DataNoise *data)
  : Noise(data)
{
  _mean  = 0.0;
  _sigma = 1.0;
  for(auto* param : *data)
  {
    if(param->name() == "mean")  _mean  = param->realValue();
    if(param->name() == "sigma") _sigma = param->realValue();
  }
}

double WhiteNoise::calculate(double in)
{
  double v = (double)((_sigma * 2.0 * ((double)rand() / (double)RAND_MAX)) - _sigma + _mean);
  return in + v;
}

