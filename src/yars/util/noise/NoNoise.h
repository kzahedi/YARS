#ifndef __NO_NOISE_H__
#define __NO_NOISE_H__

#include "Noise.h"

#include <yars/defines/types.h>
#include <yars/configuration/data/DataNoise.h>

class NoNoise : public Noise
{
  public:
    NoNoise(DataNoise *data);

    double calculate(double in);

  private:

};

#endif // __NO_NOISE_H__



