#include <yars/configuration/ProgramOptions.h>

#include <yars/view/console/ConsoleView.h>
#include <yars/util/YarsErrorHandler.h>
#include <yars/util/Directories.h>

#include <yars/defines/program_options.h>
#include <yars/defines/keyboard_shortcuts.h>

#include <yars/configuration/data/XmlChangeLog.h>

#include <fstream>

using namespace std;

ProgramOptions::ProgramOptions(int argc, char **argv,
    ConfigurationContainer *cc,
    KeyboardShortcuts *keyboardShortcuts,
    ProgramOptionsConfiguration *programOptions)
  : _app("YARS - Yet Another Robot Simulator")
{
  _cc                = cc;
  _configuration     = programOptions;
  _keyboardShortcuts = keyboardShortcuts;

  // Allow config file
  _app.set_config("--config", _configuration->configFile, __PO_OPTION_CONFIG_DESCRIPTION);

  // Generic options
  // Note: CLI11 already provides -h and --help by default
  _app.set_help_flag(); // Disable default help
  _app.add_flag("--help,-h", _helpRequested, __PO_OPTION_HELP_DESCRIPTION);
  _app.add_flag("--version", _versionRequested, __PO_OPTION_VERSION_DESCRIPTION);
  _app.add_flag("--license", _licenseRequested, __PO_OPTION_LICENSE_DESCRIPTION);

  // Debug options
  _app.add_option("--xml,-x", _configuration->xml, __PO_OPTION_XML_DESCRIPTION);
  _app.add_option("--debug", _configuration->debug, __PO_OPTION_DEBUG_DESCRIPTION)
      ->capture_default_str();
  _app.add_flag("--silent", _silentSet, __PO_OPTION_DEBUG_LEVEL_SILENT_DESCRIPTION);
  _app.add_flag("--cr", _useCRSet, __PO_OPTION_USE_CR_DESCRIPTION);
  _app.add_flag("--printTime", _printTimeSet, __PO_OPTION_PRINT_TIME_DESCRIPTION);
  _app.add_flag("--printRobotConfiguration", _printRobotConfigSet, __PO_OPTION_PRINT_ROBOT_CONFIGURATION_DESCRIPTION);
  _app.add_flag("--printConfig", _printConfigSet, __PO_OPTION_PRINT_CONFIGURATION_DESCRIPTION);
  _app.add_flag("--printKeyboardShortcuts", _printKeyboardSet, __PO_OPTION_PRINT_KEYBOARD_SHORTCUTS_DESCRIPTION);
  _app.add_option("--list", _configuration->listCommand, __PO_OPTION_LIST_DESCRIPTION);
  _app.add_option("--export", _configuration->exportCommand, __PO_OPTION_EXPORT_DESCRIPTION);
  _app.add_option("--log", _configuration->logDirectory, __PO_OPTION_LOGGING_DESCRIPTION);

  // Video options
  _app.add_option("--captureName", _configuration->captureName, __PO_OPTION_CAPTURE_NAME_DESCRIPTION);
  _app.add_option("--captureFrameRate", _configuration->captureFrameRate, __PO_OPTION_CAPTURE_FRAME_RATE_DESCRIPTION);
  _app.add_option("--captureDirectory", _configuration->captureDirectory, __PO_OPTION_CAPTURE_DIRECTORY_DESCRIPTION);
  _app.add_flag("--capture", _captureSet, __PO_OPTION_CAPTURE_DESCRIPTION);
  _app.add_option("--vcodec", _configuration->videoCodec, __PO_OPTION_VIDEO_CODEC_DESCRIPTION);
  _app.add_option("--framesDirectory", _configuration->framesDirectory, __PO_OPTION_FRAMES_DIRECTORY_DESCRIPTION);

  // Control options
  _app.add_option("--iterations", _configuration->maxIterations, __PO_OPTION_MAX_ITERATIONS_DESCRIPTION);
  _app.add_option("--writeConfig", _configuration->outConfigFile, __PO_OPTION_WRITE_CONFIG_DESCRIPTION);
  _app.add_option("--simfreq", _configuration->simulationFrequency, __PO_OPTION_SIMULATION_FREQUENCY_DESCRIPTION);
  _app.add_option("--seed", _configuration->randomSeed, __PO_OPTION_RANDOM_SEED_DESCRIPTION);
  _app.add_option("--ctrlfreq", _configuration->controllerUpdateFrequency, __PO_OPTION_CONTROL_FREQUENCY_DESCRIPTION);
  _app.add_option("--realtime", _configuration->realtimeFactor, __PO_OPTION_REALTIME_DESCRIPTION)
      ->default_val(1.0);

  // GUI options
  _app.add_flag("--texture", _textureSet, __PO_OPTION_TEXTURE_DESCRIPTION);
  // Note: --synched flag is currently not used (synchronous mode is default)
  _app.add_flag("--nogui", _guiSet, __PO_OPTION_GUI_DESCRIPTION);
  _app.add_flag("--noctrl", _controllerSet, __PO_OPTION_CONTROLLER_DESCRIPTION);
  _app.add_flag("--trace", _traceSet, __PO_OPTION_TRACE_DESCRIPTION);
  _app.add_option("--follow", _configuration->followMode, __PO_OPTION_FOLLOW_DESCRIPTION)
      ->default_val(0);
  _app.add_flag("--osd", _osdSet, __PO_OPTION_ON_SCREEN_DISPLAY_DESCRIPTION);
  _app.add_option("--geometry", _configuration->geometryString, __PO_OPTION_GEOMETRY_DESCRIPTION);
  _app.add_flag("--pause", _pauseSet, __PO_OPTION_PAUSE_DESCRIPTION);

  // Path options
  _app.add_option("--textures", _configuration->textures, __PO_OPTION_TEXTURES_DESCRIPTION);
  _app.add_option("--lib", _configuration->lib, __PO_OPTION_LIB_DESCRIPTION);

  // Keyboard shortcuts (from config file)
  _app.add_option("--captureVideoKey", keyboardShortcuts->captureVideo.keyCode);
  _app.add_option("--quitKey", keyboardShortcuts->quit.keyCode);
  _app.add_option("--resetKey", keyboardShortcuts->reset.keyCode);
  _app.add_option("--pauseKey", keyboardShortcuts->pause.keyCode);
  _app.add_option("--realtimeKey", keyboardShortcuts->realtime.keyCode);
  _app.add_option("--singleStepKey", keyboardShortcuts->singleStep.keyCode);
  _app.add_option("--restoreViewpointKey", keyboardShortcuts->restoreViewpoint.keyCode);
  _app.add_option("--printTimeKey", keyboardShortcuts->printTime.keyCode);
  _app.add_option("--resetSimSpeedKey", keyboardShortcuts->resetSimSpeed.keyCode);
  _app.add_option("--decreaseSimSpeedKey", keyboardShortcuts->decreaseSimSpeed.keyCode);
  _app.add_option("--increaseSimSpeedKey", keyboardShortcuts->increaseSimSpeed.keyCode);
  _app.add_option("--printKeyboardShortcutsKey", keyboardShortcuts->printKeyboardShortcuts.keyCode);
  _app.add_option("--toggleReloadOnResetKey", keyboardShortcuts->toggleReloadOnReset.keyCode);
  _app.add_option("--writeFramesKey", keyboardShortcuts->writeFrames.keyCode);
  _app.add_option("--visualiseAxesKey", keyboardShortcuts->visualiseAxes.keyCode);
  _app.add_option("--openNewWindowKey", keyboardShortcuts->openNewWindow.keyCode);
  _app.add_option("--setWindowTitleKey", keyboardShortcuts->setWindowTitle.keyCode);
  _app.add_option("--showWindowConfigurationDialogKey", keyboardShortcuts->showWindowConfigurationDialog.keyCode);
  _app.add_option("--setWindowSizeKey", keyboardShortcuts->setWindowSize.keyCode);
  _app.add_option("--osdFpsKey", keyboardShortcuts->onScreenDisplay_FramesPerSecond.keyCode);
  _app.add_option("--osdElapsedTimeKey", keyboardShortcuts->onScreenDisplay_ElapsedTime.keyCode);
  _app.add_option("--osdKey", keyboardShortcuts->onScreenDisplay.keyCode);
  _app.add_option("--toggleTexturesKey", keyboardShortcuts->toggleTextures.keyCode);
  _app.add_option("--printViewPointKey", keyboardShortcuts->printViewPoint.keyCode);
  _app.add_option("--toggleFollowModeKey", keyboardShortcuts->toggleFollowMode.keyCode);
  _app.add_option("--toggleTracesKey", keyboardShortcuts->toggleTraces.keyCode);
  _app.add_option("--previousFollowableKey", keyboardShortcuts->previousFollowable.keyCode);
  _app.add_option("--nextFollowableKey", keyboardShortcuts->nextFollowable.keyCode);
  _app.add_option("--previousFollowModeKey", keyboardShortcuts->previousFollowMode.keyCode);
  _app.add_option("--nextFollowModeKey", keyboardShortcuts->nextFollowMode.keyCode);

  // Positional argument for XML file
  _app.add_option("xmlfile", _configuration->xml, "XML simulation file");

  // Allow extras for unrecognized options (we'll report them)
  _app.allow_extras(true);

  try {
    _app.parse(argc, argv);
  } catch (const CLI::ParseError &e) {
    if (e.get_exit_code() != 0) {
      cout << _app.help() << endl;
      __exit(e.get_exit_code());
    }
  }

  // Check which options were set
  _debugSet = _app.count("--debug") > 0;
  _simFreqSet = _app.count("--simfreq") > 0;
  _ctrlFreqSet = _app.count("--ctrlfreq") > 0;
  _randomSeedSet = _app.count("--seed") > 0;
  _followSet = _app.count("--follow") > 0;
  _captureFrameRateSet = _app.count("--captureFrameRate") > 0;
  _captureNameSet = _app.count("--captureName") > 0;
  _captureDirSet = _app.count("--captureDirectory") > 0;
  _framesDirSet = _app.count("--framesDirectory") > 0;
  _realtimeSet = _app.count("--realtime") > 0;
  _geometrySet = _app.count("--geometry") > 0;
  _texturesPathSet = _app.count("--textures") > 0;
  _libPathSet = _app.count("--lib") > 0;
  _xmlSet = _app.count("--xml") > 0 || _app.count("xmlfile") > 0;
  _loggingSet = _app.count("--log") > 0;
  _videoCaptureSet = _app.count("--vcodec") > 0;

  __parseProgramOptionsParameters();

  _keyboardShortcuts->process();
}


