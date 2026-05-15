#ifndef __YARS_LOGGING_CONTROL_H__
#define __YARS_LOGGING_CONTROL_H__

#include <yars/logging/YarsLoggingModel.h>

class YarsLoggingControl
{
  public:
    YarsLoggingControl();
    virtual ~YarsLoggingControl() { };

    void setModel(YarsLoggingModel *model);

    // Direct control methods
    void init();
    void step();
    void reset();
    void quit();

  private:
    YarsLoggingModel *_model;

};

#endif // __YARS_LOGGING_CONTROL_H__


