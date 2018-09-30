#include <yars/main/YarsMainControl.h>
#include <yars/view/gui/KeyHandler.h>
#include <yars/util/Random.h>

//#ifdef SUPPRESS_ALL_OUTPUT
#  define PRINT_START_UP_MESSAGE(a) ;
//#else
//#  define PRINT_START_UP_MESSAGE(a) cout << a << endl;
//#endif

YarsMainControl::YarsMainControl(int argc, char **argv)
{
  Random::initialise();
  YarsErrorHandler *e = YarsErrorHandler::instance();
  e->addObserver(this);
  _ycc = YarsConfiguration::instance();
  _cv  = ConsoleView::instance();
  _sig = SignalHandler::instance();
  _ycc->init(argc, argv);
  _sig->addObserver(this);
#ifndef SUPPRESS_ALL_OUTPUT
  addObserver(_cv);         // 1. first things first, we need debug output
#endif // SUPPRESS_ALL_OUTPUT
  Y_DEBUG("YarsMainControl instantiating models and controls.");
  _keepOnRunning = true;

  // models are deleted by their controls, and controls do not have a destructor
  // (they don't need one)

  PRINT_START_UP_MESSAGE("Starting physics handler.");
  _ypc   = new YarsPhysicsControl();
  _ypm   = new YarsPhysicsModel();
  _ypc->setModel(_ypm);

  _ylc = new YarsLoggingControl();
  _ylm = new YarsLoggingModel();
  _ylc->setModel(_ylm);

  Y_DEBUG("YarsMainControl adding myself as observable to the controls.");

  //addObserver(_ycc); // 2. the configuration
  addObserver(_ypc); // 3. the physics
  addObserver(_ylc); // after physics!

  _ypm->addObserver(this);
  _ycc->addObserver(this);

  Y_DEBUG("YarsMainControl sending init messages to all observers");

#ifdef USE_VISUALISATION
  // for the quit key, and may be also some other stuff?
  if(__YARS_GET_USE_VISUALISATION)
  {
    PRINT_START_UP_MESSAGE("Starting key handler.");
    KeyHandler *keyHandler = KeyHandler::instance();
    keyHandler->registerKeyboardShortcuts();
    keyHandler->addObserver(this);
  }
#endif // USE_VISUALISATION

  // at least, add runtime control, i.e. auto-reset, auto-end, etc.
  PRINT_START_UP_MESSAGE("Starting runtime control.");
  _rtc = new RuntimeControl();
  addObserver(_rtc);
  _rtc->addObserver(this);

  // TODO: clean up all controls and models

  notifyObservers(_m_init);
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
  // delete _rtc;
}

void YarsMainControl::run()
{

  if(__YARS_GET_SYNC_GUI)
  {
    notifyObservers(_m_toggleSyncedGui);
  }

  while(_keepOnRunning)
  {
    if(!__YARS_GET_USE_PAUSE || (__YARS_GET_USE_PAUSE && __YARS_GET_USE_SINGLE_STEP))
    {
      __YARS_SET_STEP(__YARS_GET_STEP + 1);
      __YARS_SET_CONTINUOUS_STEP(__YARS_GET_CONTINUOUS_STEP + 1);
    }
    else
    {
      usleep(100);
    }
    notifyObservers(_m_nextStep);
  }

#ifdef USE_VISUALISATION
  if(__YARS_GET_USE_VISUALISATION)
  {
    if(!__YARS_GET_SYNC_GUI)
    {
      notifyObservers(_m_toggleSyncedGui);
      usleep(500);
    }
    notifyObservers(_m_quit_gui_called);
  }
#endif // USE_VISUALISATION

  notifyObservers(_m_quit);

  __closeApplication();
}

void YarsMainControl::__closeApplication()
{
  Data::close();
  if(!__YARS_GET_USE_VISUALISATION) exit(0);
// #ifdef USE_VISUALISATION
//   OSG::osgExit();
//   QCoreApplication::exit(0);
// #endif

}


void YarsMainControl::notify(ObservableMessage *message)
{
  // only catch and send messages here.
  // work as a router and broadcaster
  switch(message->type())
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
      notifyObservers(_m_reset);
      break;
    case __M_AUTO_TOGGLE_CAPTURE_VIDEO:
      notifyObservers(_m_autoToggleCaptureVideo); // pass it from RuntimeControl to others
      break;
    case __M_SIGNAL_HANDLER_ACTIVATED:
      notifyObservers(_m_quit_called);
      _keepOnRunning = false;
      break;
  }
}
