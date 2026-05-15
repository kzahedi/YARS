#include "YarsLoggingControl.h"
#include <yars/configuration/YarsConfiguration.h>

YarsLoggingControl::YarsLoggingControl()
{
  _model = nullptr;
}

// notify() removed; YarsMainControl::run() calls init/step/reset/quit directly.

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
  // Flush all CSV / file loggers before __closeApplication runs exit(0),
  // which would otherwise skip the std::ofstream destructors and lose the
  // last few buffered rows. See openspec/changes/fix-logging-handler-shutdown-flush.
  if (_model != nullptr) _model->flush();
  // Model lifetime is owned by YarsMainControl; do not delete here.
}
