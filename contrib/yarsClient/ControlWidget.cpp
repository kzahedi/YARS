#include "ControlWidget.h"
#include "YarsClientControl.h"

ControlWidget::ControlWidget(QWidget *parent)
  : QWidget(parent)
{
  QGroupBox   *buttonGroup   = new QGroupBox("control", this);
  QVBoxLayout *layout        = new QVBoxLayout;
  QPushButton *quit          = new QPushButton("quit");
  QPushButton *connectButton = new QPushButton("connect");
  QPushButton *close         = new QPushButton("close");

  layout->addWidget(connectButton);
  layout->addWidget(close);
  layout->addWidget(quit);

  buttonGroup->setLayout(layout);

  YarsClientControl *ycc = YarsClientControl::instance();
  connect(quit,          SIGNAL(clicked()), ycc, SLOT(quit()));
  connect(connectButton, SIGNAL(clicked()), ycc, SLOT(openConnection()));
  connect(close,         SIGNAL(clicked()), ycc, SLOT(closeConnection()));
}
