#ifndef __NAME_DIMENSION_DOMAIN_H__
#define __NAME_DIMENSION_DOMAIN_H__

#include <string>
#include "Domain.h"

using namespace std;

class NameDimensionDomain
{
  public:
    NameDimensionDomain(string n = "", int d = 0)
    {
      name      = n;
      dimension = d;
    }
    string name;
    int    dimension;
    std::vector<Domain> internal;
    std::vector<Domain> external;
    std::vector<string> names;
};

#endif // __NAME_DIMENSION_DOMAIN_H__
