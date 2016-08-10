#ifndef __YARS_VIEW_CONTROL_H__
#define __YARS_VIEW_CONTROL_H__

#include <yars/util/Observer.h>
#include <yars/view/YarsViewModel.h>

#include <QObject>

class YarsViewControl : public QObject, public Observer
{
  Q_OBJECT
  public:
    YarsViewControl();
    ~YarsViewControl();

    void setModel(YarsViewModel *model);
    void notify(ObservableMessage *message);

  public slots:
    void notifySlot(ObservableMessage *message);

  signals:
    void guiDoneSignal();

  private:
    void __drawSynchronousMode();
    void __drawAsynchoronousMode();

    YarsViewModel *_model;
    QMutex drawMutex;
    int _visualisationStep;

};
#endif // __YARS_VIEW_CONTROL_H__


