#include "SensorPanel.h"

#include <iostream>

using namespace std;

SensorPanel::SensorPanel(int sensorIndex, QWidget *parent)
  : QWidget(parent)
{
  _communicationModel = CommunicationModel::instance();
  int dimension       = _communicationModel->getSensorDimension(sensorIndex);
  QHBoxLayout *layout = new QHBoxLayout();
  _numbersPanel       = new NumbersPanel(__NUMBERS_PANEL_SENSOR, sensorIndex, dimension);
  _sensorIndex        = sensorIndex;

  layout->addWidget(_numbersPanel);

  setLayout(layout);
}
