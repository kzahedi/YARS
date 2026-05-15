#ifndef __YARS_PROGRAM_OPTIONS_PARSER_H__
#define __YARS_PROGRAM_OPTIONS_PARSER_H__

#include <yars/configuration/container/ProgramOptionsConfiguration.h>
#include <yars/configuration/container/ConfigurationContainer.h>
#include <yars/configuration/container/KeyboardShortcuts.h>

#include <CLI/CLI.hpp>

#include <string>
#include <vector>
#include <iostream>

class ProgramOptions
{
  public:
    ProgramOptions(int argc, char **argv,
        ConfigurationContainer *configuration,
        KeyboardShortcuts *keyboardShortcuts,
        ProgramOptionsConfiguration *programOptions);

  private:

    // functions to handle command line arguments
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
    void __controller();
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
    int  __getX(std::string s);
    int  __getY(std::string s);
    int  __getW(std::string s);
    int  __getH(std::string s);

    // end
    void __parseProgramOptionsParameters();
    void __exit(int returnValue);
    std::string __writeBool(bool b);

    ConfigurationContainer   *_cc;

    CLI::App _app;
    KeyboardShortcuts *_keyboardShortcuts;
    ProgramOptionsConfiguration *_configuration;

    // Flags to track which options were provided
    bool _helpRequested = false;
    bool _versionRequested = false;
    bool _licenseRequested = false;
    bool _debugSet = false;
    bool _silentSet = false;
    bool _simFreqSet = false;
    bool _ctrlFreqSet = false;
    bool _randomSeedSet = false;
    bool _followSet = false;
    bool _videoCaptureSet = false;
    bool _captureFrameRateSet = false;
    bool _captureNameSet = false;
    bool _captureDirSet = false;
    bool _framesDirSet = false;
    bool _captureSet = false;
    bool _pauseSet = false;
    bool _realtimeSet = false;
    bool _textureSet = false;
    bool _guiSet = false;
    bool _controllerSet = false;
    bool _traceSet = false;
    bool _printTimeSet = false;
    bool _osdSet = false;
    bool _geometrySet = false;
    bool _printRobotConfigSet = false;
    bool _texturesPathSet = false;
    bool _libPathSet = false;
    bool _xmlSet = false;
    bool _printConfigSet = false;
    bool _printKeyboardSet = false;
    bool _loggingSet = false;
    bool _useCRSet = false;
};

#endif // __YARS_PROGRAM_OPTIONS_PARSER_H__
