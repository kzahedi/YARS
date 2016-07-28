#include "InputPanel.h"

#include "CommunicationModel.h"

#include <QLineEdit>
#include <string>

InputPanel::InputPanel(QWidget *parent)
  : QWidget(parent)
{
  CommunicationModel *cm = CommunicationModel::instance();

  QString initHostname(cm->hostname().c_str());
  QString initPort;

  initPort.setNum(cm->port());

  QGroupBox   *inputGroup = new QGroupBox("yars address", this);
  QHBoxLayout *layout     = new QHBoxLayout;
  QLineEdit   *ip         = new QLineEdit(initHostname);
  QLineEdit   *port       = new QLineEdit(initPort);

  layout->addWidget(ip);
  layout->addWidget(port);

  inputGroup->setLayout(layout);
}

