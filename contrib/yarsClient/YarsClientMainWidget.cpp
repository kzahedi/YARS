#include "YarsClientMainWidget.h"

#include "ControlWidget.h"
#include "YarsControlWidget.h"
#include "CanvasWidget.h"
#include "StatusBarControl.h"
#include "InputPanel.h"

#define __MAIN_WIDTH  440
#define __MAIN_HEIGHT 350

YarsClientMainWidget::YarsClientMainWidget()
  :QMainWindow()
{
  setWindowTitle(QString("YARS Client"));
  setFixedSize(QSize(__MAIN_WIDTH, __MAIN_HEIGHT));
  QWidget *centralWidget = new QWidget();

  QGridLayout       *layout = new QGridLayout();
  ControlWidget     *cw     = new ControlWidget();
  YarsControlWidget *ycw    = new YarsControlWidget();
  CanvasWidget      *caw    = new CanvasWidget();
  StatusBarControl  *sbc    = new StatusBarControl(this);
  InputPanel        *ip     = new InputPanel();

  layout->addWidget(ip,  0, 0, 1, 3);
  layout->addWidget(ycw, 1, 0, 1, 1);
  layout->addWidget(caw, 1, 1, 1, 1);
  layout->addWidget(cw,  1, 2, 1, 1);
  centralWidget->setLayout(layout);
  setCentralWidget(centralWidget);
  show();
}
