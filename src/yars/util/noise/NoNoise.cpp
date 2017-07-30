#include <yars/util/noise/NoNoise.h>
#include <yars/configuration/data/DataParameter.h>

NoNoise::NoNoise(DataNoise *data)
  : Noise(data)
{ }

double NoNoise::calculate(double in)
{
  return in;
}

