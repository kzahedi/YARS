#ifndef __DATA_SCALE_FACTORY_H__
#define __DATA_SCALE_FACTORY_H__

#include <yars/types/P3D.h>

#include <yars/configuration/data/DataParseElement.h>

class DataScaleFactory
{
  public:
    static void set(P3D &scale, DataParseElement *element)
    {
      element->set(YARS_STRING_X, scale.x);
      element->set(YARS_STRING_Y, scale.y);
      element->set(YARS_STRING_Z, scale.z);
    }
};

#endif // __DATA_SCALE_FACTORY_H__



