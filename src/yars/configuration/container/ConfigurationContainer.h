#ifndef __CONFIGURATION_CONTAINER_H__
#define __CONFIGURATION_CONTAINER_H__


#ifdef USE_THREAD_SAFE
#  include "pthread.h"
#endif // USE_THREAD_SAFE

#include <yars/types/Colour.h>
#include <yars/types/P3D.h>

#include <string>
#include <vector>

using namespace std;

class YarsConfiguration; // forward declaration

#  define __YARS_GET_USE_CR                            YarsConfiguration::instance()->getUseCR()
#  define __YARS_GET_VIDEO_CODEC                       YarsConfiguration::instance()->getVideoCodec()
#  define __YARS_GET_CAMERA_POSITION(a)                YarsConfiguration::instance()->getCameraPosition(a)
#  define __YARS_GET_CAMERA_ROTATION(a)                YarsConfiguration::instance()->getCameraOrientation(a)
#  define __YARS_GET_FRAMES_DIRECTORY                  YarsConfiguration::instance()->getFramesDirectory()
#  define __YARS_GET_MATERIALS_DIRECTORY               YarsConfiguration::instance()->getMaterialsDirectory()
#  define __YARS_GET_CAPTURE_DIRECTORY                 YarsConfiguration::instance()->getCaptureDirectory()
#  define __YARS_GET_CAPTURE_FRAME_RATE                YarsConfiguration::instance()->getCaptureFrameRate()
#  define __YARS_GET_CAPTURE_NAME                      YarsConfiguration::instance()->getCaptureName()
#  define __YARS_GET_EXIT                              YarsConfiguration::instance()->getExit()
#  define __YARS_GET_FOLLOW_MODE                       YarsConfiguration::instance()->getFollowMode()
#  define __YARS_GET_GLOBAL_LIBRARIES                  YarsConfiguration::instance()->getLibraries()
#  define __YARS_GET_GLOBAL_SIMULATION_FREQUENCY       YarsConfiguration::instance()->getSimulationFrequency()
#  define __YARS_GET_GLOBAL_STEP_SIZE                  YarsConfiguration::instance()->getStepSize()
#  define __YARS_GET_GLOBAL_TEXTURES                   YarsConfiguration::instance()->getTextures()
#  define __YARS_GET_GLOBAL_XML                        YarsConfiguration::instance()->getXml()
#  define __YARS_GET_GROUND_COLOR(a)                   YarsConfiguration::instance()->getGroundColour(a)
#  define __YARS_GET_PATH_TO_LIBS                      YarsConfiguration::instance()->getPathToLibs()
#  define __YARS_GET_RESET                             YarsConfiguration::instance()->getReset()
#  define __YARS_GET_SKY_COLOR(a)                      YarsConfiguration::instance()->getSkyColour(a)
#  define __YARS_GET_STEP                              YarsConfiguration::instance()->getCurrentTimeStep()
#  define __YARS_GET_CONTINOUS_STEP                    YarsConfiguration::instance()->getContinousTimeStep()
#  define __YARS_GET_SYNC_GUI                          YarsConfiguration::instance()->getUseSyncGui()
#  define __YARS_GET_USE_GUI                           YarsConfiguration::instance()->getUseGui()
#  define __YARS_GET_USE_AXES_VISUALISATION            YarsConfiguration::instance()->getUseAxesVisualisation()
#  define __YARS_GET_USE_CAPTURE_CL                    YarsConfiguration::instance()->getUseCapture()
#  define __YARS_GET_USE_PAUSE                         YarsConfiguration::instance()->getUsePause()
#  define __YARS_GET_USE_OSD                           YarsConfiguration::instance()->getUseOsd()
#  define __YARS_GET_USE_PRINT_TIME_INFORMATION        YarsConfiguration::instance()->getUsePrintTimeInformation()
#  define __YARS_GET_USE_REAL_TIME                     YarsConfiguration::instance()->getUseRealTimeMode()
#  define __YARS_GET_USE_RESET                         YarsConfiguration::instance()->getUseReset()
#  define __YARS_GET_USE_SINGLE_STEP                   YarsConfiguration::instance()->getUseSingleStep()
#  define __YARS_GET_USE_TEXTURES                      YarsConfiguration::instance()->getUseTextures()
#  define __YARS_GET_USE_TRACES                        YarsConfiguration::instance()->getUseTraces()
#  define __YARS_GET_USE_VISUALISATION                 YarsConfiguration::instance()->getUseVisualisation()
#  define __YARS_GET_USE_CONTROLLER                    YarsConfiguration::instance()->getUseController()
#  define __YARS_GET_REAL_TIME_FACTOR                  YarsConfiguration::instance()->getRealTimeFactor()
#  define __YARS_GET_CAPTURE_OPTION_SET                YarsConfiguration::instance()->getUseCapture()

