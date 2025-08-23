#ifndef __YARS_VIEW_CONTROL_H__
#define __YARS_VIEW_CONTROL_H__

#include <yars/view/YarsViewModel.h>

class YarsViewControl
{
  public:
    YarsViewControl();
    ~YarsViewControl();

    void setModel(YarsViewModel *model);
    
    // Direct methods to replace Observer pattern
    void onReset();
    void onResetViewpoint();
    void onQuit();
    void onNextStep();
    void onNewWindow();
    void onToggleShadows();
    void onToggleCaptureVideo();

  private:

    YarsViewModel *_model;

};
#endif // __YARS_VIEW_CONTROL_H__


