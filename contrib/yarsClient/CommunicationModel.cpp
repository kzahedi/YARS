#include "CommunicationModel.h"

CommunicationModel* CommunicationModel::_me = NULL;

CommunicationModel::CommunicationModel()
  : YarsClientCom()
{
  __standardSettings();
}

CommunicationModel::~CommunicationModel()
{
}

CommunicationModel* CommunicationModel::instance()
{
  if(_me == NULL)
  {
    _me = new CommunicationModel();
  }
  return _me;
}

string CommunicationModel::hostname()
{
  return _hostname;
}

int CommunicationModel::port()
{
  return _port;
}

void CommunicationModel::__standardSettings()
{
  _port      = 4500;
  _hostname  = "localhost";
  _connected = false;
}

void CommunicationModel::connect()
{
  init(_hostname, _port);
  _connected = true;
  string configurationString;
  configuration(&configurationString);
  cout << "YARS configuration:" << endl;
  cout << configurationString << endl;
}

bool CommunicationModel::isConnected()
{
  return _connected;
}
