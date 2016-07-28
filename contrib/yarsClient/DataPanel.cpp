#include "DataPanel.h"

#include "DataContentPanel.h"
#include "SelectionPanel.h"
#include "CommunicationModel.h"

#include <iostream>

DataPanel::DataPanel(QWidget *parent)
  : QFrame(parent)
{
  setMinimumSize(200,100);
  resize(400,200);
  QGridLayout *layout = new QGridLayout();

  _communicationModel = CommunicationModel::instance();
  _values             = new SelectionPanel(this);
  _content            = new DataContentPanel(this);

  layout->addWidget(_values, 0,0);
  layout->addWidget(_content,1,0);

  layout->setRowStretch(0,0);
  layout->setRowStretch(1,1);

  setLayout(layout);

  connect(this, SIGNAL(swichPanel(int)), _content, SLOT(swichPanel(int)));
}


void DataPanel::resizeEvent(QResizeEvent *event)
{
  resize(event->size());
}

void DataPanel::selectionChanged(int index)
{
  cout << "index changed to " << index << endl;
  emit swichPanel(index);
}


