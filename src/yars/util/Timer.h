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
#include <iomanip>
#include <ctime>

#ifndef _MSC_VER
#include <sys/time.h>
#endif

#include <thread>
#include <chrono>

using namespace std;

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
      auto time_t_now = std::chrono::system_clock::to_time_t(now);
      std::tm tm_now;
#ifndef _MSC_VER
      localtime_r(&time_t_now, &tm_now);
#else
      localtime_s(&tm_now, &time_t_now);
#endif
      std::ostringstream oss;
      oss << std::put_time(&tm_now, "%Y-%m-%d");
      *dateString = oss.str();
    };

    static void getDateTimeString(std::string *dateString)
    {
      auto now = std::chrono::system_clock::now();
      auto time_t_now = std::chrono::system_clock::to_time_t(now);
      std::tm tm_now;
#ifndef _MSC_VER
      localtime_r(&time_t_now, &tm_now);
#else
      localtime_s(&tm_now, &time_t_now);
#endif
      std::ostringstream oss;
      oss << std::put_time(&tm_now, "%Y-%m-%d-%H-%M-%S");
      *dateString = oss.str();
    };

    Timer()
    {
      _last = std::chrono::steady_clock::now();
    };

    void reset()
    {
      _last = std::chrono::steady_clock::now();
    };

    long get()
    {
      auto now = std::chrono::steady_clock::now();
      auto diff = std::chrono::duration_cast<std::chrono::microseconds>(now - _last);
      return diff.count();
    };


    void sleep(long m)
    {
      std::this_thread::sleep_for(std::chrono::microseconds(m));
    };

  private:
      std::chrono::steady_clock::time_point _last;

};

#endif // __TIMER_H_

