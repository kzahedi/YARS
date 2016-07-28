#include "CanvasWidget.h"
#include "YarsClientControl.h"

CanvasWidget::CanvasWidget(QWidget *parent)
  : QWidget(parent)
{
  QGroupBox   *buttonGroup = new QGroupBox("canvas control", this);
  QVBoxLayout *layout      = new QVBoxLayout();
  QPushButton *newCanvas   = new QPushButton("new");
  QPushButton *close       = new QPushButton("close");
  QPushButton *arrange     = new QPushButton("arrange");
  layout->addWidget(newCanvas);
  layout->addWidget(arrange);
  layout->addWidget(close);
  buttonGroup->setLayout(layout);

  YarsClientControl *ycc = YarsClientControl::instance();
  connect(newCanvas, SIGNAL(clicked()), ycc, SLOT(newCanvas()));
  connect(close,     SIGNAL(clicked()), ycc, SLOT(closeCanvas()));
  connect(arrange,   SIGNAL(clicked()), ycc, SLOT(arrageCanvas()));

}