#  define __YARS_SET_USE_CR(a)                         YarsConfiguration::instance()->setUseCR(a)
#  define __YARS_SET_VIDEO_CODEC(a)                    YarsConfiguration::instance()->setVideoCodec(a)
#  define __YARS_SET_CAMERA_POSITION(a)                YarsConfiguration::instance()->setCameraPosition(a)
#  define __YARS_SET_CAMERA_ROTATION(a)                YarsConfiguration::instance()->setCameraOrientation(a)
#  define __YARS_SET_FRAMES_DIRECTORY(a)               YarsConfiguration::instance()->setFramesDirectory(a)
#  define __YARS_SET_CAPTURE_DIRECTORY(a)              YarsConfiguration::instance()->setCaptureDirectory(a)
#  define __YARS_SET_CAPTURE_FRAME_RATE(a)             YarsConfiguration::instance()->setCaptureFrameRate(a)
#  define __YARS_SET_CAPTURE_NAME(a)                   YarsConfiguration::instance()->setCaptureName(a)
#  define __YARS_SET_EXIT(a)                           YarsConfiguration::instance()->setExit(a)
#  define __YARS_SET_FOLLOW_MODE(a)                    YarsConfiguration::instance()->setFollowMode(a)
#  define __YARS_SET_GLOBAL_LIBRARIES(a)               YarsConfiguration::instance()->setLibraries(a)
#  define __YARS_SET_GLOBAL_STEP_TIMER_REAL_TIME(a)    YarsConfiguration::instance()->setStepTimerRT(a)
#  define __YARS_SET_GLOBAL_TEXTURES(a)                YarsConfiguration::instance()->setTextures(a)
#  define __YARS_SET_GROUND_COLOR(a)                   YarsConfiguration::instance()->setGroundColour(a)
#  define __YARS_SET_RESET(a)                          YarsConfiguration::instance()->setReset(a)
#  define __YARS_SET_SKY_COLOR(a)                      YarsConfiguration::instance()->setSkyColour(a)
#  define __YARS_SET_STEP(a)                           YarsConfiguration::instance()->setCurrentTimeStep(a)
#  define __YARS_SET_CONTINOUS_STEP(a)                 YarsConfiguration::instance()->setContinousTimeStep(a)
#  define __YARS_SET_SYNC_GUI(a)                       YarsConfiguration::instance()->setUseSyncGui(a)
#  define __YARS_SET_USE_GUI(a)                        YarsConfiguration::instance()->setUseGui(a)
#  define __YARS_SET_USE_AXES_VISUALISATION(a)         YarsConfiguration::instance()->setUseAxesVisualisation(a)
#  define __YARS_SET_USE_CAPTURE(a)                    YarsConfiguration::instance()->setUseCapture(a)
#  define __YARS_SET_USE_FOLLOW_MODE(a)                YarsConfiguration::instance()->setUseFollowMode(a)
#  define __YARS_SET_USE_PAUSE(a)                      YarsConfiguration::instance()->setUsePause(a)
#  define __YARS_SET_USE_OSD(a)                        YarsConfiguration::instance()->setUseOsd(a)
#  define __YARS_SET_USE_PRINT_TIME_INFORMATION(a)     YarsConfiguration::instance()->setUsePrintTimeInformation(a)
#  define __YARS_SET_USE_REAL_TIME(a)                  YarsConfiguration::instance()->setUseRealTimeMode(a)
#  define __YARS_SET_USE_RESET(a)                      YarsConfiguration::instance()->setUseReset(a)
#  define __YARS_SET_USE_SINGLE_STEP(a)                YarsConfiguration::instance()->setUseSingleStep(a)
#  define __YARS_SET_USE_TEXTURES(a)                   YarsConfiguration::instance()->setUseTextures(a)
#  define __YARS_SET_USE_TRACES(a)                     YarsConfiguration::instance()->setUseTraces(a)
#  define __YARS_SET_USE_VISUALISATION(a)              YarsConfiguration::instance()->setUseVisualisation(a)
#  define __YARS_SET_REAL_TIME_FACTOR(a)               YarsConfiguration::instance()->setRealTimeFactor(a)
#  define __YARS_SET_ORBIT_SPEED(a)                    YarsConfiguration::instance()->setOrbitSpeed(a)

