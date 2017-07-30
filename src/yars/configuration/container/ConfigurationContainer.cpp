#include <yars/configuration/container/ConfigurationContainer.h>

#include <yars/defines/defaults.h>

#include <iostream>

ConfigurationContainer::ConfigurationContainer()
{
  _pathToLibs                 = "";
  _pathToTextures             = "";

  _paused                     = false;

  _groundColour                = Colour::YELLOW;
  _skyColour                   = Colour::BLUE;

  _currentTimeStep            = 0;
  _continousTimeStep          = 0;
  _currentRealTime            = 0.0;

  _useWindowGeometry          = false;
  _winPositionX               = 0;
  _winPositionY               = 0;

  _usePause                   = DEFAULT_USE_PAUSE;
  _useOsd                     = DEFAULT_USE_OSD;
  _useRealTimeMode            = DEFAULT_USE_REALTIME_MODE;
  _useTextures                = DEFAULT_USE_TEXTURES;
  _useAxesVisualisation       = DEFAULT_USE_AXES_VISUALISATION;
  _usePrintFramesPerSecond    = DEFAULT_USE_PRINT_FRAMES_PER_SECOND;
  _usePrintTimeInformation    = DEFAULT_USE_PRINT_TIME_INFORMATION;
  _useVisualisation           = DEFAULT_USE_VISUALISATION;
  _useTraces                  = DEFAULT_USE_TRACES;
  _useReset                   = DEFAULT_USE_RESET;
  _usePrintKeyboardShortcuts  = DEFAULT_USE_PRINT_KEYBOARD_SHORTCUTS;
  _useSingleStep              = DEFAULT_USE_SINGLE_STEP;
  _exportGraphviz             = DEFAULT_EXPORT_GRAPHVIZ;
  _useFollowMode              = DEFAULT_USE_FOLLOW_MODE;
  _exit                       = DEFAULT_EXIT;
  _syncGui                    = DEFAULT_SYNC_GUI;
  _followMode                 = DEFAULT_FOLLOW_MODE;
  _textures                   = DEFAULT_TEXTURES;
  _libraries                  = DEFAULT_LIBRARIES;
  _useCapture                 = DEFAULT_USE_CAPTURE;
  _realtimeFactor             = DEFAULT_REALTIME_FACTOR;
  _videoCodec                 = DEFAULT_VIDEO_CODEC;
  _useCR                      = DEFAULT_USE_CR;
  _silent                     = DEFAULT_BE_SILENT;
  _usePrintConfiguration      = DEFAULT_PRINT_CONFIGURATION;
  _useRandomSeed              = DEFAULT_USE_RANDOM_SEED;
  _randomSeed                 = DEFAULT_RANDOM_SEED;


  _reset                      = -1;
  _captureName                = DEFAULT_CAPTURE_NAME;
  _captureDirectory           = DEFAULT_CAPTURE_DIRECTORY;
  _captureFrameRate           = DEFAULT_CAPTURE_FRAME_RATE;
  _cameraPosition.x           = 0;
  _cameraPosition.y           = 0;
  _cameraPosition.z           = 0;
  _cameraOrientation.x        = 0;
  _cameraOrientation.y        = 0;
  _cameraOrientation.z        = 0;

  _framesDirectory            = DEFAULT_FRAMES_DIRECTORY;
  _logDirectory               = DEFAULT_LOG_DIRECTORY;
  _defaultSimulatorFrequency  = DEFAULT_SIMULATION_FREQUENCY;
  _defaultControllerFrequency = DEFAULT_CONTROLLER_FREQUENCY;

  _xml                        = DEFAULT_XML; // default is stdin

#ifdef USE_THREAD_SAFE
  pthread_mutexattr_t pthread_mutexattr_default;
  pthread_mutexattr_init(&pthread_mutexattr_default);
  pthread_mutex_init(&_defaultControllerFrequencyMutex, &pthread_mutexattr_default);
  pthread_mutex_init(&_defaultSimulatorFrequencyMutex,  &pthread_mutexattr_default);
  pthread_mutex_init(&_cameraOrientationMutex,          &pthread_mutexattr_default);
  pthread_mutex_init(&_usePrintKeyboardShortcutsMutex,  &pthread_mutexattr_default);
  pthread_mutex_init(&_cameraPositionMutex,             &pthread_mutexattr_default);
  pthread_mutex_init(&_captureDirectoryMutex,           &pthread_mutexattr_default);
  pthread_mutex_init(&_captureFrameRateMutex,           &pthread_mutexattr_default);
  pthread_mutex_init(&_captureNameMutex,                &pthread_mutexattr_default);
  pthread_mutex_init(&_comStatusMutex,                  &pthread_mutexattr_default);
  pthread_mutex_init(&_controllerLocationsMutex,        &pthread_mutexattr_default);
  pthread_mutex_init(&_currentRealTimeMutex,            &pthread_mutexattr_default);
  pthread_mutex_init(&_currentTimeStepMutex,            &pthread_mutexattr_default);
  pthread_mutex_init(&_continousTimeStepMutex,            &pthread_mutexattr_default);
  pthread_mutex_init(&_exitMutex,                       &pthread_mutexattr_default);
  pthread_mutex_init(&_followModeMutex,                 &pthread_mutexattr_default);
  pthread_mutex_init(&_framesDirectoryMutex,            &pthread_mutexattr_default);
  pthread_mutex_init(&_materialsDirectoryMutex,         &pthread_mutexattr_default);
  pthread_mutex_init(&_cappedCylinderQualityMutex,      &pthread_mutexattr_default);
  pthread_mutex_init(&_groundColourMutex,                &pthread_mutexattr_default);
  pthread_mutex_init(&_librariesMutex,                  &pthread_mutexattr_default);
  pthread_mutex_init(&_logDirectoryMutex,               &pthread_mutexattr_default);
  pthread_mutex_init(&_pathToLibsMutex,                 &pthread_mutexattr_default);
  pthread_mutex_init(&_pathToTexturesMutex,             &pthread_mutexattr_default);
  pthread_mutex_init(&_pausedMutex,                     &pthread_mutexattr_default);
  pthread_mutex_init(&_realtimeFactorMutex,             &pthread_mutexattr_default);
  pthread_mutex_init(&_resetMutex,                      &pthread_mutexattr_default);
  pthread_mutex_init(&_skyColourMutex,                   &pthread_mutexattr_default);
  pthread_mutex_init(&_syncGuiMutex,                    &pthread_mutexattr_default);
  pthread_mutex_init(&_texturesMutex,                   &pthread_mutexattr_default);
  pthread_mutex_init(&_useAxesVisualisationMutex,       &pthread_mutexattr_default);
  pthread_mutex_init(&_useCaptureMutex,                 &pthread_mutexattr_default);
  pthread_mutex_init(&_useFollowModeMutex,              &pthread_mutexattr_default);
  pthread_mutex_init(&_useOsdMutex,                     &pthread_mutexattr_default);
  pthread_mutex_init(&_usePauseMutex,                   &pthread_mutexattr_default);
  pthread_mutex_init(&_usePrintFramesPerSecondMutex,    &pthread_mutexattr_default);
  pthread_mutex_init(&_usePrintTimeInformationMutex,    &pthread_mutexattr_default);
  pthread_mutex_init(&_useRealTimeModeMutex,            &pthread_mutexattr_default);
  pthread_mutex_init(&_useResetMutex,                   &pthread_mutexattr_default);
  pthread_mutex_init(&_useSingleStepMutex,              &pthread_mutexattr_default);
  pthread_mutex_init(&_useTexturesMutex,                &pthread_mutexattr_default);
  pthread_mutex_init(&_useTracesMutex,                  &pthread_mutexattr_default);
  pthread_mutex_init(&_useVisualisationMutex,           &pthread_mutexattr_default);
  pthread_mutex_init(&_xmlMutex,                        &pthread_mutexattr_default);
  pthread_mutex_init(&_silentMutex,                     &pthread_mutexattr_default);
  pthread_mutex_init(&_randomSeedMutex,                 &pthread_mutexattr_default);
  pthread_mutex_init(&_windowPosition,                  &pthread_mutexattr_default);
#endif // USE_THREAD_SAFE
};

