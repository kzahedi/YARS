#include <yars/configuration/YarsConfiguration.h>
#include <yars/view/console/ConsoleView.h>
#include <yars/configuration/ProgramOptions.h>
#include <yars/util/FileSystemOperations.h>
#include <yars/util/YarsErrorHandler.h>
#include <yars/util/Timer.h>
#include <yars/util/VideoCodecs.h>
#include <yars/defines/defaults.h>

#include <yars/configuration/xsd/parser/YarsXSDSaxParser.h>
#include <yars/configuration/xsd/graphviz/XsdGraphvizExporter.h>
#include <yars/configuration/xsd/generator/YarsXSDGenerator.h>

#include <yars/defines/program_options.h>
#include <yars/defines/keyboard_shortcuts.h>

#ifdef USE_VISUALISATION
#include <yars/view/gui/CameraFactory.h>
#endif // USE_VISUALISATION

#include <fstream>

YarsConfiguration *YarsConfiguration::_me = NULL;

YarsConfiguration *YarsConfiguration::instance()
{
  if (_me == NULL)
  {
    _me = new YarsConfiguration();
  }
  return _me;
}

YarsConfiguration::YarsConfiguration()
{
  _argc = 0;
  _argv = NULL;
  _directories = new Directories();
  _keyboardShortcuts = new KeyboardShortcuts();
  _programOptions = new ProgramOptionsConfiguration();
  _reset = false;

  _directories->configFile(&_programOptions->configFile);
  _data = Data::instance();
}

YarsConfiguration::~YarsConfiguration()
{
  delete _directories;
}

void YarsConfiguration::init(int argc, char **argv)
{
  bool exitYars = false;
  _argc = argc;
  _argv = argv;

  // read command line parameter and configure file
  __processProgramOptions();

  if (_programOptions->outConfigFile.length() > 0)
  {
    __writeConfig();
    exitYars = true;
  }

  if (_programOptions->listCommand.length() > 0)
  {
    __processListCommand();
    exitYars = true;
  }

  if (_programOptions->exportCommand.length() > 0)
  {
    __processExportCommand();
    exitYars = true;
  }

  if (exitYars)
    exit(0);

  // validate directories
  //   read directories:  xml, lib, textures, xsd
  //   write directories: video, frames, logs
  __validateDirectoriesAndNames();

  // read xml file, if provided
  __readXmlFiles(); // also checks and sets all paths to all libs, textures, etc.

  if (_programOptions->maxIterations > 0)
  {
    Data::instance()->current()->simulator()->setIterations(_programOptions->maxIterations);
  }

  setReset(Data::instance()->current()->simulator()->autoReset());
  setUseVisualisation(Data::instance()->current()->screens()->hasVisible() & getUseVisualisation());

  if (getUsePrintConfiguration() && Y_GET_DEBUG_LEVEL != YARS_FATAL)
  {
    __printConfiguration();
  }

  if (getUsePrintKeyboardShortcuts() && Y_GET_DEBUG_LEVEL != YARS_FATAL)
  {
    __printKeyboardShortcuts();
  }

  if (false && Y_GET_DEBUG_LEVEL != YARS_FATAL) // TODO
  {
    __processRobotConfiguration();
  }
}

void YarsConfiguration::__printVideoCodecs()
{
  VideoCodecs::printAvailableVideoCodecs();
}

void YarsConfiguration::__processListCommand()
{
  if (_programOptions->listCommand == __PO_OPTION_FOLLOW)
  {
    __printListCommandFollowModes();
  }
  else if (_programOptions->listCommand == __PO_OPTION_DEBUG)
  {
    __printListCommandDebugModes();
  }
  else if (_programOptions->listCommand == __PO_OPTION_VIDEO_CODEC)
  {
    __printVideoCodecs();
  }
  else
  {
    cout << "yars --list current takes one of the following arguments:" << endl;
    cout << " " << __PO_OPTION_FOLLOW << " : " << __PO_OPTION_FOLLOW_DESCRIPTION << endl;
    cout << " " << __PO_OPTION_DEBUG << "  : " << __PO_OPTION_DEBUG_DESCRIPTION << endl;
    cout << " " << __PO_OPTION_VIDEO_CODEC << " : " << __PO_OPTION_VIDEO_CODEC_DESCRIPTION << endl;
  }
}

