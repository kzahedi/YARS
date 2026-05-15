#ifndef __RUNTIME_CONTROL_H__
#define __RUNTIME_CONTROL_H__

#include <yars/configuration/data/Data.h>
#include <yars/util/Timer.h>
#include <functional>

namespace yars {

/** \brief This class implements the YARS runtime control.
 *
 * This class checks for the maximal number of iterations to be performed, when
 * to perform an automatic reset, and if timing information is to be print.
 */
class RuntimeControl
{
  public:
    /** \brief Constructor */
    RuntimeControl();
    ~RuntimeControl();

    void init();
    void step();
    void reset();
    void quit();
    bool shouldQuit();

#ifdef USE_CAPTURE_VIDEO
    /** \brief Register callback to toggle video capture when recording starts/stops. */
    void setToggleCaptureCallback(std::function<void()> cb) { _toggleCaptureCallback = cb; }
#endif
    
  private:
    Data  *_data;
    Timer *_timer;
    DataRecording *_recording;
    bool           _captureRunning;
    bool           _shouldQuit;
#ifdef USE_CAPTURE_VIDEO
    std::function<void()> _toggleCaptureCallback;
#endif

};

} // namespace yars

#endif // __RUNTIME_CONTROL_H__