ConfigurationContainer::~ConfigurationContainer()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_destroy(&_defaultControllerFrequencyMutex);
  pthread_mutex_destroy(&_defaultSimulatorFrequencyMutex);
  pthread_mutex_destroy(&_usePrintKeyboardShortcutsMutex);
  pthread_mutex_destroy(&_cameraOrientationMutex);
  pthread_mutex_destroy(&_cameraPositionMutex);
  pthread_mutex_destroy(&_captureDirectoryMutex);
  pthread_mutex_destroy(&_captureFrameRateMutex);
  pthread_mutex_destroy(&_captureNameMutex);
  pthread_mutex_destroy(&_comStatusMutex);
  pthread_mutex_destroy(&_controllerLocationsMutex);
  pthread_mutex_destroy(&_currentRealTimeMutex);
  pthread_mutex_destroy(&_currentTimeStepMutex);
  pthread_mutex_destroy(&_continousTimeStepMutex);
  pthread_mutex_destroy(&_exitMutex);
  pthread_mutex_destroy(&_followModeMutex);
  pthread_mutex_destroy(&_framesDirectoryMutex);
  pthread_mutex_destroy(&_materialsDirectoryMutex);
  pthread_mutex_destroy(&_groundColourMutex);
  pthread_mutex_destroy(&_librariesMutex);
  pthread_mutex_destroy(&_logDirectoryMutex);
  pthread_mutex_destroy(&_pathToLibsMutex);
  pthread_mutex_destroy(&_pathToTexturesMutex);
  pthread_mutex_destroy(&_pausedMutex);
  pthread_mutex_destroy(&_realtimeFactorMutex);
  pthread_mutex_destroy(&_resetMutex);
  pthread_mutex_destroy(&_skyColourMutex);
  pthread_mutex_destroy(&_syncGuiMutex);
  pthread_mutex_destroy(&_texturesMutex);
  pthread_mutex_destroy(&_useAxesVisualisationMutex);
  pthread_mutex_destroy(&_useCaptureMutex);
  pthread_mutex_destroy(&_useFollowModeMutex);
  pthread_mutex_destroy(&_useOsdMutex);
  pthread_mutex_destroy(&_usePauseMutex);
  pthread_mutex_destroy(&_usePrintFramesPerSecondMutex);
  pthread_mutex_destroy(&_usePrintTimeInformationMutex);
  pthread_mutex_destroy(&_useRealTimeModeMutex);
  pthread_mutex_destroy(&_useReloadOnEveryResetMutex);
  pthread_mutex_destroy(&_useResetMutex);
  pthread_mutex_destroy(&_useSingleStepMutex);
  pthread_mutex_destroy(&_useTexturesMutex);
  pthread_mutex_destroy(&_useTracesMutex);
  pthread_mutex_destroy(&_useVisualisationMutex);
  pthread_mutex_destroy(&_xmlMutex);
  pthread_mutex_destroy(&_silentMutex);
  pthread_mutex_destroy(&_randomSeedMutex);
