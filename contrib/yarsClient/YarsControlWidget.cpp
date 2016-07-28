#include "YarsControlWidget.h"

#include "YarsClientControl.h"

YarsControlWidget::YarsControlWidget(QWidget *parent)
  : QWidget(parent)
{
  QGroupBox   *buttonGroup = new QGroupBox("yars control", this);
  QVBoxLayout *layout      = new QVBoxLayout;
  QPushButton *start       = new QPushButton("start");
  QPushButton *stop        = new QPushButton("stop");
  QPushButton *quit        = new QPushButton("quit");
  layout->addWidget(start);
  layout->addWidget(stop);
  layout->addWidget(quit);
  buttonGroup->setLayout(layout);

  YarsClientControl *ycc = YarsClientControl::instance();
  connect(start, SIGNAL(clicked()), ycc, SLOT(startCommunication()));
  connect(stop,  SIGNAL(clicked()), ycc, SLOT(stopCommunication()));
  connect(quit,  SIGNAL(clicked()), ycc, SLOT(quitYars()));

}

