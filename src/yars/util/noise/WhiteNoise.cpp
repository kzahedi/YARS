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
  for(std::vector<DataParameter*>::iterator p = data->begin(); p != data->end(); p++)
  {
    if((*p)->name() == "mean")  _mean  = (*p)->realValue();
    if((*p)->name() == "sigma") _sigma = (*p)->realValue();
  }
}

yReal WhiteNoise::calculate(yReal in)
{
  yReal v = (yReal)((_sigma * 2.0 * ((yReal)rand() / (yReal)RAND_MAX)) - _sigma + _mean);
  return in + v;
}

