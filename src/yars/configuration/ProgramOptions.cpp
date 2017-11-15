#include <yars/configuration/ProgramOptions.h>

#include <yars/view/console/ConsoleView.h>
#include <yars/util/YarsErrorHandler.h>

#include <yars/defines/program_options.h>
#include <yars/defines/keyboard_shortcuts.h>

#include <boost/program_options/parsers.hpp>
#include <fstream>

ProgramOptions::ProgramOptions(int argc, char **argv,
    ConfigurationContainer *cc,
    KeyboardShortcuts *keyboardShortcuts,
    ProgramOptionsConfiguration *programOptions)
  : _visible("Usage: yars [options]"),
    _parsed(0)
{

  _cc                = cc;
  _configuration     = programOptions;
  _keyboardShortcuts = keyboardShortcuts;

  options_description generic("Generic options");
  options_description debug("Debug options");
  options_description logging("Logging options");
  options_description control("Control options");
  options_description path("Path configurations");
  options_description video("Video export options");
  options_description gui("GUI options");
  options_description keyboard("Keyboard Shortcut settings");
  options_description cmdline_options;
  positional_options_description positional;

  generic.add_options()
    (__PO_OPTION_HELP,
     __PO_OPTION_HELP_DESCRIPTION)
    (__PO_OPTION_VERSION,
     __PO_OPTION_VERSION_DESCRIPTION)
    (__PO_OPTION_LICENSE,
     __PO_OPTION_LICENSE_DESCRIPTION);

  debug.add_options()

    (__PO_OPTION_XML,
     value<string>(&(_configuration->xml)),
     __PO_OPTION_XML_DESCRIPTION)

    (__PO_OPTION_DEBUG,
     value<string>(&(_configuration->debug)),
     __PO_OPTION_DEBUG_DESCRIPTION)

    (__PO_OPTION_DEBUG_LEVEL_SILENT,
     // value<bool>(&(_configuration->silent))->implicit_value(true),
     __PO_OPTION_DEBUG_LEVEL_SILENT_DESCRIPTION)

    (__PO_OPTION_USE_CR,
     // value<bool>(&(_configuration->useCR))->implicit_value(true),
     __PO_OPTION_USE_CR_DESCRIPTION)

    (__PO_OPTION_PRINT_TIME,
     // value<bool>(&(_configuration->printTime))->implicit_value(true),
     __PO_OPTION_PRINT_TIME_DESCRIPTION)

    (__PO_OPTION_PRINT_ROBOT_CONFIGURATION,
     // value<bool>(&(_configuration->usePrintRobotConfiguration))->implicit_value(true),
     __PO_OPTION_PRINT_ROBOT_CONFIGURATION_DESCRIPTION)

    (__PO_OPTION_PRINT_CONFIGURATION,
     // value<bool>(&(_configuration->printConfiguration))->implicit_value(true),
     __PO_OPTION_PRINT_CONFIGURATION_DESCRIPTION)

    (__PO_OPTION_PRINT_KEYBOARD_SHORTCUTS,
     // value<bool>(&(_configuration->printKeyboardShortcuts))->implicit_value(true),
     __PO_OPTION_PRINT_KEYBOARD_SHORTCUTS_DESCRIPTION)

    (__PO_OPTION_LIST,
     value<string>(&(_configuration->listCommand))->implicit_value("unknown"),
     __PO_OPTION_LIST_DESCRIPTION)

    (__PO_OPTION_EXPORT,
     value<string>(&(_configuration->exportCommand)),
     __PO_OPTION_EXPORT_DESCRIPTION)

    (__PO_OPTION_LOGGING,
     value<string>(&(_configuration->logDirectory)),
     __PO_OPTION_LOGGING_DESCRIPTION);

//#ifdef USE_CAPTURE_VIDEO // not included, for .yarsrc compatibility reasons
  video.add_options()

    (__PO_OPTION_CAPTURE_NAME,
     value<string>(&(_configuration->captureName)),
     __PO_OPTION_CAPTURE_NAME_DESCRIPTION)

    (__PO_OPTION_CAPTURE_FRAME_RATE,
     value<int>(&(_configuration->captureFrameRate)),
     __PO_OPTION_CAPTURE_FRAME_RATE_DESCRIPTION)

    (__PO_OPTION_CAPTURE_DIRECTORY,
     value<string>(&(_configuration->captureDirectory)),
     __PO_OPTION_CAPTURE_DIRECTORY_DESCRIPTION)

    (__PO_OPTION_CAPTURE,
     // value<bool>(&(_configuration->useCapture))->implicit_value(true),
     __PO_OPTION_CAPTURE_DESCRIPTION)

    (__PO_OPTION_VIDEO_CODEC,
     value<string>(&(_configuration->videoCodec)),
     __PO_OPTION_VIDEO_CODEC_DESCRIPTION)

    (__PO_OPTION_FRAMES_DIRECTORY,
     value<string>(&(_configuration->framesDirectory)),
     __PO_OPTION_FRAMES_DIRECTORY_DESCRIPTION);

  control.add_options()
    (__PO_OPTION_CONFIG,
     value<string>(&(_configuration->configFile)),
     __PO_OPTION_CONFIG_DESCRIPTION)

    (__PO_OPTION_MAX_ITERATIONS,
     value<int>(&(_configuration->maxIterations)),
     __PO_OPTION_CONFIG_DESCRIPTION)

    (__PO_OPTION_WRITE_CONFIG,
     value<string>(&(_configuration->outConfigFile))->implicit_value("yarsrc"),
     __PO_OPTION_WRITE_CONFIG_DESCRIPTION)

    (__PO_OPTION_SIMULATION_FREQUENCY,
     value<int>(&(_configuration->simulationFrequency)),
     __PO_OPTION_SIMULATION_FREQUENCY_DESCRIPTION)

    (__PO_OPTION_RANDOM_SEED,
     value<int>(&(_configuration->randomSeed)),
     __PO_OPTION_RANDOM_SEED_DESCRIPTION)

    (__PO_OPTION_CONTROL_FREQUENCY,
     value<int>(&(_configuration->controllerUpdateFrequency)),
     __PO_OPTION_CONTROL_FREQUENCY_DESCRIPTION)

    (__PO_OPTION_REALTIME,
     value<double>(&(_configuration->realtimeFactor))->implicit_value(1.0),
     __PO_OPTION_REALTIME_DESCRIPTION);

//#if USE_VISUALISATION
  gui.add_options()
    (__PO_OPTION_TEXTURE,
     // value<bool>(&(_configuration->useTextures))->implicit_value(true),
     __PO_OPTION_TEXTURE_DESCRIPTION)

    (__PO_OPTION_SYNCHRONOUS,
     // value<bool>(&(_configuration->useSynchronous))->implicit_value(true),
     __PO_OPTION_SYNCHRONOUS_DESCRIPTION)

    (__PO_OPTION_GUI,
     // value<bool>(&(_configuration->useGui))->implicit_value(true),
     __PO_OPTION_GUI_DESCRIPTION)

    (__PO_OPTION_CONTROLLER,
     // value<bool>(&(_configuration->useGui))->implicit_value(true),
     __PO_OPTION_CONTROLLER_DESCRIPTION)

    (__PO_OPTION_TRACE,
     // value<bool>(&(_configuration->useTraces))->implicit_value(true),
     __PO_OPTION_TRACE_DESCRIPTION)

    (__PO_OPTION_FOLLOW,
     value<int>(&(_configuration->followMode))->implicit_value(0),
     __PO_OPTION_FOLLOW_DESCRIPTION)

    (__PO_OPTION_ON_SCREEN_DISPLAY,
     // value<bool>(&(_configuration->useOsd))->implicit_value(true),
     __PO_OPTION_ON_SCREEN_DISPLAY_DESCRIPTION)

    (__PO_OPTION_GEOMETRY,
     value<string>(&(_configuration->geometryString)),
     __PO_OPTION_GEOMETRY_DESCRIPTION)

    (__PO_OPTION_PAUSE,
     // value<bool>(&(_configuration->usePause))->implicit_value(true),
     __PO_OPTION_PAUSE_DESCRIPTION);

  keyboard.add_options()
    (__KEYBOARD_OPTIONS_CAPTURE_VIDEO_KEY_OPTION,
     value<string>(&keyboardShortcuts->captureVideo.keyCode),
     __KEYBOARD_OPTIONS_CAPTURE_VIDEO_KEY_DESCRIPTION)

    (__KEYBOARD_OPTIONS_QUIT_KEY_OPTION,
     value<string>(&keyboardShortcuts->quit.keyCode),
     __KEYBOARD_OPTIONS_QUIT_KEY_DESCRIPTION)

    (__KEYBOARD_OPTIONS_RESET_KEY_OPTION,
     value<string>(&keyboardShortcuts->reset.keyCode),
     __KEYBOARD_OPTIONS_RESET_KEY_DESCRIPTION)

    (__KEYBOARD_OPTIONS_PAUSE_KEY_OPTION,
     value<string>(&keyboardShortcuts->pause.keyCode),
     __KEYBOARD_OPTIONS_PAUSE_KEY_DESCRIPTION)

    (__KEYBOARD_OPTIONS_REALTIME_KEY_OPTION,
     value<string>(&keyboardShortcuts->realtime.keyCode),
     __KEYBOARD_OPTIONS_REALTIME_KEY_DESCRIPTION)

    (__KEYBOARD_OPTIONS_SINGLE_STEP_KEY_OPTION,
     value<string>(&keyboardShortcuts->singleStep.keyCode),
     __KEYBOARD_OPTIONS_SINGLE_STEP_KEY_DESCRIPTION)

    (__KEYBOARD_OPTIONS_RESTORE_VIEWPOINT_KEY_OPTION,
     value<string>(&keyboardShortcuts->restoreViewpoint.keyCode),
     __KEYBOARD_OPTIONS_RESTORE_VIEWPOINT_KEY_DESCRIPTION)

    (__KEYBOARD_OPTIONS_PRINT_TIME_KEY_OPTION,
     value<string>(&keyboardShortcuts->printTime.keyCode),
     __KEYBOARD_OPTIONS_PRINT_TIME_KEY_DESCRIPTION)

    (__KEYBOARD_OPTIONS_RESET_SIMULATION_SPEED_KEY_OPTION,
     value<string>(&keyboardShortcuts->resetSimSpeed.keyCode),
     __KEYBOARD_OPTIONS_RESET_SIMULATION_SPEED_KEY_DESCRIPTION)

    (__KEYBOARD_OPTIONS_DECREASE_SIMULATION_SPEED_KEY_OPTION,
     value<string>(&keyboardShortcuts->decreaseSimSpeed.keyCode),
     __KEYBOARD_OPTIONS_DECREASE_SIMULATION_SPEED_KEY_DESCRIPTION)

    (__KEYBOARD_OPTIONS_INCREASE_SIMULATION_SPEED_KEY_OPTION,
     value<string>(&keyboardShortcuts->increaseSimSpeed.keyCode),
     __KEYBOARD_OPTIONS_INCREASE_SIMULATION_SPEED_KEY_DESCRIPTION)

    (__KEYBOARD_OPTIONS_PRINT_KEYBOARD_SHORT_CUTS_KEY_OPTION,
     value<string>(&keyboardShortcuts->printKeyboardShortcuts.keyCode),
     __KEYBOARD_OPTIONS_PRINT_KEYBOARD_SHORT_CUTS_KEY_DESCRIPTION)

    (__KEYBOARD_OPTIONS_RELOAD_ON_RESET_KEY_OPTION,
     value<string>(&keyboardShortcuts->toggleReloadOnReset.keyCode),
     __KEYBOARD_OPTIONS_RELOAD_ON_RESET_KEY_DESCRIPTION)

    (__KEYBOARD_OPTIONS_WRITE_FRAMES_KEY_OPTION,
     value<string>(&keyboardShortcuts->writeFrames.keyCode),
     __KEYBOARD_OPTIONS_WRITE_FRAMES_DESCRIPTION)

    (__KEYBOARD_OPTIONS_VISUALISE_AXES_KEY_OPTION,
     value<string>(&keyboardShortcuts->visualiseAxes.keyCode),
     __KEYBOARD_OPTIONS_VISUALISE_AXES_DESCRIPTION)

    (__KEYBOARD_OPTIONS_OPEN_NEW_WINDOW_KEY_OPTION,
     value<string>(&keyboardShortcuts->openNewWindow.keyCode),
     __KEYBOARD_OPTIONS_OPEN_NEW_WINDOW_DESCRIPTION)

    (__KEYBOARD_OPTIONS_SET_WINDOW_TITLE_KEY_OPTION,
     value<string>(&keyboardShortcuts->setWindowTitle.keyCode),
     __KEYBOARD_OPTIONS_SET_WINDOW_TITLE_DESCRIPTION)

    (__KEYBOARD_OPTIONS_SHOW_WINDOW_CONFIGURATION_DIALOG_KEY_OPTION,
     value<string>(&keyboardShortcuts->showWindowConfigurationDialog.keyCode),
     __KEYBOARD_OPTIONS_SHOW_WINDOW_CONFIGURATION_DIALOG_DESCRIPTION)

    (__KEYBOARD_OPTIONS_SET_WINDOW_SIZE_KEY_OPTION,
     value<string>(&keyboardShortcuts->setWindowSize.keyCode),
     __KEYBOARD_OPTIONS_SET_WINDOW_SIZE_DESCRIPTION)

    (__KEYBOARD_OPTIONS_ON_SCREEN_DISPLAY_FRAMES_PER_SECOND_KEY_OPTION,
     value<string>(&keyboardShortcuts->onScreenDisplay_FramesPerSecond.keyCode),
     __KEYBOARD_OPTIONS_ON_SCREEN_DISPLAY_FRAMES_PER_SECOND_DESCRIPTION)

    (__KEYBOARD_OPTIONS_ON_SCREEN_DISPLAY_ELAPSED_TIME_KEY_OPTION,
     value<string>(&keyboardShortcuts->onScreenDisplay_ElapsedTime.keyCode),
     __KEYBOARD_OPTIONS_ON_SCREEN_DISPLAY_ELAPSED_TIME_DESCRIPTION)

    (__KEYBOARD_OPTIONS_ON_SCREEN_DISPLAY_KEY_OPTION,
     value<string>(&keyboardShortcuts->onScreenDisplay.keyCode),
     __KEYBOARD_OPTIONS_ON_SCREEN_DISPLAY_DESCRIPTION)

    (__KEYBOARD_OPTIONS_TOGGLE_TEXTURES_KEY_OPTION,
     value<string>(&keyboardShortcuts->toggleTextures.keyCode),
     __KEYBOARD_OPTIONS_TOGGLE_TEXTURES_DESCRIPTION)

    (__KEYBOARD_OPTIONS_PRINT_VIEW_POINT_KEY_OPTION,
     value<string>(&keyboardShortcuts->printViewPoint.keyCode),
     __KEYBOARD_OPTIONS_PRINT_VIEW_POINT_DESCRIPTION)

    (__KEYBOARD_OPTIONS_TOGGLE_FOLLOW_MODE_KEY_OPTION,
     value<string>(&keyboardShortcuts->toggleFollowMode.keyCode),
     __KEYBOARD_OPTIONS_TOGGLE_FOLLOW_MODE_DESCRIPTION)

    (__KEYBOARD_OPTIONS_TOGGLE_TRACES_KEY_OPTION,
     value<string>(&keyboardShortcuts->toggleTraces.keyCode),
     __KEYBOARD_OPTIONS_TOGGLE_TRACES_DESCRIPTION)

    (__KEYBOARD_OPTIONS_PREVIOUS_FOLLOWABLE_KEY_OPTION,
     value<string>(&keyboardShortcuts->previousFollowable.keyCode),
     __KEYBOARD_OPTIONS_PREVIOUS_FOLLOWABLE_DESCRIPTION)

    (__KEYBOARD_OPTIONS_NEXT_FOLLOWABLE_KEY_OPTION,
     value<string>(&keyboardShortcuts->nextFollowable.keyCode),
     __KEYBOARD_OPTIONS_NEXT_FOLLOWABLE_DESCRIPTION)

    (__KEYBOARD_OPTIONS_PREVIOUS_FOLLOW_MODE_KEY_OPTION,
     value<string>(&keyboardShortcuts->previousFollowMode.keyCode),
     __KEYBOARD_OPTIONS_PREVIOUS_FOLLOW_MODE_DESCRIPTION)

    (__KEYBOARD_OPTIONS_NEXT_FOLLOW_MODE_KEY_OPTION,
     value<string>(&keyboardShortcuts->nextFollowMode.keyCode),
     __KEYBOARD_OPTIONS_NEXT_FOLLOW_MODE_DESCRIPTION);


//#endif // USE_VISUALISATION

  path.add_options()

    (__PO_OPTION_TEXTURES,
     value<string>(&(_configuration->textures)),
     __PO_OPTION_TEXTURES_DESCRIPTION)

    (__PO_OPTION_LIB,
     value<string>(&(_configuration->lib)),
     __PO_OPTION_LIB_DESCRIPTION);

  positional.add(__PO_OPTION_XML, -1); // default option, if no -x is given the last is interpreted as xml file

  cmdline_options.add(generic).add(path).add(control).add(gui).add(video).add(logging).add(debug);
  _config_file_options.add(path).add(control).add(gui).add(video).add(logging).add(debug).add(keyboard);
  _visible.add(generic).add(path).add(control).add(gui).add(video).add(logging).add(debug);

  // TODO run twice? there must be a better solution
  _parsed = command_line_parser(argc, (char**) argv).
    options(cmdline_options).positional(positional).allow_unregistered().run();

  store(command_line_parser(argc, (char**) argv).
      options(cmdline_options).positional(positional).allow_unregistered().run(),
      _vm);

  notify(_vm);

  __parseProgramOptionsParameters();

  // this does not overwrite variables set by the command line
  ifstream ifs(_configuration->configFile.c_str());
  store(parse_config_file(ifs, _config_file_options), _vm);
  notify(_vm);
  __parseProgramOptionsParameters();
}


