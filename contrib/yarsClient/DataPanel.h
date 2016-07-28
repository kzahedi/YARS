#ifndef __DATA_PANEL_H__
#define __DATA_PANEL_H__

#include "DataContentPanel.h"
#include "SelectionPanel.h"
#include "CommunicationModel.h"

#include <QtGui>

class DataPanel : public QFrame
{

  Q_OBJECT

  public:
    DataPanel(QWidget *parent = 0);

  protected:
    void resizeEvent(QResizeEvent *event);

  private:
    SelectionPanel     *_values;
    DataContentPanel   *_content;
    CommunicationModel *_communicationModel;

  public slots:
    void selectionChanged(int index);

  signals:
    void swichPanel(int);

};

#endif // __DATA_PANEL_H__


