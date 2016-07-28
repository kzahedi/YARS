#ifndef __NOISE_FACTORY_H__
#define __NOISE_FACTORY_H__

#include <yars/defines/types.h>
#include <yars/configuration/data/DataNoise.h>

#include <yars/util/noise/Noise.h>
#include <yars/util/noise/NoNoise.h>
#include <yars/util/noise/GaussianNoise.h>
#include <yars/util/noise/WhiteNoise.h>

class NoiseFactory
{
  public:
    static Noise* create(DataNoise *data)
    {
      if(data == NULL)              return new NoNoise(NULL);
      if(data->module() == "gauss") return new GaussianNoise(data);
      if(data->module() == "white") return new WhiteNoise(data);
                                    return new NoNoise(NULL);
    }

};

#endif // __NOISE_H__

