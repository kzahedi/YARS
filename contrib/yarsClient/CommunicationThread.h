#ifndef __COMMUNICATION_THREAD_H__
#define __COMMUNICATION_THREAD_H__

#include "CommunicationModel.h"

#include <QThread>

class CommunicationThread : public QThread
{
  Q_OBJECT
  public:
    CommunicationThread();
    ~CommunicationThread();

    void run();
    void stop();
    bool isCommunicating();

  private:
    bool                _communicate;
    CommunicationModel *_communicationModel;

};

#endif // __COMMUNICATION_THREAD_H__