#endif // USE_THREAD_SAFE
}


unsigned long ConfigurationContainer::getCurrentTimeStep()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_currentTimeStepMutex);
#endif // USE_THREAD_SAFE
  unsigned long r = _currentTimeStep;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_currentTimeStepMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ConfigurationContainer::setCurrentTimeStep(unsigned long currentTimeStep)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_currentTimeStepMutex);
#endif // USE_THREAD_SAFE
  _currentTimeStep = currentTimeStep;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_currentTimeStepMutex);
#endif // USE_THREAD_SAFE
}

unsigned long ConfigurationContainer::getContinousTimeStep()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_continousTimeStepMutex);
#endif // USE_THREAD_SAFE
  unsigned long r = _continousTimeStep;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_continousTimeStepMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ConfigurationContainer::setContinousTimeStep(unsigned long continousTimeStep)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_continousTimeStepMutex);
#endif // USE_THREAD_SAFE
  _continousTimeStep = continousTimeStep;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_continousTimeStepMutex);
#endif // USE_THREAD_SAFE
}

double ConfigurationContainer::getCurrentRealTime()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_currentRealTimeMutex);
#endif // USE_THREAD_SAFE
  double r = _currentRealTime;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_currentRealTimeMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ConfigurationContainer::setCurrentRealTime(double currentRealTime)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_currentRealTimeMutex);
#endif // USE_THREAD_SAFE
  _currentRealTime = currentRealTime;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_currentRealTimeMutex);
#endif // USE_THREAD_SAFE
}


bool ConfigurationContainer::getUsePause()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_usePauseMutex);
#endif // USE_THREAD_SAFE
  bool r = _usePause;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_usePauseMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ConfigurationContainer::setUsePause(bool usePause)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_usePauseMutex);
#endif // USE_THREAD_SAFE
  _usePause = usePause;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_usePauseMutex);
#endif // USE_THREAD_SAFE
}

bool ConfigurationContainer::getUseOsd()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_useOsdMutex);
#endif // USE_THREAD_SAFE
  bool r = _useOsd;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_useOsdMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ConfigurationContainer::setUseOsd(bool useOsd)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_useOsdMutex);
#endif // USE_THREAD_SAFE
  _useOsd = useOsd;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_useOsdMutex);
#endif // USE_THREAD_SAFE
}

bool ConfigurationContainer::getUseRealTimeMode()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_useRealTimeModeMutex);
#endif // USE_THREAD_SAFE
  bool r = _useRealTimeMode;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_useRealTimeModeMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ConfigurationContainer::setUseRealTimeMode(bool useRealTimeMode)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_useRealTimeModeMutex);
