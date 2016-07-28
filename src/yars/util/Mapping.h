#ifndef __MAPPING_H__
#define __MAPPING_H__

#include <yars/types/Domain.h>

class Mapping
{
  public:
    Mapping();

    void setInputDomain(Domain in);
    void setOutputDomain(Domain out);
    void setInputDomain(yReal  min, yReal max);
    void setOutputDomain(yReal min, yReal max);

    yReal map(yReal value);
    yReal invMap(yReal value);

  private:
    Domain _in;
    Domain _out;
};

#endif // __MAPPING_H__
