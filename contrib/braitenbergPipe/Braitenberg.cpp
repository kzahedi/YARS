#include "../namedPipeClientCom/YarsNamedPipeCom.h"

#include <iostream>
#include <string>

#include <math.h>
#include <stdlib.h>


using namespace std;

int main(int argc, char** argv)
{
  yReal l1 = 0;
  yReal l2 = 0;
  yReal l3 = 0;
  yReal r1 = 0;
  yReal r2 = 0;
  yReal r3 = 0;
  yReal leftOutput = 0;
  yReal rightOutput = 0;
  yReal leftInput = 0;
  yReal rightInput = 0;
  std::cout << "starting client" << std::endl;
  YarsNamedPipeCom *com = new YarsNamedPipeCom();

  if(argc > 1)
  {
    cout << "using pipe named " << argv[1] << endl;
    com->init(string(argv[1]));
  }
  else
  {
    cout << "using default pipe" << endl;
    com->init("yars-pipe");
  }

  for(int i = 0; i < 1000000; ++i)
  {
    com->update();
    com->getSensorValue(&l1, 0);
    com->getSensorValue(&l2, 1);
    com->getSensorValue(&l3, 2);
    com->getSensorValue(&r1, 3);
    com->getSensorValue(&r2, 4);
    com->getSensorValue(&r3, 5);
    leftInput   = (l1 + l2 + l3) / 3.0;
    rightInput  = (r1 + r2 + r3) / 3.0;
    leftOutput  = tanh(-2 * rightInput);
    rightOutput = tanh(-2 * leftInput);
    // cout << "input:  " << leftInput << " & " << rightInput << endl;
    // cout << "output: " << leftOutput << " & " << rightOutput << endl;
    com->setJointValue(leftOutput, 0);
    com->setJointValue(rightOutput, 1);
    if(i % 500 == 0)
    {
      cout << "sending reset" << endl;
      com->sendReset();
    }
  }

  cout << "sending quit" << endl;
  com->sendQuit();
  exit(0);

  return 0;
}