#endif // USE_THREAD_SAFE
  _useRealTimeMode = useRealTimeMode;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_useRealTimeModeMutex);
#endif // USE_THREAD_SAFE
}


bool ConfigurationContainer::getUseTextures()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_useTexturesMutex);
#endif // USE_THREAD_SAFE
  bool r = _useTextures;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_useTexturesMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ConfigurationContainer::setUseTextures(bool useTextures)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_useTexturesMutex);
#endif // USE_THREAD_SAFE
  _useTextures = useTextures;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_useTexturesMutex);
#endif // USE_THREAD_SAFE
}


bool ConfigurationContainer::getUseAxesVisualisation()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_useAxesVisualisationMutex);
#endif // USE_THREAD_SAFE
  bool r = _useAxesVisualisation;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_useAxesVisualisationMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ConfigurationContainer::setUseAxesVisualisation(bool useAxesVisualisation)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_useAxesVisualisationMutex);
#endif // USE_THREAD_SAFE
  _useAxesVisualisation = useAxesVisualisation;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_useAxesVisualisationMutex);
#endif // USE_THREAD_SAFE
}

bool ConfigurationContainer::getUsePrintFramesPerSecond()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_usePrintFramesPerSecondMutex);
#endif // USE_THREAD_SAFE
  bool r = _usePrintFramesPerSecond;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_usePrintFramesPerSecondMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ConfigurationContainer::setUsePrintFramesPerSecond(bool usePrintFramesPerSecond)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_usePrintFramesPerSecondMutex);
#endif // USE_THREAD_SAFE
  _usePrintFramesPerSecond = usePrintFramesPerSecond;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_usePrintFramesPerSecondMutex);
#endif // USE_THREAD_SAFE
}


bool ConfigurationContainer::getUsePrintTimeInformation()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_usePrintTimeInformationMutex);
#endif // USE_THREAD_SAFE
  bool r = _usePrintTimeInformation;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_usePrintTimeInformationMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ConfigurationContainer::setUsePrintTimeInformation(bool usePrintTimeInformation)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_usePrintTimeInformationMutex);
#endif // USE_THREAD_SAFE
  _usePrintTimeInformation = usePrintTimeInformation;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_usePrintTimeInformationMutex);
#endif // USE_THREAD_SAFE
}

bool ConfigurationContainer::getUseVisualisation()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_useVisualisationMutex);
#endif // USE_THREAD_SAFE
  bool r = _useVisualisation;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_useVisualisationMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ConfigurationContainer::setUseVisualisation(bool useVisualisation)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_useVisualisationMutex);
#endif // USE_THREAD_SAFE
  _useVisualisation = useVisualisation;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_useVisualisationMutex);
#endif // USE_THREAD_SAFE
}

bool ConfigurationContainer::getUsePrintConfiguration()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_usePrintConfigurationMutex);
#endif // USE_THREAD_SAFE
  bool r = _usePrintConfiguration;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_usePrintConfigurationMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ConfigurationContainer::setUsePrintConfiguration(bool usePrintConfiguration)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_usePrintConfigurationMutex);
#endif // USE_THREAD_SAFE
  _usePrintConfiguration = usePrintConfiguration;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_usePrintConfigurationMutex);
#endif // USE_THREAD_SAFE
}

bool ConfigurationContainer::getUseTraces()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_useTracesMutex);
#endif // USE_THREAD_SAFE
  bool r = _useTraces;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_useTracesMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ConfigurationContainer::setUseTraces(bool useTraces)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_useTracesMutex);
#endif // USE_THREAD_SAFE
  _useTraces = useTraces;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_useTracesMutex);
#endif // USE_THREAD_SAFE
}



bool ConfigurationContainer::getUseReset()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_useResetMutex);
#endif // USE_THREAD_SAFE
  bool r = _useReset;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_useResetMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ConfigurationContainer::setUseReset(bool useReset)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_useResetMutex);
#endif // USE_THREAD_SAFE
  _useReset = useReset;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_useResetMutex);
#endif // USE_THREAD_SAFE
}


bool ConfigurationContainer::getUseSingleStep()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_useSingleStepMutex);
#endif // USE_THREAD_SAFE
  bool r = _useSingleStep;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_useSingleStepMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ConfigurationContainer::setUseSingleStep(bool useSingleStep)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_useSingleStepMutex);
#endif // USE_THREAD_SAFE
  _useSingleStep = useSingleStep;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_useSingleStepMutex);
#endif // USE_THREAD_SAFE
}


