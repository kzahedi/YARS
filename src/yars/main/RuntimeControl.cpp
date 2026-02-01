#include <yars/main/RuntimeControl.h>
#include <yars/configuration/YarsConfiguration.h>
#include <yars/view/console/ConsoleView.h>
#include <yars/util/Random.h>

RuntimeControl::RuntimeControl()
{
  _data = Data::instance();
  _recording = _data->current()->screens()->recording();
  _timer = new Timer();
  _captureRunning = false;
}

RuntimeControl::~RuntimeControl()
{
  delete _timer;
}

void RuntimeControl::init()
{
  if (__YARS_GET_USE_RANDOM_SEED)
  {
    Random::initialise(__YARS_GET_RANDOM_SEED);
  }
  _timer->reset();
}

void RuntimeControl::step()
{
#ifdef USE_CAPTURE_VIDEO
  if (_captureRunning == true && _recording->record() == false)
  {
    if (_videoCaptureCallback) _videoCaptureCallback();
  }
  if (_captureRunning == false && _recording->record() == true)
  {
    if (_videoCaptureCallback) _videoCaptureCallback();
  }
  _captureRunning = _recording->record();
#endif

  if (__YARS_GET_USE_PRINT_TIME_INFORMATION)
  {
    ConsoleView::printTime();
  }

  if (__YARS_GET_MAX_ITERATIONS > 0)
  {
    if (__YARS_GET_STEP > (unsigned int)__YARS_GET_MAX_ITERATIONS)
    {
      Y_INFO("Maximum number of physics iterations (%d) reached.", __YARS_GET_MAX_ITERATIONS);
      Y_DEBUG("Exiting YARS from RuntimeControl.");
      if (_quitCallback) _quitCallback();
      return;
    }
  }

  if (__YARS_GET_RESET > 0)
  {
    if (__YARS_GET_STEP > 0 && __YARS_GET_STEP % __YARS_GET_RESET == 0)
    {
      __YARS_SET_RESET_SIMULATION;
    }
  }

  if (__YARS_GET_USE_REAL_TIME)
  {
    long remaining = (900000.0 / (double)__YARS_GET_SIMULATOR_FREQUENCY * __YARS_GET_REAL_TIME_FACTOR) - _timer->get();
    if (remaining > 0)
    {
      _timer->sleep(remaining);
    }
    _timer->reset();
  }
}
