#ifndef __YARS_CLIENT_CONTROL_H__
#define __YARS_CLIENT_CONTROL_H__

#include "CommunicationModel.h"
#include "CommunicationThread.h"
#include "DataPanel.h"

#include <QtGui>

#include <vector>

using namespace std;

class YarsClientControl : public QWidget
{

  Q_OBJECT

  public:
    ~YarsClientControl();

    static YarsClientControl* instance();


  private:
    YarsClientControl();
    static YarsClientControl *_me;
    CommunicationModel       *_communicationModel;
    CommunicationThread      *_communicationThread;
    std::vector<DataPanel*>        _dataPanels;

  public slots:
    void quit();
    void openConnection();
    void closeConnection();
    void newCanvas();
    void closeCanvas();
    void arrageCanvas();
    void startCommunication();
    void stopCommunication();
    void quitYars();

};

#endif // __YARS_CLIENT_CONTROL_H__


