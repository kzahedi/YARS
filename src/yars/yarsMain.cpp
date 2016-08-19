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


#include <yars/main/YarsMainControl.h>

#include <yars/util/YarsException.h>
#include <yars/view/console/ConsoleView.h>


#ifdef USE_VISUALISATION

#include <QApplication>

#include <yars/main/MainLoopThread.h>
#include <yars/main/YarsApplication.h>
#include <yars/view/YarsViewControl.h>
#include <yars/view/YarsViewModel.h>

#include <iostream>

int mainFunction(int argc, char **argv)
{
  YarsApplication app(argc, argv);

  MainLoopThread *mainLoop = new MainLoopThread(argc, argv);

  if(__YARS_GET_USE_VISUALISATION)
  {
    ConsoleView     *cv  = ConsoleView::instance();
    YarsViewControl *yvc = new YarsViewControl();
    YarsViewModel   *yvm = new YarsViewModel();
    yvc->setModel(yvm);
#ifndef SUPPRESS_ALL_OUTPUT
    yvm->addObserver(cv);
#endif // SUPPRESS_ALL_OUTPUT
    mainLoop->hookUp(yvc);
  }

  mainLoop->start();

  return app.exec();
}

#else // NO VISUALISATION

int mainFunction(int argc, char **argv)
{
  try
  {
    YarsMainControl *ymc = new YarsMainControl(argc, argv);
    ymc->run();
  }
  catch(YarsException yse)
  {
    std::cerr << "Yars could not be started due to configuration errors:" << std::endl;
    std::cerr << yse.what() << std::endl;
    exit(-1);
  }
  Y_DEBUG("yarsMain: will delete yars main control.");
  return 0;
}
#endif


int main(int argc, char **argv)
{  
  //(void) signal(SIGINT,leave);
  Y_DEBUG("yarsMain: starting with yars main control.");
  int i = mainFunction(argc, argv);
  Y_DEBUG("yarsMain: DONE. Will exit now.");
  cout << "Good bye." << endl;
  return i;
}

// void leave(int sig) {
//         fprintf(temp_file,"\nInterrupted..\n");
//         fclose(temp_file);
//         exit(sig);
// }

