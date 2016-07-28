#ifndef __DATA_POSITION_FACTORY_H__
#define __DATA_POSITION_FACTORY_H__

#include <yars/types/P3D.h>
#include <yars/configuration/data/DataParseElement.h>

# define YARS_STRING_X     (char*)"x"
# define YARS_STRING_Y     (char*)"y"
# define YARS_STRING_Z     (char*)"z"

class DataPositionFactory
{
  public:
    static void set(P3D &position, DataParseElement *element)
    {
      element->set(YARS_STRING_X, position.x);
      element->set(YARS_STRING_Y, position.y);
      element->set(YARS_STRING_Z, position.z);
    }
};

#endif // __DATA_POSITION_FACTORY_H__
