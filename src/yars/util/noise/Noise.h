#ifndef __NOISE_H__
#define __NOISE_H__

#include <yars/defines/types.h>
#include <yars/configuration/data/DataNoise.h>

class Noise
{
  public:
    Noise(DataNoise *data) { _data = data; }
    virtual ~Noise() { };

    virtual double calculate(double in) = 0;

  protected:
    DataNoise *_data;
};

#endif // __NOISE_H__