void YarsConfiguration::__processExportCommand()
{
  if (_programOptions->exportCommand == __PO_OPTION_EXPORT_PDF)
  {
    XsdGraphvizExporter::writeDotFile(__PO_OPTION_EXPORT_PDF);
  }
  if (_programOptions->exportCommand == __PO_OPTION_EXPORT_PNG)
  {
    XsdGraphvizExporter::writeDotFile(__PO_OPTION_EXPORT_PNG);
  }

  if (_programOptions->exportCommand == __PO_OPTION_EXPORT_XSD)
  {
    YarsXSDGenerator *xsd = new YarsXSDGenerator();
    // cout << (*xsd) << endl;
    ofstream myfile;
    stringstream filename;
    filename << "rosiml.xsd";
    myfile.open(filename.str().c_str());
    myfile << (*xsd) << endl;
    myfile.close();
    cout << "rosiml.xsd written to current directory." << endl;
    delete xsd;
  }
}

void YarsConfiguration::__align(string s, string *t, int length)
{
  *t = s;
  for (unsigned int i = 0; i < length - s.length(); i++)
  {
    *t += " ";
  }
}

void YarsConfiguration::__printListCommandDebugModes()
{
  string s;
  cout << "--- Debug Modes ---" << endl;
  __align(string(__PO_OPTION_DEBUG_LEVEL_DEBUG), &s, 7);
  cout << s << " : " << __PO_OPTION_DEBUG_LEVEL_DEBUG_DESCRIPTION << endl;

  __align(string(__PO_OPTION_DEBUG_LEVEL_INFO), &s, 7);
  cout << s << " : " << __PO_OPTION_DEBUG_LEVEL_INFO_DESCRIPTION << endl;

  cout << "--- End Debug Modes ---" << endl;
}

void YarsConfiguration::__printListCommandFollowModes()
{
#ifdef USE_VISUALISATION
  std::vector<FollowCamera *> list;
  CameraFactory::create(&list, NULL);
  cout << "--- Followable Modes ---" << endl;
  int index = 0;
  for (std::vector<FollowCamera *>::iterator i = list.begin(); i != list.end(); i++)
  {
    string s;
    (*i)->name(&s);
    cout << index << " : " << s << endl;
    index++;
    delete *i;
  }
  cout << "--- End Followable Modes ---" << endl;
  list.clear();
#endif // USE_VISUALISATION
}

void YarsConfiguration::__readXmlFiles()
{
  Data::instance()->clear();
  string xml = getXml();

  YarsXSDSaxParser *parser = new YarsXSDSaxParser();
  // TODO parser should add new xml files to current data-structure (might already be the case?)
  parser->read(xml);
  if (parser->errors() > 0)
  {
    for (std::vector<string>::iterator i = parser->w_begin(); i != parser->w_end(); i++)
      cout << "WARNING: " << *i << endl;
    for (std::vector<string>::iterator i = parser->e_begin(); i != parser->e_end(); i++)
      cout << "ERROR: " << *i << endl;
    for (std::vector<string>::iterator i = parser->f_begin(); i != parser->f_end(); i++)
      cout << "FATAL: " << *i << endl;
    delete parser;
    exit(-1);
  }
  delete parser;

  if (useRandomSeed())
    Data::instance()->last()->simulator()->setRandomSeed(getRandomSeed());

  __setPathsInData();
  __setCurrent(0);
}

void YarsConfiguration::__setCurrent(int index)
{
  // TODO: randomisation triggered here
  Data::instance()->initialise(index);
}

void YarsConfiguration::__processProgramOptions()
{
  ProgramOptions *po = new ProgramOptions(
      _argc, _argv,
      (ConfigurationContainer *)this,
      _keyboardShortcuts,
      _programOptions);
  delete po;
}

void YarsConfiguration::__validateDirectoriesAndNames()
{

  __applicationsDirectory();

  __validateLibrariesPath();
  __validateXmlPath();
  __validateCaptureName();

  __generateLoggingPath();
  __generateVideoPath();
  __generateFramesPath();

  // TODO TODO
  //setCurrentScenario(0);
}

