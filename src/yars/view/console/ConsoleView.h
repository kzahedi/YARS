#ifndef __CONSOLE_VIEW_H__
#define __CONSOLE_VIEW_H__

// TODO: remove dependency of console view on other libraries

#define _PRINT_COM_STRUCTURE_OFF                 0
#define _PRINT_COM_STRUCTURE_SIMPLE_MODE         1
#define _PRINT_COM_STRUCTURE_EXTENDED_MODE       2

#include <stdarg.h>
#include <string>

#include <yars/types/P3D.h>
#include <yars/types/Colour.h>

#define YARS_DEBUG      0
#define YARS_INFO       1
#define YARS_WARN       2
#define YARS_MESSAGE    3
#define YARS_FATAL      4

#define Y_DEBUG(...)   ConsoleView::printMessage(YARS_DEBUG, __VA_ARGS__);
#define Y_INFO(...)    ConsoleView::printMessage(YARS_INFO, __VA_ARGS__);
#define Y_FATAL(...)   ConsoleView::printMessage(YARS_FATAL, __VA_ARGS__);
#define Y_WARN(...)    ConsoleView::printMessage(YARS_WARN, __VA_ARGS__);
#define Y_MESSAGE(...) ConsoleView::printMessage(YARS_MESSAGE, __VA_ARGS__);

#define Y_SET_DEBUG_LEVEL(a) ConsoleView::setDebugLevel(a)
#define Y_GET_DEBUG_LEVEL    ConsoleView::getDebugLevel()



#ifdef SUPPRESS_ALL_OUTPUT
#  undef  Y_INFO
#  define Y_INFO(...)
#  undef  Y_FATAL
#  define Y_FATAL(...)
#  undef  Y_WARN
#  define Y_WARN(...)
#endif

namespace yars {

class ConsoleView
{
  public:

    static ConsoleView* instance();
    virtual ~ConsoleView() { };

    // Direct method to handle reset (replaces Observer pattern)
    void onReset();

    static void printMessage(int type, const char *string, ...);
    static void printMessage(int type, std::string message, ...);
    static void setDebugLevel(int level);
    static int  getDebugLevel();

    static void printTime();
    static void printViewpoint(P3D xyz, P3D hpr);

    static void printCapturingInformation(int frameIndex);

  private:
    ConsoleView();

    static ConsoleView* _me;

    static int _currentDebugLevel;
    static unsigned long _debugTiming;

    static void __debug(std::string prefix, P3D p);
    static void __debug(std::string prefix, Colour c);

    void __printOldAndNewFollowMode(int direction);

    static int __days();
    static int __hours();
    static int __minutes();
    static int __seconds();
    static int __milliSeconds();


    double _framePerSecondInterval;

    static unsigned long _printTimeLastMeasurement;
    static unsigned long _printTimeLastStep;
    static unsigned long _captureTimeLastMeasurement;
    static unsigned long _captureTimeLastStep;
    static double _printTimeUPS;
    static double _printRealTimeFactor;
};

} // namespace yars

// Temporary global using directive for backward compatibility during namespace transition
using yars::ConsoleView;

#endif // __CONSOLE_VIEW_H__

