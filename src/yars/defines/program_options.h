#ifndef __ENGLISH_PROGRAM_OPTIONS_ENGLISH_H__
#define __ENGLISH_PROGRAM_OPTIONS_ENGLISH_H__



// generic
#define __PO_OPTION_HELP                              "help"
#define __PO_OPTION_HELP_DESCRIPTION \
  "Produce help message and exit."

#define __PO_OPTION_VERSION                           "version"
#define __PO_OPTION_VERSION_DESCRIPTION \
  "Print version information and exit."

#define __PO_OPTION_LICENSE                           "license"
#define __PO_OPTION_LICENSE_DESCRIPTION \
  "Print license information and exit."

//      debug
#define __PO_OPTION_DEBUG                             "debug"
#define __PO_OPTION_DEBUG_DESCRIPTION \
  "Set debug level to [debug|info|silent]."

#define __PO_OPTION_DEBUG_LEVEL_FAIL \
  "You have specified an unknown debug option:"

#define __PO_OPTION_DEBUG_LEVEL_DEBUG \
  "debug"
#define __PO_OPTION_DEBUG_LEVEL_DEBUG_DESCRIPTION \
  "Prints out a lot of stuff."

#define __PO_OPTION_DEBUG_LEVEL_INFO \
  "info"
#define __PO_OPTION_DEBUG_LEVEL_INFO_DESCRIPTION \
  "Prints out useful information."

#define __PO_OPTION_DEBUG_LEVEL_FATAL \
  "fatal"
#define __PO_OPTION_DEBUG_LEVEL_FATAL_DESCRIPTION \
  "Prints out fatal errors."

#define __PO_OPTION_DEBUG_LEVEL_SILENT \
  "silent"
#define __PO_OPTION_DEBUG_LEVEL_SILENT_DESCRIPTION \
  "Suppresses all output, e.g. for cluster usage."

#define __PO_OPTION_PRINT_TIME                        "printTime"
#define __PO_OPTION_PRINT_TIME_DESCRIPTION \
  "Print simulation iterations, simulated time and averaged frames per second on the console."

#define __PO_OPTION_PRINT_COMMUNICATION_CONFIGURATION "printComConfig"
#define __PO_OPTION_PRINT_COMMUNICATION_CONFIGURATION_DESCRIPTION \
  "Prints the communication configuration during start-up."

#define __PO_OPTION_PRINT_CONFIGURATION               "printConfig"
#define __PO_OPTION_PRINT_CONFIGURATION_DESCRIPTION \
     "Print YARS configuration setting during start-up."

#define __PO_OPTION_PRINT_VIDEO_CODECS               "printVideoCodecs"
#define __PO_OPTION_PRINT_VIDEO_CODECS_DESCRIPTION \
     "Print available video codes and quit."

#define __PO_OPTION_LIST                              "list"
#define __PO_OPTION_LIST_DESCRIPTION \
  "Takes an argument and lists possible settings. The possible arguments are listed, if no argument is given."

//      logging
#define __PO_OPTION_PRINT_SEGMENT_POSITION            "psp"
#define __PO_OPTION_PRINT_SEGMENT_POSITION_DESCRIPTION \
  "Print Segment Positions to file SegmentPosOut.txt"

//      video
#define __PO_OPTION_CAPTURE                           "capture"
#define __PO_OPTION_CAPTURE_DESCRIPTION \
  "Automatically start video capturing."

#define __PO_OPTION_CAPTURE_NAME                      "captureName"
#define __PO_OPTION_CAPTURE_NAME_DESCRIPTION \
  "Automatically start video capturing to specified file."

#define __PO_OPTION_CAPTURE_FRAME_RATE                "captureFrameRate"
#define __PO_OPTION_CAPTURE_FRAME_RATE_DESCRIPTION \
  "Set the capturing frame rate. The default is to capture every frame, hence, the frame rate is then equivalent to the simulations frequency."

#define __PO_OPTION_CAPTURE_DIRECTORY                 "captureDirectory"
#define __PO_OPTION_CAPTURE_DIRECTORY_DESCRIPTION \
  "Set video capturing directory. Default is video-YYYY-MM-DD<-counter>"

#define __PO_OPTION_FRAMES_DIRECTORY                  "framesDirectory"
#define __PO_OPTION_FRAMES_DIRECTORY_DESCRIPTION \
  "Set frames writing directory. Default is frames-YYYY-MM-DD<-counter>"

//      control
#define __PO_OPTION_CONFIG                            "config"
#define __PO_OPTION_CONFIG_DESCRIPTION \
  "Set alternative configuration file."

#define __PO_OPTION_WRITE_CONFIG                      "writeConfig"
#define __PO_OPTION_WRITE_CONFIG_DESCRIPTION \
  "Write configuration file according to the currently set command line parameters."

#define __PO_OPTION_SIMULATION_FREQUENCY              "simfreq"
#define __PO_OPTION_SIMULATION_FREQUENCY_DESCRIPTION \
     "Set the default update frequency of physics (default 100 [Hz]). WARNING: This setting is overwritten by setting in the XML file."

#define __PO_OPTION_CONTROL_FREQUENCY                 "ctrlfreq"
#define __PO_OPTION_CONTROL_FREQUENCY_DESCRIPTION \
  "Set the default global update frequency for the robot behaviours. This is used for any controller (RobotController, Communication, ...). The default is 25 [Hz]. YARS will exit, if the controller frequency is not a full divisor of the physics update frequency (simfreq % ctrlfreq == 0). WARNING: This setting is overwritten by setting in the XML file."

