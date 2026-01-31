#ifndef __RUNTIME_CONTROL_H__
#define __RUNTIME_CONTROL_H__

#include <yars/configuration/data/Data.h>
#include <yars/util/Timer.h>

#include <functional>

/** \brief Runtime control for YARS simulation.
 *
 * Handles iteration limits, auto-reset, timing, and video capture control.
 */
class RuntimeControl
{
public:
  using QuitCallback = std::function<void()>;
  using VideoCaptureCallback = std::function<void()>;

  RuntimeControl();
  ~RuntimeControl();

  /** \brief Initialize/reset the runtime control state. */
  void init();

  /** \brief Called each simulation step. */
  void step();

  /** \brief Set callback for quit requests. */
  void setQuitCallback(QuitCallback callback) { _quitCallback = std::move(callback); }

  /** \brief Set callback for video capture toggle. */
  void setVideoCaptureCallback(VideoCaptureCallback callback) { _videoCaptureCallback = std::move(callback); }

private:
  Data *_data;
  Timer *_timer;
  DataRecording *_recording;
  bool _captureRunning;
  QuitCallback _quitCallback;
  VideoCaptureCallback _videoCaptureCallback;
};

#endif // __RUNTIME_CONTROL_H__
