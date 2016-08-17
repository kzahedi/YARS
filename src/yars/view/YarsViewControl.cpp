/*************************************************************************
 *                                                                       *
 * This file is part of Yet Another Robot Simulator (YARS).              *
 * Copyright (C) 2003-2006 Keyan Zahedi and Arndt von Twickel.           *
 * All rights reserved.                                                  *
 * Email: {keyan,twickel}@users.sourceforge.net                          *
 * Web: http://sourceforge.net/projects/yars                             *
 *                                                                       *
 * For a list of contributors see the file AUTHORS.                      *
 *                                                                       *
 * YARS is free software; you can redistribute it and/or modify it under *
 * the terms of the GNU General Public License as published by the Free  *
 * Software Foundation; either version 2 of the License, or (at your     *
 * option) any later version.                                            *
 *                                                                       *
 * YARS is distributed in the hope that it will be useful, but WITHOUT   *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or *
 * FITNESS FOR A PARTICULAR PURPOSE.                                     *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with YARS in the file COPYING; if not, write to the Free        *
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor,               *
 * Boston, MA 02110-1301, USA                                            *
 *                                                                       *
 *************************************************************************/



#include "view/YarsViewControl.h"
#include <yars/view/gui/KeyHandler.h>

#include "configuration/YarsConfiguration.h"
#include "util/macros.h"


YarsViewControl::YarsViewControl()
{
  KeyHandler::instance()->addObserver(this);
  _visualisationStep = 1;
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
    // case __M_INIT:
      // cout << "init " << endl;
      // _model->initialiseView();
      // _model->initialiseFollowables();
      // break;
    case __M_NEXT_STEP:
      if(__YARS_GET_SYNC_GUI) // asynchronous mode
      {
        __drawSynchronousMode();
      }
      else
      {
        __drawAsynchoronousMode();
      }
      break;
    case __M_RESET:
      _model->reset(); // after model reset, also reset the viewpoint
    case __M_RESET_VIEWPOINT:
      __YARS_GET_CAMERA_POSITION(&c_p);
      __YARS_GET_CAMERA_ROTATION(&c_r);
      // TODO
      //__YARS_SET_VIEW_XYZ(c_p);
      //__YARS_SET_VIEW_HPR(c_r);
      break;
    case __M_QUIT_GUI_CALLED:
      Y_DEBUG("YarsViewControl quit called");
      _model->quit();
      break;
    // case __M_INC_VISUALISATION_FREQUENCY:
      // _visualisationStep+= 10;
      // break;
    // case __M_DEC_VISUALISATION_FREQUENCY:
      // _visualisationStep-= 10;
      // _visualisationStep = MAX(1,_visualisationStep);
      // break;
  }
  emit guiDoneSignal();
}

void YarsViewControl::notifySlot(ObservableMessage *m)
{
  notify(m);
}

void YarsViewControl::__drawSynchronousMode()
{
  if(__YARS_GET_STEP % _visualisationStep == 0)
  {
    _model->visualiseScene();
  }
}

void YarsViewControl::__drawAsynchoronousMode()
{
  if(drawMutex.tryLock()) // false if already locked
  {
    _model->visualiseScene();
    drawMutex.unlock();
  }
}