#define __PO_OPTION_COMMUNICATION                  "comm"
#define __PO_OPTION_COMMUNICATION_DESCRIPTION \
  "Enable/disable use of communication. Any \"controlled\" robot is set to passive, if disabled."

#define __PO_OPTION_XML                               "xml"
#define __PO_OPTION_XML_DESCRIPTION \
  "The XML description file(s) to be loaded."

#define __PO_OPTION_REALTIME                          "realtime"
#define __PO_OPTION_REALTIME_DESCRIPTION \
  "Start YARS in real-time mode."

//      gui
#define __PO_OPTION_TEXTURE                        "texture"
#define __PO_OPTION_TEXTURE_DESCRIPTION \
     "Initially enable/disable textures (openGL)."


#define __PO_OPTION_SYNCHRONOUS                       "synched"
#define __PO_OPTION_SYNCHRONOUS_DESCRIPTION \
  "Start YARS with synchronous GUI mode (default is on). YARS will exit if both, synched and asynched are given."

#define __PO_OPTION_GUI                            "nogui"
#define __PO_OPTION_GUI_DESCRIPTION \
     "Start YARS without graphical users interface."

#define __PO_OPTION_TRACE                             "trace"
#define __PO_OPTION_TRACE_DESCRIPTION \
  "Activate traces on start up (default is off)."

#define __PO_OPTION_FOLLOW                            "follow"
#define __PO_OPTION_FOLLOW_DESCRIPTION \
  "Start YARS with following active. For a list of follow modes, use --list follow"

#define __PO_OPTION_ON_SCREEN_DISPLAY                 "osd"
#define __PO_OPTION_ON_SCREEN_DISPLAY_DESCRIPTION \
  "Start with On Screen Display."

#define __PO_OPTION_PAUSE                             "pause"
#define __PO_OPTION_PAUSE_DESCRIPTION \
  "Start YARS paused."

//      paths
#define __PO_OPTION_XSD                               "xsd"
#define __PO_OPTION_XSD_DESCRIPTION \
  "Path to <path>/RoSiML.xsd."

#define __PO_OPTION_TEXTURES                          "textures"
#define __PO_OPTION_TEXTURES_DESCRIPTION \
  "Path to <path>/ground.ppm and <path>/sky.ppm."

#define __PO_OPTION_LIB                               "lib"
#define __PO_OPTION_LIB_DESCRIPTION \
  "Path to <path>/lib."

#define __PO_OPTION_MAX_ITERATIONS                    "iterations"
#define __PO_OPTION_MAX_ITERATIONS_DESCRIPTION \
  "Number of simulations steps, before YARS exits."

#define __PO_OPTION_LOGGING                           "log"
#define __PO_OPTION_LOGGING_DESCRIPTION \
  "Set the directory for logging. Default is ./log-<date>-<counter>."

#define __PO_OPTION_PRINT_KEYBOARD_SHORTCUTS          "printKeyboardShortcuts"
#define __PO_OPTION_PRINT_KEYBOARD_SHORTCUTS_DESCRIPTION \
  "Print the keyboard short cuts on the console during start-up."

#define __PO_OPTION_GEOMETRY                          "geometry"
#define __PO_OPTION_GEOMETRY_DESCRIPTION \
  "Set window size and position."

#define __PO_OPTION_PRINT_ROBOT_CONFIGURATION      "printRobotConfiguration"
#define __PO_OPTION_PRINT_ROBOT_CONFIGURATION_DESCRIPTION \
  "Prints out the sensor and motor configuration of each robot on the console"

#define __PO_OPTION_VIDEO_CODEC      "vcodec"
#define __PO_OPTION_VIDEO_CODEC_DESCRIPTION \
  "Choose the video codec. Use " __PO_OPTION_PRINT_VIDEO_CODECS " to get a list of available names."

#define __PO_OPTION_USE_CR           "cr"
#define __PO_OPTION_USE_CR_DESCRIPTION \
  "Force <CR> instead of \"\\r\" on output"

#define __PO_OPTION_ORBIT_SPEED "orbitSpeed"
#define __PO_OPTION_ORBIT_SPEED_DESCRIPTION \
  "Set the rotation speed of the orbit cameras"

#define __PO_OPTION_RANDOM_SEED "seed"
#define __PO_OPTION_RANDOM_SEED_DESCRIPTION \
  "Set the random seed of YARS."

#define __PO_OPTION_CAPPED_CYLINDER_QUALITY "ccq"
#define __PO_OPTION_CAPPED_CYLINDER_QUALITY_DESCRIPTION \
  "Set the depth of the cylinder visualisation. The higher, the nicer its drawn (increases CPU usage)."

#define __PO_OPTION_EXPORT_PDF      "pdf"
#define __PO_OPTION_EXPORT_PNG      "png"
#define __PO_OPTION_EXPORT_XSD      "xsd"
#define __PO_OPTION_EXPORT_PY       "py"

#define __PO_OPTION_EXPORT "export"
#define __PO_OPTION_EXPORT_DESCRIPTION \
  "Use this option to export the rosiml.xsd (--export=xsd) or a set of graphs, visualising the grammar (--export=graphviz)."

#endif // __ENGLISH_PROGRAM_OPTIONS_ENGLISH_H__
