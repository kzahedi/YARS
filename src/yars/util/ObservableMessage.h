#ifndef __OBSERVABLE_MESSAGE_H__
#define __OBSERVABLE_MESSAGE_H__

#include <string>

#define __M_QUIT                        0
#define __M_INIT                        1
#define __M_NEXT_STEP                   2
#define __M_RESET                       3

#define __M_DEC_SIMULATION_SPEED        100
#define __M_INC_SIMULATION_SPEED        101

#define __M_RESET_VIEWPOINT             202

#define __M_TOGGLE_RELOAD_MESSAGE       300
#define __M_TOGGLE_PAUSE_MESSAGE        302
#define __M_TOGGLE_REAL_TIME_MESSAGE    303
#define __M_TOGGLE_SINGLE_STEP_MESSAGE  304
#define __M_TOGGLE_VISUALISATION        305
#define __M_TOGGLE_SYNCED_GUI           307
#define __M_TOGGLE_SHADOWS              308
#define __M_TOGGLE_CAPTURE_VIDEO        309

#define __M_QUIT_CALLED                 500
#define __M_QUIT_GUI_CALLED             501
#define __M_NEW_WINDOW                  502

#define __M_ERROR                       601

#define __M_SIGNAL_HANDLER_ACTIVATED  -1001

class ObservableMessage
{
  public:
    ObservableMessage(std::string string, int type, int argument = 0)
    {
      _string = string;
      _type = type;
      _argument = argument;
    };

    std::string string()
    {
      return _string;
    };

    int type()
    {
      return _type;
    };

    int argument()
    {
      return _argument;
    };

    void setArgument(int argument)
    {
      _argument = argument;
    }

    static void deleteAllMessage()
    {
      // TODO
    };

  private:
    std::string _string;
    int _type;
    int _argument;
};

static ObservableMessage *_m_quit                     = new ObservableMessage("quit",                                                __M_QUIT);
static ObservableMessage *_m_init                     = new ObservableMessage("initialise yourself",                                 __M_INIT);
static ObservableMessage *_m_nextStep                 = new ObservableMessage("perform the next simulation step",                    __M_NEXT_STEP);
static ObservableMessage *_m_reset                    = new ObservableMessage("reset the simulation",                                __M_RESET);
static ObservableMessage *_m_resetInitiailViewpoint   = new ObservableMessage("reset the camera to the initially defined viewpoint", __M_RESET_VIEWPOINT);
static ObservableMessage *_m_toggleReload             = new ObservableMessage("toggle: reload on reset",                             __M_TOGGLE_RELOAD_MESSAGE);
static ObservableMessage *_m_togglePause              = new ObservableMessage("toggle: pause simulation",                            __M_TOGGLE_PAUSE_MESSAGE);
static ObservableMessage *_m_toggleRealtime           = new ObservableMessage("toggle: real-time mode",                              __M_TOGGLE_REAL_TIME_MESSAGE);
static ObservableMessage *_m_toggleSingleStep         = new ObservableMessage("toggle: single-step simulation mode",                 __M_TOGGLE_SINGLE_STEP_MESSAGE);
static ObservableMessage *_m_toggleVisualisation      = new ObservableMessage("toggle: visualisation",                               __M_TOGGLE_VISUALISATION);
static ObservableMessage *_m_toggleSyncedGui          = new ObservableMessage("toggling synced GUI mode",                            __M_TOGGLE_SYNCED_GUI);
static ObservableMessage *_m_quit_called              = new ObservableMessage("quit called",                                         __M_QUIT_CALLED);
static ObservableMessage *_m_quit_gui_called          = new ObservableMessage("quit gui called",                                     __M_QUIT_GUI_CALLED);
static ObservableMessage *_m_signal_handler_activated = new ObservableMessage("signal handler was called",                           __M_SIGNAL_HANDLER_ACTIVATED);
static ObservableMessage *_m_error                    = new ObservableMessage("error",                                               __M_ERROR);
static ObservableMessage *_m_newWindow                = new ObservableMessage("new window",                                          __M_NEW_WINDOW);
static ObservableMessage *_m_toggleShadow             = new ObservableMessage("toggle Shadows",                                      __M_TOGGLE_SHADOWS);
static ObservableMessage *_m_toggleCaptureVideo       = new ObservableMessage("toggle capture video",                                __M_TOGGLE_CAPTURE_VIDEO);

#endif // __OBSERVABLE_MESSAGE_H__
