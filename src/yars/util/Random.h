#ifndef __RANDOM_H__
#define __RANDOM_H__

#include <yars/defines/types.h>

class Random
{
  public:
    static void  initialise();
    static void  initialise(int seed);
    static double unit();
    static int   rand(int min, int max);
};

#endif // __RANDOM_H__
