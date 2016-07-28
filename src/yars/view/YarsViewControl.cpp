#include "view/YarsViewControl.h"
#include "gui/KeyHandler.h"

#include "configuration/YarsConfiguration.h"
#include "util/macros.h"


YarsViewControl::YarsViewControl()
{
  KeyHandler::instance()->addObserver(this);
}

YarsViewControl::~YarsViewControl()
{ }

void YarsViewControl::setModel(YarsViewModel *model)
{
  _model = model;
}

void YarsViewControl::notify(ObservableMessage *message)
{
  Y_DEBUG("YarsViewControl: caught message with type %d and text \"%s\"", message->type(), message->string().c_str());
  P3D c_p;
  P3D c_r;
  switch(message->type())
  {
    case __M_RESET:
      _model->reset(); // after model reset, also reset the viewpoint
    case __M_RESET_VIEWPOINT:
      __YARS_GET_CAMERA_POSITION(&c_p);
      __YARS_GET_CAMERA_ROTATION(&c_r);
      // TODO reset cam position
      //__YARS_SET_VIEW_XYZ(c_p);
      //__YARS_SET_VIEW_HPR(c_r);
      break;
    case __M_QUIT_GUI_CALLED:
      Y_DEBUG("YarsViewControl quit called");
      _model->quit();
      Y_DEBUG("YarsViewControl quit completed");
      break;
    case __M_NEXT_STEP:
      _model->synched();
      break;
    case __M_TOGGLE_SHADOWS:
      _model->toggleShadows();
      break;
      case __M_TOGGLE_CAPTURE_VIDEO:
      _model->toggleCaptureVideo();
      break;
  }
}
