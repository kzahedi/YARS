#include <yars/util/noise/NoNoise.h>
#include <yars/configuration/data/DataParameter.h>

NoNoise::NoNoise(DataNoise *data)
  : Noise(data)
{ }

yReal NoNoise::calculate(yReal in)
{
  return in;
}

