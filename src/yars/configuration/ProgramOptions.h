#ifndef __YARS_PROGRAM_OPTIONS_PARSER_H__
#define __YARS_PROGRAM_OPTIONS_PARSER_H__

#include <yars/configuration/container/ProgramOptionsConfiguration.h>
#include <yars/configuration/container/ConfigurationContainer.h>
#include <yars/configuration/container/KeyboardShortcuts.h>

#include <string>
#include <vector>
#include <iostream>

#include <boost/program_options.hpp>

using namespace std;
using namespace boost::program_options;

class ProgramOptions
{
  public:
    ProgramOptions(int argc, char **argv,
        ConfigurationContainer *configuration,
        KeyboardShortcuts *keyboardShortcuts,
        ProgramOptionsConfiguration *programOptions);

  private:

    // functions to handle command line arguments
    void __help();
    void __version();
    void __license();

    void __simulationFrequency();
    void __controlFrequency();
    void __follow();
    void __silent();
    void __printKeyboardShortCuts();
    void __info();
    void __debug();
    void __setDebugLevel();
    void __capture();
    void __captureFrameRate();
    void __captureName();
    void __captureDirectory();
    void __captureFramesDirectory();
    void __pause();
    void __realTime();
    void __texture();
    void __gui();
    void __trace();
    void __printTime();
    void __onScreenDisplay();
    void __textures();
    void __lib();
    void __xml();
    void __printConfiguration();
    void __logging();
    void __writeConfig();
    void __printRobotConfiguration();
    void __videoCodec();
    void __useCR();
    void __geometry();
    void __setRandomSeed();
    int  __getX(string s);
    int  __getY(string s);
    int  __getW(string s);
    int  __getH(string s);

    // end
    void __parseProgramOptionsParameters();
    void __exit(int returnValue);
    string __writeBool(bool b);

    ConfigurationContainer   *_cc;

    variables_map _vm;
    options_description _visible;
    options_description _config_file_options;
    parsed_options _parsed;
    KeyboardShortcuts *_keyboardShortcuts;
    ProgramOptionsConfiguration *_configuration;
};

#endif // __YARS_PROGRAM_OPTIONS_PARSER_H__

