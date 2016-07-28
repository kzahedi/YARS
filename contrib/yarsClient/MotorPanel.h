#ifndef __MOTOR_PANEL_H__
#define __MOTOR_PANEL_H__

#include "NumbersPanel.h"

#include "CommunicationModel.h"

#include <QtGui>

class MotorPanel : public QWidget
{

  Q_OBJECT

  public:
    MotorPanel(int motorIndex, QWidget *parent = 0);

    NumbersPanel       *_numbersPanel;
    int                 _motorIndex;
    CommunicationModel *_communicationModel;

};

#endif // __MOTOR_PANEL_H__


