#ifndef __FILTER_H__
#define __FILTER_H__

#include <yars/defines/types.h>

class Filter
{
  public:
    Filter();

    yReal calculate(yReal in);
};

#endif // __FILTER_H__

