#include "StatusBarControl.h"

#include <QtGui>

StatusBarControl::StatusBarControl(QWidget *parent)
  : QStatusBar(parent)
{
  ((QMainWindow*)parent)->statusBar()->showMessage(tr("Ready"));
}