void ProgramOptions::__parseProgramOptionsParameters()
{

  if(_vm.count(__PO_OPTION_HELP))                              __help();
  if(_vm.count(__PO_OPTION_VERSION))                           __version();
  if(_vm.count(__PO_OPTION_LICENSE))                           __license();

  std::vector<string> unknown = collect_unrecognized(_parsed.options, exclude_positional);

  if(unknown.size() > 0)
  {
    cout << "Unknown options given:" << endl;
    for(std::vector<string>::iterator i = unknown.begin(); i != unknown.end(); i++)
    {
      cout << "  " << *i << endl;
    }
    cout << "Please use --help to check." << endl;
    __exit(0);
  }


  if(_vm.count(__PO_OPTION_DEBUG))                             __setDebugLevel();
  if(_vm.count(__PO_OPTION_DEBUG_LEVEL_SILENT))                __silent();

  if(_vm.count(__PO_OPTION_SIMULATION_FREQUENCY))              __simulationFrequency();
  if(_vm.count(__PO_OPTION_CONTROL_FREQUENCY))                 __controlFrequency();
  if(_vm.count(__PO_OPTION_RANDOM_SEED))                       __setRandomSeed();
  if(_vm.count(__PO_OPTION_FOLLOW))                            __follow();

#if USE_CAPTURE_VIDEO
  if(_vm.count(__PO_OPTION_VIDEO_CODEC))                       __videoCodec();
  if(_vm.count(__PO_OPTION_CAPTURE_FRAME_RATE))                __captureFrameRate();
  if(_vm.count(__PO_OPTION_CAPTURE_NAME))                      __captureName();
  if(_vm.count(__PO_OPTION_CAPTURE_DIRECTORY))                 __captureDirectory();
  if(_vm.count(__PO_OPTION_FRAMES_DIRECTORY))                  __captureFramesDirectory();
  if(_vm.count(__PO_OPTION_CAPTURE))                           __capture();
#endif // USE_CAPTURE_VIDEO

//#if USE_VISUALISATION
  if(_vm.count(__PO_OPTION_PAUSE))                             __pause();
  if(_vm.count(__PO_OPTION_REALTIME))                          __realTime();
  if(_vm.count(__PO_OPTION_TEXTURE))                           __texture();
  if(_vm.count(__PO_OPTION_GUI))                               __gui();
  if(_vm.count(__PO_OPTION_CONTROLLER))                        __controller();
  if(_vm.count(__PO_OPTION_TRACE))                             __trace();
  if(_vm.count(__PO_OPTION_PRINT_TIME))                        __printTime();
  if(_vm.count(__PO_OPTION_ON_SCREEN_DISPLAY))                 __onScreenDisplay();
  if(_vm.count(__PO_OPTION_GEOMETRY))                          __geometry();
//#endif // USE_VISUALISATION

  if(_vm.count(__PO_OPTION_PRINT_ROBOT_CONFIGURATION))       __printRobotConfiguration();
  if(_vm.count(__PO_OPTION_TEXTURES))                          __textures();
  if(_vm.count(__PO_OPTION_LIB))                               __lib();
  if(_vm.count(__PO_OPTION_XML))                               __xml();
  if(_vm.count(__PO_OPTION_PRINT_CONFIGURATION))               __printConfiguration();
  if(_vm.count(__PO_OPTION_PRINT_KEYBOARD_SHORTCUTS))          __printKeyboardShortCuts();
  if(_vm.count(__PO_OPTION_LOGGING))                           __logging();
  if(_vm.count(__PO_OPTION_USE_CR))                            __useCR();

  if(_vm.count(__PO_OPTION_XML) == 0) // no xml given
  {
    _cc->setUseRealTimeMode(true);
  }

  _keyboardShortcuts->process();

}



void ProgramOptions::__exit(int returnValue)
{
  exit(returnValue);
}

////////////////////////////////////////////////////////////////////////////////
// functions handling command line parameters
////////////////////////////////////////////////////////////////////////////////

void ProgramOptions::__help()
{
  cout << _visible << endl;
  __exit(0);
}

void ProgramOptions::__version()
{
#ifndef _MSC_VER
  cout << "yars version "     << YARS_VERSION << endl;
  cout << "  compiled date: " << __DATE__ << endl;
  cout << "  compiled with: " << YARS_COMPILER_VERSION << endl;
  cout << "  compiled for:  " << YARS_COMPILED_OS << endl;
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
  // if(_configuration->debug == __PO_OPTION_DEBUG_LEVEL_FATAL)
  // {
  //   __silent();
  //   return;
  // };
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

  for(int i = index; i < s.length(); i++)
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

  for(int i = index; i < s.length(); i++)
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

  for(int i = 0; i < s.length(); i++)
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
  int index  = 0;
  while(s[index] != 'x') index++;
  index++;

  for(int i = index; i < s.length(); i++)
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
