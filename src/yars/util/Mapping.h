#ifndef __MAPPING_H__
#define __MAPPING_H__

#include <yars/types/Domain.h>

class Mapping
{
  public:
    Mapping();

    void setInputDomain(Domain in);
    void setOutputDomain(Domain out);
    void setInputDomain(double  min, double max);
    void setOutputDomain(double min, double max);

    double map(double value);
    double invMap(double value);

  private:
    Domain _in;
    Domain _out;
};

#endif // __MAPPING_H__