bool ConfigurationContainer::getUseFollowMode()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_useFollowModeMutex);
#endif // USE_THREAD_SAFE
  bool r = _useFollowMode;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_useFollowModeMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ConfigurationContainer::setUseFollowMode(bool useFollowMode)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_useFollowModeMutex);
#endif // USE_THREAD_SAFE
  _useFollowMode = useFollowMode;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_useFollowModeMutex);
#endif // USE_THREAD_SAFE
}


bool ConfigurationContainer::getExit()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_exitMutex);
#endif // USE_THREAD_SAFE
  bool r = _exit;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_exitMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ConfigurationContainer::setExit(bool exit)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_exitMutex);
#endif // USE_THREAD_SAFE
  _exit = exit;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_exitMutex);
#endif // USE_THREAD_SAFE
}

bool ConfigurationContainer::getUseCapture()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_useCaptureMutex);
#endif // USE_THREAD_SAFE
  bool r = _useCapture;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_useCaptureMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ConfigurationContainer::setUseCapture(bool useCapture)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_useCaptureMutex);
#endif // USE_THREAD_SAFE
  _useCapture = useCapture;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_useCaptureMutex);
#endif // USE_THREAD_SAFE
}

bool ConfigurationContainer::getUseSyncGui()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_syncGuiMutex);
#endif // USE_THREAD_SAFE
  bool r = _syncGui;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_syncGuiMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ConfigurationContainer::setUseSyncGui(bool syncGui)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_syncGuiMutex);
#endif // USE_THREAD_SAFE
  _syncGui = syncGui;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_syncGuiMutex);
#endif // USE_THREAD_SAFE
}

int ConfigurationContainer::getFollowMode()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_followModeMutex);
#endif // USE_THREAD_SAFE
  int r = _followMode;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_followModeMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ConfigurationContainer::setFollowMode(int followMode)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_followModeMutex);
#endif // USE_THREAD_SAFE
  _followMode = followMode;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_followModeMutex);
#endif // USE_THREAD_SAFE
}

int ConfigurationContainer::getReset()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_resetMutex);
#endif // USE_THREAD_SAFE
  int r = _reset;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_resetMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ConfigurationContainer::setReset(int reset)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_resetMutex);
#endif // USE_THREAD_SAFE
  _reset = reset;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_resetMutex);
#endif // USE_THREAD_SAFE
}


string ConfigurationContainer::getApplicationDirectory()
{
  string returnvalue;
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_yarsMutex);
#endif // USE_THREAD_SAFE
  returnvalue = _yars;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_yarsMutex);
#endif // USE_THREAD_SAFE
  return returnvalue;
}

void ConfigurationContainer::setApplicationDirectory(string yars)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_yarsMutex);
#endif // USE_THREAD_SAFE
  _yars = yars;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_yarsMutex);
#endif // USE_THREAD_SAFE
}

string ConfigurationContainer::getXml()
{
  return _xml;
}

void ConfigurationContainer::setXml(string xml)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_xmlMutex);
#endif // USE_THREAD_SAFE
  _xml = xml;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_xmlMutex);
#endif // USE_THREAD_SAFE
}

void ConfigurationContainer::getControllerLocations(std::vector<string> *controllers)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_controllerLocationsMutex);
#endif // USE_THREAD_SAFE
  (*controllers) = _controllerLocations;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_controllerLocationsMutex);
#endif // USE_THREAD_SAFE
}

void ConfigurationContainer::setControllerLoctaions(std::vector<string> controllers)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_controllerLocationsMutex);
#endif // USE_THREAD_SAFE
  _controllerLocations = controllers;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_controllerLocationsMutex);
#endif // USE_THREAD_SAFE
}


string ConfigurationContainer::getTextures()
{
  string returnvalue;
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_texturesMutex);
#endif // USE_THREAD_SAFE
  returnvalue = _textures;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_texturesMutex);
#endif // USE_THREAD_SAFE
  return returnvalue;
}

void ConfigurationContainer::setTextures(string textures)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_texturesMutex);
#endif // USE_THREAD_SAFE
  _textures = textures;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_texturesMutex);
#endif // USE_THREAD_SAFE
}


string ConfigurationContainer::getLibraries()
{
  string returnvalues;
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_librariesMutex);
#endif // USE_THREAD_SAFE
  returnvalues = _libraries;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_librariesMutex);
#endif // USE_THREAD_SAFE
  return returnvalues;
}

void ConfigurationContainer::setLibraries(string libraries)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_librariesMutex);
#endif // USE_THREAD_SAFE
  _libraries = libraries;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_librariesMutex);
#endif // USE_THREAD_SAFE
}

