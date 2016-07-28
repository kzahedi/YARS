#include "DataContentPanel.h"

#include "CommunicationModel.h"
#include "SensorPanel.h"
#include "MotorPanel.h"

#include <iostream>

using namespace std;

DataContentPanel::DataContentPanel(QWidget *parent)
  : QWidget(parent)
{
  _layout             = new QStackedLayout();
  _communicationModel = CommunicationModel::instance();

  for(int i = 0; i < _communicationModel->numberOfSensors(); i++)
  {
    SensorPanel *sp = new SensorPanel(i, this);
    _layout->addWidget(sp);
  }

  for(int i = 0; i < _communicationModel->numberOfJoints(); i++)
  {
    MotorPanel *mp = new MotorPanel(i, this);
    _layout->addWidget(mp);
  }

  setLayout(_layout);
}

void DataContentPanel::swichPanel(int index)
{
  cout << "DataContentPanel::swichPanel " << index << endl;
  _layout->setCurrentIndex(index);
}
