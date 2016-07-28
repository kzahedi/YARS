#ifndef __NUMBERS_PANEL_H__
#define __NUMBERS_PANEL_H__

#define __NUMBERS_PANEL_SENSOR 1001
#define __NUMBERS_PANEL_MOTOR  1002

#include "CommunicationModel.h"
#include <YarsClientCom.h>

#include <QtGui>


class NumbersPanel : public QWidget
{

  Q_OBJECT

  public:
    NumbersPanel(int type, int index, int dimension, QWidget *parent = 0);

  private:
    void __add(Domain *d, int row, int index, QString label, QGridLayout *layout);

    int                 _type;
    int                 _index;
    Entity              _entity;
    CommunicationModel *_communicationModel;

};

#endif // __NUMBERS_PANEL_H__


