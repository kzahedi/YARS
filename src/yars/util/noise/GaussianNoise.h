#ifndef __GAUSS_NOISE_H__
#define __GAUSS_NOISE_H__

#include "Noise.h"

#include <yars/defines/types.h>
#include <yars/configuration/data/DataNoise.h>

class GaussianNoise : public Noise
{
  public:
    GaussianNoise(DataNoise *data);

    double calculate(double in);

  private:
    double  _mean;
    double  _sigma;

};

#endif // __GAUSS_NOISE_H__

