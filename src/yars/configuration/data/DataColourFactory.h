#ifndef __DATA_COLOUR_FACTORY_H__
#define __DATA_COLOUR_FACTORY_H__

#include <yars/types/Colour.h>

#include <stdio.h>
#include <stdlib.h>

# define DCF_YARS_STRING_VALUE (char*)"value"

class DataColourFactory
{
  public:
    static void set(Colour& color, DataParseElement *element, string attribute = DCF_YARS_STRING_VALUE)
    {
      string value;
      element->set(attribute, value);
      set(color, value);
    }

    static void set(Colour& color, string value)
    {
      char *red   = new char[3];
      char *green = new char[3];
      char *blue  = new char[3];
      char *alpha = new char[3];
      red[0]   = value[0];
      red[1]   = value[1];
      red[2]   = 0;
      green[0] = value[2];
      green[1] = value[3];
      green[2] = 0;
      blue[0]  = value[4];
      blue[1]  = value[5];
      blue[2]  = 0;
      alpha[0] = 'F';
      alpha[1] = 'F';
      alpha[2] = 0;
      if(value.length() == 8)
      {
        alpha[0] = value[6];
        alpha[1] = value[7];
      }
      double r = ((double)strtol(red,   NULL, 16)) / (double)255;
      double g = ((double)strtol(green, NULL, 16)) / (double)255;
      double b = ((double)strtol(blue,  NULL, 16)) / (double)255;
      double a = ((double)strtol(alpha, NULL, 16)) / (double)255;
      color.setRed(r);
      color.setGreen(g);
      color.setBlue(b);
      color.setAlpha(a);
      delete [] red;
      delete [] green;
      delete [] blue;
      delete [] alpha;
    }
};

#endif // __DATA_COLOUR_FACTORY_H__
