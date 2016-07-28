#include <iostream>

#include "YarsClientMainWidget.h"

#include <QtGui>

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  YarsClientMainWidget *ycmm = new YarsClientMainWidget();

  return app.exec();
}
