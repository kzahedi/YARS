#include "YarsClientControl.h"

#include <iostream>

using namespace std;

YarsClientControl* YarsClientControl::_me = NULL;

YarsClientControl::YarsClientControl()
{
  _communicationModel = CommunicationModel::instance();
  _communicationThread = NULL;
}

YarsClientControl::~YarsClientControl()
{
  if(_me != NULL)
  {
    delete _me;
  }
  if(_communicationThread != NULL)
  {
    delete _communicationThread;
  }
}

YarsClientControl* YarsClientControl::instance()
{
  if(_me == NULL)
  {
    _me = new YarsClientControl();
  }
  return _me;
}

void YarsClientControl::quit()
{
  cout << "quitting application" << endl;
  QCoreApplication::exit(0);
}

void YarsClientControl::openConnection()
{
  cout << "opening connection to yars" << endl;
  _communicationModel->connect();
}

void YarsClientControl::closeConnection()
{
  cout << "closing connection to yars" << endl;
}

void YarsClientControl::newCanvas()
{
  if(!_communicationModel->isConnected())
  {
    QMessageBox *box = new QMessageBox(QMessageBox::Warning,"Not Connected",
        "Please connect before opening a data panel.");
    box->show();
    return;
  }
  cout << "opening new cancas" << endl;
  DataPanel *dp = new DataPanel(0);
  dp->show();
  _dataPanels.push_back(dp);
}

void YarsClientControl::closeCanvas()
{

}

void YarsClientControl::arrageCanvas()
{

}

void YarsClientControl::startCommunication()
{
  if(_communicationThread == NULL)
  {
    _communicationThread = new CommunicationThread();
  }
  _communicationThread->start();
}

void YarsClientControl::stopCommunication()
{
  if(_communicationThread != NULL)
  {
    if(_communicationThread->isCommunicating())
    {
      _communicationThread->stop();
    }
  }
}

void YarsClientControl::quitYars()
{

}
