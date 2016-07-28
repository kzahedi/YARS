#ifndef __MACRO_USER_INTERFACE_H__
#define __MACRO_USER_INTERFACE_H__

#include "DataMacros.h"

class MacroUserInterface
{
  public:
    virtual DataMacros* macros() = 0;
};

#endif // __MACRO_USER_INTERFACE_H__


