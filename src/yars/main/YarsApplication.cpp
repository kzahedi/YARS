/*************************************************************************
 *                                                                       *
 * This file is part of Yet Another Robot Simulator (YARS).              *
 * Copyright (C) 2003-2006 Keyan Zahedi, Arndt von Twickel.              *
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
 

#include "YarsApplication.h" 
#include <yars/view/console/ConsoleView.h>

YarsApplication::YarsApplication(int &c, char **v)
: QApplication(c, v)
{}

// needs to be overwritten to catch the yars exceptions, which are otherwise not
// shown
bool YarsApplication::notify(QObject *rec, QEvent *ev)
{
  try
  {
    return QApplication::notify(rec, ev);
  }
  catch (char const *str)
  {
    Y_FATAL("Exception caught: %s", str);
    return false;
  }
  catch (YarsException e) {
    Y_FATAL("Exception caught: %s", e.what());
    abort();
  }
  catch (std::exception std) {
    Y_FATAL("Exception caught: %s", std.what());
    abort();
  }
  catch (...) {
    Y_FATAL("Unknown exception!");
    abort();
  }
}

