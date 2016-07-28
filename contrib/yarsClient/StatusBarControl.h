#ifndef __STATUS_BAR_CONTROL_H__
#define __STATUS_BAR_CONTROL_H__

#include <QStatusBar>

class StatusBarControl : public QStatusBar
{

  Q_OBJECT

  public:
    StatusBarControl(QWidget *parent = 0);

};

#endif // __STATUS_BAR_CONTROL_H__