string ConfigurationContainer::getCaptureName()
{
  string returnvalue;
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_captureNameMutex);
#endif // USE_THREAD_SAFE
  returnvalue =  _captureName;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_captureNameMutex);
#endif // USE_THREAD_SAFE
  return returnvalue;
}

void ConfigurationContainer::setCaptureName(string captureName)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_captureNameMutex);
#endif // USE_THREAD_SAFE
  _captureName = captureName;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_captureNameMutex);
#endif // USE_THREAD_SAFE
}


int ConfigurationContainer::getCaptureFrameRate()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_captureFrameRateMutex);
#endif // USE_THREAD_SAFE
  int r = _captureFrameRate;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_captureFrameRateMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ConfigurationContainer::setCaptureFrameRate(int captureFrameRate)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_captureFrameRateMutex);
#endif // USE_THREAD_SAFE
  _captureFrameRate = captureFrameRate;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_captureFrameRateMutex);
#endif // USE_THREAD_SAFE
}

void ConfigurationContainer::getCameraPosition(P3D *p)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_cameraPositionMutex);
#endif // USE_THREAD_SAFE
  p->x = _cameraPosition.x;
  p->y = _cameraPosition.y;
  p->z = _cameraPosition.z;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_cameraPositionMutex);
#endif // USE_THREAD_SAFE
}

void ConfigurationContainer::setCameraPosition(P3D cameraPosition)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_cameraPositionMutex);
#endif // USE_THREAD_SAFE
  _cameraPosition.x = cameraPosition.x;
  _cameraPosition.y = cameraPosition.y;
  _cameraPosition.z = cameraPosition.z;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_cameraPositionMutex);
#endif // USE_THREAD_SAFE
}


void ConfigurationContainer::getCameraOrientation(P3D *o)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_cameraOrientationMutex);
#endif // USE_THREAD_SAFE
  o->x = _cameraOrientation.x;
  o->y = _cameraOrientation.y;
  o->z = _cameraOrientation.z;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_cameraOrientationMutex);
#endif // USE_THREAD_SAFE
}

void ConfigurationContainer::setCameraOrientation(P3D cameraOrientation)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_cameraOrientationMutex);
#endif // USE_THREAD_SAFE
  _cameraOrientation.x = cameraOrientation.x;
  _cameraOrientation.y = cameraOrientation.y;
  _cameraOrientation.z = cameraOrientation.z;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_cameraOrientationMutex);
#endif // USE_THREAD_SAFE
}

void ConfigurationContainer::setPathToLibs(string pathToLibs)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_pathToLibsMutex);
#endif // USE_THREAD_SAFE
  _pathToLibs = pathToLibs;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_pathToLibsMutex);
#endif // USE_THREAD_SAFE
}

string ConfigurationContainer::getPathToLibs()
{
  string returnvalue;
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_pathToLibsMutex);
#endif // USE_THREAD_SAFE
  returnvalue = _pathToLibs;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_pathToLibsMutex);
#endif // USE_THREAD_SAFE
  return returnvalue;
}



void ConfigurationContainer::setPathToTextures(string pathToTextures)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_pathToTexturesMutex);
#endif // USE_THREAD_SAFE
  _pathToTextures = pathToTextures;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_pathToTexturesMutex);
#endif // USE_THREAD_SAFE
}

string ConfigurationContainer::getPathToTextures()
{
  string returnvalue;
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_pathToTexturesMutex);
#endif // USE_THREAD_SAFE
  returnvalue = _pathToTextures;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_pathToTexturesMutex);
#endif // USE_THREAD_SAFE
  return returnvalue;
}


void ConfigurationContainer::setPaused(bool paused)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_pausedMutex);
#endif // USE_THREAD_SAFE
  _paused = paused;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_pausedMutex);
#endif // USE_THREAD_SAFE
}

void ConfigurationContainer::getPaused(bool *paused)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_pausedMutex);
#endif // USE_THREAD_SAFE
  *paused = _paused;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_pausedMutex);
#endif // USE_THREAD_SAFE
}

void ConfigurationContainer::setExportGraphviz(bool exportGraphviz)
{
  _exportGraphviz = exportGraphviz;
}

bool ConfigurationContainer::getExportGraphviz()
{
  return _exportGraphviz;
}

void ConfigurationContainer::setExportXSD(bool exportXSD)
{
  _exportXSD = exportXSD;
}

bool ConfigurationContainer::getExportXSD()
{
  return _exportXSD;
}


void ConfigurationContainer::setGroundColour(Colour groundColour)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_groundColourMutex);
#endif // USE_THREAD_SAFE
  _groundColour = groundColour;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_groundColourMutex);
#endif // USE_THREAD_SAFE
}

