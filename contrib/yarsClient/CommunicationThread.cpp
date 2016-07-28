#include "CommunicationThread.h"

#include "CommunicationModel.h"

#include <iostream>

using namespace std;

CommunicationThread::CommunicationThread()
{
  _communicate = true;
  _communicationModel = CommunicationModel::instance();
}

CommunicationThread::~CommunicationThread()
{

}

void CommunicationThread::run()
{
  if(!_communicationModel->isConnected()) return;
  string data;
  _communicate = true;
  while(_communicate)
  {
    _communicationModel->update();
    _communicationModel->data(&data);
    cout << "Data:" << endl;
    cout << data;
  }
  cout << "loop terminated" << endl;
}

void CommunicationThread::stop()
{
  _communicate = false;
}

bool CommunicationThread::isCommunicating()
{
  return _communicate;
}
