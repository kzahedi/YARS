#include <yars/view/console/ConsoleView.h>

#include <yars/util/Timer.h>
#include <yars/util/OSD.h>

#include <string>
#include <sstream>

ConsoleView*  ConsoleView::_me                         = NULL;
int           ConsoleView::_currentDebugLevel          = YARS_INFO;
unsigned long ConsoleView::_debugTiming                = 0;
unsigned long ConsoleView::_printTimeLastMeasurement   = 0;
unsigned long ConsoleView::_captureTimeLastMeasurement = 0;
unsigned long ConsoleView::_captureTimeLastStep        = 0;
unsigned long ConsoleView::_printTimeLastStep          = 0;
double         ConsoleView::_printTimeUPS               = 0.0;
double         ConsoleView::_printRealTimeFactor        = 0.0;

ConsoleView* ConsoleView::instance()
{
  if(_me == NULL)
  {
    _me = new ConsoleView();
  }
  return _me;
}

ConsoleView::ConsoleView()
{
  _framePerSecondInterval = 1000.0;
  _debugTiming            = Timer::getTime();
}


void ConsoleView::notify(ObservableMessage *m)
{
  Y_DEBUG("ConsoleView: A message \"%s\" was send.",m->string().c_str()); // gets all keyboard messages
  switch(m->type())
  {
    //case __M_NEXT_STEP:
      // nothing
      //break;
    case __M_RESET:
      Y_DEBUG("Reset called");
      break;
  }
}

void ConsoleView::printViewpoint(P3D xyz, P3D hpr)
{
  if(_currentDebugLevel== YARS_FATAL) return;
  cout << "Camera Viewpoint is given by:" << endl;
  cout << "<position x=\"" << xyz.x << "\" y=\"" << xyz.y << "\" z=\"" << xyz.z << "\"/>" << endl;
  cout << "<angles   x=\"" << RAD_TO_DEG(hpr.x) << "\" y=\"" << RAD_TO_DEG(hpr.y) << "\" z=\"" << RAD_TO_DEG(hpr.z) << "\"/>" << endl;
}


void ConsoleView::printMessage(int type, const char *message, ...)
{
  if(type < _currentDebugLevel) return;
  switch(type)
  {
    case YARS_DEBUG:
      fprintf(stdout, "[DEBUG] -");
      break;
    // case YARS_INFO:
      // fprintf(stdout, "[INFO]  -");
      // break;
    case YARS_WARN:
      fprintf(stdout, "[WARN]  -");
      break;
    case YARS_FATAL:
      fprintf(stdout, "[FATAL] -");
      break;
    case YARS_MESSAGE:
      fprintf(stdout, "[MSG]   - ");
      break;
  }

  // fprintf(stdout, "%8u - ", (unsigned int)(Timer::getTime() - _debugTiming));
  va_list ap;
  va_start (ap, message);
  vfprintf (stdout, message, ap);
  va_end (ap);
  fprintf(stdout,"\n");
}

void ConsoleView::printMessage(int type, std::string message, ...)
{
  va_list ap;
  printMessage(type, message.c_str(), ap);
}

void ConsoleView::setDebugLevel(int level)
{
  _currentDebugLevel = level;
}

int ConsoleView::getDebugLevel()
{
  return _currentDebugLevel;
}


void ConsoleView::__debug(std::string prefix, P3D p)
{
  Y_INFO("%s = [%f, %f, %f]", prefix.c_str(), p.x, p.y, p.z);
}

void ConsoleView::__debug(std::string prefix, Colour c)
{
  Y_INFO("%s = [%f, %f, %f, %f]", prefix.c_str(), c.red(), c.green(), c.blue(), c.alpha());
}

void ConsoleView::printTime()
{
  if(_currentDebugLevel == YARS_FATAL) return;
  unsigned long t = Timer::getTime();
  if( (Timer::getTime() - _printTimeLastMeasurement) > 500)
  {
    double diff               = ((double)(__YARS_GET_CONTINUOUS_STEP - _printTimeLastStep));
    double time               = ((double)(t - _printTimeLastMeasurement))/1000.0;
    _printTimeUPS             = diff / time;
    _printTimeLastStep        = __YARS_GET_CONTINUOUS_STEP;
    _printTimeLastMeasurement = Timer::getTime();
    _printRealTimeFactor      = _printTimeUPS / ((double)__YARS_GET_SIMULATOR_FREQUENCY);
    printf("\rrunning %012d iterations: %s | %.2f x RT%s",
        (int)__YARS_GET_CONTINUOUS_STEP,
         OSD::getElapsedTimeString().c_str(),
        _printRealTimeFactor,
      (__YARS_GET_USE_CR)?"\n":"");
    fflush( stdout );
  }
}

void ConsoleView::printCapturingInformation(int frameIndex)
{
  if(_currentDebugLevel == YARS_FATAL) return;
  unsigned long t = Timer::getTime();
  if( (Timer::getTime() - _captureTimeLastMeasurement) > 1000)
  {
    double diff               = ((double)(__YARS_GET_CONTINUOUS_STEP - _captureTimeLastStep));
    double time               = ((double)(t - _captureTimeLastMeasurement))/1000.0;
    double captureTimeUPS     = diff / time;
    _captureTimeLastStep        = __YARS_GET_CONTINUOUS_STEP;
    _captureTimeLastMeasurement = Timer::getTime();
    double captureRealTimeFactor = captureTimeUPS / ((double)__YARS_GET_SIMULATOR_FREQUENCY);
    printf("\rcaptured %012d frames: %s | %.2f x RT%s",
        (int)__YARS_GET_CONTINUOUS_STEP,
         OSD::getElapsedTimeString().c_str(),
         captureRealTimeFactor,
      (__YARS_GET_USE_CR)?"\n":"");
    fflush( stdout );
  }
}