#  define __YARS_SET_WINDOW_GEOMETRY(w,h,x,y)          YarsConfiguration::instance()->setWindowGeometry(w, h, x, y)
#  define __YARS_GET_X_POSITION                        YarsConfiguration::instance()->getWindowPositionX()
#  define __YARS_GET_Y_POSITION                        YarsConfiguration::instance()->getWindowPositionY()
#  define __YARS_GET_USE_WINDOW_GEOMETRY               YarsConfiguration::instance()->getUseWindowGeometry()

#  define __YARS_GET_WINDOW_HEIGHT                     YarsConfiguration::instance()->getWindowHeight()
#  define __YARS_GET_WINDOW_WIDTH                      YarsConfiguration::instance()->getWindowWidth()

class ConfigurationContainer
{
  public:
    ConfigurationContainer();
    ~ConfigurationContainer();
    unsigned long getCurrentTimeStep();
    void          setCurrentTimeStep(unsigned long currentTimeStep);
    unsigned long getContinousTimeStep();
    void          setContinousTimeStep(unsigned long currentTimeStep);
    double         getCurrentRealTime();
    void          setCurrentRealTime(double currentRealTime);
    bool          getUsePause();
    void          setUsePause(bool usePause);
    bool          getUseOsd();
    void          setUseOsd(bool useOsd);
    bool          getUseRealTimeMode();
    void          setUseRealTimeMode(bool useRealTimeMode);
    bool          getUseTextures();
    void          setUseTextures(bool useTextures);
    bool          getUseAxesVisualisation();
    void          setUseAxesVisualisation(bool useAxesVisualisation);
    bool          getUsePrintFramesPerSecond();
    void          setUsePrintFramesPerSecond(bool usePrintFramesPerSecond);
    bool          getUsePrintTimeInformation();
    void          setUsePrintTimeInformation(bool usePrintTimeInformation);
    bool          getUseVisualisation();
    void          setUseVisualisation(bool useVisualisation);
    bool          getUseController();
    void          setUseController(bool useController);
    bool          getUseTraces();
    void          setUsePrintConfiguration(bool usePrintConfiguration);
    bool          getUsePrintConfiguration();
    void          setUseTraces(bool useTraces);
    bool          getUseReset();
    void          setUseReset(bool useReset);
    bool          getUseSingleStep();
    void          setUseSingleStep(bool useSingleStep);
    bool          getUseFollowMode();
    void          setUseFollowMode(bool useFollowMode);
    bool          getExit();
    void          setExit(bool exit);
    bool          getUseSyncGui();
    void          setUseSyncGui(bool syncGui);
    int           getFollowMode();
    void          setFollowMode(int followMode);
    int           getReset();
    void          setReset(int reset);
    //void          getXml(string *xml);
    string        getXml();
    void          setXml(string xml);
    void          getControllerLocations(std::vector<string> *controllers);
    void          setControllerLoctaions(std::vector<string> controllers);
    string        getApplicationDirectory();
    void          setApplicationDirectory(string yars);
    string        getTextures();
    void          setTextures(string textures);
    string        getLibraries();
    void          setLibraries(string libraries);
    string        getCaptureName();
    void          setCaptureName(string captureName);
    string        getCaptureDirectory();
    void          setCaptureDirectory(string captureDirectory);
    string        getFramesDirectory();
    void          setMaterialsDirectory(string materialsDirectory);
    string        getMaterialsDirectory();
    void          setFramesDirectory(string framesDirectory);
    string        getLogDirectory();
    void          setLogDirectory(string logDirectory);
    void          setUseCapture(bool useCapture);
    bool          getUseCapture();
    int           getCaptureFrameRate();
    void          setCaptureFrameRate(int captureFrameRate);
    void          getCameraPosition(P3D *p);
    void          setCameraPosition(P3D cameraPosition);
    void          getCameraOrientation(P3D *o);
    void          setCameraOrientation(P3D cameraOrientation);
    void          setPathToLibs(string pathToLibs);
    string        getPathToLibs();


