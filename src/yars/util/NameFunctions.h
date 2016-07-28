#ifndef __NAME_FUNCTIONS_H__
#define __NAME_FUNCTIONS_H__

#include <string>
#include <sstream>

#include "exception/YarsException.h"

class NameFunctions
{
  public:
    static void first(const string name, string *first)
    {
      string::size_type firstPos = name.find_first_not_of("/", 0);
      string::size_type lastPos  = name.find_first_of(":", 0);
      std::stringstream s;
      s << "/" << name.substr(firstPos, (lastPos-1)) << "/";
      *first = s.str();
    };

    static void last(const string name, string *last)
    {
      string::size_type lastPos  = name.find_last_of(":", 1);
      std::stringstream s;
      s << "/" << name.substr(lastPos, name.length()-1) << "/";
      *last = s.str();
    };

    static void filenameWithoutExtension(string name, string *returnValue)
    {
      string::size_type startPos = name.find_last_of((char)'/');
      string::size_type endPos   = name.find_last_of(".xml");
      *returnValue = name.substr(startPos+1, endPos-1);
    }
};


#endif // __NAME_FUNCTIONS_H__
