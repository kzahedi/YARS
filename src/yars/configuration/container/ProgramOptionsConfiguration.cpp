#include <yars/configuration/container/ProgramOptionsConfiguration.h>
#include <yars/defines/defaults.h>

ProgramOptionsConfiguration::ProgramOptionsConfiguration()
{
  usePrintTimeInformation      = DEFAULT_USE_PRINT_TIME_INFORMATION;
  useReload                    = DEFAULT_USE_RELOAD;
  useShadows                   = DEFAULT_USE_SHADOWS;
  useTextures                  = DEFAULT_USE_TEXTURES;
  useTraces                    = DEFAULT_USE_TRACES;
  usePsp                       = DEFAULT_USE_PSP;
  captureFrameRate             = DEFAULT_CAPTURE_FRAME_RATE;
  controllerUpdateFrequency    = DEFAULT_CONTROLLER_UPDATE_FREQUENCY;
  followMode                   = DEFAULT_FOLLOW_MODE;
  simulationFrequency          = DEFAULT_SIMULATION_FREQUENCY;
  captureDirectory             = DEFAULT_CAPTURE_DIRECTORY;
  captureName                  = DEFAULT_CAPTURE_NAME;
  configFile                   = DEFAULT_CONFIG_FILE;
  framesDirectory              = DEFAULT_FRAMES_DIRECTORY;
  lib                          = DEFAULT_LIB;
  listCommand                  = DEFAULT_LIST_COMMAND;
  exportCommand                = DEFAULT_EXPORT_COMMAND;
  outConfigFile                = DEFAULT_OUT_CONFIG_FILE;
  textures                     = DEFAULT_TEXTURES;
  logDirectory                 = DEFAULT_LOG_DIRECTORY;
  debug                        = DEFAULT_DEBUG;
  geometrydebug                = DEFAULT_GEOMETRYDEBUG;
  videoCodec                   = DEFAULT_VIDEO_CODEC;
  cappedCylinderQuality        = DEFAULT_CAPPED_CYLINDER_QUALITY;
  silent                       = DEFAULT_BE_SILENT;
  useRandomSeed                = DEFAULT_USE_RANDOM_SEED;
  randomSeed                   = DEFAULT_RANDOM_SEED;
  useRealtime                  = DEFAULT_USE_REALTIME_MODE;
  realtimeFactor               = DEFAULT_REALTIME_FACTOR;
  useMaxIterations             = false;
  maxIterations                = -1;
}