void YarsConfiguration::__validateCaptureName()
{
  string name;
  name = getCaptureName();
  if (name.length() == 0)
  {
    name = DEFAULT_CAPTURE_NAME;
  }
  setCaptureName(name);
}

void YarsConfiguration::__validateXmlPath()
{
  string xml = getXml();
  if (xml.size() == 0)
  {
    cout << "No XML file given." << endl;
    exit(0);
  }

  if (xml != "-")
  {
    stringstream oss;
    if (!_directories->doesFileExist(xml))
    {
      oss << "The given xml file " << xml << " does not exist.";
    }
    if (oss.str().length() > 0)
    {
      YarsErrorHandler::push(oss.str());
    }
  }
}

void YarsConfiguration::__applicationsDirectory()
{
  string yarsDir;
  _directories->getApplicationDirectory(&yarsDir, _argv[0]);
  setApplicationDirectory(yarsDir);
}

void YarsConfiguration::__generateVideoPath()
{
  string captureDirectory = getCaptureDirectory();
  if (captureDirectory.length() == 0)
  {
    captureDirectory = DEFAULT_CAPTURE_DIRECTORY;
  }
  if (captureDirectory == DEFAULT_CAPTURE_DIRECTORY)
  {
    _directories->getUniqueDirectoryName(&captureDirectory);
  }
  setCaptureDirectory(captureDirectory);
}

void YarsConfiguration::__generateFramesPath()
{
  string framesDirectory;
  framesDirectory = getFramesDirectory();
  if (framesDirectory.length() == 0)
  {
    framesDirectory = DEFAULT_FRAMES_DIRECTORY;
  }
  if (framesDirectory == DEFAULT_FRAMES_DIRECTORY)
  {
    _directories->getUniqueDirectoryName(&framesDirectory);
  }
  setFramesDirectory(framesDirectory);
}

void YarsConfiguration::__generateLoggingPath()
{
  string loggingDirectory;
  loggingDirectory = getLogDirectory();
  if (loggingDirectory == DEFAULT_LOG_DIRECTORY)
  {
    _directories->getUniqueDirectoryName(&loggingDirectory);
    setLogDirectory(loggingDirectory);
  }
}

void YarsConfiguration::__validateLibrariesPath()
{
  string librariesPath;
  librariesPath = getLibraries();
  if (librariesPath.length() > 0)
  {
    if (!_directories->checkLibrariesPath(librariesPath))
    {
      YarsErrorHandler *e = YarsErrorHandler::instance();
      (*e) << "The given libraries path " << librariesPath << " is not valid.";
      YarsErrorHandler::push();
    }
  }
  // can only be checked for content, as soon as a library is given
}

