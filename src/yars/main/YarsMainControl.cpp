#include <yars/main/YarsMainControl.h>
#include <yars/util/Random.h>

#ifdef USE_VISUALISATION
#include <yars/view/gui/KeyHandler.h>
#endif

YarsMainControl::YarsMainControl(int argc, char **argv)
{
  Random::initialise();
  _ycc = YarsConfiguration::instance();
  _sig = SignalHandler::instance();
  _ycc->init(argc, argv);

  // Set up signal handler callback
  _sig->setQuitCallback([this]() { requestQuit(); });

  _keepOnRunning = true;

  Y_DEBUG("YarsMainControl: Creating physics model");
  _ypm = new YarsPhysicsModel();

  Y_DEBUG("YarsMainControl: Creating logging model");
  _ylm = new YarsLoggingModel();

  Y_DEBUG("YarsMainControl: Creating runtime control");
  _rtc = new RuntimeControl();
  _rtc->setQuitCallback([this]() { requestQuit(); });

#ifdef USE_VISUALISATION
  if (__YARS_GET_USE_VISUALISATION)
  {
    KeyHandler *keyHandler = KeyHandler::instance();
    keyHandler->registerKeyboardShortcuts();
    keyHandler->setQuitCallback([this]() { requestQuit(); });
  }
#endif

  __init();
}

YarsMainControl::~YarsMainControl()
{
  Y_DEBUG("YarsMainControl destructor called");
  delete _ylm;
  delete _ypm;
  delete _rtc;
}

void YarsMainControl::__init()
{
  Y_DEBUG("YarsMainControl: Initializing subsystems");

  if (__YARS_GET_USE_RANDOM_SEED)
  {
    Random::initialise(__YARS_GET_RANDOM_SEED);
  }

  _ypm->initialisePhysics();
  _ylm->init();
  _rtc->init();
}

void YarsMainControl::__step()
{
  bool isPaused = __YARS_GET_USE_PAUSE;
  if (!isPaused || (isPaused && __YARS_GET_USE_SINGLE_STEP))
  {
    _ypm->performOneSimulationStep();
    _ylm->step();
    __YARS_SET_USE_SINGLE_STEP(false);

    // Signal GUI to do synchronized update (for video capture)
    if (_viewSyncCallback && __YARS_GET_SYNC_GUI)
    {
      _viewSyncCallback();
    }
  }
  else
  {
    usleep(100);
  }

  _rtc->step();
}

void YarsMainControl::__reset()
{
  Y_DEBUG("YarsMainControl: Processing reset");

  if (__YARS_GET_USE_RANDOM_SEED)
  {
    Random::initialise(__YARS_GET_RANDOM_SEED);
  }

  YarsConfiguration::instance()->reset();
  _ypm->reset();
  _ylm->reset();
  _ylm->step();
  _rtc->init();
}

void YarsMainControl::__quit()
{
  Y_DEBUG("YarsMainControl: Quitting");
  // Cleanup handled by destructor
}

void YarsMainControl::run()
{
  if (__YARS_GET_SYNC_GUI)
  {
    __YARS_SET_SYNC_GUI(true);
  }

  while (_keepOnRunning)
  {
    bool isPaused = __YARS_GET_USE_PAUSE;
    if (!isPaused || (isPaused && __YARS_GET_USE_SINGLE_STEP))
    {
      __YARS_SET_STEP(__YARS_GET_STEP + 1);
      __YARS_SET_CONTINUOUS_STEP(__YARS_GET_CONTINUOUS_STEP + 1);
    }

    if (__YARS_IS_RESET_SIMULATION)
    {
      __YARS_UNSET_RESET_SIMULATION;
      __reset();
    }
    else
    {
      __step();
    }
  }

#ifdef USE_VISUALISATION
  if (__YARS_GET_USE_VISUALISATION)
  {
    // Signal view to quit so it can finalize video capture
    if (_viewQuitCallback)
    {
      _viewQuitCallback();
    }
    // Give GUI time to quit gracefully
    usleep(500000); // 500ms for video finalization
  }
#endif

  __quit();
  __closeApplication();
}

void YarsMainControl::requestQuit()
{
  _keepOnRunning = false;
}

void YarsMainControl::setViewQuitCallback(std::function<void()> callback)
{
  _viewQuitCallback = callback;
}

void YarsMainControl::setViewSyncCallback(std::function<void()> callback)
{
  _viewSyncCallback = callback;
}

void YarsMainControl::__closeApplication()
{
  Data::close();
  if (!__YARS_GET_USE_VISUALISATION)
    exit(0);
}
