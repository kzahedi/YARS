#ifndef __RUNTIME_CONTROL_H__
#define __RUNTIME_CONTROL_H__

#include <yars/util/Observable.h>
#include <yars/util/Observer.h>
#include <yars/configuration/data/Data.h>
#include <yars/util/Timer.h>

/** \brief This class implements the YARS runtime control.
 *
 * This class checks for the maximal number of iterations to be performed, when
 * to perform an automatic reset, and if timing information is to be print.
 */
class RuntimeControl : public Observer, public Observable
{
  public:
    /** \brief Constructor */
    RuntimeControl();
    ~RuntimeControl();

    /** \brief Implements the runtime control.
     *
     * \param[in]  __M_NEXT_STEP
     * \param[out] __M_RESET
     * \param[out] __M_QUIT_CALLED
     */
    void notify(ObservableMessage *message);
  private:

    Data  *_data;
    Timer *_timer;
    DataRecording *_recording;
    bool           _captureRunning;

};
#endif // __RUNTIME_CONTROL_H__
