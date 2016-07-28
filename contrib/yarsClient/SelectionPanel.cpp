#include "SelectionPanel.h"
#include "DataPanel.h"

#include "CommunicationModel.h"

SelectionPanel::SelectionPanel(QWidget *parent)
  :QComboBox(parent)
{
  QStringList *items = new QStringList();
  _communicationModel = CommunicationModel::instance();
  for(int i = 0; i < _communicationModel->numberOfSensors(); i++)
  {
    string s;
    _communicationModel->getSensorName(i, &s);
    QString name(s.c_str());
    (*items) << name;
  }
  for(int i = 0; i < _communicationModel->numberOfJoints(); i++)
  {
    string s;
    _communicationModel->getJointName(i, &s);
    QString name(s.c_str());
    (*items) << name;
  }
  addItems(*items);

  connect(((QComboBox*)this),   SIGNAL(currentIndexChanged(int)),
          ((DataPanel*)parent), SLOT(selectionChanged(int)));
}