void YarsConfiguration::__printConfiguration()
{
  string xsd;
  std::vector<string> xml;
  std::vector<string> controllers;
  string simName;
  P3D cameraPosition;
  P3D cameraRotation;
  ScreenGeometry d;
  Colour c;
  Colour ground;
  Colour sky;
  // TODO TODO
  // __YARS_GET_WINDOW_SIZE(&d);
  // __YARS_GET_GLOBAL_XML(&xml);
  // __YARS_GET_SIMULATION_NAME(&simName);
  // __YARS_GET_CAMERA_POSITION(&cameraPosition);
  // __YARS_GET_CAMERA_ROTATION(&cameraRotation);
  // __YARS_GET_GROUND_COLOR(&ground);
  // __YARS_GET_SKY_COLOR(&sky);
  getControllerLocations(&controllers);

  uint width = d.width();
  uint height = d.height();

  cout << "--- Yars Initial Configuration   " << endl;
  cout << "  Yars Current Dir              : " << getApplicationDirectory() << endl;
  cout << "  Simulator frequency (Hz)      : " << __YARS_GET_SIMULATOR_FREQUENCY << endl;
  cout << "  Step size                     : " << __YARS_GET_STEP_SIZE << endl;
  cout << "  Textures path                 : " << __YARS_GET_GLOBAL_TEXTURES << endl;
  cout << "  Libraries path                : " << __YARS_GET_GLOBAL_LIBRARIES << endl;
  cout << "  Start with textures           : " << ((__YARS_GET_USE_TEXTURES) ? "true" : "false") << endl;
  cout << "  Start with pause              : " << ((__YARS_GET_USE_PAUSE) ? "true" : "false") << endl;
  cout << "  Start with traces             : " << ((__YARS_GET_USE_TRACES) ? "true" : "false") << endl;
  cout << "  Start with single step        : " << ((__YARS_GET_USE_SINGLE_STEP) ? "true" : "false") << endl;
  cout << "  Use axes visualisation        : " << ((__YARS_GET_USE_AXES_VISUALISATION) ? "true" : "false") << endl;
  cout << "  Use visualisation             : " << ((__YARS_GET_USE_VISUALISATION) ? "true" : "false") << endl;
  cout << "  Use simulation name           : " << simName << endl;
  cout << "  Use capture name              : " << __YARS_GET_CAPTURE_NAME << endl;
  cout << "  Use capture codec             : " << __YARS_GET_VIDEO_CODEC << endl;
  cout << "  Use capture frame rate        : " << __YARS_GET_CAPTURE_FRAME_RATE << endl;
  cout << "  Initial window size           : " << width << " pixels wide, and " << height << " pixels high" << endl;
  cout << "  Video export path             : " << getCaptureDirectory() << endl;
  cout << "  Logging path                  : " << getLogDirectory() << endl;
  cout << "  Frames path                   : " << getFramesDirectory() << endl;
  if (xml.size() > 0)
  {
    cout << "  XML files                     : " << xml[0] << endl;
    for (unsigned int i = 1; i < xml.size(); i++)
    {
      cout << "                                : " << xml[i] << endl;
    }
  }
  if (controllers.size() > 0)
  {
    cout << "  Controller found              : " << controllers[0] << endl;
    for (unsigned int i = 1; i < xml.size(); i++)
    {
      cout << "                                : " << controllers[i] << endl;
    }
  }
  cout << "  Scenario settings             : " << endl;
  // TODO TODO
  // for(int i = 0; i < numberOfScenarios(); i++)
  // {
  //   setCurrentScenario(i);
  //   string name;
  //   getCurrentScenarioName(&name);
  //   int simulatorFrequency = __YARS_GET_SIMULATOR_FREQUENCY;
  //   int controllerFrequency = __YARS_GET_CONTROLLER_FREQUENCY;
  //   cout << "                           Name : " << name << endl;
  //   cout << "            Simulator Frequency :  " << simulatorFrequency << endl;
  //   cout << "           Controller Frequency :  " << controllerFrequency << endl;
  // }
}

void YarsConfiguration::__printKeyboardShortcuts()
{
  string keyboardString;
  _keyboardShortcuts->toString(&keyboardString);
  cout << "--- Keyboard settings ---" << endl;
  cout << keyboardString << endl;
  cout << "--- done ---" << endl;
}

KeyboardShortcuts *YarsConfiguration::getKeyboardShortcuts()
{
  return _keyboardShortcuts;
}

void YarsConfiguration::openCaptureDirectory()
{
  string s = getCaptureDirectory();
  if (!FileSystemOperations::doesDirExist(s))
  {
    FileSystemOperations::createDir(s);
  }
}

void YarsConfiguration::openFramesDirectory()
{
  string s = getFramesDirectory();
  if (!FileSystemOperations::doesDirExist(s))
  {
    FileSystemOperations::createDir(s);
  }
}

string YarsConfiguration::__writeBool(bool b)
{
  return (b) ? string("yes") : string("no");
}

