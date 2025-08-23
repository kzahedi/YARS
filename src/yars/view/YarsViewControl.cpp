#include "YarsViewControl.h"
#include <yars/view/gui/KeyHandler.h>
#include <yars/configuration/YarsConfiguration.h>
#include <yars/util/macros.h>


YarsViewControl::YarsViewControl()
{
  // Observer pattern removed - KeyHandler should call methods directly
}

YarsViewControl::~YarsViewControl()
{ }

void YarsViewControl::setModel(YarsViewModel *model)
{
  _model = model;
}

void YarsViewControl::onReset()
{
  Y_DEBUG("YarsViewControl: onReset called");
  _model->reset();
  onResetViewpoint(); // after model reset, also reset the viewpoint
}

void YarsViewControl::onResetViewpoint()
{
  Y_DEBUG("YarsViewControl: onResetViewpoint called");
  P3D c_p;
  P3D c_r;
  __YARS_GET_CAMERA_POSITION(&c_p);
  __YARS_GET_CAMERA_ROTATION(&c_r);
  // TODO reset cam position
  //__YARS_SET_VIEW_XYZ(c_p);
  //__YARS_SET_VIEW_HPR(c_r);
}

void YarsViewControl::onQuit()
{
  Y_DEBUG("YarsViewControl quit called");
  _model->quit();
  Y_DEBUG("YarsViewControl quit completed");
}

void YarsViewControl::onNextStep()
{
  _model->synched();
}

void YarsViewControl::onNewWindow()
{
  _model->createNewWindow();
}

void YarsViewControl::onToggleShadows()
{
  _model->toggleShadows();
}

void YarsViewControl::onToggleCaptureVideo()
{
  _model->toggleCaptureVideo();
}
