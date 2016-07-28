#ifndef __TEXTURE_DEFINITION_H__
#define __TEXTURE_DEFINITION_H__

#include "P2D.h"

#include <string>

using namespace std;

class TextureDefinition
{
  public:
    TextureDefinition()
    {
      filename = "#UNDEFINED#";
      scale.x  = 1.0;
      scale.y  = 1.0;
    }

    TextureDefinition & operator=(const TextureDefinition &b)
    {
      filename = b.filename;
      scale = b.scale;
      return *this;
    }

    friend std::ostream& operator<<(std::ostream& str, TextureDefinition& t)
    {
      str << t.filename << ": " << t.scale;
      return str;
    };

    string filename;
    P2D    scale;
};

#endif // __TEXTURE_DEFINITION_H__
