#ifndef __TIMER_H_
#define __TIMER_H_

#ifdef _MSC_VER
#  define SPRINTF(a, b, c) sprintf_s(a, b, c)
#else
#  define SPRINTF(a, b, c) sprintf(a, b, c)
#endif // _MSC_VER

#include <stdio.h>
#include <string>
#include <sstream>

#include <chrono>
#include <iomanip>

#ifndef _MSC_VER
#include <sys/time.h>
#include <ctime>
#endif

#include <thread>
#include <chrono>

using std::string;
using std::ostringstream;

class Timer
{
  public:
    // TODO: thread safe
    static unsigned long getTime()
    {
#ifndef _MSC_VER
      timeval getTimeOfDayTime;
      gettimeofday(&getTimeOfDayTime, 0);
      return ((getTimeOfDayTime.tv_sec  % 10000) * 1000 + getTimeOfDayTime.tv_usec / 1000);
#else // _MSC_VER
      return 0;
#endif // _MSC_VER
    };

    static unsigned long getUTime()
    {
#ifndef _MSC_VER
      timeval getTimeOfDayTime;
      gettimeofday(&getTimeOfDayTime, 0);
      return ((getTimeOfDayTime.tv_sec  % 10000) * 1000 + getTimeOfDayTime.tv_usec);
#else // _MSC_VER
      return 0;
#endif // _MSC_VER

    }

    static void getDateString(std::string *dateString)
    {
      auto now = std::chrono::system_clock::now();
      auto time_t = std::chrono::system_clock::to_time_t(now);
      auto tm = *std::localtime(&time_t);
      std::stringstream ss;
      ss << std::put_time(&tm, "%Y-%m-%d");
      *dateString = ss.str();
    };

    static void getDateTimeString(std::string *dateString)
    {
      stringstream oss;
#ifndef _MSC_VER
      char buf[2];
      time_t rawtime;
      struct tm * ptm;
      time ( &rawtime );
      ptm = gmtime ( &rawtime );

      auto now = std::chrono::system_clock::now();
      auto time_t = std::chrono::system_clock::to_time_t(now);
      auto tm = *std::localtime(&time_t);
      oss << std::put_time(&tm, "%Y-%m-%d-%H-%M-%S");
#else // _MSC_VER
      cout << "getDateTimeString not yet supported in windows version" << endl;
#endif // _MSC_VER

      *dateString = oss.str();
    };

    Timer()
    {
      _last = std::chrono::high_resolution_clock::now();
    };

    void reset()
    {
      _last = std::chrono::high_resolution_clock::now();
    };

    long get()
    {
      auto now = std::chrono::high_resolution_clock::now();
      auto diff = std::chrono::duration_cast<std::chrono::microseconds>(now - _last);
      return diff.count();
    };


    void sleep(long m)
    {
      std::this_thread::sleep_for(std::chrono::microseconds(m));
    };

  private:
      std::chrono::high_resolution_clock::time_point _last;

};

#endif // __TIMER_H_

