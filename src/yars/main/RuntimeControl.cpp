#include <yars/main/RuntimeControl.h>
#include <yars/view/console/ConsoleView.h>
#include <yars/configuration/YarsConfiguration.h>
#include <yars/util/Random.h>

#ifdef _WIN32
#endif // _WIN32


RuntimeControl::RuntimeControl()
{
  _data           = Data::instance();
  _recording      = _data->current()->screens()->recording();
  _timer          = new Timer();
  _captureRunning = false;
}

RuntimeControl::~RuntimeControl()
{
  delete _timer;
}

void RuntimeControl::notify(ObservableMessage *m)
{
  switch(m->type())
  {
    case __M_RESET:
    case __M_INIT:
      if(__YARS_GET_USE_RANDOM_SEED)
      {
        Random::initialise(__YARS_GET_RANDOM_SEED);
      }
      _timer->reset();
      break;
    case __M_NEXT_STEP:

#ifdef USE_CAPTURE_VIDEO
      if(_captureRunning == true  && _recording->record() == false) notifyObservers(_m_autoToggleCaptureVideo);
      if(_captureRunning == false && _recording->record() == true)  notifyObservers(_m_autoToggleCaptureVideo);
      _captureRunning = _recording->record();
#endif // USE_CAPTURE_VIDEO


      if(__YARS_GET_USE_PRINT_TIME_INFORMATION)
      {
        ConsoleView::printTime();
      }
      if(__YARS_GET_MAX_ITERATIONS > 0)
      {
        if(__YARS_GET_STEP > (unsigned int)__YARS_GET_MAX_ITERATIONS)
        {
          Y_INFO("Maximum number of physics iterations (%d) reached.", __YARS_GET_MAX_ITERATIONS);
          Y_DEBUG("Exiting YARS from RuntimeControl.");
          notifyObservers(_m_quit_called);
          Y_DEBUG("Exiting YARS from RuntimeControl notification sent.");
          return;
        }
      }

      if(__YARS_GET_RESET > 0)
      {
        if(__YARS_GET_STEP > 0 && __YARS_GET_STEP % __YARS_GET_RESET == 0)
        {
          notifyObservers(_m_reset);
        }
      }

      if(__YARS_GET_USE_REAL_TIME)
      {
        long remaining = (900000.0 / (yReal)__YARS_GET_SIMULATOR_FREQUENCY * __YARS_GET_REAL_TIME_FACTOR) - _timer->get();
        if(remaining > 0)
        {
          _timer->sleep(remaining);
        }
        _timer->reset();
      }
      break;
  }
}
