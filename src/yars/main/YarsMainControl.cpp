#include <yars/main/YarsMainControl.h>
#include <yars/view/gui/KeyHandler.h>
#include <yars/util/Random.h>

// #ifdef SUPPRESS_ALL_OUTPUT
#define PRINT_START_UP_MESSAGE(a) ;
// #else
// #  define PRINT_START_UP_MESSAGE(a) cout << a << endl;
// #endif

namespace yars {

YarsMainControl::YarsMainControl(int argc, char **argv)
{
  Random::initialise();
  YarsErrorHandler *e = YarsErrorHandler::instance();
  // e->addObserver(this);
  _ycc = YarsConfiguration::instance();
  _cv = ConsoleView::instance();
  _sig = SignalHandler::instance();
  _ycc->init(argc, argv);
  // Signal handler setup - no longer uses observer pattern
#ifndef SUPPRESS_ALL_OUTPUT
  // Console view setup - no longer uses observer pattern 
#endif              // SUPPRESS_ALL_OUTPUT
  Y_DEBUG("YarsMainControl instantiating models and controls.");
  _keepOnRunning = true;

  // models are deleted by their controls, and controls do not have a destructor
  // (they don't need one)

  PRINT_START_UP_MESSAGE("Starting physics handler.");
  _ypc = new YarsPhysicsControl();
  _ypm = new YarsPhysicsModel();
  _ypc->setModel(_ypm);

  _ylc = new YarsLoggingControl();
  _ylm = new YarsLoggingModel();
  _ylc->setModel(_ylm);

  Y_DEBUG("YarsMainControl adding myself as observable to the controls.");

  // addObserver(_ycc); // 2. the configuration
  // Physics and logging controls setup - no longer uses observer pattern

  // Models and configuration setup - no longer uses observer pattern

  Y_DEBUG("YarsMainControl sending init messages to all observers");

#ifdef USE_VISUALISATION
  // for the quit key, and may be also some other stuff?
  if (__YARS_GET_USE_VISUALISATION)
  {
    PRINT_START_UP_MESSAGE("Starting key handler.");
    KeyHandler *keyHandler = KeyHandler::instance();
    keyHandler->registerKeyboardShortcuts();
    // Key handler setup - no longer uses observer pattern
  }
#endif // USE_VISUALISATION

  // at least, add runtime control, i.e. auto-reset, auto-end, etc.
  PRINT_START_UP_MESSAGE("Starting runtime control.");
  _rtc = new RuntimeControl();

  // TODO: clean up all controls and models

  // Initialize all components directly
  _ypc->init();
  _ylc->init();
  if (_rtc) _rtc->init();
}

YarsMainControl::~YarsMainControl()
{
  Y_DEBUG("YarsMainControl destructor called");
  delete _cv;
  delete _sig;
  delete _ylc;
  delete _ylm;
  delete _ycc;
  delete _ypc;
  delete _ypm;
  delete _rtc;
}

void YarsMainControl::run()
{

  if (__YARS_GET_SYNC_GUI)
  {
    // GUI sync handling - no longer uses observer pattern
  }

  while (_keepOnRunning)
  {
    if (!__YARS_GET_USE_PAUSE || (__YARS_GET_USE_PAUSE && __YARS_GET_USE_SINGLE_STEP))
    {
      __YARS_SET_STEP(__YARS_GET_STEP + 1);
      __YARS_SET_CONTINUOUS_STEP(__YARS_GET_CONTINUOUS_STEP + 1);
    }
    else
    {
      usleep(100);
    }
    if (__YARS_IS_RESET_SIMULATION)
    {
      __YARS_UNSET_RESET_SIMULATION;
      // Reset all components directly
      _ypc->reset();
      _ylc->reset();
      if (_rtc) _rtc->reset();
    }
    else
    {
      // Step all components directly
      _ypc->step();
      _ylc->step();
      if (_rtc) 
      {
        _rtc->step();
        if (_rtc->shouldQuit())
        {
          _keepOnRunning = false;
        }
      }
    }
  }

#ifdef USE_VISUALISATION
  if (__YARS_GET_USE_VISUALISATION)
  {
    if (!__YARS_GET_SYNC_GUI)
    {
      // GUI sync handling - no longer uses observer pattern
      usleep(500);
    }
    // GUI quit handling - no longer uses observer pattern
  }
#endif // USE_VISUALISATION

  // Quit all components directly
  _ypc->quit();
  _ylc->quit();
  if (_rtc) _rtc->quit();

  __closeApplication();
}

void YarsMainControl::__closeApplication()
{
  // In headless mode (no GUI) we can safely clean up the global Data
  // instance here. When the visualisation is enabled, however, deleting
  // Data in this (physics) thread may race with the GUI thread which is
  // still rendering / handling SDL events. In that case we defer the
  // actual destruction of Data until **after** the GUI thread has fully
  // terminated (see yarsMain.cpp).

  if (!__YARS_GET_USE_VISUALISATION)
  {
    Data::close();
    exit(0);
  }
}

void YarsMainControl::notify(ObservableMessage *message)
{
  // only catch and send messages here.
  // work as a router and broadcaster
  switch (message->type())
  {
  case __M_ERROR:
    Y_FATAL("Error occured");
    cout << YarsErrorHandler::instance()->message() << endl;
    // TODO: close stuff before exiting
    __closeApplication();
    break;
  case __M_QUIT_CALLED:
    _keepOnRunning = false;
    break;
  case __M_RESET:
    YarsConfiguration::instance()->reset();
    // Reset handling - no longer uses observer pattern
    break;
  case __M_AUTO_TOGGLE_CAPTURE_VIDEO:
    // Video capture handling - no longer uses observer pattern
    break;
  case __M_SIGNAL_HANDLER_ACTIVATED:
    // Signal handling - no longer uses observer pattern
    _keepOnRunning = false;
    break;
  }
}

} // namespace yars
