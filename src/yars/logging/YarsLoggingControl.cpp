#include "YarsLoggingControl.h"
#include <yars/configuration/YarsConfiguration.h>

YarsLoggingControl::YarsLoggingControl()
{
  _model = nullptr;
}

void YarsLoggingControl::notify(ObservableMessage *message)
{
  switch(message->type())
  {
  case __M_NEXT_STEP:
    if (!__YARS_GET_USE_PAUSE || (__YARS_GET_USE_PAUSE && __YARS_GET_USE_SINGLE_STEP))
    {
      if (_model != nullptr)
      {
        _model->step();
      }
    }
    break;
  case __M_INIT:
    _model->init();
    break;
  case __M_RESET:
    _model->reset();
    _model->step();
    break;
  case __M_QUIT:
    delete _model;
    break;
  }
}

void YarsLoggingControl::setModel(YarsLoggingModel *model)
{
  _model = model;
}

// Direct control methods
void YarsLoggingControl::init()
{
  if (_model != nullptr)
  {
    _model->init();
  }
}

void YarsLoggingControl::step()
{
  if (!__YARS_GET_USE_PAUSE || (__YARS_GET_USE_PAUSE && __YARS_GET_USE_SINGLE_STEP))
  {
    if (_model != nullptr)
    {
      _model->step();
    }
  }
}

void YarsLoggingControl::reset()
{
  if (_model != nullptr)
  {
    _model->reset();
    _model->step();
  }
}

void YarsLoggingControl::quit()
{
  delete _model;
}
