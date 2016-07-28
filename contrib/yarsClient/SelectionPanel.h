#ifndef __SELECTION_PANEL_H__
#define __SELECTION_PANEL_H__

#include "CommunicationModel.h"

#include <QtGui>
#include <QComboBox>

class SelectionPanel : public QComboBox
{

  Q_OBJECT

  public:
    SelectionPanel(QWidget *parent = 0);

  private:
    CommunicationModel *_communicationModel;

};

#endif // __SELECTION_PANEL_H__