    void          setPathToTextures(string pathToTextures);
    string        getPathToTextures();

    void          setPaused(bool  paused);
    void          getPaused(bool *paused);

    void          setExportGraphviz(bool exportGraphviz);
    bool          getExportGraphviz();

    void          setExportXSD(bool exportXSD);
    bool          getExportXSD();

    void          setViewXyz(P3D  viewXyz);
    void          getViewXyz(P3D *viewXyz);

    void          setViewHpr(P3D  viewHpr);
    void          getViewHpr(P3D *viewHpr);

    void          setGroundColour(Colour  groundColour);
    void          getGroundColour(Colour *groundColour);

    void          setSkyColour(Colour  skyColour);
    void          getSkyColour(Colour *skyColour);

    double         getRealTimeFactor();
    void          setRealTimeFactor(double realtimeFactor);

    int           getDefaultControllerFrequency();
    void          setDefaultControllerFrequency(int defaultControllerFrequency);

    int           getDefaultSimulatorFrequency();
    void          setDefaultSimulatorFrequency(int defaultSimulatorFrequency);

    bool          getUsePrintKeyboardShortcuts();
    void          setUsePrintKeyboardShortcuts(bool printKeyboardShortcuts);

    void          setVideoCodec(string vcodec);
    string        getVideoCodec();

    void          setUseCR(bool useCR);
    bool          getUseCR();

    bool          getSilent();
    void          setSilent(bool silent);

    void          setWindowGeometry(int w, int h, int x, int y);
    bool          getUseWindowGeometry();
    int           getWindowPositionX();
    int           getWindowPositionY();
    int           getWindowWidth();
    int           getWindowHeight();

    void          setRandomSeed(int randomSeed);
    int           getRandomSeed();
    bool          useRandomSeed();