void ProgramOptions::__parseProgramOptionsParameters()
{
  if (_helpRequested) {
    cout << _app.help() << endl;
    __exit(0);
  }
  if (_versionRequested) __version();
  if (_licenseRequested) __license();

  // Check for unknown options
  auto remaining = _app.remaining();
  if (!remaining.empty()) {
    cout << "Unknown options given:" << endl;
    for (const auto& opt : remaining) {
      cout << "  " << opt << endl;
    }
    cout << "Please use --help to check." << endl;
    __exit(0);
  }

  if (_debugSet)              __setDebugLevel();
  if (_silentSet)             __silent();

  if (_simFreqSet)            __simulationFrequency();
  if (_ctrlFreqSet)           __controlFrequency();
  if (_randomSeedSet)         __setRandomSeed();
  if (_followSet)             __follow();

#if USE_CAPTURE_VIDEO
  if (_videoCaptureSet)       __videoCodec();
  if (_captureFrameRateSet)   __captureFrameRate();
  if (_captureNameSet)        __captureName();
  if (_captureDirSet)         __captureDirectory();
  if (_framesDirSet)          __captureFramesDirectory();
  if (_captureSet)            __capture();
#endif // USE_CAPTURE_VIDEO

  if (_pauseSet)              __pause();
  if (_realtimeSet)           __realTime();
  if (_textureSet)            __texture();
  if (_guiSet)                __gui();
  if (_controllerSet)         __controller();
  if (_traceSet)              __trace();
  if (_printTimeSet)          __printTime();
  if (_osdSet)                __onScreenDisplay();
  if (_geometrySet)           __geometry();

  if (_printRobotConfigSet)   __printRobotConfiguration();
  if (_texturesPathSet)       __textures();
  if (_libPathSet)            __lib();
  if (_xmlSet)                __xml();
  if (_printConfigSet)        __printConfiguration();
  if (_printKeyboardSet)      __printKeyboardShortCuts();
  if (_loggingSet)            __logging();
  if (_useCRSet)              __useCR();

  if (!_xmlSet) // no xml given
  {
    _cc->setUseRealTimeMode(true);
  }
}



