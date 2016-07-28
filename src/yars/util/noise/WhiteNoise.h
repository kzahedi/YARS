#ifndef __WHITE_NOISE_H__
#define __WHITE_NOISE_H__

#include <yars/util/noise/Noise.h>
#include <yars/defines/types.h>
#include <yars/configuration/data/DataNoise.h>

class WhiteNoise : public Noise
{
  public:
    WhiteNoise(DataNoise *data);

    yReal calculate(yReal in);

  private:
    yReal _mean;
    yReal _sigma;

};

#endif // __WHITE_NOISE_H__