  private:
    // one mutex for each variable, such that read/write or write/write cannot
    // occur at the same time
    P3D                       _cameraOrientation;
    P3D                       _cameraPosition;
    bool                      _exit;
    bool                      _syncGui;
    bool                      _useGui;
    bool                      _useAxesVisualisation;
    bool                      _useCapture;
    bool                      _useFollowMode;
    bool                      _usePause;
    bool                      _useOsd;
    bool                      _usePrintFramesPerSecond;
    bool                      _usePrintTimeInformation;
    bool                      _useRealTimeMode;
    bool                      _useReset;
    bool                      _useSingleStep;
    bool                      _useTextures;
    bool                      _useTraces;
    bool                      _useVisualisation;
    bool                      _usePrintConfiguration;
    bool                      _usePrintKeyboardShortcuts;
    bool                      _useCR;
    bool                      _silent;
    double                     _currentRealTime;
    int                       _captureFrameRate;
    int                       _followMode;
    int                       _reset;
    string                    _captureName;
    string                    _captureDirectory;
    string                    _framesDirectory;
    string                    _materialsDirectory;
    string                    _logDirectory;
    string                    _libraries;
    string                    _textures;
    string                    _xml;
    std::vector<string>       _controllerLocations;
    string                    _yars;
    unsigned long             _currentTimeStep;
    unsigned long             _continousTimeStep;
    string                    _pathToLibs;
    string                    _pathToTextures;
    bool                      _paused;
    Colour                    _groundColour;
    Colour                    _skyColour;
    double                     _realtimeFactor;
    int                       _defaultSimulatorFrequency;
    int                       _defaultControllerFrequency;
    string                    _videoCodec;
    bool                      _exportGraphviz;
    bool                      _exportXSD;
    bool                      _useRandomSeed;
    bool                      _useWindowGeometry;
    bool                      _useController;
    int                       _winPositionX;
    int                       _winPositionY;
    int                       _winHeight;
    int                       _winWidth;
    int                       _randomSeed;

#ifdef USE_THREAD_SAFE
    pthread_mutex_t           _cameraOrientationMutex;
    pthread_mutex_t           _cameraPositionMutex;
    pthread_mutex_t           _captureFrameRateMutex;
    pthread_mutex_t           _captureNameMutex;
    pthread_mutex_t           _captureDirectoryMutex;
    pthread_mutex_t           _framesDirectoryMutex;
    pthread_mutex_t           _materialsDirectoryMutex;
    pthread_mutex_t           _logDirectoryMutex;
    pthread_mutex_t           _controllerFrequencyMutex;
    pthread_mutex_t           _currentRealTimeMutex;
    pthread_mutex_t           _currentTimeStepMutex;
    pthread_mutex_t           _continousTimeStepMutex;
    pthread_mutex_t           _exitMutex;
    pthread_mutex_t           _followModeMutex;
    pthread_mutex_t           _librariesMutex;
    pthread_mutex_t           _resetMutex;
    pthread_mutex_t           _syncGuiMutex;
    pthread_mutex_t           _useGuiMutex;
    pthread_mutex_t           _texturesMutex;
    pthread_mutex_t           _useAxesVisualisationMutex;
    pthread_mutex_t           _useCaptureMutex;
    pthread_mutex_t           _useFollowModeMutex;
    pthread_mutex_t           _usePauseMutex;
    pthread_mutex_t           _useOsdMutex;
    pthread_mutex_t           _usePrintFramesPerSecondMutex;
    pthread_mutex_t           _usePrintTimeInformationMutex;
    pthread_mutex_t           _useRealTimeModeMutex;
    pthread_mutex_t           _useResetMutex;
    pthread_mutex_t           _useSingleStepMutex;
    pthread_mutex_t           _useTexturesMutex;
    pthread_mutex_t           _useTracesMutex;
    pthread_mutex_t           _useVisualisationMutex;
    pthread_mutex_t           _usePrintConfigurationMutex;
    pthread_mutex_t           _controllerLocationsMutex;
    pthread_mutex_t           _xmlMutex;
    pthread_mutex_t           _yarsMutex;
    pthread_mutex_t           _pathToLibsMutex;
    pthread_mutex_t           _pathToTexturesMutex;
    pthread_mutex_t           _pausedMutex;
    pthread_mutex_t           _groundColourMutex;
    pthread_mutex_t           _skyColourMutex;
    pthread_mutex_t           _realtimeFactorMutex;
    pthread_mutex_t           _defaultSimulatorFrequencyMutex;
    pthread_mutex_t           _defaultControllerFrequencyMutex;
    pthread_mutex_t           _usePrintKeyboardShortcutsMutex;
    pthread_mutex_t           _videoCodecMutex;
    pthread_mutex_t           _useCRMutex;
    pthread_mutex_t           _silentMutex;
    pthread_mutex_t           _randomSeedMutex;
    pthread_mutex_t           _windowPosition;

#endif // USE_THREAD_SAFE
};
#endif // __CONFIGURATION_CONTAINER_H__


