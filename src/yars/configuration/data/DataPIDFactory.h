#ifndef __DATA_PID_FACTORY_H__
#define __DATA_PID_FACTORY_H__

#include <yars/util/PID.h>
#include <yars/configuration/data/DataParseElement.h>

# define YARS_STRING_P    (char*)"p"
# define YARS_STRING_I    (char*)"i"
# define YARS_STRING_D    (char*)"d"
# define YARS_STRING_SIZE (char*)"size"

class DataPIDFactory
{
  public:
    static void set(PID &pid, DataParseElement *element)
    {
      double p = pid.p();
      double i = pid.i();
      double d = pid.d();
      int size = pid.size();

      element->set(YARS_STRING_P, p);
      element->set(YARS_STRING_I, i);
      element->set(YARS_STRING_D, d);
      element->set(YARS_STRING_SIZE, size);

      pid.setPID(p, i, d, size);
    }
};

#endif // __DATA_PID_FACTORY_H__



