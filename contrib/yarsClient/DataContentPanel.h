#ifndef __DATA_CONTENT_PANEL_H__
#define __DATA_CONTENT_PANEL_H__

#include "CommunicationModel.h"

#include <QtGui>

class DataContentPanel : public QWidget
{

  Q_OBJECT

  public:
    DataContentPanel(QWidget *parent = 0);

  private:
    CommunicationModel *_communicationModel;
    QStackedLayout     *_layout;

  public slots:
    void swichPanel(int index);

};

#endif // __DATA_CONTENT_PANEL_H__