void ConfigurationContainer::getGroundColour(Colour *groundColour)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_groundColourMutex);
#endif // USE_THREAD_SAFE
  *groundColour = _groundColour;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_groundColourMutex);
#endif // USE_THREAD_SAFE
}


void ConfigurationContainer::setSkyColour(Colour skyColour)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_skyColourMutex);
#endif // USE_THREAD_SAFE
  _skyColour = skyColour;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_skyColourMutex);
#endif // USE_THREAD_SAFE
}

void ConfigurationContainer::getSkyColour(Colour *skyColour)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_skyColourMutex);
#endif // USE_THREAD_SAFE
  *skyColour = _skyColour;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_skyColourMutex);
#endif // USE_THREAD_SAFE
}

string ConfigurationContainer::getCaptureDirectory()
{
  string returnvalue;
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_captureDirectoryMutex);
#endif // USE_THREAD_SAFE
  returnvalue = _captureDirectory;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_captureDirectoryMutex);
#endif // USE_THREAD_SAFE
  return returnvalue;
}

void ConfigurationContainer::setCaptureDirectory(string captureDirectory)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_captureDirectoryMutex);
#endif // USE_THREAD_SAFE
  _captureDirectory = captureDirectory;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_captureDirectoryMutex);
#endif // USE_THREAD_SAFE
}

void ConfigurationContainer::setFramesDirectory(string framesDirectory)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_framesDirectoryMutex);
#endif // USE_THREAD_SAFE
  _framesDirectory = framesDirectory;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_framesDirectoryMutex);
#endif // USE_THREAD_SAFE
}

string ConfigurationContainer::getFramesDirectory()
{
  string returnvalue;
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_framesDirectoryMutex);
#endif // USE_THREAD_SAFE
  returnvalue = _framesDirectory;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_framesDirectoryMutex);
#endif // USE_THREAD_SAFE
  return returnvalue;
}

void ConfigurationContainer::setRealTimeFactor(double realtimeFactor)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_realtimeFactorMutex);
#endif // USE_THREAD_SAFE
  _realtimeFactor = realtimeFactor;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_realtimeFactorMutex);
#endif // USE_THREAD_SAFE
}

double ConfigurationContainer::getRealTimeFactor()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_realtimeFactorMutex);
#endif // USE_THREAD_SAFE
  double d = _realtimeFactor;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_realtimeFactorMutex);
#endif // USE_THREAD_SAFE
  return d;
}

void ConfigurationContainer::setLogDirectory(string logDirectory)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_logDirectoryMutex);
#endif // USE_THREAD_SAFE
  _logDirectory = logDirectory;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_logDirectoryMutex);
#endif // USE_THREAD_SAFE
}

string ConfigurationContainer::getLogDirectory()
{
  string returnvalue;
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_logDirectoryMutex);
#endif // USE_THREAD_SAFE
  returnvalue = _logDirectory;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_logDirectoryMutex);
#endif // USE_THREAD_SAFE
  return returnvalue;
}

void ConfigurationContainer::setDefaultSimulatorFrequency(int defaultSimulatorFrequency)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_defaultSimulatorFrequencyMutex);
#endif // USE_THREAD_SAFE
  _defaultSimulatorFrequency = defaultSimulatorFrequency;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_defaultSimulatorFrequencyMutex);
#endif // USE_THREAD_SAFE
}

int ConfigurationContainer::getDefaultSimulatorFrequency()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_defaultSimulatorFrequencyMutex);
#endif // USE_THREAD_SAFE
  int d = _defaultSimulatorFrequency;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_defaultSimulatorFrequencyMutex);
#endif // USE_THREAD_SAFE
  return d;
}

void ConfigurationContainer::setDefaultControllerFrequency(int defaultControllerFrequency)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_defaultControllerFrequencyMutex);
#endif // USE_THREAD_SAFE
  _defaultControllerFrequency = defaultControllerFrequency;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_defaultControllerFrequencyMutex);
#endif // USE_THREAD_SAFE
}

int ConfigurationContainer::getDefaultControllerFrequency()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_defaultControllerFrequencyMutex);
#endif // USE_THREAD_SAFE
  int d = _defaultControllerFrequency;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_defaultControllerFrequencyMutex);
#endif // USE_THREAD_SAFE
  return d;
}

bool ConfigurationContainer::getUsePrintKeyboardShortcuts()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_usePrintKeyboardShortcutsMutex);
#endif // USE_THREAD_SAFE
  bool r = _usePrintKeyboardShortcuts;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_usePrintKeyboardShortcutsMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ConfigurationContainer::setUsePrintKeyboardShortcuts(bool printKeyboardShortcuts)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_usePrintKeyboardShortcutsMutex);
