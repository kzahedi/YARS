#include "../cppClientCom/YarsClientCom.h"

#include <yars/util/YarsException.h>

#include <iostream>
#include <string>

#include <math.h>
#include <stdlib.h>


using namespace std;

int main(int argc, char** argv)
{
  double l1 = 0;
  double l2 = 0;
  double l3 = 0;
  double r1 = 0;
  double r2 = 0;
  double r3 = 0;
  double leftOutput = 0;
  double rightOutput = 0;
  double leftInput = 0;
  double rightInput = 0;
  std::cout << "starting client" << std::endl;
  YarsClientCom *com = new YarsClientCom();

  // if (argc == 2)
  // {
  cout << "starting yars with " << argv[1] << " " << argv[2] << endl;
  com->init(string(argv[1]), string(argv[2]));

  cout << "opened" << endl;

  // }
  // else if (argc == 2)
  // {
    // int port = atoi(argv[2]);
    // cout << "using " << argv[1] << " with " << port << endl;
    // com->init(argv[1], port);
  // }
  // else
  // if(argc == 1)
  // {
    // int port = atoi(argv[1]);
    // cout << "using localhost with " << port << endl;
    // com->init("127.0.0.1", port);
  // }
  // else
  // if(argc == 0)
  // {
    // cout << "using localhost with " << 4500 << endl;
    // com->init();
  // }

  int keepOnRunning = true;
  while(keepOnRunning)
  {
    try
    {
      for(int i = 0; i < 1000000; ++i)
      {
        com->update();
        l1 = com->getSensorValue(0);
        l2 = com->getSensorValue(1);
        l3 = com->getSensorValue(2);
        r1 = com->getSensorValue(3);
        r2 = com->getSensorValue(4);
        r3 = com->getSensorValue(5);
        leftInput   = (l1 + l2 + l3) / 3.0;
        rightInput  = (r1 + r2 + r3) / 3.0;
        leftOutput  = tanh(-2.0 * rightInput);
        rightOutput = tanh(-2.0 * leftInput);
        // cout << "input:  " << leftInput << " & " << rightInput << endl;
        // cout << "output: " << leftOutput << " & " << rightOutput << endl;
        com->setActuatorValue(0, leftOutput);
        com->setActuatorValue(1, rightOutput);
        if(i % 500 == 0)
        {
          cout << "sending reset" << endl;
          com->sendReset();
        }
      }
    }
    catch(YarsException &toCatch)
    {
      cout << "Caught exception: " << toCatch.what() << endl;
      // com->reopen();
      keepOnRunning = false;
    }
  }

  cout << "sending quit" << endl;
  com->sendQuit();
  exit(0);

  return 0;
}

