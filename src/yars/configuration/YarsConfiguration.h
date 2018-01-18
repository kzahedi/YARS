#ifndef __YARS_CONFIGURATION_CONTROL_H__
#define __YARS_CONFIGURATION_CONTROL_H__

#include <yars/util/Observable.h>
#include <yars/configuration/container/ConfigurationContainer.h>
#include <yars/configuration/container/KeyboardShortcuts.h>
#include <yars/configuration/container/ProgramOptionsConfiguration.h>
#include <yars/util/Directories.h>
#include <yars/util/YarsErrorHandler.h>
#include <yars/configuration/data/Data.h>

// TODO TODO: keep a copy of DataRobotSimulationDescription in YarsConfiguration
#define __YARS_GET_STEP_SIZE                       Data::instance()->current()->simulator()->stepSize()
#define __YARS_GET_SOLVER_ITERATIONS               Data::instance()->current()->simulator()->getSolverIterations()
#define __YARS_GET_SIMULATOR_FREQUENCY             Data::instance()->current()->simulator()->simulatorFrequency()
#define __YARS_GET_BEHAVIOUR_FREQUENCY             Data::instance()->current()->simulator()->behaviourFrequency()
#define __YARS_GET_USE_FOLLOW_MODE(a)              YarsConfiguration::instance()->getUseFollowMode() || \
                                                   Data::instance()->current()->screens()->screen(a)->autoFollow()
#define __YARS_GET_MAX_ITERATIONS                  Data::instance()->current()->simulator()->iterations()
#define __YARS_SET_MAX_ITERATIONS(a)               Data::instance()->current()->simulator()->setIterations(a)

#define __YARS_OPEN_CAPTURE_DIRECTORY              YarsConfiguration::instance()->openCaptureDirectory()
#define __YARS_OPEN_FRAMES_DIRECTORY               YarsConfiguration::instance()->openFramesDirectory()

#define __YARS_GET_CURRENT_ENVIRONMENT             Data::instance()->current()->environments()->environment(0)
#define __YARS_GET_ROBOTS                          Data::instance()->current()->robots()

#define __YARS_GET_VIEW_POSITION(index)            Data::instance()->current()->screens()->screen(index)->camera()->position()
#define __YARS_GET_VIEW_LOOK_AT(index)             Data::instance()->current()->screens()->screen(index)->camera()->lookAt()
#define __YARS_GET_VIEW_ANGLES(index)              Data::instance()->current()->screens()->screen(index)->camera()->angles()
#define __YARS_GET_VIEW_UP(index)                  Data::instance()->current()->screens()->screen(index)->camera()->up()
#define __YARS_GET_VIEW_NAME(index)                Data::instance()->current()->screens()->screen(index)->name()
#define __YARS_GET_USE_CAPTURE(index)              Data::instance()->current()->screens()->screen(index)->autoCapture()
#define __YARS_GET_VIEW_GEOMETRY(index)            Data::instance()->current()->screens()->screen(index)->geometry()
#define __YARS_GET_SKY                             Data::instance()->current()->screens()->sky()
#define __YARS_CURRENT_DATA                        Data::instance()->current()
#define __YARS_DATA_GET_CURRENT_ENVIRONMENT(index) Data::instance()->current()->environment()
#define __YARS_HAS_ENVIRONMENTS                    (Data::instance()->current()->environment() != NULL)

#define __YARS_USE_RIGID_BODY_DYNAMICS             true

#define __YARS_GET_ARGC                            YarsConfiguration::instance()->argc()
#define __YARS_GET_ARGV                            YarsConfiguration::instance()->argv()

#define __YARS_GET_OSD_TIME_FONT_NAME(index) \
  Data::instance()->current()->screens()->screen(index)->osdTimeFontName()
#define __YARS_GET_OSD_TIME_FONT_SIZE(index) \
  Data::instance()->current()->screens()->screen(index)->osdTimeFontSize()
#define __YARS_GET_OSD_TIME_FONT_COLOR(index) \
  Data::instance()->current()->screens()->screen(index)->osdTimeFontColour()

#define __YARS_GET_OSD_ROBOT_FONT_NAME(index) \
  Data::instance()->current()->screens()->screen(index)->osdRobotFontName()
#define __YARS_GET_OSD_ROBOT_FONT_SIZE(index) \
  Data::instance()->current()->screens()->screen(index)->osdRobotFontSize()
#define __YARS_GET_OSD_ROBOT_FONT_COLOR(index) \
  Data::instance()->current()->screens()->screen(index)->osdRobotFontColour()

#  define __YARS_GET_RANDOM_SEED \
  Data::instance()->current()->simulator()->getRandomSeed()
#  define __YARS_GET_USE_RANDOM_SEED \
  Data::instance()->current()->simulator()->useRandomSeed()

//#define __YARS_DATA_GET_CURRENT_ENVIRONMENT(index) Data::instance()->current()->environments()->environment(index)
//#define __YARS_HAS_ENVIRONMENTS                    (Data::instance()->current()->environments() != NULL)

/* \brief Catches ObservableMessage and translates them to function calls of
 * YarsConfigurationModel.
 */
class YarsConfiguration :
  public Observable,
  public ConfigurationContainer
{
  public:
    static YarsConfiguration* instance();
    ~YarsConfiguration();

    void init(int argc, char **argv);
    KeyboardShortcuts* getKeyboardShortcuts();

    void openCaptureDirectory();
    void openFramesDirectory();
    void reset();

    int argc();
    const char** argv();

  private:
    YarsConfiguration();

    void __applicationsDirectory();

    /** \brief Set all configuration default values.
     *
     * Its the first step in the initialisation process.
     *
     */
    void __setDefaultValues();

    /** \brief Reads and parses the command line arguments.
     */
    void __processProgramOptions();

    /** \brief Check and generates directories.
     */
    void __validateDirectoriesAndNames();

    void __generateFramesPath();
    void __generateVideoPath();
    void __generateLoggingPath();
    void __validateLibrariesPath();
    void __validateXmlPath();
    void __validateCaptureName();
    void __checkLibraries();

    void __setPathsInData();
    void __setControllerPaths();

    void __printConfiguration();
    void __printKeyboardShortcuts();

    void __readXmlFiles();
    void __addFileToScenarios(string file, int index);
    string __writeBool(bool b);
    void __writeConfig();
    void __processListCommand();
    void __processExportCommand();
    void __printListCommandFollowModes();
    void __printListCommandDebugModes();
    void __processRobotConfiguration();
    void __printVideoCodecs();
    void __align(string s, string *t, int length);
    void __setCurrent(int index = 0);

    static YarsConfiguration     *_me;

    int                           _argc;
    char                        **_argv;
    Directories                  *_directories;
    KeyboardShortcuts            *_keyboardShortcuts;
    ProgramOptionsConfiguration  *_programOptions;
    Data                         *_data;
};
#endif // __YARS_CONFIGURATION_CONTROL_H__
