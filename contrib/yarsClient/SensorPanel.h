#ifndef __SENSOR_PANEL_H__
#define __SENSOR_PANEL_H__

#include "NumbersPanel.h"

#include "CommunicationModel.h"

#include <QtGui>

class SensorPanel : public QWidget
{

  Q_OBJECT

  public:
    SensorPanel(int sensorIndex, QWidget *parent = 0);

  private:
    NumbersPanel       *_numbersPanel;
    int                 _sensorIndex;
    CommunicationModel *_communicationModel;

};

#endif // __SENSOR_PANEL_H__


