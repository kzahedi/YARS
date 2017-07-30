#ifndef __PROGRAM_OPTIONS_CONFIGURATION_H__
#define __PROGRAM_OPTIONS_CONFIGURATION_H__

#include <yars/defines/types.h>

#include <string>
#include <vector>

using namespace std;

class ProgramOptionsConfiguration
{
  public:
    ProgramOptionsConfiguration();

    bool     usePrintTimeInformation;
    bool     useReload;
    bool     useShadows;
    bool     useTextures;
    bool     useTraces;
    bool     usePsp;
    bool     silent;
    bool     useRandomSeed;
    bool     useMaxIterations;
    int      captureFrameRate;
    int      controllerUpdateFrequency;
    int      followMode;
    int      simulationFrequency;
    int      cappedCylinderQuality;
    int      randomSeed;
    int      maxIterations;
    double    orbitSpeed;
    string   captureDirectory;
    string   captureName;
    string   configFile;
    string   framesDirectory;
    string   lib;
    string   listCommand;
    string   exportCommand;
    string   outConfigFile;
    string   textures;
    string   logDirectory;
    string   debug;
    string   geometrydebug;
    string   videoCodec;
    string   geometryString;
    double    realtimeFactor;
    bool     useRealtime;

    string   xml;
};

#endif // __PROGRAM_OPTIONS_CONFIGURATION_H__