void YarsConfiguration::__writeConfig()
{
  string dateString;
  Timer::getDateString(&dateString);
  ofstream configOutStream;
  configOutStream.open(_programOptions->outConfigFile.c_str());

  configOutStream << "###" << endl;
  configOutStream << "### Automatically generated YARS configure file" << endl;
  configOutStream << "### Date " << dateString << endl;
  configOutStream << "###" << endl;

  configOutStream << endl
                  << endl
                  << endl
                  << "### Debug" << endl;
  configOutStream << "# " << __PO_OPTION_DEBUG
                  << " = " << endl;
  configOutStream << __PO_OPTION_PRINT_TIME
                  << " = " << __writeBool(__YARS_GET_USE_PRINT_TIME_INFORMATION) << endl;
  configOutStream << __PO_OPTION_PRINT_CONFIGURATION
                  << " = " << __writeBool(getUsePrintConfiguration()) << endl;
  configOutStream << "# " << __PO_OPTION_LIST
                  << " = " << endl;
  configOutStream << __PO_OPTION_PRINT_KEYBOARD_SHORTCUTS
                  << " = " << __writeBool(getUsePrintKeyboardShortcuts()) << endl;

  configOutStream << endl
                  << endl
                  << endl
                  << "### Capture" << endl;
  configOutStream << __PO_OPTION_CAPTURE
                  << " = " << __writeBool(__YARS_GET_CAPTURE_OPTION_SET) << endl;
  configOutStream << "# " << __PO_OPTION_CAPTURE_NAME
                  << " = " << _programOptions->captureName << endl;
  configOutStream << __PO_OPTION_CAPTURE_FRAME_RATE
                  << " = " << _programOptions->captureFrameRate << endl;
  configOutStream << "# " << __PO_OPTION_CAPTURE_DIRECTORY
                  << " = " << _programOptions->captureDirectory << endl;
  configOutStream << "# " << __PO_OPTION_FRAMES_DIRECTORY
                  << " = " << _programOptions->framesDirectory << endl;

  configOutStream << endl
                  << endl
                  << endl
                  << "### Control" << endl;
  configOutStream << __PO_OPTION_SIMULATION_FREQUENCY
                  << " = " << _programOptions->simulationFrequency << endl;
  configOutStream << __PO_OPTION_CONTROL_FREQUENCY
                  << " = " << _programOptions->controllerUpdateFrequency << endl;
  configOutStream << endl
                  << endl
                  << endl
                  << "### GUI" << endl;
  configOutStream << __PO_OPTION_REALTIME
                  << " = " << __writeBool(__YARS_GET_USE_REAL_TIME) << endl;
  configOutStream << __PO_OPTION_TEXTURE
                  << " = " << __writeBool(_programOptions->useTextures) << endl;
  configOutStream << __PO_OPTION_GUI
                  << " = " << __writeBool(__YARS_GET_USE_VISUALISATION) << endl;
  configOutStream << __PO_OPTION_TRACE
                  << " = " << __writeBool(_programOptions->useTraces) << endl;
  configOutStream << ((getUseFollowMode()) ? "" : "# ") << __PO_OPTION_FOLLOW
                  << " = " << _programOptions->followMode << endl;
  configOutStream << __PO_OPTION_ON_SCREEN_DISPLAY
                  << " = " << __writeBool(__YARS_GET_USE_OSD) << endl;
  configOutStream << __PO_OPTION_PAUSE
                  << " = " << __writeBool(__YARS_GET_USE_PAUSE) << endl;

  configOutStream << endl
                  << endl
                  << endl
                  << "### Directories" << endl;
  configOutStream << "# " << __PO_OPTION_TEXTURES
                  << " = " << _programOptions->textures << endl;
  configOutStream << "# " << __PO_OPTION_LIB
                  << " = " << _programOptions->lib << endl;
  configOutStream << "# " << __PO_OPTION_LOGGING
                  << " = " << _programOptions->logDirectory << endl;

  configOutStream << endl
                  << endl
                  << endl
                  << "### Keyboard Shortcuts" << endl;

  configOutStream
      << __KEYBOARD_OPTIONS_QUIT_KEY_OPTION
      << " = " << _keyboardShortcuts->quit.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_RESET_KEY_OPTION
      << " = " << _keyboardShortcuts->reset.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_PAUSE_KEY_OPTION
      << " = " << _keyboardShortcuts->pause.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_REALTIME_KEY_OPTION
      << " = " << _keyboardShortcuts->realtime.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_SINGLE_STEP_KEY_OPTION
      << " = " << _keyboardShortcuts->singleStep.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_RESTORE_VIEWPOINT_KEY_OPTION
      << " = " << _keyboardShortcuts->restoreViewpoint.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_PRINT_TIME_KEY_OPTION
      << " = " << _keyboardShortcuts->printTime.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_RESET_SIMULATION_SPEED_KEY_OPTION
      << " = " << _keyboardShortcuts->resetSimSpeed.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_DECREASE_SIMULATION_SPEED_KEY_OPTION
      << " = " << _keyboardShortcuts->decreaseSimSpeed.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_INCREASE_SIMULATION_SPEED_KEY_OPTION
      << " = " << _keyboardShortcuts->increaseSimSpeed.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_PRINT_KEYBOARD_SHORT_CUTS_KEY_OPTION
      << " = " << _keyboardShortcuts->printKeyboardShortcuts.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_RELOAD_ON_RESET_KEY_OPTION
      << " = " << _keyboardShortcuts->toggleReloadOnReset.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_CAPTURE_VIDEO_KEY_OPTION
      << " = " << _keyboardShortcuts->captureVideo.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_WRITE_FRAMES_KEY_OPTION
      << " = " << _keyboardShortcuts->writeFrames.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_VISUALISE_AXES_KEY_OPTION
      << " = " << _keyboardShortcuts->visualiseAxes.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_OPEN_NEW_WINDOW_KEY_OPTION
      << " = " << _keyboardShortcuts->openNewWindow.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_SET_WINDOW_TITLE_KEY_OPTION
      << " = " << _keyboardShortcuts->setWindowSize.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_SHOW_WINDOW_CONFIGURATION_DIALOG_KEY_OPTION
      << " = " << _keyboardShortcuts->showWindowConfigurationDialog.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_SET_WINDOW_SIZE_KEY_OPTION
      << " = " << _keyboardShortcuts->setWindowSize.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_ON_SCREEN_DISPLAY_FRAMES_PER_SECOND_KEY_OPTION
      << " = " << _keyboardShortcuts->onScreenDisplay_FramesPerSecond.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_ON_SCREEN_DISPLAY_ELAPSED_TIME_KEY_OPTION
      << " = " << _keyboardShortcuts->onScreenDisplay_ElapsedTime.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_ON_SCREEN_DISPLAY_KEY_OPTION
      << " = " << _keyboardShortcuts->onScreenDisplay.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_TOGGLE_TEXTURES_KEY_OPTION
      << " = " << _keyboardShortcuts->toggleTextures.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_TOGGLE_FOLLOW_MODE_KEY_OPTION
      << " = " << _keyboardShortcuts->toggleFollowMode.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_TOGGLE_TRACES_KEY_OPTION
      << " = " << _keyboardShortcuts->toggleTraces.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_PREVIOUS_FOLLOWABLE_KEY_OPTION
      << " = " << _keyboardShortcuts->previousFollowable.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_NEXT_FOLLOWABLE_KEY_OPTION
      << " = " << _keyboardShortcuts->nextFollowable.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_PREVIOUS_FOLLOW_MODE_KEY_OPTION
      << " = " << _keyboardShortcuts->previousFollowMode.keyCode
      << endl;
  configOutStream
      << __KEYBOARD_OPTIONS_NEXT_FOLLOW_MODE_KEY_OPTION
      << " = " << _keyboardShortcuts->nextFollowMode.keyCode
      << endl;

  configOutStream.close();

  cout << _programOptions->outConfigFile.c_str() << " written." << endl;
}