void ProgramOptions::__exit(int returnValue)
{
  exit(returnValue);
}

////////////////////////////////////////////////////////////////////////////////
// functions handling command line parameters
////////////////////////////////////////////////////////////////////////////////

void ProgramOptions::__version()
{
#ifndef _MSC_VER
  cout << "yars version "     << XmlChangeLog::version() << endl;
#else
  cout << "--version currently not supported on windows" << endl;
#endif
  __exit(0);
}

void ProgramOptions::__license()
{
  cout << "-------------------------------------------------------------" << endl;
  cout << "| Yet Another Robot Simulator (YARS).                       |" << endl;
  cout << "| Copyright (C) 2003-2015 Keyan Zahedi, Arndt von Twickel.  |" << endl;
  cout << "| All rights reserved.                                      |" << endl;
  cout << "| Email: keyan.zahedi@googlemail.com                        |" << endl;
  cout << "| Web: https://github.com/kzahedi/YARS                      |" << endl;
  cout << "| For a list of contributors see the file AUTHORS.          |" << endl;
  cout << "| YARS is distributed under the GNU General Public License, |" << endl;
  cout << "| either version 2 of the License, or (at your option) any  |" << endl;
  cout << "| later version. The license can be found in the file       |" << endl;
  cout << "| COPYING, if not write to the Free Software Foundation.    |" << endl;
  cout << "-------------------------------------------------------------" << endl;
  __exit(0);
}


