#include "NumbersPanel.h"

#include <iostream>

#include <util/NameFunctions.h>

using namespace std;

NumbersPanel::NumbersPanel(int type, int index, int dimension, QWidget *parent)
  : QWidget(parent)
{
  QGridLayout *layout         = new QGridLayout();
  QLabel      *nameLabel      = new QLabel("Name");
  QLabel      *dimensionLabel = new QLabel("Dimension");
  QLabel      *nameValue;
  string       name;
  string       tmp;
  _communicationModel = CommunicationModel::instance();

  switch(type)
  {
    case __NUMBERS_PANEL_MOTOR:
      _communicationModel->getJointName(index, &name);
      NameFunctions::last(name, &tmp);
      nameValue = new QLabel(tmp.c_str());
      break;
    case __NUMBERS_PANEL_SENSOR:
      _communicationModel->getSensorName(index, &name);
      NameFunctions::last(name, &tmp);
      nameValue = new QLabel(tmp.c_str());
      break;
    default:
      cerr << "error" << endl;
      exit(-1);
  }

  _communicationModel = CommunicationModel::instance();

  _type  = type;
  _index = index;

  layout->addWidget(nameLabel,      0, 0);
  layout->addWidget(nameValue,      0, 1);
  layout->addWidget(dimensionLabel, 1, 0);

  cout << "dimension count " << dimension << endl;

    for(int i = 0; i < dimension; i++)
    {
      Domain *d = new Domain(-10, 10);
      switch(_type)
      {
        case __NUMBERS_PANEL_SENSOR:
          _communicationModel->getSensorRawDomain(d, index, i);
          break;
        case __NUMBERS_PANEL_MOTOR:
          _communicationModel->getJointRawDomain(d, index, i);
          break;
      }
      __add(d, 2 + i, i, "Raw value", layout);
    }
    for(int i = 0; i < dimension; i++)
    {
      Domain d;
      switch(_type)
      {
        case __NUMBERS_PANEL_SENSOR:
          _communicationModel->getSensorMappedDomain(&d, index, i);
          break;
        case __NUMBERS_PANEL_MOTOR:
          _communicationModel->getJointMappedDomain(&d, index, i);
          break;
      }
      __add(&d, 2 + i + dimension, i, "Mapped value", layout);
    }

  setLayout(layout);
}

void NumbersPanel::__add(Domain *d, int row, int index, QString label, QGridLayout *layout)
{
  QString num;
  num.setNum(index);
  QString min;
  QString max;
  min.setNum(d->min);
  max.setNum(d->max);
  QString domainString("in [");
  label.append(" ");
  label.append(num);
  domainString.append(min);
  domainString.append(", ");
  domainString.append(max);
  domainString.append("]");
  QLabel *myLabel     = new QLabel(label);
  QLabel *domainLabel = new QLabel(domainString);
  layout->addWidget(myLabel,     row, 0);
  layout->addWidget(domainLabel, row, 2);
}
