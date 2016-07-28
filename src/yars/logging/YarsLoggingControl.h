#ifndef __YARS_LOGGING_CONTROL_H__
#define __YARS_LOGGING_CONTROL_H__

#include <yars/util/Observer.h>
#include <yars/logging/YarsLoggingModel.h>

class YarsLoggingControl : public Observer
{
  public:
    YarsLoggingControl();
    virtual ~YarsLoggingControl() { };

    void notify(ObservableMessage *message);
    void setModel(YarsLoggingModel *model);

  private:
    YarsLoggingModel *_model;

};

#endif // __YARS_LOGGING_CONTROL_H__