void YarsConfiguration::__processRobotConfiguration()
{
}

void YarsConfiguration::reset()
{
  Y_DEBUG("YarsConfiguration::reset()");
  __YARS_SET_STEP(-1); // will be increased in next_step
  __setCurrent(0);
}

void YarsConfiguration::__setPathsInData()
{
  __setControllerPaths();
}

void YarsConfiguration::__setControllerPaths()
{
  DataRobots *robots = _data->last()->robots();
  if (robots == NULL)
    return;

  for (std::vector<DataRobot *>::iterator i = robots->begin(); i != robots->end(); i++)
  {
    DataRobot *robot = *i;
    if (robot->controller() != NULL)
    {
      string module = robot->controller()->module();
      string result;
      if (!_directories->library(&result, module))
      {
        YarsErrorHandler *e = YarsErrorHandler::instance();
        (*e) << "Robot " << robot->name()
             << " requires controller \"" << module << "\" which is not found.";
        YarsErrorHandler::push();
      }
      robot->controller()->setModule(result);
    }
  }
}

int YarsConfiguration::argc()
{
  return _argc;
}

const char **YarsConfiguration::argv()
{
  return (const char **)_argv;
}

void YarsConfiguration::setResetSimulation()
{
  _reset = true;
}

void YarsConfiguration::unsetResetSimulation()
{
  _reset = false;
}

bool YarsConfiguration::isResetSimulation()
{
  return _reset;
}