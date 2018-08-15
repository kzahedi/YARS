#ifndef __DATA_DOMAIN_FACTORY_H__
#define __DATA_DOMAIN_FACTORY_H__

#include <yars/types/Domain.h>

#include <yars/configuration/data/DataParseElement.h>

# define YARS_STRING_MIN (char*)"min"
# define YARS_STRING_MAX (char*)"max"

class DataDomainFactory
{
  public:
    static void set(Domain &domain, DataParseElement *element)
    {
      element->set(YARS_STRING_MIN, domain.min);
      element->set(YARS_STRING_MAX, domain.max);
    }
};

#endif // __DATA_DOMAIN_FACTORY_H__
