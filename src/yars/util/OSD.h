#ifndef __OSD_H__
#define __OSD_H__

#include <yars/configuration/YarsConfiguration.h>

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

# define FILL(a) setfill('0') << setw(a)

using namespace std;

class OSD
{
  public:
    static int days()
    {
      return (int)(((double)(__YARS_GET_STEP)) / ((double)(24.0 * 60.0 * 60.0 * __YARS_GET_SIMULATOR_FREQUENCY)));
    };

    static int hours()
    {
      return (int)(((double)(__YARS_GET_STEP)) / ((double)(60.0 * 60.0 * __YARS_GET_SIMULATOR_FREQUENCY))) % 24;
    };

    static int minutes()
    {
      return (int)(((double)(__YARS_GET_STEP)) / ((double)(60.0 * __YARS_GET_SIMULATOR_FREQUENCY))) % 60;
    };

    static int seconds()
    {
      return (int)(((double)(__YARS_GET_STEP)) / ((double)__YARS_GET_SIMULATOR_FREQUENCY)) % 60;
    };

    static int milliSeconds()
    {
      return (int)(((double)((__YARS_GET_STEP) % __YARS_GET_SIMULATOR_FREQUENCY)) / ((double)__YARS_GET_SIMULATOR_FREQUENCY) * 1000);
    };

    static string getElapsedTimeString()
    {
      std::stringstream oss;
      if(OSD::days()         > 0) oss << FILL(2) << OSD::days()         << "d";
      if(OSD::hours()        > 0 || oss.str().length() > 0) oss << FILL(2) << OSD::hours()        << "h";
      if(OSD::minutes()      > 0 || oss.str().length() > 0) oss << FILL(2) << OSD::minutes()      << "m";
      if(OSD::seconds()      > 0 || oss.str().length() > 0) oss << FILL(2) << OSD::seconds()      << "s";
                                                            oss << FILL(3) << OSD::milliSeconds() << "ms";
      return oss.str();
    }
};

#endif // __OSD_H__