void ProgramOptions::__simulationFrequency()
{
  _cc->setDefaultSimulatorFrequency(_configuration->simulationFrequency);
}

void ProgramOptions::__controlFrequency()
{
  _cc->setDefaultControllerFrequency(_configuration->controllerUpdateFrequency);
}

void ProgramOptions::__setRandomSeed()
{
  _cc->setRandomSeed(_configuration->randomSeed);
}

void ProgramOptions::__follow()
{
  _cc->setFollowMode(_configuration->followMode);
  _cc->setUseFollowMode(true);
}

void ProgramOptions::__debug()
{
  Y_SET_DEBUG_LEVEL(YARS_DEBUG);
  Y_DEBUG("log level set to DEBUG");
}

void ProgramOptions::__info()
{
  Y_SET_DEBUG_LEVEL(YARS_INFO);
  Y_INFO("log level set to INFO");
}

void ProgramOptions::__silent()
{
  Y_SET_DEBUG_LEVEL(YARS_FATAL);
}

void ProgramOptions::__setDebugLevel()
{
  if(_configuration->debug == __PO_OPTION_DEBUG_LEVEL_DEBUG)
  {
    __debug();
    return;
  };
  if(_configuration->debug == __PO_OPTION_DEBUG_LEVEL_INFO)
  {
    __info();
    return;
  };
  cout << __PO_OPTION_DEBUG_LEVEL_FAIL << _configuration->debug << endl;
  __exit(0);
}

void ProgramOptions::__capture()
{
  _cc->setUseCapture(true);
}

int ProgramOptions::__getX(string s)
{
  string t = "";
  int index  = 0;
  while(s[index] != '-' && s[index] != '+') index++;

  for(size_t i = index; i < s.length(); i++)
  {
    switch(s[i])
    {
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
        t = t + s[i];
        break;
      case '-':
      case '+':
        if(t.length() > 0)
        {
          return atoi(t.c_str());
        }
        else
        {
          t = t + s[i];
        }
        break;
      default:
        YarsErrorHandler::push("Unknown positions string: \"%s\".", s.c_str());
        break;
    }
  }

  if(t.length() == 0)
  {
    YarsErrorHandler::push("Unknown positions string: \"%s\".", s.c_str());
  }
  return atoi(t.c_str());
}

int ProgramOptions::__getY(string s)
{
  string t   = "";
  int index  = 0;
  while(s[index] != '-' && s[index] != '+') index++;
  index++;
  while(s[index] != '-' && s[index] != '+') index++;

  for(size_t i = index; i < s.length(); i++)
  {
    switch(s[i])
    {
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
      case '-':
      case '+':
        t = t + s[i];
        break;
      default:
        YarsErrorHandler::push("Unknown positions string: \"%s\".", s.c_str());
        break;
    }
  }

  if(t.length() == 0)
  {
    YarsErrorHandler::push("Unknown positions string: \"%s\".", s.c_str());
  }
  return atoi(t.c_str());
}