#endif // USE_THREAD_SAFE
  _usePrintKeyboardShortcuts = printKeyboardShortcuts;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_usePrintKeyboardShortcutsMutex);
#endif // USE_THREAD_SAFE
}

string ConfigurationContainer::getVideoCodec()
{
  string returnvalue;
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_videoCodecMutex);
#endif // USE_THREAD_SAFE
  returnvalue = _videoCodec;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_videoCodecMutex);
#endif // USE_THREAD_SAFE
  return returnvalue;
}

void ConfigurationContainer::setVideoCodec(string videoCodec)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_videoCodecMutex);
#endif // USE_THREAD_SAFE
  _videoCodec = videoCodec;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_videoCodecMutex);
#endif // USE_THREAD_SAFE
}

bool ConfigurationContainer::getUseCR()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_useCRMutex);
#endif // USE_THREAD_SAFE
  bool r = _useCR;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_useCRMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ConfigurationContainer::setUseCR(bool useCR)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_useCRMutex);
#endif // USE_THREAD_SAFE
  _useCR = useCR;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_useCRMutex);
#endif // USE_THREAD_SAFE
}

void ConfigurationContainer::setMaterialsDirectory(string materialsDirectory)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_materialsDirectoryMutex);
#endif // USE_THREAD_SAFE
  _materialsDirectory = materialsDirectory;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_materialsDirectoryMutex);
#endif // USE_THREAD_SAFE
}

string ConfigurationContainer::getMaterialsDirectory()
{
  string returnvalue;
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_materialsDirectoryMutex);
#endif // USE_THREAD_SAFE
  returnvalue = _materialsDirectory;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_materialsDirectoryMutex);
#endif // USE_THREAD_SAFE
  return returnvalue;
}

bool ConfigurationContainer::getSilent()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_silentMutex);
#endif // USE_THREAD_SAFE
  bool r = _silent;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_silentMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ConfigurationContainer::setSilent(bool silent)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_silentMutex);
#endif // USE_THREAD_SAFE
  _silent = silent;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_silentMutex);
#endif // USE_THREAD_SAFE
}

void ConfigurationContainer::setRandomSeed(int randomSeed)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_randomSeedMutex);
#endif // USE_THREAD_SAFE
  _randomSeed    = randomSeed;
  _useRandomSeed = true;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_randomSeedMutex);
#endif // USE_THREAD_SAFE
}

int ConfigurationContainer::getRandomSeed()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_randomSeedMutex);
#endif // USE_THREAD_SAFE
  int r = _randomSeed;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_randomSeedMutex);
#endif // USE_THREAD_SAFE
  return r;
}

bool ConfigurationContainer::useRandomSeed()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_randomSeedMutex);
#endif // USE_THREAD_SAFE
  bool r = _useRandomSeed;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_randomSeedMutex);
#endif // USE_THREAD_SAFE
  return r;
}

void ConfigurationContainer::setWindowGeometry(int w, int h, int x, int y)
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_windowPosition);
#endif // USE_THREAD_SAFE
  _useWindowGeometry = true;
  _winPositionX      = x;
  _winPositionY      = y;
  _winHeight         = h;
  _winWidth          = w;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_windowPosition);
#endif // USE_THREAD_SAFE
}

bool ConfigurationContainer::getUseWindowGeometry()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_windowPosition);
#endif // USE_THREAD_SAFE
  bool r = _useWindowGeometry;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_windowPosition);
#endif // USE_THREAD_SAFE
  return r;
}

int  ConfigurationContainer::getWindowPositionX()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_windowPosition);
#endif // USE_THREAD_SAFE
  int r = _winPositionX;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_windowPosition);
#endif // USE_THREAD_SAFE
  return r;
}

int  ConfigurationContainer::getWindowPositionY()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_windowPosition);
#endif // USE_THREAD_SAFE
  int r = _winPositionY;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_windowPosition);
#endif // USE_THREAD_SAFE
  return r;
}

int  ConfigurationContainer::getWindowHeight()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_windowPosition);
#endif // USE_THREAD_SAFE
  int r = _winHeight;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_windowPosition);
#endif // USE_THREAD_SAFE
  return r;
}

int  ConfigurationContainer::getWindowWidth()
{
#ifdef USE_THREAD_SAFE
  pthread_mutex_lock(&_windowPosition);
#endif // USE_THREAD_SAFE
  int r = _winWidth;
#ifdef USE_THREAD_SAFE
  pthread_mutex_unlock(&_windowPosition);
#endif // USE_THREAD_SAFE
  return r;
}
