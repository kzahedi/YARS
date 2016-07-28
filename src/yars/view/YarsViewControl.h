#ifndef __YARS_VIEW_CONTROL_H__
#define __YARS_VIEW_CONTROL_H__

#include "util/Observer.h"
#include "view/YarsViewModel.h"

class YarsViewControl : public Observer
{
  public:
    YarsViewControl();
    ~YarsViewControl();

    void setModel(YarsViewModel *model);
    void notify(ObservableMessage *message);

  private:

    YarsViewModel *_model;

};
#endif // __YARS_VIEW_CONTROL_H__


