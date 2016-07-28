#ifndef __COMMUNICATION_MODEL_H__
#define __COMMUNICATION_MODEL_H__

#include <YarsClientCom.h>

#include <string>

using namespace std;

class CommunicationModel : public YarsClientCom
{
  public:
    ~CommunicationModel();

    static CommunicationModel* instance();

    void   connect();
    string hostname();
    int    port();
    bool   isConnected();

  private:
    CommunicationModel();
    void __standardSettings();

    static CommunicationModel *_me;

    string                     _hostname;
    int                        _port;
    bool                       _connected;

    YarsClientCom             *_ycc;

};

#endif // __COMMUNICATION_MODEL_H__