int ProgramOptions::__getW(string s)
{
  if(s[0] < 48 || s[0] > 57) return 0;
  string t   = "";

  for(size_t i = 0; i < s.length(); i++)
  {
    switch(s[i])
    {
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
        t = t + s[i];
        break;
      case '-':
      case '+':
      case 'x':
        return atoi(t.c_str());
        break;
      default:
        YarsErrorHandler::push("Unknown positions string: \"%s\".", s.c_str());
        break;
    }
  }

  if(t.length() == 0)
  {
    YarsErrorHandler::push("Unknown positions string: \"%s\".", s.c_str());
  }
  return atoi(t.c_str());
}

int ProgramOptions::__getH(string s)
{
  string t   = "";
  size_t index  = 0;
  while(s[index] != 'x') index++;
  index++;

  for(size_t i = index; i < s.length(); i++)
  {
    switch(s[i])
    {
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
        t = t + s[i];
        break;
      case '-':
      case '+':
        return atoi(t.c_str());
        break;
      default:
        YarsErrorHandler::push("Unknown positions string: \"%s\".", s.c_str());
        break;
    }
  }

  if(t.length() == 0)
  {
    YarsErrorHandler::push("Unknown positions string: \"%s\".", s.c_str());
  }
  return atoi(t.c_str());
}

void ProgramOptions::__geometry()
{
  int w = __getW(_configuration->geometryString);
  int h = __getH(_configuration->geometryString);
  int x = __getX(_configuration->geometryString);
  int y = __getY(_configuration->geometryString);
  _cc->setWindowGeometry(w,h,x,y);
}

void ProgramOptions::__captureFrameRate()
{
  _cc->setCaptureFrameRate(_configuration->captureFrameRate);
}

void ProgramOptions::__captureName()
{
  _cc->setCaptureName(_configuration->captureName);
}

void ProgramOptions::__captureDirectory()
{
  _cc->setCaptureDirectory(_configuration->captureDirectory);
}

void ProgramOptions::__captureFramesDirectory()
{
  _cc->setFramesDirectory(_configuration->framesDirectory);
}

void ProgramOptions::__pause()
{
  _cc->setUsePause(true);
}

void ProgramOptions::__realTime()
{
  _cc->setUseRealTimeMode(true);
  _cc->setRealTimeFactor(1.0/_configuration->realtimeFactor);
}

void ProgramOptions::__texture()
{
  _cc->setUseTextures(_configuration->useTextures);
}

void ProgramOptions::__gui()
{
  _cc->setUseVisualisation(false);
}

void ProgramOptions::__controller()
{
  _cc->setUseController(true);
}

void ProgramOptions::__trace()
{
  _cc->setUseTraces(_configuration->useTraces);
}

void ProgramOptions::__printTime()
{
  _cc->setUsePrintTimeInformation(true);
}

void ProgramOptions::__onScreenDisplay()
{
  _cc->setUseOsd(true);
}

void ProgramOptions::__printConfiguration()
{
  _cc->setUsePrintConfiguration(true);
}

void ProgramOptions::__textures()
{
  _cc->setTextures(_configuration->textures);
}

void ProgramOptions::__lib()
{
  _cc->setLibraries(_configuration->lib);
  // Also register with the static Directories registry so every new
  // Directories instance — including those constructed via XML parsing —
  // searches the user-supplied path before the hardcoded defaults.
  Directories::setUserLibraryPath(_configuration->lib);
}

void ProgramOptions::__xml()
{
  _cc->setXml(_configuration->xml);
}

void ProgramOptions::__logging()
{
  _cc->setLogDirectory(_configuration->logDirectory);
}

void ProgramOptions::__printKeyboardShortCuts()
{
  _cc->setUsePrintKeyboardShortcuts(true);
}

void ProgramOptions::__printRobotConfiguration()
{
  Y_FATAL("%s currently not implemented", __PO_OPTION_PRINT_ROBOT_CONFIGURATION);
}

void ProgramOptions::__videoCodec()
{
  _cc->setVideoCodec(_configuration->videoCodec);
}

void ProgramOptions::__useCR()
{
  _cc->setUseCR(true);
}
