#include "MotorPanel.h"

MotorPanel::MotorPanel(int motorIndex, QWidget *parent)
  : QWidget(parent)
{
  _communicationModel = CommunicationModel::instance();
  int dimension       = _communicationModel->getJointDimension(motorIndex);
  QHBoxLayout *layout = new QHBoxLayout();
  _numbersPanel       = new NumbersPanel(__NUMBERS_PANEL_MOTOR, motorIndex, dimension);
  _motorIndex         = motorIndex;

  layout->addWidget(_numbersPanel);

  setLayout(layout);
}
